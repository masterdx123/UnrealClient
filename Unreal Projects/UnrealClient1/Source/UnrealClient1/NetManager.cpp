// Fill out your copyright notice in the Description page of Project Settings.


#include "NetManager.h"
#include <string>

ANetManager* ANetManager::singleton;

TArray<UNetActorComponent*> ANetManager::localNetObjects;

// Sets default values in constructor
ANetManager::ANetManager()
{
	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	Socket = nullptr;


}

ANetManager::~ANetManager()
{
}




// Called when the game starts or when spawned
void ANetManager::PostInitializeComponents()
{
	if (singleton == nullptr) singleton = this; //if it’s null, it becomes the current instance

	Super::PostInitializeComponents();
	SocketSubsystem = nullptr;
	
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
			//configure parameters of socket
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
	localNetObjects.Empty(); //this empties the array, so it’s clear next time we start playmode
	singleton = nullptr; //it becomes null for next time we press play 
	Super::EndPlay(EndPlayReason);
	ANetManager::localNetObjects.Empty();
	SocketSubsystem->DestroySocket(Socket);
	Socket = nullptr;
	SocketSubsystem = nullptr;
	
}

void ANetManager::AddNetObject(UNetActorComponent* component)
{
	ANetManager::localNetObjects.Add(component);
	if ((component->GetGlobalID() == 0) && (component->GetIsLocallyOwned())) {
		FString t = "I need a UID for local object:" + FString::FromInt(component->GetLocalID());
		sendMessage(t);
	}
}


void ANetManager::Tick(float DeltaTime)
{

	//keep sending messages to the server
	Super::Tick(DeltaTime);
	timer += DeltaTime;
	for (UNetActorComponent* netObject : ANetManager::localNetObjects) {
		if (netObject->GetIsLocallyOwned() && netObject->GetGlobalID() != 0) {
			if (timer > 0.8f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Sending: %s"), *netObject->ToPacket());
				sendMessage(netObject->ToPacket());
				timer = 0;
			}
		}
	}
	Listen(); // Listen for messages
}

void ANetManager::Listen()
{
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint32 Size;
	while (Socket->HasPendingData(Size))
	{
		//receive data from server
		uint8* Recv = new uint8[Size];
		int32 BytesRead = 0;

		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
		Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);

		char ansiiData[4096];
		memcpy(ansiiData, ReceivedData.GetData(), BytesRead);
		ansiiData[BytesRead] = 0;

		FString data = ANSI_TO_TCHAR(ansiiData);

		//show message from server on screen
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Message by UDP: " + data);

		//if message is to assign Ids
		if (data.Contains("Assigned UID:")) {

			FString message, info;
			//split off the 'Assigned UID:' bit, by delimiting at the :
			if (data.Split(TEXT(":"), &message, &info)) {
				FString lid, gid;
				//split into local and global ID, by delimiting at the ;
				if (info.Split(TEXT(";"), &lid, &gid)) {

					//the Atoi function is the equivalent of Int32.Parse in C#, converting a string to an int32
					int32 intGlobalID = FCString::Atoi(*gid);
					int32 intLocalID = FCString::Atoi(*lid);

					//iterate netObjects, find the one the local ID corresponds to, and assign the global ID
					for (UNetActorComponent* netObject : ANetManager::localNetObjects) {
						if (netObject->GetLocalID() == intLocalID) {
							netObject->SetGlobalID(intGlobalID);
							UE_LOG(LogTemp, Warning, TEXT("Assigned: %d"), intGlobalID);
						}

					}

				}
			}

		}
		//if it the message relates to object data
		else if (data.Contains("Object data;")) 
		{

			UE_LOG(LogTemp, Warning, TEXT("parsing state data"));
			bool foundActor = false;
			//loop gameobjects
			for (UNetActorComponent* netObject : ANetManager::localNetObjects) 
			{
				//if gameobject id equale the one from the packet update information of the gameobject
				if (netObject->GetGlobalID() == netObject->GlobalIDFromPacket(data) || netObject->GetGlobalID()==0) {
					if (!netObject->GetIsLocallyOwned()) {
						netObject->FromPacket(data);
					}
					foundActor = true;
				}
			}
			//if the object was njot found spawn it adn set its data to be the one we got from the server
			if (!foundActor) {
				UE_LOG(LogTemp, Warning, TEXT("spawning"));
				AActor* actor = GetWorld()->SpawnActor<AActor>(OtherPlayerAvatars, FVector::ZeroVector, FRotator::ZeroRotator);
				UNetActorComponent* netActor = actor->FindComponentByClass<UNetActorComponent>();
				netActor->SetGlobalID(netActor->GlobalIDFromPacket(data));
				netActor->FromPacket(data);
			}

		}
		//if message relates to the losing of hp 
		else if (data.Contains("Id:;")) 
		{
			//loop gameobjects and change hp of the one that matches the id sent
			for (UNetActorComponent* netObject : ANetManager::localNetObjects) {
				if (netObject->GetGlobalID() == netObject->GlobalIDFromPacket(data) || netObject->GetGlobalID() == 0) {
					
					netObject->ChangeHp(data);					
				}
			}
		}
	}

	
}

//send message function
bool ANetManager::sendMessage(FString Message)
{

	//send message trough the socket
	if (!Socket) return false;
	int32 BytesSent;

	FTimespan waitTime = FTimespan(10);
	//this is where we create the packet, in this case by serialising the character array
	TCHAR* serializedChar = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(serializedChar);

	bool success = Socket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndpoint.ToInternetAddr());
	UE_LOG(LogTemp, Warning, TEXT("Sent message: %s : %s : Address - %s : BytesSent - %d"), *Message, (success ? TEXT("true") : TEXT("false")), *RemoteEndpoint.ToString(), BytesSent);
	UE_LOG(LogTemp, Warning, TEXT("count: %d"), ANetManager::localNetObjects.Num());
	

	if (success && BytesSent > 0) return true;
	else return false;
}

void ANetManager::messageQueue()
{
}


