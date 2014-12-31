#pragma once
#include "FPSWeapon.h"
#include "FPSWeapon_Hitscan.generated.h"

/**
 * 
 */


USTRUCT()
struct FInstantHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	float ReticleSpread;

	UPROPERTY()
	int32 RandomSeed;
};

USTRUCT()
struct FInstantWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float WeaponSpread;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float TargetingSpreadMod;

	UPROPERTY(EditDefaultsOnly, Category = Accuracy)
		float FiringSpreadMax;

	UPROPERTY(EditDefaultsOnly, Category=Accuracy)
		float FiringSpreadIncrement;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float WeaponRange;

	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		int32 HitDamage;
	
	UPROPERTY(EditDefaultsOnly, Category = HitVerification)
		float ClientSideHitLeeway;

	UPROPERTY(EditDefaultsOnly, Category = HitVerification)
		float AllowedDotHitDir;

	//Default values for raycast weapons
	FInstantWeaponData()
	{
		WeaponSpread = 5.0f;
		TargetingSpreadMod = 0.25f;
		FiringSpreadIncrement = 1.0f;
		FiringSpreadMax = 10.0f;
		WeaponRange = 10000.0f;
		HitDamage = 10;
		ClientSideHitLeeway = 2.0f;
		AllowedDotHitDir = 0.8f;
	}

};


UCLASS()
class FPSPROJECT_API AFPSWeapon_Hitscan : public AFPSWeapon
{
	GENERATED_UCLASS_BODY()

	float GetWeaponSpread() const;

protected:
/** instant hit notify for replication */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_HitNotify)
	FInstantHitInfo HitNotify;

	/** current spread from continuous firing */
	float CurrentFiringSpread;

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** server notified of hit from client to verify */
	UFUNCTION(reliable, server, WithValidation)
	void ServerNotifyHit(const FHitResult Impact, FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category=Config)
		FInstantWeaponData InstantConfig;

	/** server notified of miss to show trail FX */
	UFUNCTION(unreliable, server, WithValidation)
	void ServerNotifyMiss(FVector_NetQuantizeNormal ShootDir, int32 RandomSeed, float ReticleSpread);

	/** process the instant hit and notify the server if necessary */
	void ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	/** continue processing the instant hit, as if it has been confirmed by the server */
	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);

	/** check if weapon should deal damage to actor */
	bool ShouldDealDamage(AActor* TestActor) const;

	/** handle damage */
	void DealDamage(const FHitResult& Impact, const FVector& ShootDir);

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() override;

	/** [local + server] update spread on firing */
	virtual void OnBurstFinished() override;

	UFUNCTION()
	void OnRep_HitNotify();

	/** called in network play to do the cosmetic fx  */
	void SimulateInstantHit(const FVector& Origin, int32 RandomSeed, float ReticleSpread);

private:
	
};
