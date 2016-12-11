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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float ShotCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	bool FullyAutomatic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	bool InheritMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	bool GunInheritMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	class UStaticMesh* GunModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	int32 AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	int32 StartAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	float Spread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	class UTexture* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	class USoundBase* ShootSound;
};
