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
	explicit AExplorerCharacter(const FObjectInitializer& ObjectInitializer);

	// Hook
private:
	UPROPERTY(EditDefaultsOnly, Category="Hook") TEnumAsByte<ECollisionChannel> HookChannel;
public:
	UFUNCTION(BlueprintCallable, Category="Hook") bool TryHook() const;
	UFUNCTION(BlueprintCallable, Category="Hook") void ReleaseHook() const;
};
