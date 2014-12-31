

#include "FPSProject.h"
#include "HealthPickup.h"


AHealthPickup::AHealthPickup(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    // the base health level of the HealthPack
    HealthAmount = 50.f;
}

void AHealthPickup::OnPickedUp_Implementation()
{
    // Call the parent
    Super::OnPickedUp_Implementation();
    //Destroy HealthPack
    Destroy();
}
