// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrabberCharacter.h"
#include "GameFramework/Character.h"
#include "ExplorerCharacter.generated.h"

UCLASS()
class GRABBER_API AExplorerCharacter : public AGrabberCharacter
{
	GENERATED_BODY()

	// References
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	class UExplorerCharacterMovementComponent* ExplorerMovementComponent;

public:
	// Sets default values for this character's properties
	AExplorerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
