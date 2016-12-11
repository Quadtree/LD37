// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "LD37.h"
#include "LD37Character.h"
#include "LD37Projectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALD37Character

ALD37Character::ALD37Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->Hand = EControllerHand::Right;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// all soldiers start with the battle rifle
	HasWeapon.Add(true);
	HasWeapon.Add(false);
	HasWeapon.Add(false);
	HasWeapon.Add(false);

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void ALD37Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	if (FMath::FRand() <= 0.05f * Health) HasWeapon[1] = true;
	if (FMath::FRand() <= 0.02f * Health) HasWeapon[2] = true;
	if (FMath::FRand() <= 0.01f * Health) HasWeapon[3] = true;

	for (int32 i = 0; i < HasWeapon.Num(); ++i)
	{
		if (HasWeapon[i])
		{
			if (!AmmoCounts.Contains(WeaponDescriptions[i].AmmoType))
				AmmoCounts.Add(WeaponDescriptions[i].AmmoType, WeaponDescriptions[i].StartAmmo);
			else
				AmmoCounts[WeaponDescriptions[i].AmmoType] += WeaponDescriptions[i].StartAmmo;
		}
		else
		{
			if (!AmmoCounts.Contains(WeaponDescriptions[i].AmmoType)) AmmoCounts.Add(WeaponDescriptions[i].AmmoType, 0);
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("Team=%s"), *FString::FromInt(Team));
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALD37Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALD37Character::TouchStarted);
	if (EnableTouchscreenMovement(PlayerInputComponent) == false)
	{
		PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALD37Character::OnStartFire);
		PlayerInputComponent->BindAction("Fire", IE_Released, this, &ALD37Character::OnStopFire);
	}

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ALD37Character::OnResetVR);

	PlayerInputComponent->BindAction("SelectWeapon0", IE_Pressed, this, &ALD37Character::SelectWeapon0);
	PlayerInputComponent->BindAction("SelectWeapon1", IE_Pressed, this, &ALD37Character::SelectWeapon1);
	PlayerInputComponent->BindAction("SelectWeapon2", IE_Pressed, this, &ALD37Character::SelectWeapon2);
	PlayerInputComponent->BindAction("SelectWeapon3", IE_Pressed, this, &ALD37Character::SelectWeapon3);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALD37Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALD37Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALD37Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALD37Character::LookUpAtRate);
}

void ALD37Character::OnFire()
{
	if (!WeaponDescriptions[CurrentWeapon].FullyAutomatic && ShotsFired >= 1) return;

	if (ShotCharge < WeaponDescriptions[CurrentWeapon].ShotCooldown) return;

	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			AActor* newPrj = nullptr;

			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				newPrj = World->SpawnActor<ALD37Projectile>(WeaponDescriptions[CurrentWeapon].ProjectileType, SpawnLocation, SpawnRotation);
			}
			else
			{
				const float spread = WeaponDescriptions[CurrentWeapon].Spread;

				const FRotator SpawnRotation = GetControlRotation() + FRotator(FMath::FRandRange(-spread, spread), FMath::FRandRange(-spread, spread), FMath::FRandRange(-spread, spread));
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				newPrj = World->SpawnActor<ALD37Projectile>(WeaponDescriptions[CurrentWeapon].ProjectileType, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}

			if (newPrj && WeaponDescriptions[CurrentWeapon].InheritMaterial)
			{
				for (auto& a : newPrj->GetComponentsByClass(UPrimitiveComponent::StaticClass()))
				{
					if (auto a2 = Cast<UPrimitiveComponent>(a))
					{
						a2->SetMaterial(0, TeamMaterials[Team]);
					}
				}
			}

			ShotsFired++;
			ShotCharge = 0;
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void ALD37Character::OnStartFire()
{
	IsFiring = true;
	ShotsFired = 0;
}

void ALD37Character::OnStopFire()
{
	IsFiring = false;
}

void ALD37Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ALD37Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ALD37Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ALD37Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ALD37Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALD37Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALD37Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALD37Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ALD37Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALD37Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ALD37Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ALD37Character::TouchUpdate);
	}
	return bResult;
}

float ALD37Character::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float amt = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= amt;

	if (Health <= 0)
	{
		// @todo: Ragdoll and such here
		Destroy();
	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		UE_LOG(LogTemp, Display, TEXT("Knockback! %s"), *FString::SanitizeFloat(amt));
	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID) && amt > 2)
	{
		

		const FRadialDamageEvent* rde = (const FRadialDamageEvent*)&DamageEvent;

		FVector delta = GetActorLocation() - rde->Origin;
		delta.Normalize();

		delta.Z = 1.5f;

		delta *= amt * 600;

		LaunchCharacter(delta, false, false);
	}

	return amt;
}

void ALD37Character::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (IsFiring) OnFire();

	ShotCharge += deltaTime;
}

void ALD37Character::SelectWeapon0()
{
	SelectWeapon(0);
}

void ALD37Character::SelectWeapon1()
{
	SelectWeapon(1);
}

void ALD37Character::SelectWeapon2()
{
	SelectWeapon(2);
}

void ALD37Character::SelectWeapon3()
{
	SelectWeapon(3);
}

void ALD37Character::SelectWeapon(int32 num)
{
	CurrentWeapon = FMath::Clamp(num, 0, WeaponDescriptions.Num() - 1);

	// @todo: Change model
}

void ALD37Character::SetTeam(int32 team)
{
	Team = team;

	for (auto& a : GetComponentsByClass(UPrimitiveComponent::StaticClass()))
	{
		if (auto a2 = Cast<UPrimitiveComponent>(a))
		{
			a2->SetMaterial(0, TeamMaterials[Team]);
		}
	}
}
