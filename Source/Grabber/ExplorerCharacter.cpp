// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplorerCharacter.h"

#include "ExplorerCharacterMovementComponent.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
AExplorerCharacter::AExplorerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UExplorerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Get Explorer Character Movement Component
	ExplorerMovementComponent = Cast<UExplorerCharacterMovementComponent>(GetCharacterMovement());
}

void AExplorerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HookedActor != nullptr)
	{
		FVector HookedActorLocation = HookedActor->GetActorLocation();
		FVector Direction = GetActorLocation() - HookedActorLocation;
		float Distance = Direction.Size();
		if (Distance < ObjectHookMinDist)
		{
			ReleaseHook();
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

bool AExplorerCharacter::TryHook()
{
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
		TraceParams))
	{
		switch (Hit.GetActor()->GetRootComponent()->Mobility)
		{
			case EComponentMobility::Movable:
				{
					HookedActor = Hit.GetActor();
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf( TEXT("Dynamic Hit %s"), *HookedActor->GetName()));
					return HookedActor != nullptr;
				}
			case EComponentMobility::Static:
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf( TEXT("Static Hit %s"), *Hit.GetActor()->GetName()));
					return ExplorerMovementComponent->Hook(Hit.Location);
				}
		}
		return false;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Can't find hook anchor"));
	return false;
	
}

void AExplorerCharacter::ReleaseHook()
{
	ExplorerMovementComponent->Unhook();
	HookedActor = nullptr;
}

