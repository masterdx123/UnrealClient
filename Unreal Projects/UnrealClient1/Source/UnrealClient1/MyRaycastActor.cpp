// Fill out your copyright notice in the Description page of Project Settings.

#include "MyRaycastActor.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMyRaycastActor::AMyRaycastActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMyRaycastActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyRaycastActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyRaycastActor::PerformRaycast()
{
	FHitResult hitResult;
	FVector startLoc = GetActorLocation();
	FVector endLoc = startLoc + (GetActorForwardVector() * 1000.f); //1000 units in front of the actor
	FCollisionQueryParams collisionParams;
	collisionParams.bTraceComplex = false;
	collisionParams.AddIgnoredActor(this); //Ignore the actor itself

	bool boolHit = GetWorld()->LineTraceSingleByChannel(hitResult, startLoc, endLoc, ECC_Visibility, collisionParams);

	if (boolHit)
	{
		//Draw a debug line to visualize the raycast
		DrawDebugLine(GetWorld(), startLoc, hitResult.ImpactPoint, FColor::Green, false, 2.f);

		AActor* hitActor = hitResult.GetActor();
		//Do something with the hit object (e.g. print its name)
		if (hitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *hitActor->GetName());
		}
	}
	else
	{
		// Draw a debug line to visualize the raycast
		DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red, false, 2.f);
	}
}







