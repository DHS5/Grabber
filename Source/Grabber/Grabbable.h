// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Grabberr.h"
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Grabbable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UGrabbable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GRABBER_API IGrabbable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnGrabbedBy(UGrabberr* grabber);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnDroppedBy(UGrabberr* grabber);
};
