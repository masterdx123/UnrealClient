// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NetActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCLIENT1_API UNetActorComponent : public UActorComponent
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	bool isLocallyOwned;
	UPROPERTY(EditAnywhere)
	int globalID;
	UPROPERTY(EditAnywhere)
	int localID;
	UPROPERTY(EditAnywhere)
	int HP = 100;



public:	
	static int32 lastLocalID;
	int32 GetGlobalID();
	int32 GetLocalID();
	void SetGlobalID(int32 gId);
	void SetLocalID(int32 lId);
	void FromPacket(FString packet);
	FString ToPacket();
	int32 GlobalIDFromPacket(FString packet);
	// Sets default values for this component's properties
	UNetActorComponent();
	bool GetIsLocallyOwned();
	void ChangeHp(FString packet);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "GetHp")
		int GetCharaterHp();
};
