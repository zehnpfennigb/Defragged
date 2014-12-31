

#pragma once

#include "GameFramework/GameState.h"
#include "FPSGameState.generated.h"

/**
 * Game information that will be replicated
 */
UCLASS()
class FPSPROJECT_API AFPSGameState : public AGameState
{
	GENERATED_UCLASS_BODY()

public:
	//Number of teams in current game
	UPROPERTY(Transient, Replicated)
		int32 NumTeams;
	
	//Team Score
	UPROPERTY(Transient, Replicated)
		TArray<float> TeamScores;

	//Time left for warmup/match
	UPROPERTY(Transient, Replicated)
		int32 RemainingTime;

	//is timer paused?
	UPROPERTY(Transient, Replicated)
		bool bTimerPaused;

	//void RequestFinishAndExitToMainMenu();
};
