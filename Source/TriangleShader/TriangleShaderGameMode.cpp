// Copyright Epic Games, Inc. All Rights Reserved.

#include "TriangleShaderGameMode.h"
#include "TriangleShaderCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATriangleShaderGameMode::ATriangleShaderGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
