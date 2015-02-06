

#pragma once

#include "GameFramework/PlayerController.h"
#include "DFPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEFRAGGED_API ADFPlayerController : public APlayerController
{
	GENERATED_UCLASS_BODY()

protected:


	virtual void PostInitializeComponents() override;

	//Retries to spawn later
	virtual void FailedToSpawnPawn() override;

	//Allows the player to respawn
	virtual void UnFreeze() override;

	//Updates the camera of the pawn upon death.
	virtual void PawnPendingDestroy(APawn* p) override;
private:
	
};
