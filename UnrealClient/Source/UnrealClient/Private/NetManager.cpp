// Fill out your copyright notice in the Description page of Project Settings.


#include "NetManager.h"
#include <string>


// Sets default values
ANetManager::ANetManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Socket = nullptr;


}

ANetManager::~ANetManager()
{
}


// Called when the game starts or when spawned
void ANetManager::BeginPlay()
{
	Super::BeginPlay();
	SocketSubsystem = nullptr;
	//more macro code. We’re using Unreal’s low level generic networking (as opposed to it’s higher level game-oriented solution).
	if (SocketSubsystem == nullptr)	SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	//packet and buffer sizes
	SendSize = 2 * 1024 * 1024;
	BufferSize = 2 * 1024 * 1024;

	//local endpoint
	LocalEndpoint = FIPv4Endpoint(FIPv4Address::Any, LocalPort);

	FIPv4Address::Parse(IP, RemoteAddress);
	//server endpoint
	RemoteEndpoint = FIPv4Endpoint(RemoteAddress, RemotePort);

	Socket = nullptr;

	if (SocketSubsystem != nullptr)
	{
		if (Socket == nullptr)
		{
			//similar to C#, we use an API to build the socket based on configuration parameters
			Socket = FUdpSocketBuilder(SocketDescription)
				.AsNonBlocking()
				.AsReusable()
				.BoundToEndpoint(LocalEndpoint)
				.WithReceiveBufferSize(SendSize)
				.WithReceiveBufferSize(BufferSize)
				.WithBroadcast();
		}
	}

	FString t = "FirstEntrance";
	sendMessage(t); // Send Message Test
}


void ANetManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SocketSubsystem->DestroySocket(Socket);
	Socket = nullptr;
	SocketSubsystem = nullptr;
}


void ANetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Listen(); // Listen for messages

	FString t = "I'm an Unreal client!";
	sendMessage(t); // Send Message Test
}

void ANetManager::Listen()
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint32 Size;
	if(Socket->HasPendingData(Size))
	{
		uint8* Recv = new uint8[Size];
		int32 BytesRead = 0;

		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);

		char ansiiData[1024];
		memcpy(ansiiData, ReceivedData.GetData(), BytesRead);
		ansiiData[BytesRead] = 0;

		FString data = ANSI_TO_TCHAR(ansiiData);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Message by UDP: " + data);
	}
}


bool ANetManager::sendMessage(FString Message)
{
	if (!Socket) return false;
	int32 BytesSent;

	FTimespan waitTime = FTimespan(10);
	//this is where we create the packet, in this case by serialising the character array
	TCHAR* serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);

	bool success = Socket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndpoint.ToInternetAddr());
	UE_LOG(LogTemp, Warning, TEXT("Sent message: %s : %s : Address - %s : BytesSent - %d"), *Message, (success ? TEXT("true") : TEXT("false")), *RemoteEndpoint.ToString(), BytesSent);
	//UE_LOG lets us log error messages, not dissimilar to Debug.Log

	if (success && BytesSent > 0) return true;
	else return false;
}

void ANetManager::messageQueue()
{
}


