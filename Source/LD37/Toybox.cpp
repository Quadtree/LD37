// Fill out your copyright notice in the Description page of Project Settings.

#include "LD37.h"
#include "Toybox.h"


// Sets default values
AToybox::AToybox(const FObjectInitializer& i)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = i.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMesh"));
	RootComponent = StaticMesh;
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

}

