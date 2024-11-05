// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExplorerCharacter.h"
#include "Grabber.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ExplorerCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum EExplorerCustomMovementMode
{
	CMOVE_None			UMETA(Hidden),
	CMOVE_Hook			UMETA(DisplayName = "Hook"),
	CMOVE_MAX			UMETA(Hidden),
};

UCLASS()
class GRABBER_API UExplorerCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

#pragma region Saved Move

	class FSavedMove_Explorer : public FSavedMove_Character
	{
	public:
		
		#pragma region Explorer Compressed Flags
	
		enum EExplorerFlags
		{
			FLAG_WantsToSprint	= 0x10,
			FLAG_WantsToGlide	= 0x20,
			FLAG_Custom_2		= 0x40,
			FLAG_Custom_3		= 0x80,
		};

		#pragma endregion
		
		// Constructor
		FSavedMove_Explorer();
		
		// Saved variables
		uint8 Saved_bWantsToSprint;
		uint8 Saved_bWantsToGlide;

		#pragma region Base Methods

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual  void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual  void PrepMoveFor(ACharacter* C) override;

		#pragma endregion 
	};

protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
#pragma endregion

#pragma region Network Prediction Data

	class FNetworkPredictionData_Client_Explorer : public FNetworkPredictionData_Client_Character
	{		
	public:
		typedef FNetworkPredictionData_Client_Character Super;
		
		FNetworkPredictionData_Client_Explorer(const UCharacterMovementComponent& ClientMovement);

		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
#pragma endregion 

	// References
protected:
	UPROPERTY(Transient) AExplorerCharacter* ExplorerCharacterOwner;
	
	// Safe variables
private:
	bool Safe_bWantsToSprint;
	bool Safe_bWantsToGlide;

	
	// Constructor & Initialization
public:
	UExplorerCharacterMovementComponent();
protected:
	virtual void InitializeComponent() override;

	// Movement Important Methods
public:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
protected:
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	
	// Movement Parameters Methods
public:
	UFUNCTION(BlueprintPure) bool IsMovementMode(const EMovementMode InMovementMode) const;
	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(EExplorerCustomMovementMode InCustomMovementMode) const;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual float GetMinAnalogSpeed() const override;
	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const override;
	virtual bool IsFalling() const override;
	virtual bool IsFlying() const override;
	virtual bool IsMovingOnGround() const override;

#pragma region Sprint
	
	// Sprint Parameters
private:
	UPROPERTY(EditDefaultsOnly, Category="Sprint") float MaxSprintSpeed = 750.f;
	// Sprint Methods
public:
	UFUNCTION(BlueprintCallable, Category="Sprint") void SprintPressed();
	UFUNCTION(BlueprintCallable, Category="Sprint") void SprintReleased();

#pragma endregion

#pragma region Glide

private:
	UPROPERTY(EditDefaultsOnly, Category="Glide", meta=(ClampMax=-0.1f)) float MinGlideZVelocity = -1.f;
	UPROPERTY(EditDefaultsOnly, Category="Glide", meta=(ClampMin=0.1f)) float MaxGlideZVelocity = 1.f;
	UPROPERTY(EditDefaultsOnly, Category="Glide", meta=(ClampMax=1f, ClampMin=0f)) float GlideLerpFactor = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category="Glide", meta=(ClampMax=1f, ClampMin=0f)) float GlideAirControl = 1.f;
	float DefaultAirControl;
public:
	UFUNCTION(BlueprintCallable, Category="Glide") void GlidePressed();
	UFUNCTION(BlueprintCallable, Category="Glide") void GlideReleased();
	UFUNCTION(BlueprintCallable, Category="Glide") bool IsGliding();

#pragma endregion

#pragma region Hook

private:
	UPROPERTY(EditDefaultsOnly, Category="Hook") float MaxHookSpeed = 1250.f;
	UPROPERTY(EditDefaultsOnly, Category="Hook") float HookAcceleration = 1.f;
	UPROPERTY(EditDefaultsOnly, Category="Hook") float HookStartImpulse = 250.f;
	UPROPERTY(EditDefaultsOnly, Category="Hook", meta=(ClampMin=100.f)) float MinHookDistance = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category="Hook", meta=(ClampMin=1000.f)) float MaxHookDistance = 20000.f;
	FVector HookTargetLocation;
private:
	virtual void OnEnterHook(EMovementMode PrevMode, EExplorerCustomMovementMode PrevCustomMode);
	virtual void OnExitHook();
	void PhysHook(float deltaTime, int32 Iterations);
protected:
	virtual bool CanHook(const FVector& TargetLocation);
public:
	bool Hook(const FVector& TargetLocation);
	void Unhook();
	FORCEINLINE float GetHookMaxDistance() const { return MaxHookDistance; }

#pragma endregion 
};
