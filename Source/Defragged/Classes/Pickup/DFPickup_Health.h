

#pragma once

#include "DFPickup.h"
#include "DFPickup_Health.generated.h"

/**
 * 
 */
UCLASS()
class DEFRAGGED_API ADFPickup_Health : public ADFPickup
{
	GENERATED_UCLASS_BODY()
    
    // Set the amount of health the Pickup will the give the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
    float HealthAmount;
	
    // This overrides the OnPickedUp function (use Implementation because this is a BlueprintNativeEvent)
    void OnPickedUp_Implementation() override;
};
