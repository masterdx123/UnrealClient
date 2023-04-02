#include "MyRaycastComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UMyRaycastComponent::UMyRaycastComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMyRaycastComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMyRaycastComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMyRaycastComponent::PerformRaycast()
{
	FHitResult HitResult;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	// Get the camera location and forward vector
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector StartLocation = CameraLocation;
	FVector EndLocation = StartLocation + (CameraRotation.Vector() * RaycastLength);
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = false;
	CollisionParams.AddIgnoredActor(GetOwner()); // Ignore the owning actor

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	if (bHit)
	{
		// Draw a debug line to visualize the raycast
		DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Green, false, 2.f);

		// Do something with the hit object (e.g. print its name)
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit %s"), *HitActor->GetName());
		}
	}
	else
	{
		// Draw a debug line to visualize the raycast
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.f);
	}
}