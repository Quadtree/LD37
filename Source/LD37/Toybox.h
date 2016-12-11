// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Toybox.generated.h"

UCLASS()
class LD37_API AToybox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AToybox(const FObjectInitializer& i);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditAnywhere, Category = Spawn)
	int32 MinOfficers;

	UPROPERTY(EditAnywhere, Category = Spawn)
	int32 MinGrunts;

	UPROPERTY(EditAnywhere, Category = Team)
	int32 Team;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Team)
	TSubclassOf<class ALD37Character> OfficerType;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Team)
	TSubclassOf<class ALD37Character> GruntType;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Spawn)
	class USceneComponent* SpawnPoint;

	UPROPERTY()
	float SpawnCharge;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Health)
	float Health;

	UPROPERTY(EditAnywhere, Category = Hit)
	class UParticleSystem* DeathExplosion;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
private:
	ALD37Character* SpawnToy(TSubclassOf<class ALD37Character> type);
};
