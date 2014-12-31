

#include "FPSProject.h"
#include "FPSGameState.h"


AFPSGameState::AFPSGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	NumTeams = 0;
	RemainingTime = 0;
	bTimerPaused = false;
	TeamScores.AddZeroed(2);
	TeamScores[0] = 2;
}

void AFPSGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameState, NumTeams);
	DOREPLIFETIME(AFPSGameState, RemainingTime);
	DOREPLIFETIME(AFPSGameState, bTimerPaused);
	DOREPLIFETIME(AFPSGameState, TeamScores);
}
