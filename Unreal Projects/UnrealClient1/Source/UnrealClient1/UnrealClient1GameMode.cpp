// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealClient1GameMode.h"
#include "UnrealClient1Character.h"
#include "UObject/ConstructorHelpers.h"

AUnrealClient1GameMode::AUnrealClient1GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
