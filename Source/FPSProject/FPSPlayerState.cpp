

#include "FPSProject.h"
#include "FPSPlayerState.h"


AFPSPlayerState::AFPSPlayerState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
}

void AFPSPlayerState::Reset()
{
	Super::Reset();

	SetTeamNum(0);
	NumKills = 0;
	NumDeaths = 0;
	
}

void AFPSPlayerState::ClientInitialize(class AController* InController)
{
	Super::ClientInitialize(InController);

	UpdateTeamColors();
}

void AFPSPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
	UpdateTeamColors();
}

void AFPSPlayerState::OnRep_TeamColor()
{
	UpdateTeamColors();
}

void AFPSPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		AFPSCharacter* ShooterCharacter = Cast<AFPSCharacter>(OwnerController->GetCharacter());
		if (ShooterCharacter != NULL)
		{
			ShooterCharacter->UpdateTeamColors();
		}
	}
}

int32 AFPSPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 AFPSPlayerState::GetKills() const
{
	return NumKills;
}

int32 AFPSPlayerState::GetDeaths() const
{
	return NumDeaths;
}

float AFPSPlayerState::GetScore() const
{
	return Score;
}

void AFPSPlayerState::ScoreKill(AFPSPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void AFPSPlayerState::ScoreDeath(AFPSPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

void AFPSPlayerState::ScorePoints(int32 Points)
{
	AFPSGameState* const MyGameState = Cast<AFPSGameState>(GetWorld()->GameState);
	if (MyGameState && TeamNumber >= 0)
	{
		if (TeamNumber >= MyGameState->TeamScores.Num())
		{
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;
	}

	Score += Points;
}

/*void AFPSPlayerState::InformAboutKill_Implementation(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState)
{

}

void AFPSPlayerState::BroadcastDeath_Implementation(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSPlayerState* KilledPlayerState)
{

}*/

void AFPSPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerState, TeamNumber);
	DOREPLIFETIME(AFPSPlayerState, NumKills);
	DOREPLIFETIME(AFPSPlayerState, NumDeaths);
}