// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameModeBase.h"
#include "LD37GameMode.generated.h"

UCLASS(minimalapi)
class ALD37GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALD37GameMode();

	void BeginPlay() override;
};



