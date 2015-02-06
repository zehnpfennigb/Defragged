

#include "Defragged.h"
#include "Player/DFCharacter.h"
#include "Weapons/DFProjectile.h"
#include "Pickup/DFPickup_Health.h"


ADFCharacter::ADFCharacter(const class FPostConstructInitializeProperties& PCIP)
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

void ADFCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Role == ROLE_Authority)
	{
		Health = GetMaxHealth();
	}
	UpdateTeamColors();
	SpawnDefaultInventory();
}

void ADFCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ADFCharacter::PossessedBy(class AController* InController)
{
	Super::PossessedBy(InController);

	// [server] as soon as Controller is assigned, set team colors of this pawn for local player
	UpdateTeamColors();
}

void ADFCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// [client] as soon as PlayerState is assigned, set team colors of this pawn for local player
	if (PlayerState != NULL)
	{
		UpdateTeamColors();
	}
}

void ADFCharacter::OnRep_CurrentWeapon(ADFWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ADFCharacter::UpdateTeamColors()
{
	ADFPlayerState* MyPlayerState = Cast<ADFPlayerState>(PlayerState);

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

FName ADFCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}

USkeletalMeshComponent* ADFCharacter::GetPawnMesh() const
{
	return IsFirstPerson() ? FirstPersonMesh : Mesh;
}

bool ADFCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

bool ADFCharacter::IsAlive() const
{
	return Health > 0;
}

void ADFCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Replicate to everyone but owner
	DOREPLIFETIME_CONDITION(ADFCharacter, bWantstoFire, COND_SkipOwner);

	// Replicate to everyone
	DOREPLIFETIME(ADFCharacter, CurrentWeapon);
	DOREPLIFETIME(ADFCharacter, Health);
}

void ADFCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    //set up gameplay key bindings
    InputComponent->BindAxis("MoveForward", this, &ADFCharacter::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &ADFCharacter::MoveRight);
    InputComponent->BindAxis("Turn", this, &ADFCharacter::AddControllerYawInput);
    InputComponent->BindAxis("LookUp", this, &ADFCharacter::AddControllerPitchInput);
    InputComponent->BindAction("Jump", IE_Pressed, this, &ADFCharacter::OnStartJump);
    InputComponent->BindAction("Jump", IE_Released, this, &ADFCharacter::OnStopJump);
    InputComponent->BindAction("Fire", IE_Pressed, this, &ADFCharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &ADFCharacter::OnStopFire);
}

void ADFCharacter::MoveForward(float Value)
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

void ADFCharacter::MoveRight(float Value)
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

USkeletalMeshComponent* ADFCharacter::GetSpecifcPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? FirstPersonMesh : Mesh;
}

float ADFCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}

	// Modify based on game rules.
	ADFGameMode* const Game = GetWorld()->GetAuthGameMode<ADFGameMode>();
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
bool ADFCharacter::CanDie()
{
	if (isDead)
		return false;
	return true;
}

//If the player is killable update net frequence and force replication, then kill the player.
bool ADFCharacter::Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser)
{
	if (!CanDie())
		return false;

	Health = FMath::Min(0.0f, Health);
	AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	GetWorld()->GetAuthGameMode<ADFTeamDeathMatch>()->Killed(Killer, KilledPlayer, this);

	NetUpdateFrequency = GetDefault<ADFCharacter>()->NetUpdateFrequency;
	CharacterMovement->ForceReplicationUpdate();
	isDead = true;
	OnDeath();
	return true;
}

//Kills the player.
void ADFCharacter::OnDeath()
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
		GetWorldTimerManager().SetTimer(this, &ADFCharacter::SetRagdoll, FMath::Min(0.1f, DeathAnimDuration), false);
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
void ADFCharacter::SetRagdoll()
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

void ADFCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	//Restart with weapon
	SetCurrentWeapon(CurrentWeapon);
}

//Destroys the pawn
void ADFCharacter::Destroyed()
{
	Super::Destroyed();
}

void ADFCharacter::OnStartJump()
{
    bPressedJump = true;
}

void ADFCharacter::OnStopJump()
{
    bPressedJump = false;
}


/*PLAYER SHOOTING */

ADFWeapon* ADFCharacter::GetWeapon() const
{
	return CurrentWeapon;
}

void ADFCharacter::SpawnDefaultInventory()
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
		ADFWeapon* NewWeapon = GetWorld()->SpawnActor<ADFWeapon>(DefaultInventoryClasses[0], SpawnInfo);
		AddWeapon(NewWeapon);
	}
	//}
	EquipWeapon(Inventory[0]);
}

void ADFCharacter::DestroyInventory()
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

void ADFCharacter::AddWeapon(ADFWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void ADFCharacter::RemoveWeapon(ADFWeapon* Weapon)
{
	if (Weapon && Role == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

void ADFCharacter::SetCurrentWeapon(class ADFWeapon* NewWeapon, class ADFWeapon* LastWeapon)
{
	ADFWeapon* LocalLastWeapon = NULL;

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

void ADFCharacter::EquipWeapon(ADFWeapon* Weapon)
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

bool ADFCharacter::ServerEquipWeapon_Validate(class ADFWeapon* NewWeapon)
{
	return true;
}

void ADFCharacter::ServerEquipWeapon_Implementation(class ADFWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void ADFCharacter::OnStartFire(){
	ADFPlayerController* MyPC = Cast<ADFPlayerController>(Controller);
	if (MyPC)
	{
		StartWeaponFire();
	}
}

void ADFCharacter::OnStopFire(){
	StopWeaponFire();
}

/** [local] starts weapon fire */
void ADFCharacter::StartWeaponFire()
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
void ADFCharacter::StopWeaponFire()
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

bool ADFCharacter::IsFiring() const
{
	return bWantstoFire;
}

//Called once the player character uses LMB

/**OLD CODE FROM THE PROJECTILE BASED GUN HAD IN THE PROTOTYPE */
void ADFCharacter::OnFire()
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
bool ADFCharacter::ServerStartFire_Validate(FVector location, FRotator ShootDir)
{
	return true;
}

//Presents the logic to spawn a bullet from the players muzzle
void ADFCharacter::ServerStartFire_Implementation(FVector location, FRotator ShootDir)
{
	FTransform SpawnTM(ShootDir, location);
	UWorld* const World = GetWorld();
	//spawn th projectile at the muzzle
	ADFProjectile* Projectile = Cast<ADFProjectile>(UGameplayStatics::BeginSpawningActorFromClass(World, ProjectileClass, SpawnTM));
	if (Projectile)
	{
		//find launch direction
		Projectile->SetOwner(this);
		Projectile->Instigator = Instigator;
		Projectile->InitVelocity(ShootDir.Vector());
	}
}

bool ADFCharacter::CanFire() const
{
	return IsAlive();
}
/**END PROTOTYPE CODE */


int32 ADFCharacter::GetMaxHealth() const
{
	return GetClass()->GetDefaultObject<ADFCharacter>()->MaxHealth;
}

void ADFCharacter::CollectHealth()
{
    float HealthAmount = 0.f;
    
    //Get overlapping Actors and store them in a CollectedActors array
    TArray<AActor*> CollectedActors;
//    CollectionSphere->GetOverlappingActors(CollectedActors);
    
    //For each Actor collected
    for (int32 iCollected = 0; iCollected < CollectedActors.Num(); ++iCollected)
    {
        // Cast the collected Actor to AHealthPickup
        ADFPickup_Health* const TestHealth = Cast<ADFPickup_Health>(CollectedActors[iCollected]);
        
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