

#pragma once

#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AFPSPlayerController : public APlayerController
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
