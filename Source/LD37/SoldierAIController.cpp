// Fill out your copyright notice in the Description page of Project Settings.

#include "LD37.h"
#include "SoldierAIController.h"
#include "LD37Character.h"
#include "EngineUtils.h"
#include "Toybox.h"

void ASoldierAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	ALD37Character* chr = Cast<ALD37Character>(GetPawn());

	if (chr)
	{
		if (chr->GetVelocity().Size() < 100)
		{
			StuckTime += deltaTime;

			//UE_LOG(LogTemp, Display, TEXT("StuckTime=%s"), *FString::SanitizeFloat(StuckTime));

			FVector enemyToyboxLocation = FVector(0, 0, 0);

			for (TActorIterator<AToybox> i(GetWorld()); i; ++i)
			{
				if (i->Team != chr->Team)
				{
					enemyToyboxLocation = i->GetActorLocation();
					break;
				}
			}

			FVector centerPoint = (chr->GetActorLocation() + enemyToyboxLocation) / 2;
			centerPoint.Z = chr->GetActorLocation().Z;
			centerPoint += FMath::RandPointInBox(FBox(FVector(-5000, -5000, 0), FVector(5000, 5000, 0)));

			MoveToLocation(centerPoint, 200);
		}
		else
		{
			StuckTime = 0;
		}

		if (StuckTime > 3)
		{
			chr->Destroy();
		}
	}
}
