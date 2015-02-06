#pragma once

#include "GameFramework/Character.h"
#include "DFCharacter.generated.h"


/**
 * 
 */
UCLASS()
class DEFRAGGED_API ADFCharacter : public ACharacter
{

	GENERATED_UCLASS_BODY()

	virtual void PostInitializeComponents() override;

    virtual void BeginPlay() override;

    //handles moving forward/backward
    UFUNCTION()
    void MoveForward(float Val);
    
    //handles strafing
    UFUNCTION()
    void MoveRight(float Val);
    
    //First person camera
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    TSubobjectPtr<UCameraComponent> FirstPersonCameraComponent;
    
	/*
    //Collection Volume
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
    TSubobjectPtr<class USphereComponent> CollectionSphere;
    */
    //Health of the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Health)
    float Health;

	//Health of the character
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Health)
	float MaxHealth;

	//
    
    // handles firing
	UFUNCTION()
    void OnFire();
	
	//Fire projectile from server
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire(FVector location, FRotator ShootDir);

	//Player pressed reload
	void OnReload();

    //sets jump flag when key is pressed
    UFUNCTION()
    void OnStartJump();
    
    //clears jump flag when key is released
    UFUNCTION()
    void OnStopJump();

	/** player pressed start fire action */
	UFUNCTION()
	void OnStartFire();

	/** player released start fire action */
	UFUNCTION()
	void OnStopFire();

	/** [local] starts weapon fire */
	void StartWeaponFire();

	/** [local] stops weapon fire */
	void StopWeaponFire();

	// check if firing
	UPROPERTY(Replicated)
		bool bWantstoFire;

	//get firing state
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsFiring() const;

	/** get mesh component */
	USkeletalMeshComponent* GetPawnMesh() const;

	// get currently equipped weapon
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
	class ADFWeapon* GetWeapon() const;

	/** current firing state */
	uint8 bWantsToFire : 1;

	/*
		INVENTORY

	*/

	void SpawnDefaultInventory();

	void DestroyInventory();

	void AddWeapon(ADFWeapon* Weapon);

	void RemoveWeapon(ADFWeapon* Weapon);

	void EquipWeapon(ADFWeapon* Weapon);

	void SetCurrentWeapon(class ADFWeapon* NewWeapon, class ADFWeapon* LastWeapon = NULL);


	/*
		DEATH AND RESPAWNING
	*/

	//virtual void KilledBy();

	void SetRagdoll();

	virtual void Destroyed() override;

	virtual void PawnClientRestart() override;
	
	bool isDead;

	bool CanDie();

	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	void OnDeath();

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	// get weapon attach point
	FName GetWeaponAttachPoint() const;	
	
	// check if character is still alive
	bool IsAlive() const;

    // Gun muzzle's offset from the camera location
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
    FVector MuzzleOffset;
    
    // Projectile class to spawn
    UPROPERTY(EditDefaultsOnly, Replicated, Category=Projectile)
    TSubclassOf<class ADFProjectile> ProjectileClass;

	/** get max health */
	int32 GetMaxHealth() const;

	/** [server] perform PlayerState related setup */
	virtual void PossessedBy(class AController* C) override;

	/** [client] perform PlayerState related setup */
	virtual void OnRep_PlayerState() override;

	UPROPERTY()
		UMaterial* TheMaterialRed;
	UPROPERTY()
		UMaterial* TheMaterialBlue;

	/** handle mesh colors on specified material instance */
	void UpdateTeamColors();

	/*
	* Get either first or third person mesh.
	*
	* @param	WantFirstPerson		If true returns the first peron mesh, else returns the third
	*/
	USkeletalMeshComponent* GetSpecifcPawnMesh(bool WantFirstPerson) const;

	/** check if pawn can fire weapon */
	bool CanFire() const;

	/** get camera view type */
	UFUNCTION(BlueprintCallable, Category = Mesh)
		virtual bool IsFirstPerson() const;
    
protected:
	/** material instances for setting team color in mesh (3rd person view) */
	UPROPERTY(Transient)
		UMaterialInstanceDynamic* Blue;

	UPROPERTY(Transient)
		UMaterialInstanceDynamic* Red;

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//Pawn mesh: 1st person view
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	TSubobjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	// Socket for attaching weapon
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
		FName WeaponAttachPoint;

	// Currently equipped weapon

    // Called when we collide with a HealthPack inside the SphereComponent
    UFUNCTION(BlueprintCallable, Category = Health)
    void CollectHealth();

	/*INVENTORY AND WEAPONS*/
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class ADFWeapon* CurrentWeapon;

	UFUNCTION()
		void OnRep_CurrentWeapon(class ADFWeapon* LastWeapon);

	UPROPERTY(Transient, Replicated)
		TArray<class ADFWeapon*> Inventory;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<TSubclassOf<class ADFWeapon> > DefaultInventoryClasses;

	UFUNCTION(reliable, server, WithValidation)
		void ServerEquipWeapon(class ADFWeapon* NewWeapon);
};
