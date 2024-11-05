// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplorerCharacterMovementComponent.h"

#include "ExplorerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

#pragma region Saved Move Methods

UExplorerCharacterMovementComponent::FSavedMove_Explorer::FSavedMove_Explorer()
{
	Saved_bWantsToSprint = 0;
	Saved_bWantsToGlide = 0;
}

bool UExplorerCharacterMovementComponent::FSavedMove_Explorer::CanCombineWith(const FSavedMovePtr& NewMove,
		ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Explorer* NewExplorerMove = static_cast<FSavedMove_Explorer*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewExplorerMove->Saved_bWantsToSprint)
	{
		return false;
	}
	if (Saved_bWantsToGlide != NewExplorerMove->Saved_bWantsToGlide)
	{
		return false;
	}
	
	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UExplorerCharacterMovementComponent::FSavedMove_Explorer::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = 0;
	Saved_bWantsToGlide = 0;
}

uint8 UExplorerCharacterMovementComponent::FSavedMove_Explorer::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (Saved_bWantsToSprint) Result |= FLAG_WantsToSprint;
	if (Saved_bWantsToGlide) Result |= FLAG_WantsToGlide;

	return Result;
}

void UExplorerCharacterMovementComponent::FSavedMove_Explorer::SetMoveFor(ACharacter* C, float InDeltaTime,
	FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UExplorerCharacterMovementComponent* CharacterMovement = Cast<UExplorerCharacterMovementComponent>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bWantsToGlide = CharacterMovement->Safe_bWantsToGlide;
}

void UExplorerCharacterMovementComponent::FSavedMove_Explorer::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UExplorerCharacterMovementComponent* CharacterMovement = Cast<UExplorerCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint == 1;
	CharacterMovement->Safe_bWantsToGlide = Saved_bWantsToGlide == 1;
}

void UExplorerCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_Explorer::FLAG_WantsToSprint) != 0;
	Safe_bWantsToGlide = (Flags & FSavedMove_Explorer::FLAG_WantsToGlide) != 0;
}

#pragma endregion

#pragma region Network Prediction Data Methods

UExplorerCharacterMovementComponent::FNetworkPredictionData_Client_Explorer::FNetworkPredictionData_Client_Explorer(
	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement) {}

FSavedMovePtr UExplorerCharacterMovementComponent::FNetworkPredictionData_Client_Explorer::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Explorer());
}

FNetworkPredictionData_Client* UExplorerCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (ClientPredictionData == nullptr)
	{
		UExplorerCharacterMovementComponent* MutableThis = const_cast<UExplorerCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Explorer(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}

#pragma endregion

// --- Character Movement Component ---

#pragma region Constructor

UExplorerCharacterMovementComponent::UExplorerCharacterMovementComponent()
{
	Safe_bWantsToSprint = false;
	Safe_bWantsToGlide = false;
}

void UExplorerCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	ExplorerCharacterOwner = Cast<AExplorerCharacter>(GetOwner());
}

#pragma endregion

#pragma region Movement Important Methods

void UExplorerCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UExplorerCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
}

void UExplorerCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UExplorerCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode,
	uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Hook) OnExitHook();

	if (IsCustomMovementMode(CMOVE_Hook)) OnEnterHook(PreviousMovementMode, static_cast<EExplorerCustomMovementMode>(PreviousCustomMode));
}

void UExplorerCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Hook:
		PhysHook(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

#pragma endregion

#pragma region Movement Parameter Methods

bool UExplorerCharacterMovementComponent::IsMovementMode(const EMovementMode InMovementMode) const
{
	return InMovementMode == MovementMode;
}

bool UExplorerCharacterMovementComponent::IsCustomMovementMode(EExplorerCustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

float UExplorerCharacterMovementComponent::GetMaxSpeed() const
{
	if (IsMovementMode(MOVE_Walking) && Safe_bWantsToSprint && !IsCrouching()) return MaxSprintSpeed;
	
	if (MovementMode != MOVE_Custom) return Super::GetMaxSpeed();

	UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	return 0;
}

FVector UExplorerCharacterMovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity,
	float DeltaTime) const
{
	FVector Result = Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);

	if (Safe_bWantsToGlide)
	{
		if (Result.Z < MinGlideZVelocity)
		{
			Result.Z = FMath::Lerp(Result.Z, MinGlideZVelocity, GlideLerpFactor);
		}
		else if (Result.Z > MaxGlideZVelocity)
		{
			Result.Z = FMath::Lerp(Result.Z, MaxGlideZVelocity, GlideLerpFactor);
		}
	}

	return Result;
}

#pragma endregion 

#pragma region Sprint

void UExplorerCharacterMovementComponent::SprintPressed()
{
	Safe_bWantsToSprint = true;
}

void UExplorerCharacterMovementComponent::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

#pragma endregion

#pragma region Glide

void UExplorerCharacterMovementComponent::GlidePressed()
{
	Safe_bWantsToGlide = true;
	DefaultAirControl = AirControl;
	AirControl = GlideAirControl;
}

void UExplorerCharacterMovementComponent::GlideReleased()
{
	Safe_bWantsToGlide = false;
	AirControl = DefaultAirControl;
}

#pragma endregion

#pragma region Hook

void UExplorerCharacterMovementComponent::OnEnterHook(EMovementMode PrevMode,
	EExplorerCustomMovementMode PrevCustomMode)
{
}

void UExplorerCharacterMovementComponent::OnExitHook()
{
}

void UExplorerCharacterMovementComponent::PhysHook(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
}

#pragma endregion
