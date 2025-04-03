// Copyright Epic Games, Inc. All Rights Reserved.

#include "SocketPythonGameMode.h"
#include "SocketPythonCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASocketPythonGameMode::ASocketPythonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
