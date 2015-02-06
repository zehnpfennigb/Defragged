

#pragma once

#include "DFWeapon.generated.h"
/**
 * 
 */

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Equipping,
	};
}

USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

		// max ammo
		UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 MaxAmmo;

	//time between two consecutive shots
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float TimeBetweenShots;

	//defaults
	FWeaponData()
	{
		MaxAmmo = 100;
		TimeBetweenShots = 0.5f;
	}
};

USTRUCT()
struct FWeaponAnim
{
	GENERATED_USTRUCT_BODY()

		// animation played in 1st person view
		UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Pawn1P;

	// animation played in 3rd person view
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* Pawn3P;
};

UCLASS(Abstract, Blueprintable)
class ADFWeapon : public AActor
{
	GENERATED_UCLASS_BODY()

	// perform initial setup
	virtual void PostInitializeComponents() override;

	virtual void Destroyed() override;

	//Inventory

	// weapon is being equipped by owner pawn
	virtual void OnEquip();

	// weapon is now equipped
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(ADFCharacter* NewOwner);

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory();

	// check if it's currently equipped
	bool IsEquipped() const;

	// check if mesh is already attached
	bool IsAttachedToPawn() const;

	/////////////
	//Ammo

	enum class EAmmoType
	{
		EBullet,
		EMax,
	};

	//server add ammo
	void GiveAmmo(int AddAmount);

	//consume a bullet
	void UseAmmo();

	//query ammo type
	virtual EAmmoType GetAmmoType() const
	{
		return EAmmoType::EBullet;
	}
	//////////
	//Input

	//[local + server] start weapon fire
	virtual void StartFire();

	//[local + server stop weapon fire
	virtual void StopFire();

	/////////
	//Control

	//check if weapon can fire
	bool CanFire() const;

	/////////
	// Reading data

	//get current weapon state
	EWeaponState::Type GetCurrentState() const;

	//get current ammo amount (total)
	int GetCurrentAmmo() const;

	//get max ammo amount
	int GetMaxAmmo() const;

	//get weapon mesh (needs pawn owner to determine variant)
	USkeletalMeshComponent* GetWeaponMesh() const;

	//get pawn owner
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class ADFCharacter* GetPawnOwner() const;

	//icon displayed on the HUD when weapon is equipped
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon WeaponIcon;

	//bullet icon used to draw current ammo
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon AmmoIcon;

	//how many icons to draw per clip
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		float IconCount;

	//defines spacing between ammo icons (left side)
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		int32 IconOffset;

	// set the weapon's owning pawn
	void SetOwningPawn(ADFCharacter* ADFCharacter);

protected:

	//pawn owner
	UPROPERTY(Transient, ReplicatedUsing=OnRep_MyPawn)
	class ADFCharacter* MyPawn;

	//weapon data
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FWeaponData WeaponConfig;

	//weapon mesh: 1st person
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<USkeletalMeshComponent> Mesh1P;

	// weapon mesh: 3rd person view 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		TSubobjectPtr<USkeletalMeshComponent> Mesh3P;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* MuzzleFX;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
		UParticleSystemComponent* MuzzlePSC;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim EquipAnim;

	/** fire animations */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		FWeaponAnim FireAnim;

	/** is muzzle FX looped? */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		uint32 bLoopedMuzzleFX : 1;

	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category = Animation)
		uint32 bLoopedFireAnim : 1;

	/** is fire animation playing? */
	uint32 bPlayingFireAnim : 1;

	/** is weapon currently equipped? */
	uint32 bIsEquipped : 1;

	/** is weapon fire active? */
	uint32 bWantsToFire : 1;

	/** is equip animation playing? */
	uint32 bPendingEquip : 1;

	/** weapon is refiring */
	uint32 bRefiring;

	/** current weapon state */
	EWeaponState::Type CurrentState;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;

	/** current total ammo */
	UPROPERTY(Transient, Replicated)
		int32 CurrentAmmo;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
		int32 BurstCounter;

	//////////////////////////////////////////////////////////////////////////
	// Input - server side

	UFUNCTION(reliable, server, WithValidation)
		void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
		void ServerStopFire();

	//UFUNCTION(reliable, server, WithValidation)
		//void ServerStartReload();

	//UFUNCTION(reliable, server, WithValidation)
		//void ServerStopReload();

	//////////////////////////////////////////////////////////////////////////
	// Replication & effects

	UFUNCTION()
		void OnRep_MyPawn();

	UFUNCTION()
		void OnRep_BurstCounter();

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AShooterWeapon::FireWeapon, );

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
		void ServerHandleFiring();

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();

	/** update weapon state */
	void SetWeaponState(EWeaponState::Type NewState);

	/** determine current weapon state */
	void DetermineWeaponState();

	//////////////////////////////////////////////////////////////////////////
	// Inventory?????

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage helpers

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* FireSound;

	/** play weapon animations */
	float PlayWeaponAnimation(const FWeaponAnim& Animation);

	/** stop playing weapon animations */
	void StopWeaponAnimation(const FWeaponAnim& Animation);

	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** Get the aim of the camera */
	FVector GetCameraAim() const;

	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
};