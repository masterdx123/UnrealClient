#include "MyRaycastComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "NetActorComponent.h"
#include "NetManager.h"

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

	//get player controller from the world 
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	//get the camera location and forward vector
	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector StartLocation = CameraLocation;
	FVector EndLocation = StartLocation + (CameraRotation.Vector() * RaycastLength);
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = false;
	CollisionParams.AddIgnoredActor(GetOwner()); //ignore the owning actor for collisions

	//get the raycast hit
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	if (bHit)
	{
		//draw a debug line to visualize the raycast when hitting something
		DrawDebugLine(GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Green, false, 2.f);

		//do something with the hit object (e.g. print its name)
		AActor* HitActor = HitResult.GetActor();

		UNetActorComponent* hitNetObject = HitActor->FindComponentByClass<UNetActorComponent>();
		

		if (hitNetObject)
		{
			int dmg = 10;
			FString losHP = FString::Printf(TEXT("lose hp;%i;%i"), hitNetObject->GetGlobalID(), dmg);
			ANetManager::singleton->sendMessage(losHP);
		}
	}
	else
	{
		//draw a debug line to visualize the raycast when missing
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 2.f);
	}
}