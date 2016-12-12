// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "LD37.h"
#include "LD37GameMode.h"
#include "LD37HUD.h"
#include "LD37Character.h"

ALD37GameMode::ALD37GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALD37HUD::StaticClass();
}

void ALD37GameMode::BeginPlay()
{
	Super::BeginPlay();

#if PLATFORM_HTML5
	GEngine->GameUserSettings->ScalabilityQuality.AntiAliasingQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.EffectsQuality = 3;
	GEngine->GameUserSettings->ScalabilityQuality.PostProcessQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.ResolutionQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.TextureQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.ViewDistanceQuality = 0;
	GEngine->GameUserSettings->ScalabilityQuality.ShadowQuality = 0;
	GEngine->GameUserSettings->ApplySettings();
#endif
}
