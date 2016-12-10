// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "SoldierAIController.generated.h"

/**
 * 
 */
UCLASS()
class LD37_API ASoldierAIController : public AAIController
{
	GENERATED_BODY()

	void Tick(float deltaTime);
	
	float StuckTime;
};
