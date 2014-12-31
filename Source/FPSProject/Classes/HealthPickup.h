

#pragma once

#include "Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AHealthPickup : public APickup
{
	GENERATED_UCLASS_BODY()
    
    // Set the amount of health the Pickup will the give the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
    float HealthAmount;
	
    // This overrides the OnPickedUp function (use Implementation because this is a BlueprintNativeEvent)
    void OnPickedUp_Implementation() override;
};
