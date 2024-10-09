// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabberr.h"

// Sets default values for this component's properties
UGrabberr::UGrabberr()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}


// Called when the game starts
void UGrabberr::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->GetComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		PhysicsHandle = NewObject<UPhysicsHandleComponent>(GetOwner());
		PhysicsHandle->RegisterComponent();
	}
}


// Called every frame
void UGrabberr::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsGrabbing)
	{
		PhysicsHandle->SetTargetLocation(GetComponentLocation());
	}
}

bool UGrabberr::TryGrab()
{
	FHitResult Hit = FHitResult(ForceInit);
	FVector start = GetComponentLocation();
	FVector end = start + GetForwardVector() * GrabMaxDistance;
	FCollisionQueryParams TraceParams(FName(TEXT("Trace")));
	TraceParams.AddIgnoredActor(GetOwner());
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	if (GetWorld()->SweepSingleByChannel(
		Hit,
		start,
		end, 
		FQuat().Identity,
		CollisionChannel,
		FCollisionShape().MakeSphere(GrabRadius),
		TraceParams))
	{
		if (Hit.GetActor() && Hit.GetActor()->Implements<UGrabbable>())
		{
			GrabbedComponent = Hit.GetComponent();
			GrabbedActor = Hit.GetActor();
			IGrabbable::Execute_OnGrabbedBy(GrabbedActor, this);
			PrimaryComponentTick.SetTickFunctionEnable(true);
			PhysicsHandle->GrabComponentAtLocation(GrabbedComponent, "", GrabbedComponent->GetComponentLocation());
			bIsGrabbing = true;

			return true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Can't find grabbable on "));
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Hit.GetActor()->GetDebugName(Hit.GetActor()));
		}
	}

	return false;
}

bool UGrabberr::Drop()
{
	if (bIsGrabbing)
	{
		bIsGrabbing = false;
		IGrabbable::Execute_OnDroppedBy(GrabbedActor, this);
		PhysicsHandle->ReleaseComponent();
		GrabbedComponent = nullptr;
		GrabbedActor = nullptr;
		PrimaryComponentTick.SetTickFunctionEnable(false);

		return true;
	}

	return false;
}

bool UGrabberr::Throw()
{
	return false;
}

