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
	
}

// Called every frame
void AToybox::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	SpawnCharge += DeltaTime;

	if (SpawnCharge > 0.5f)
	{
		int32 gruntsUp = 0;
		int32 officersUp = 0;

		for (TActorIterator<ALD37Character> i(GetWorld()); i; ++i)
		{
			if (i->Team == Team)
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

		FVector deltaToCenter = FVector(0, 0, 0) - SpawnPoint->GetComponentLocation();
		deltaToCenter.Normalize();

		FVector launchVel = deltaToCenter;
		launchVel.Z = 3;
		launchVel *= 2000;

		UE_LOG(LogTemp, Display, TEXT("Launch=%s"), *launchVel.ToString());

		while (officersUp < MinOfficers)
		{
			auto officer = GetWorld()->SpawnActor<ALD37Character>(OfficerType, SpawnPoint->GetComponentLocation(), SpawnPoint->GetComponentRotation());
			if (officer) {
				officer->SpawnDefaultController();
				officer->LaunchCharacter(launchVel, true, true);
			}
			officersUp++;
		}

		SpawnCharge = 0;
	}
}

