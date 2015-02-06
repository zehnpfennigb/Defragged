

#include "Defragged.h"
#include "Weapons/DFProjectile.h"
#include "Player/DFCharacter.h"


ADFProjectile::ADFProjectile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &ADFProjectile::OnHit);
    CollisionComp->InitSphereRadius(15.0f);
    RootComponent = CollisionComp;
    
    // Use a ProjectileMovementComponent to govern this projectile's movement
    ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 0.3f;
    
    // Die after 3 seconds by default
    InitialLifeSpan = 3.0f;
}

void ADFProjectile::InitVelocity(const FVector& ShootDirection)
{
    if(ProjectileMovement)
    {
        //set the projectile's velocity to the desired direction
        ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
    }
}

void ADFProjectile::OnHit(AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
    if(OtherActor && (OtherActor != GetOwner()) && OtherComp)
    {
        OtherComp->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);
    }
	ADFCharacter* Player = Cast<ADFCharacter>(OtherActor);
	AController* const Shooter =  Cast<AController>(Player->GetInstigatorController());
	if (Player && Shooter && Player != GetOwner())
	{
//		Player->TakeDamage(10.0, Shooter, GetOwner());
	}
	
}

