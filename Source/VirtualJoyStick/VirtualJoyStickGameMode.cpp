// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "VirtualJoyStickGameMode.h"
#include "VirtualJoyStickCharacter.h"
#include "UObject/ConstructorHelpers.h"

AVirtualJoyStickGameMode::AVirtualJoyStickGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
