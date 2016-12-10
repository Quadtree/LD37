// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "WeaponDescription.generated.h"

/**
 * 
 */
USTRUCT()
struct LD37_API FWeaponDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Weapon)
	float ShotCooldown;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<AActor> ProjectileType;

	UPROPERTY(EditAnywhere, Category = Weapon)
	bool FullyAutomatic;
	
	UPROPERTY(EditAnywhere, Category = Weapon)
	bool InheritMaterial;

	UPROPERTY(EditAnywhere, Category = Weapon)
	class UStaticMesh* GunModel;
};
