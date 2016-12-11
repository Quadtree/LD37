// Fill out your copyright notice in the Description page of Project Settings.

#include "LD37.h"
#include "Furniture.h"


// Sets default values
AFurniture::AFurniture()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFurniture::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFurniture::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

float AFurniture::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		UE_LOG(LogTemp, Display, TEXT("%s took radial damage"), *GetName());

		const FRadialDamageEvent* evt = (FRadialDamageEvent*) &DamageEvent;

		for (auto a : GetComponentsByClass(UPrimitiveComponent::StaticClass()))
		{
			if (auto a2 = Cast<UPrimitiveComponent>(a))
			{
				float dist = FVector::Dist(a2->GetComponentLocation(), evt->Origin);
				float actualDamage = FMath::Clamp(1 - dist / evt->Params.OuterRadius, 0.f, 1.f) * DamageAmount;

				if (!CompDamage.Contains(a2)) CompDamage.Add(a2, 0);

				CompDamage[a2] += actualDamage;

				if (CompDamage[a2] > 3.f)
				{
					GoFlipping(a2, evt->Origin, actualDamage);

					TArray<FHitResult> hr;

					if (GetWorld()->LineTraceMultiByObjectType(hr, a2->GetComponentLocation(), a2->GetComponentLocation() + FVector(0, 0, 1000), FCollisionObjectQueryParams::AllObjects))
					{
						for (auto hit : hr)
						{
							if (hit.Component.Get() && hit.Component->GetOwner() == this)
							{
								GoFlipping(hit.Component.Get(), evt->Origin, 2);
							}
						}
					}
				}
			}
		}
	}

	return DamageAmount;
}

void AFurniture::GoFlipping(UPrimitiveComponent* comp, FVector origin, float actualDamage)
{
	comp->SetSimulatePhysics(true);

	FVector delta = (comp->GetComponentLocation() - origin);
	delta.Normalize();
	delta.Z = 2;
	delta *= 5000 * FMath::Min(actualDamage, 1.5f);

	comp->AddImpulse(delta, NAME_None, true);
}

