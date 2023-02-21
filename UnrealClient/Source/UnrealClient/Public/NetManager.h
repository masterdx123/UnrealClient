// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "NetManager.generated.h"



UCLASS() //insert boilerplate UE code
class  ANetManager : public AActor
{
	GENERATED_BODY() //insert boilerplate UE code

public:
	ANetManager();
	~ANetManager();

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
	FString IP = "10.0.74.153";

	ISocketSubsystem* SocketSubsystem;

	

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	void Listen();

	//we’ll think about blueprint integration later, but this is the starting bit
	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		bool sendMessage(FString Message);

	void messageQueue();
};

