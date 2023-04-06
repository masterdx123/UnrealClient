// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "NetActorComponent.h"
#include "NetManager.generated.h"




UCLASS() //insert boilerplate UE code
class  ANetManager : public AActor
{
	GENERATED_BODY() //insert boilerplate UE code

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> OtherPlayerAvatars;

	float timer = 0;
public:
	ANetManager();
	~ANetManager();

	static ANetManager* singleton;
	static TArray<UNetActorComponent*> localNetObjects; 

	void AddNetObject(UNetActorComponent* component); //add a new netobject

	FSocket* Socket;

	// Local Endpoint
	FString SocketDescription = "UDP Listen Socket";
	FIPv4Endpoint LocalEndpoint;
	uint16 LocalPort = 9051;
	int32 SendSize;
	TArray<uint8> ReceivedData;

	// Remote Endpoint
	FIPv4Endpoint RemoteEndpoint;
	FIPv4Address RemoteAddress;
	uint16 RemotePort = 9050;
	int32 BufferSize;
	FString IP = "25.53.60.215";

	ISocketSubsystem* SocketSubsystem;

	

protected:
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	void Listen();

	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		bool sendMessage(FString Message);

	void messageQueue();
};

