

#include "Defragged.h"
#include "Pickup/DFPickup_Health.h"


ADFPickup_Health::ADFPickup_Health(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    // the base health level of the HealthPack
    HealthAmount = 50.f;
}

void ADFPickup_Health::OnPickedUp_Implementation()
{
    // Call the parent
    Super::OnPickedUp_Implementation();
    //Destroy HealthPack
    Destroy();
}
