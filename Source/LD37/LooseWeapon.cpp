// Fill out your copyright notice in the Description page of Project Settings.

#include "LD37.h"
#include "LD37Character.h"
#include "LooseWeapon.h"


// Sets default values
ALooseWeapon::ALooseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	Root->SetRelativeScale3D(FVector(0.056642f));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Root);
	Mesh->SetNotifyRigidBodyCollision(true);

	Mesh->OnComponentHit.AddDynamic(this, &ALooseWeapon::OnHit);

	Mesh->SetSimulatePhysics(true);

	

	RootComponent = Root;

	LifeSpan = 99999;
}

// Called when the game starts or when spawned
void ALooseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALooseWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	LifeSpan -= DeltaTime;

	if (LifeSpan <= 0) Destroy();
}

void ALooseWeapon::SetWeaponType(ALD37Character* chrRef, int32 weaponType)
{
	WeaponType = weaponType;

	Mesh->SetStaticMesh(chrRef->WeaponDescriptions[weaponType].GunModel);
	Mesh->SetMaterial(0, chrRef->WeaponMaterials[weaponType]);
}

void ALooseWeapon::OnHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (auto act = Cast<ALD37Character>(OtherActor))
	{
		if (act->Health > 0)
		{
			act->AmmoCounts[act->WeaponDescriptions[WeaponType].AmmoType] += Ammo;
			if (!act->HasWeapon[WeaponType])
			{
				act->HasWeapon[WeaponType] = true;
				act->WeaponMaterials[WeaponType] = Mesh->GetMaterial(0);
			}
			
			Destroy();
		}
	}
}

