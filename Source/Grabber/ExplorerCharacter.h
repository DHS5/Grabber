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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	virtual void Tick(float DeltaSeconds) override;

	// --- JUMP ---
protected:
	virtual bool CanJumpInternal_Implementation() const override;
	virtual bool CanCoyoteJump() const;
	
	// --- HOOK ---
	// Parameters
private:
	UPROPERTY(EditDefaultsOnly, Category="Hook") float ObjectHookSpeed = 100.f;
	UPROPERTY(EditDefaultsOnly, Category="Hook") float ObjectHookMinDist = 100.f;

	// Variables
protected:
	// Server Variables
	UPROPERTY() TObjectPtr<AActor> HookedActor;
	UPROPERTY() TObjectPtr<UPrimitiveComponent> HookedPrimitiveComponent;
	UPROPERTY() bool bHookedObjectCanBeMoved;
	// Replicated Variables
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Hook") bool bIsHookingObject;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Hook") bool bIsHookingAnchor;

	// Server-side
private:
	// RPCs
	UFUNCTION(Server, Reliable) void Server_TryHook(FVector TraceStart, FVector TraceEnd);
	UFUNCTION(Server, Reliable) void Server_ReleaseHook();
	// Server Functions
	UFUNCTION() void Server_HookActor(AActor* Actor);

	// Client-side
protected:
	UFUNCTION(Client, Reliable) void Client_OnHookObject(AActor* Actor);
	UFUNCTION(Client, Reliable) void Client_OnHookAnchor(FVector AnchorLocation);

	// Blueprint Functions (Local)
public:
	UFUNCTION(BlueprintGetter, Category="Hook") bool CanHook() const;
	UFUNCTION(BlueprintCallable, Category="Hook") void TryHook();
	UFUNCTION(BlueprintCallable, Category="Hook") void ReleaseHook();
};
