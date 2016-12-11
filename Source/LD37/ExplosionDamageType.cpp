// Fill out your copyright notice in the Description page of Project Settings.

#include "LD37.h"
#include "ExplosionDamageType.h"

UExplosionDamageType::UExplosionDamageType()
{
	bCausedByWorld = false;
	bScaleMomentumByMass = true;
	bRadialDamageVelChange = true;
	DamageImpulse = 100000;
	DestructibleImpulse = 100000;
	DestructibleDamageSpreadScale = 1;
	DamageFalloff = 1;
}


