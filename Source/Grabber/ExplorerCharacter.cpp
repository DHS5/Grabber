// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplorerCharacter.h"

#include "ExplorerCharacterMovementComponent.h"
#include "Hookable.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AExplorerCharacter::AExplorerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UExplorerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Get Explorer Character Movement Component
	ExplorerMovementComponent = Cast<UExplorerCharacterMovementComponent>(GetCharacterMovement());

	bReplicates = true;
}

void AExplorerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AExplorerCharacter, bIsHookingObject);
}

void AExplorerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsHookingObject &&
		HookedActor != nullptr &&
		HookedActor->HasAuthority())
	{
		FVector HookedActorLocation = HookedActor->GetActorLocation();
		FVector Direction = GetActorLocation() - HookedActorLocation;
		float Distance = Direction.Size();
		if (Distance < ObjectHookMinDist)
		{
			Server_UnhookActor();
		}
		else
		{
			HookedActor->SetActorLocation(HookedActorLocation + Direction.GetSafeNormal() * ObjectHookSpeed * DeltaSeconds);
		}
	}
}

bool AExplorerCharacter::CanJumpInternal_Implementation() const
{
	return !bIsCrouched && (JumpIsAllowedInternal() || CanCoyoteJump());
}

bool AExplorerCharacter::CanCoyoteJump() const
{
	if (ExplorerMovementComponent->CanAttemptJump())
	{
		if (!bWasJumping
			&& JumpCurrentCountPreJump == 0
			&& ExplorerMovementComponent->CanCoyoteJump())
		{
			return true;
		}
	}

	return false;
}

void AExplorerCharacter::Server_HookActor_Implementation(AActor* Actor)
{
	bIsHookingObject = true;
	HookedActor = Actor;
	IHookable::Execute_OnHooked(HookedActor);
}

void AExplorerCharacter::Server_UnhookActor_Implementation()
{
	if (HookedActor != nullptr)
	{
		IHookable::Execute_OnUnhooked(HookedActor);
		HookedActor = nullptr;
	}
	bIsHookingObject = false;
}

void AExplorerCharacter::Client_HookActor(AActor* Actor)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf( TEXT("Dynamic Hit %s"), *HookedActor->GetName()));
	Server_HookActor(Actor);
}

bool AExplorerCharacter::CanHook() const
{
	return !bIsHookingObject && !ExplorerMovementComponent->IsHooking();
}

void AExplorerCharacter::TryHook()
{
	if (!CanHook()) return;
	
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);
	FVector End = CameraLocation + CameraRotation.Vector() * ExplorerMovementComponent->GetHookMaxDistance();

	FHitResult Hit = FHitResult();
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")));
	TraceParams.AddIgnoredActor(this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		CameraLocation,
		End,
		ECC_WorldStatic,
		TraceParams)
		&& Hit.GetActor())
	{
		switch (Hit.GetActor()->GetRootComponent()->Mobility)
		{
			case EComponentMobility::Movable:
				{
					if (Hit.GetActor()->Implements<UHookable>()
						&& IHookable::Execute_CanBeHooked(Hit.GetActor()))
					{
						Client_HookActor(Hit.GetActor());
					}
					return;
				}
			case EComponentMobility::Static:
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf( TEXT("Static Hit %s"), *Hit.GetActor()->GetName()));
					ExplorerMovementComponent->TryHook(Hit.Location);
					return;
				}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Can't find hook anchor"));	
	}
}

void AExplorerCharacter::ReleaseHook()
{
	ExplorerMovementComponent->Unhook();
	Server_UnhookActor();
}

