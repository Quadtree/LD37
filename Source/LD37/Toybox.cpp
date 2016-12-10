// Fill out your copyright notice in the Description page of Project Settings.

#include "LD37.h"
#include "Toybox.h"
#include "EngineUtils.h"
#include "LD37Character.h"


// Sets default values
AToybox::AToybox(const FObjectInitializer& i)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = i.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	RootComponent = StaticMesh;

	SpawnPoint = i.CreateDefaultSubobject<USceneComponent>(this, TEXT("SpawnPoint"));
	SpawnPoint->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AToybox::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnCharge = 1000;
}

// Called every frame
void AToybox::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	SpawnCharge += DeltaTime;

	if (SpawnCharge > 3.f)
	{
		int32 gruntsUp = 0;
		int32 officersUp = 0;

		for (TActorIterator<ALD37Character> i(GetWorld()); i; ++i)
		{
			if (i->Team == Team && i->Health > 0)
			{
				if (i->GetClass()->IsChildOf(OfficerType))
				{
					officersUp++;
				}
				else
				{
					gruntsUp++;
				}
			}
		}

		//UE_LOG(LogTemp, Display, TEXT("Launch=%s"), *launchVel.ToString());

		while (officersUp < MinOfficers)
		{
			auto officer = SpawnToy(OfficerType);

			if (officer && Team == 0)
			{
				for (TActorIterator<APlayerController> i(GetWorld()); i; ++i)
				{
					if (!Cast<ALD37Character>(i->GetPawn()) || Cast<ALD37Character>(i->GetPawn())->Health <= 0)
					{
						i->Possess(officer);
					}
				}
			}

			officersUp++;
		}

		while (gruntsUp < MinGrunts)
		{
			SpawnToy(GruntType);
			gruntsUp++;
		}

		SpawnCharge = 0;
	}
}

ALD37Character* AToybox::SpawnToy(TSubclassOf<class ALD37Character> type)
{
	FVector deltaToCenter = FVector(0, 0, 0) - SpawnPoint->GetComponentLocation();
	deltaToCenter.Normalize();

	FVector launchVel = deltaToCenter;
	launchVel.Z = 3;
	launchVel *= 2000;

	auto officer = GetWorld()->SpawnActor<ALD37Character>(type, SpawnPoint->GetComponentLocation() + FMath::RandPointInBox(FBox(FVector(-100, -100, 0), FVector(100, 100, 0))), SpawnPoint->GetComponentRotation());
	if (officer)
	{
		officer->SpawnDefaultController();
		officer->LaunchCharacter(launchVel + FMath::RandPointInBox(FBox(FVector(-100, -100, -100), FVector(100, 100, 100))), true, true);
		officer->SetTeam(Team);
	}

	return officer;
}

