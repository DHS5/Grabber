// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hookable.generated.h"

UENUM(BlueprintType)
enum EHookableType
{
	HOOK_None			UMETA(Hidden),
	HOOK_Object			UMETA(DisplayName = "Object"),
	HOOK_Anchor			UMETA(DisplayName = "Anchor"),
	HOOK_MAX			UMETA(Hidden),
};

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UHookable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRABBER_API IHookable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintImplementableEvent) EHookableType GetType();
	UFUNCTION(BlueprintImplementableEvent) bool CanBeHooked();
	UFUNCTION(BlueprintImplementableEvent) bool CanBeMoved();
	UFUNCTION(BlueprintImplementableEvent) USceneComponent* GetAnchor();

	UFUNCTION(BlueprintImplementableEvent) void OnHooked();
	UFUNCTION(BlueprintImplementableEvent) void OnUnhooked();
};
