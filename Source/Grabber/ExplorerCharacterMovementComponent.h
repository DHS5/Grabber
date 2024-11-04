// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grabber.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ExplorerCharacterMovementComponent.generated.h"


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
			FLAG_Custom_1		= 0x20,
			FLAG_Custom_2		= 0x40,
			FLAG_Custom_3		= 0x80,
		};

		#pragma endregion
		
		// Constructor
		FSavedMove_Explorer();
		
		// Saved variables
		uint8 Saved_bWantsToSprint;

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

	// Safe variables
private:
	bool Safe_bWantsToSprint;

	
	// Constructor
public:
	UExplorerCharacterMovementComponent();

	// Base Movement Important Methods
	// Base Movement Parameters Methods
public:
	virtual bool IsMovementMode(const EMovementMode InMovementMode) const;
	virtual float GetMaxSpeed() const override;
	
	// Sprint Parameters
private:
	UPROPERTY(EditDefaultsOnly, Category="Sprint") float MaxSprintSpeed = 750.f;
	// Sprint Methods
public:
	UFUNCTION(BlueprintCallable, Category="Sprint") void SprintPressed();
	UFUNCTION(BlueprintCallable, Category="Sprint") void SprintReleased();
};
