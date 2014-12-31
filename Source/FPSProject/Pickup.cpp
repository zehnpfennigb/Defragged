

#include "FPSProject.h"
#include "Pickup.h"


APickup::APickup(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    //The pickup is valid when it is created
    bIsActive = true;
    
    // Create the root SphereComponent to handle collisions
    BaseCollisionComponent = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("BaseSphereComponent"));
    
    //Set the SphereComponent to the root component
    RootComponent = BaseCollisionComponent;
    
    //Create the static mesh component
    PickupMesh = PCIP.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Pickup"));
    
    //Turn physics on for the static mesh
    PickupMesh->SetSimulatePhysics(true);
    
    //Attach the StaticMeshComponent to the root component
    PickupMesh->AttachTo(RootComponent);
}

void APickup::OnPickedUp_Implementation()
{
    //There is no default behavior for a Pickup. Other classes will use this.
}
