

#pragma once

#include "GameFramework/Actor.h"
#include "Player/DFCharacter.h"
#include "DFProjectile.generated.h"

/**
 * 
 */
UCLASS()
class DEFRAGGED_API ADFProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
    TSubobjectPtr<USphereComponent> CollisionComp;
    //Projectile movement component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement)
    TSubobjectPtr<class UProjectileMovementComponent> ProjectileMovement;
	// inits velocity of the projectile in the shoot direction
    void InitVelocity(const FVector& ShootDirection);
    // called when projectile hits something
    UFUNCTION()
		void OnHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
