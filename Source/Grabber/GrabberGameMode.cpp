// Copyright Epic Games, Inc. All Rights Reserved.

#include "GrabberGameMode.h"
#include "GrabberCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGrabberGameMode::AGrabberGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
