// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
//#include "PhysicsHandleComponent.generated.h"
#include "Grabbable.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Grabberr.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GRABBER_API UGrabberr : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabberr();

	UPROPERTY(EditAnywhere, Category = "Parameters")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float GrabMaxDistance;
	
	UPROPERTY(EditAnywhere, Category = "Parameters")
	float GrabRadius;

	bool IsGrabbing() const { return bIsGrabbing; }
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	class UPhysicsHandleComponent* PhysicsHandle;
	
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	class UPrimitiveComponent* GrabbedComponent;
	
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	class AActor* GrabbedActor;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsGrabbing;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Grab")
	bool TryGrab();
	
	UFUNCTION(BlueprintCallable, Category = "Grab")
	bool Drop();
	
	UFUNCTION(BlueprintCallable, Category = "Grab")
	bool Throw();
};
