// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LooseWeapon.generated.h"

UCLASS()
class LD37_API ALooseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALooseWeapon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, Category = Model)
	class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = Model)
	class UStaticMeshComponent* Mesh;
	
	UPROPERTY()
	int32 WeaponType;

	void SetWeaponType(class ALD37Character* chrRef, int32 weaponType);

	UPROPERTY()
	int32 Ammo;

	UPROPERTY()
	float LifeSpan;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
