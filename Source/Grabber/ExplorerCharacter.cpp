// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplorerCharacter.h"

#include "ExplorerCharacterMovementComponent.h"
#include "Hookable.h"
#include "Camera/CameraComponent.h"
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
	DOREPLIFETIME(AExplorerCharacter, bIsHookingAnchor);
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
			Server_ReleaseHook();
		}
		else
		{
			UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(HookedActor->GetRootComponent());
			if (PrimitiveComponent != nullptr)
			{
				PrimitiveComponent->SetPhysicsLinearVelocity(Direction.GetSafeNormal() * ObjectHookSpeed, false);
			}
			else
			{
				HookedActor->GetRootComponent()->SetWorldLocation(HookedActorLocation + Direction.GetSafeNormal() * ObjectHookSpeed * DeltaSeconds, true);	
			}
		}
	}
}

#pragma region Coyote Jump

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

#pragma endregion

void AExplorerCharacter::Server_HookActor(AActor* Actor)
{
	HookedActor = Actor;
	IHookable::Execute_OnHooked(HookedActor);
	
	switch (IHookable::Execute_GetType(Actor))
	{
	case EHookableType::HOOK_Object:
		bIsHookingObject = true;
		Client_OnHookObject(HookedActor);
		return;
		
	case EHookableType::HOOK_Anchor:
		bIsHookingAnchor = true;
		Client_OnHookAnchor(IHookable::Execute_GetAnchor(HookedActor)->GetComponentLocation());
		return;
	default: ;
	}
}

void AExplorerCharacter::Server_ReleaseHook_Implementation()
{
	if (HookedActor != nullptr)
	{
		IHookable::Execute_OnUnhooked(HookedActor);
		HookedActor = nullptr;
	}
	bIsHookingObject = false;
	bIsHookingAnchor = false;
}

void AExplorerCharacter::Server_TryHook_Implementation(FVector TraceStart, FVector TraceEnd)
{
	FHitResult Hit = FHitResult();
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")));
	TraceParams.AddIgnoredActor(this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		TraceParams))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor != nullptr
			&& HitActor->Implements<UHookable>()
			&& IHookable::Execute_CanBeHooked(HitActor))
		{
			Server_HookActor(HitActor);
		}
	}
}

#pragma region Hook Client Callbacks

void AExplorerCharacter::Client_OnHookObject_Implementation(AActor* Actor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf( TEXT("Hook object %s"), *Actor->GetName()));
}
void AExplorerCharacter::Client_OnHookAnchor_Implementation(FVector AnchorLocation)
{
	ExplorerMovementComponent->TryHook(AnchorLocation);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf( TEXT("Hook anchor %s"), *AnchorLocation.ToString()));
}

#pragma endregion

#pragma region Hook Blueprint Functions

bool AExplorerCharacter::CanHook() const
{
	return !bIsHookingObject && !bIsHookingAnchor;
}

void AExplorerCharacter::TryHook()
{
	if (!CanHook()) return;
	
	FVector CameraLocation = GetFollowCamera()->GetComponentLocation();
	FRotator CameraRotation = GetFollowCamera()->GetForwardVector().Rotation();
	FVector End = CameraLocation + CameraRotation.Vector() * ExplorerMovementComponent->GetHookMaxDistance();

	Server_TryHook(CameraLocation, End);
}

void AExplorerCharacter::ReleaseHook()
{
	ExplorerMovementComponent->Unhook();
	Server_ReleaseHook();
}

#pragma endregion
