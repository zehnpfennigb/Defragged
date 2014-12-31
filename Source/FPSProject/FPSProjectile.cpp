

#include "FPSProject.h"
#include "FPSProjectile.h"
#include "FPSCharacter.h"


AFPSProjectile::AFPSProjectile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
    CollisionComp->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);
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

void AFPSProjectile::InitVelocity(const FVector& ShootDirection)
{
    if(ProjectileMovement)
    {
        //set the projectile's velocity to the desired direction
        ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
    }
}

void AFPSProjectile::OnHit(AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
    if(OtherActor && (OtherActor != GetOwner()) && OtherComp)
    {
        OtherComp->AddImpulseAtLocation(ProjectileMovement->Velocity * 100.0f, Hit.ImpactPoint);
    }
	AFPSCharacter* Player = Cast<AFPSCharacter>(OtherActor);
	AController* const Shooter =  Cast<AController>(Player->GetInstigatorController());
	if (Player && Shooter && Player != GetOwner())
	{
//		Player->TakeDamage(10.0, Shooter, GetOwner());
	}
	
}

