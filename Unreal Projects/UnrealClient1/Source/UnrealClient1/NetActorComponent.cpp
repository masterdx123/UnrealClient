// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActorComponent.h"
#include "NetManager.h"

int32 UNetActorComponent::lastLocalID = 1;

int32 UNetActorComponent::GetGlobalID() {
	return globalID;
}

int32 UNetActorComponent::GetLocalID() {
	return localID;
}

void UNetActorComponent::SetGlobalID(int32 gid) {
	globalID = gid;
}

void UNetActorComponent::SetLocalID(int32 lid) {
	localID = lid;
}


// Sets default values for this component's properties
UNetActorComponent::UNetActorComponent()
{
	globalID = 0;
	localID = 0; 
	HP = 100;
	isLocallyOwned = false;
	// Set this component to be initialized when the game starts, and to be ticked every frame. 

	PrimaryComponentTick.bCanEverTick = true;

}

bool UNetActorComponent::GetIsLocallyOwned()
{
	return isLocallyOwned;
}


// Called when the game starts
void UNetActorComponent::BeginPlay()
{
	Super::BeginPlay();
	if (isLocallyOwned) {  //get the local ID
		localID = lastLocalID;
		lastLocalID++;
	}

	ANetManager::singleton->AddNetObject(this); //calls the above method on the singleton network manager

}


// Called every frame
void UNetActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int UNetActorComponent::GetCharaterHp()
{
	return HP;
}

//information to be sent to the server containing the object data
FString UNetActorComponent::ToPacket() {
	AActor* parentActor = GetOwner();
	FVector position = parentActor->GetActorLocation();
	FRotator rotation = parentActor->GetActorRotation(); //unreal uses euler angles..
	FQuat quaternionRotation = FQuat(rotation); //so we have to convert to Quaternion for Unity consistence

	FString returnVal = FString::Printf(TEXT("Object data;%i;%f;%f;%f;%f;%f;%f;%f;%i;"), globalID,
		position.X,
		position.Y,
		position.Z,
		quaternionRotation.X,
		quaternionRotation.Y,
		quaternionRotation.Z,
		quaternionRotation.W,
		HP
	);
	return returnVal;
}

//Get id from the server
int32 UNetActorComponent::GlobalIDFromPacket(FString packet) {
	TArray<FString> parsed;
	packet.ParseIntoArray(parsed, TEXT(";"), false);
	return FCString::Atoi(*parsed[1]);
}

//Get information on gameobject from the server and update the current information
void UNetActorComponent::FromPacket(FString packet) {
	packet = packet.Replace(TEXT(","), TEXT("."));
	TArray<FString> parsed;
	packet.ParseIntoArray(parsed, TEXT(";"), false);

	if (parsed.Num() == 11)
	{
		//*parsed[3].Replace(TEXT(","), TEXT("."));
		AActor* parentActor = GetOwner();
		FVector position = FVector(FCString::Atof(*parsed[2]), FCString::Atof(*parsed[3]), FCString::Atof(*parsed[4]));
		FQuat rotation = FQuat(FCString::Atof(*parsed[5]), FCString::Atof(*parsed[6]), FCString::Atof(*parsed[7]), FCString::Atof(*parsed[8]));
		parentActor->SetActorLocation(position);
		parentActor->SetActorRotation(rotation);
	}
	else //if the packet received doesnt match show its bad data
	{
		UE_LOG(LogTemp, Warning, TEXT("bad packet data recieved"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Bad Packet Data REceived: ");
	}
	
	
}

//cahnge the hp of the object when taking damage
void UNetActorComponent::ChangeHp(FString packet)
{
	TArray<FString> parsed;
	packet.ParseIntoArray(parsed, TEXT(";"), false);
	int dmg = FCString::Atoi(*parsed[2]);
	HP -= dmg;
	
}
;


