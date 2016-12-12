// Fill out your copyright notice in the Description page of Project Settings.

#include "LD37.h"
#include "SoldierAIController.h"
#include "LD37Character.h"
#include "EngineUtils.h"
#include "Toybox.h"
#include "DrawDebugHelpers.h"

void ASoldierAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	ALD37Character* chr = Cast<ALD37Character>(GetPawn());

	if (AggroedOn)
	{
		if (auto c = Cast<ALD37Character>(AggroedOn)) if (c->Health <= 0) StopAggro();
		if (auto c = Cast<AToybox>(AggroedOn)) if (c->Health <= 0) StopAggro();
	}

	if (chr)
	{
		if (chr->GetVelocity().Size() < 100)
		{
			PathCharge += deltaTime;

			if (PathCharge > 1)
			{
				StuckTime += PathCharge;
				PathCharge = 0;

				//UE_LOG(LogTemp, Display, TEXT("StuckTime=%s"), *FString::SanitizeFloat(StuckTime));

				if (!AggroedOn)
				{
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

					auto pathResult = MoveToLocation(centerPoint, 200);

					if (pathResult == EPathFollowingRequestResult::AlreadyAtGoal) StuckTime = 0;
				}
				else
				{
					Aggro(AggroedOn);
				}
			}
		}
		else
		{
			StuckTime = 0;
			//chr->StopJumping();
		}

		if (StuckTime > 2)
		{
			//chr->Jump();
			//chr->AddMovementInput(chr->GetActorForwardVector(), 1);

			FVector jump = chr->GetActorForwardVector();
			jump.Normalize();
			jump.Z = 2;
			jump *= 1200;

			chr->LaunchCharacter(jump, false, false);
		}

		if (StuckTime > 5)
		{
			chr->TakeDamage(1000, FDamageEvent(), nullptr, nullptr);
		}

		//DrawDebugString(GetWorld(), chr->GetActorLocation(), *FString::SanitizeFloat(StuckTime), nullptr, FColor::Red, deltaTime, true);

		ScanCharge += deltaTime;

		if (ScanCharge > 2)
		{
			TArray<FOverlapResult> res;

			if (GetWorld()->OverlapMultiByObjectType(res, chr->GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(3000)))
			{
				for (auto a : res)
				{
					if (auto a2 = Cast<ALD37Character>(a.Actor.Get()))
					{
						if (a2->Health > 0)
						{
							float aggroDistSquared = 9999999999;
							if (AggroedOn) aggroDistSquared = FVector::DistSquared(chr->GetActorLocation(), AggroedOn->GetActorLocation());

							float distSquared = FVector::DistSquared(chr->GetActorLocation(), a2->GetActorLocation());

							if (distSquared < aggroDistSquared)
							{
								FCollisionQueryParams params;
								params.AddIgnoredActor(chr);
								params.AddIgnoredActor(a2);

								if (a2->Team != chr->Team && !GetWorld()->LineTraceTestByChannel(chr->GetActorLocation() + FVector(0, 0, 150), a2->GetActorLocation() + FVector(0, 0, 150), ECollisionChannel::ECC_Visibility, params))
								{
									Aggro(a2);
								}
							}
						}
					}
				}

				if (!AggroedOn)
				{
					for (auto a : res)
					{
						if (auto a2 = Cast<AToybox>(a.Actor.Get()))
						{
							if (a2->Team != chr->Team)
							{
								Aggro(a2);
								break;
							}
						}
					}
				}
			}
		}

		if (AggroedOn)
		{
			int32 bestWeapon = 0;

			for (bestWeapon = 2; bestWeapon >= 0; --bestWeapon)
			{
				if (chr->HasWeapon[bestWeapon] && chr->AmmoCounts[chr->WeaponDescriptions[bestWeapon].AmmoType] > 0) break;
			}

			chr->SelectWeapon(bestWeapon);

			SetControlRotation((AggroedOn->GetActorLocation() - chr->GetActorLocation()).ToOrientationRotator() + FRotator(FMath::FRandRange(-1, 1), FMath::FRandRange(-1, 1), FMath::FRandRange(-1, 1)));
			chr->OnStartFire();
			chr->ShotsFired = 0;
		}
		else
		{
			chr->OnStopFire();
		}
	}
}

void ASoldierAIController::Aggro(AActor * target)
{
	AggroedOn = target;

	auto pathResult = MoveToActor(target, 2000);

	if (pathResult == EPathFollowingRequestResult::AlreadyAtGoal) StuckTime = 0;
}

void ASoldierAIController::StopAggro()
{
	AggroedOn = nullptr;
	StopMovement();
}
