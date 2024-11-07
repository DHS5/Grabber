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

bool AExplorerCharacter::TryHook() const
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
		HookChannel,
		TraceParams))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf( TEXT("Hit %s"), *Hit.GetActor()->GetName()));
		return ExplorerMovementComponent->Hook(Hit.Location);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Can't find hook anchor"));
	return false;
	
}

void AExplorerCharacter::ReleaseHook() const
{
	ExplorerMovementComponent->Unhook();
}

