

#include "FPSProject.h"
#include "FPSCharacter.h"
#include "FPSProjectile.h"
#include "HealthPickup.h"


AFPSCharacter::AFPSCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    
    // set the health of the character
	MaxHealth = 100.f;
	isDead = false;
	//REPLICATION TEST
	bWantstoFire = false;
    
	/*
    //Create our health collection volume
    CollectionSphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollectionSphere"));
    CollectionSphere->AttachTo(RootComponent);
    CollectionSphere->SetSphereRadius(200.f);
    */
    // create a camera component
    FirstPersonCameraComponent = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->AttachParent = CapsuleComponent;
    
    // position the camrea a bit above the eyes
    FirstPersonCameraComponent->RelativeLocation = FVector(0,0,50.0f + BaseEyeHeight);
    
    // Create a mesh component that will be used when being viewed from a 1st person view
    FirstPersonMesh = PCIP.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("SkeletalMesh'/Game/Characters/FPP/HeroFPPBlue.HeroFPPBlue'"));
    FirstPersonMesh->SetOnlyOwnerSee(true);
    FirstPersonMesh->AttachParent = FirstPersonCameraComponent;
    FirstPersonMesh->bCastDynamicShadow = false;
    FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->SetCollisionObjectType(ECC_Pawn);
	FirstPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FirstPersonMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

    // Everyone but the owner can see the regular body mesh
    Mesh->SetOwnerNoSee(true);
	Mesh->SetCollisionObjectType(ECC_Pawn);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	CapsuleComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
}

void AFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		Health = GetMaxHealth();
	}
	UpdateTeamColors();
	SpawnDefaultInventory();
}

void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AFPSCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	// [server] as soon as Controller is assigned, set team colors of this pawn for local player
	UpdateTeamColors();
}

void AFPSCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local player
	if (PlayerState != NULL)
	{
		UpdateTeamColors();
	}
}

void AFPSCharacter::OnRep_CurrentWeapon(AFPSWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void AFPSCharacter::UpdateTeamColors()
{
	AFPSPlayerState* MyPlayerState = Cast<AFPSPlayerState>(PlayerState);

	if (MyPlayerState != NULL)
	{
		float Team = (float)MyPlayerState->GetTeamNum();
		if (Team == 0)
		{
			//FirstPersonMesh->SetMaterial(0, Red);
			//Mesh->SetMaterial(0, Red);
			//Mesh->SetMaterial(1, Red);
		}
		else if (Team == 1)
		{
			//FirstPersonMesh->SetMaterial(0, Blue);
			//Mesh->SetMaterial(0, Blue);
			//Mesh->SetMaterial(1, Blue);
		}
	}
}

FName AFPSCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

USkeletalMeshComponent* AFPSCharacter::GetPawnMesh() const
{
	return IsFirstPerson() ? FirstPersonMesh : Mesh;
}

bool AFPSCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

bool AFPSCharacter::IsAlive() const
{
	return Health > 0;
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicate to everyone but owner
	DOREPLIFETIME_CONDITION(AFPSCharacter, bWantstoFire, COND_SkipOwner);

	// Replicate to everyone
	DOREPLIFETIME(AFPSCharacter, CurrentWeapon);
	DOREPLIFETIME(AFPSCharacter, Health);
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    //set up gameplay key bindings
    InputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
    InputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
    InputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);
    InputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::OnStartJump);
    InputComponent->BindAction("Jump", IE_Released, this, &AFPSCharacter::OnStopJump);
    InputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AFPSCharacter::OnStopFire);
}

void AFPSCharacter::MoveForward(float Value)
{
    if((Controller != NULL)&&(Value != 0.0f))
    {
        //find out which way is forward
        FRotator Rotation = Controller->GetControlRotation();
        //Limit pitch when walking or falling
        if(CharacterMovement->IsMovingOnGround() || CharacterMovement->IsFalling())
        {
            Rotation.Pitch = 0.0f;
        }
        //add movement in that direction
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AFPSCharacter::MoveRight(float Value)
{
    if((Controller != NULL) && (Value != 0.0f))
    {
        //find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        //add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

/** DEATH AND RESPAWNING */

USkeletalMeshComponent* AFPSCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? FirstPersonMesh : Mesh;
}

float AFPSCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Modify based on game rules.
	AFPSGameMode* const Game = GetWorld()->GetAuthGameMode<AFPSGameMode>();
	Damage = Game ? Game->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : 4.f;

	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			/*Used to generate an on hit effect on the player*/
			//PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		//MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}

	return ActualDamage;
}

//Checks to see if the player is killable
bool AFPSCharacter::CanDie()
{
	if (isDead)
		return false;
	return true;
}

//If the player is killable update net frequence and force replication, then kill the player.
bool AFPSCharacter::Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser)
{
	if (!CanDie())
		return false;

	Health = FMath::Min(0.0f, Health);
	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<AFPSTeamDeathMatch>()->Killed(Killer, KilledPlayer, this);

	NetUpdateFrequency = GetDefault<AFPSCharacter>()->NetUpdateFrequency;
	CharacterMovement->ForceReplicationUpdate();
	isDead = true;
	OnDeath();
	return true;
}

//Kills the player.
void AFPSCharacter::OnDeath()
{
	//Set health = 0
	Health = 0;

	//Detach the Pawn from the Controller
	DetachFromControllerPendingDestroy();

	//Disable collisions on capsule component
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

//	CollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//	CollectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	if (Mesh)
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		Mesh->SetCollisionProfileName(CollisionProfileName);
	}

	SetActorEnableCollision(true);

	//Set physics to ragdoll (if time)
	//Do this over an intervial of time

	float DeathAnimDuration = 10.0f;

	if (DeathAnimDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(this, &AFPSCharacter::SetRagdoll, FMath::Min(0.1f, DeathAnimDuration), false);
	}
	else
	{
		SetRagdoll();
	}

	//Delete Weapons and Inventory

	//Disable player movement
	bReplicateMovement = false;

	//Stop any animations
}

//Applys Ragdoll Physics - MUST HAVE PHYSICS ENABLED
void AFPSCharacter::SetRagdoll()
{
	bool bInRagdoll = false;

	Mesh->SetAllBodiesSimulatePhysics(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();
	Mesh->bBlendPhysics = true;
	bInRagdoll = true;

	CharacterMovement->StopMovementImmediately();
	CharacterMovement->DisableMovement();
	CharacterMovement->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(3.0);
	}
	else
	{
		SetLifeSpan(0.3);
	}
}

void AFPSCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	//Restart with weapon
	SetCurrentWeapon(CurrentWeapon);
}

//Destroys the pawn
void AFPSCharacter::Destroyed()
{
	Super::Destroyed();
}

void AFPSCharacter::OnStartJump()
{
    bPressedJump = true;
}

void AFPSCharacter::OnStopJump()
{
    bPressedJump = false;
}


/*PLAYER SHOOTING */

AFPSWeapon* AFPSCharacter::GetWeapon() const
{
	return CurrentWeapon;
}

void AFPSCharacter::SpawnDefaultInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	//int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	//for (int32 i = 0; i < NumWeaponClasses; i++)
	//{
	if (DefaultInventoryClasses[0])
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoCollisionFail = true;
		AFPSWeapon* NewWeapon = GetWorld()->SpawnActor<AFPSWeapon>(DefaultInventoryClasses[0], SpawnInfo);
		AddWeapon(NewWeapon);
	}
	//}
	EquipWeapon(Inventory[0]);
}

void AFPSCharacter::DestroyInventory()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (Inventory[0])
	{
		RemoveWeapon(Inventory[0]);
		Inventory[0]->Destroy();
	}
}

void AFPSCharacter::AddWeapon(AFPSWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void AFPSCharacter::RemoveWeapon(AFPSWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

void AFPSCharacter::SetCurrentWeapon(class AFPSWeapon* NewWeapon, class AFPSWeapon* LastWeapon)
{
	AFPSWeapon* LocalLastWeapon = NULL;

	if (LastWeapon != NULL)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}
	// unequip previous below
	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		NewWeapon->OnEquip();
	}
}

void AFPSCharacter::EquipWeapon(AFPSWeapon* Weapon)
{
	if (Weapon)
	{
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

bool AFPSCharacter::ServerEquipWeapon_Validate(class AFPSWeapon* NewWeapon)
{
	return true;
}

void AFPSCharacter::ServerEquipWeapon_Implementation(class AFPSWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void AFPSCharacter::OnStartFire(){
	AFPSPlayerController* MyPC = Cast<AFPSPlayerController>(Controller);
	if (MyPC)
	{
		StartWeaponFire();
	}
}

void AFPSCharacter::OnStopFire(){
	StopWeaponFire();
}

/** [local] starts weapon fire */
void AFPSCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}

/** [local] stops weapon fire */
void AFPSCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

bool AFPSCharacter::IsFiring() const
{
	return bWantstoFire;
}

//Called once the player character uses LMB

/**OLD CODE FROM THE PROJECTILE BASED GUN HAD IN THE PROTOTYPE */
void AFPSCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		// Get the camera transform
		FVector CameraLoc;
		FRotator CameraRot;
		GetActorEyesViewPoint(CameraLoc, CameraRot);

		//MuzzleOffset is in camera space, so transform it to world space before offsetting from the camera to find the final muzzle position
		FVector const MuzzleLocation = CameraLoc + FTransform(CameraRot).TransformFVector4(MuzzleOffset);     //TransformFVector(MuzzleOffset);
		FRotator MuzzleRotation = CameraRot;
		MuzzleRotation.Pitch += 10.0f; //skew the aim upwards a bit

		if (Role < ROLE_Authority)
			ServerStartFire(MuzzleLocation, MuzzleRotation);
	}
}

//Any validation logic will go here, perhaps verify if the player has ammo to shoot.
bool AFPSCharacter::ServerStartFire_Validate(FVector location, FRotator ShootDir)
{
	return true;
}

//Presents the logic to spawn a bullet from the players muzzle
void AFPSCharacter::ServerStartFire_Implementation(FVector location, FRotator ShootDir)
{
	FTransform SpawnTM(ShootDir, location);
	UWorld* const World = GetWorld();
	//spawn th projectile at the muzzle
	AFPSProjectile* Projectile = Cast<AFPSProjectile>(UGameplayStatics::BeginSpawningActorFromClass(World, ProjectileClass, SpawnTM));
	if (Projectile)
	{
		//find launch direction
		Projectile->SetOwner(this);
		Projectile->Instigator = Instigator;
		Projectile->InitVelocity(ShootDir.Vector());
	}
}

bool AFPSCharacter::CanFire() const
{
	return IsAlive();
}
/**END PROTOTYPE CODE */


int32 AFPSCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<AFPSCharacter>()->MaxHealth;
}

void AFPSCharacter::CollectHealth()
{
    float HealthAmount = 0.f;
    
    //Get overlapping Actors and store them in a CollectedActors array
    TArray<AActor*> CollectedActors;
//    CollectionSphere->GetOverlappingActors(CollectedActors);
    
    //For each Actor collected
    for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
    {
        // Cast the collected Actor to AHealthPickup
        AHealthPickup* const TestHealth = Cast<AHealthPickup>(CollectedActors[iCollected]);
        
        //if the cast is successful and the health pickup is valid and active
        if (TestHealth && !TestHealth->IsPendingKill() && TestHealth->bIsActive)
        {
            //Store the amount for adding to the character's health
            Health = Health + TestHealth->HealthAmount;
            
            //Restore some health of the Character NEED TO DO
            TestHealth->OnPickedUp();
            
            //Deactivate the HealthPack
            TestHealth->bIsActive = false;
        }
    }
    
    if (HealthAmount > 0.f)
    {
        
    }
        
}