

#include "Defragged.h"
#include "Game/DFGameState.h"


ADFGameState::ADFGameState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	NumTeams = 0;
	RemainingTime = 0;
	bTimerPaused = false;
	TeamScores.AddZeroed(2);
	TeamScores[0] = 2;
}

void ADFGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADFGameState, NumTeams);
	DOREPLIFETIME(ADFGameState, RemainingTime);
	DOREPLIFETIME(ADFGameState, bTimerPaused);
	DOREPLIFETIME(ADFGameState, TeamScores);
}
