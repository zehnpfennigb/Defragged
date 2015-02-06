

#pragma once

#include "GameFramework/Actor.h"
#include "DFPickup.generated.h"

/**
 * 
 */
UCLASS()
class DEFRAGGED_API ADFPickup : public AActor
{
	GENERATED_UCLASS_BODY()

    // True when the DFPickup is able to be pickedd up, false if something deactivates the DFPickup
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DFPickup)
    bool bIsActive;
    
    // Simple collision primitive to use as the root component
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = DFPickup)
    TSubobjectPtr<USphereComponent> BaseCollisionComponent;
	
    // StaticMeshComponent to represent the DFPickup in the level
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = DFPickup)
    TSubobjectPtr<UStaticMeshComponent> PickupMesh;
    
    // Set the behavior for when picked up
    UFUNCTION(BlueprintNativeEvent)
    void OnPickedUp();
    
};
