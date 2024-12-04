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

	// Jump
protected:
	virtual bool CanJumpInternal_Implementation() const override;
	virtual bool CanCoyoteJump() const;
	
	// Hook
private:
	UFUNCTION(Server, Reliable) void Server_HookActor(AActor* Actor);
	UFUNCTION(Server, Reliable) void Server_UnhookActor();
	void Client_HookActor(AActor* Actor);
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Hook") AActor* HookedActor;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Hook") bool bIsHookingObject;
private:
	UPROPERTY(EditDefaultsOnly, Category="Hook") TEnumAsByte<ECollisionChannel> HookChannel;
	UPROPERTY(EditDefaultsOnly, Category="Hook") float ObjectHookSpeed = 100.f;
	UPROPERTY(EditDefaultsOnly, Category="Hook") float ObjectHookMinDist = 100.f;
public:
	UFUNCTION(BlueprintGetter, Category="Hook") bool CanHook() const;
	UFUNCTION(BlueprintCallable, Category="Hook") void TryHook();
	UFUNCTION(BlueprintCallable, Category="Hook") void ReleaseHook();
};
