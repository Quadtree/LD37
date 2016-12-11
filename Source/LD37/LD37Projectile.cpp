// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "LD37.h"
#include "LD37Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ExplosionDamageType.h"

ALD37Projectile::ALD37Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ALD37Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void ALD37Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (DamageRadiusOnHit < 3)
	{
		if (OtherActor)
		{
			OtherActor->TakeDamage(DamageOnHit, FDamageEvent(), GetInstigatorController(), this);
		}
	}
	else
	{
		UGameplayStatics::ApplyRadialDamage(this, DamageOnHit, GetActorLocation(), DamageRadiusOnHit, UExplosionDamageType::StaticClass(), TArray<AActor*>(), this, GetInstigatorController());

		/*TArray<FOverlapResult> res;

		FRadialDamageEvent evt;
		evt.Origin = GetActorLocation();
		evt.Params.BaseDamage = DamageOnHit;
		evt.Params.DamageFalloff = 1;
		evt.Params.InnerRadius = DamageRadiusOnHit / 2;
		evt.Params.MinimumDamage = DamageOnHit / 4;
		evt.Params.OuterRadius = DamageRadiusOnHit;

		if (GetWorld()->OverlapMultiByObjectType(res, GetActorLocation(), FQuat::Identity, FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(DamageRadiusOnHit)))
		{
			TMap<AActor*, TArray<UPrimitiveComponent*>> compsHit;

			for (auto a : res)
			{
				if (a.Actor.Get() && a.Component.Get())
				{
					if (!compsHit.Contains(a.Actor.Get())) compsHit.Add(a.Actor.Get(), TArray<UPrimitiveComponent*>());
					
					compsHit[a.Actor.Get()].Add(a.Component.Get());
				}
			}

			for (auto a : compsHit)
			{
				evt.ComponentHits.Empty();

				for (auto comp : a.Value)
				{
					FHitResult hr;
					hr.Actor = a.Key;
					hr.bBlockingHit = true;
					hr.Component = comp;
					hr.ImpactPoint = comp->GetComponentLocation();
					hr.ImpactNormal = (comp->GetComponentLocation() - GetActorLocation()).GetSafeNormal();

					evt.ComponentHits.Add(hr);
				}

				a.Key->TakeDamage(DamageOnHit, evt, GetInstigatorController(), this);

				UE_LOG(LogTemp, Display, TEXT("Explosion hit %s"), *a.Key->GetName());
			}
		}*/
	}

	Destroy();
}