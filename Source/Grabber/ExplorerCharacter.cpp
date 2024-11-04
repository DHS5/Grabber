// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplorerCharacter.h"

#include "ExplorerCharacterMovementComponent.h"
#include "GameFramework/DefaultPawn.h"


// Sets default values
AExplorerCharacter::AExplorerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UExplorerCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Get Explorer Character Movement Component
	ExplorerMovementComponent = Cast<UExplorerCharacterMovementComponent>(GetCharacterMovement());
}

// Called when the game starts or when spawned
void AExplorerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplorerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AExplorerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

