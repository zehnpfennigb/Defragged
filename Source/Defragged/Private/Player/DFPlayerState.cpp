

#include "Defragged.h"
#include "Player/DFPlayerState.h"


ADFPlayerState::ADFPlayerState(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
}

void ADFPlayerState::Reset()
{
	Super::Reset();

	SetTeamNum(0);
	NumKills = 0;
	NumDeaths = 0;
	
}

void ADFPlayerState::ClientInitialize(class AController* InController)
{
	Super::ClientInitialize(InController);

	UpdateTeamColors();
}

void ADFPlayerState::SetTeamNum(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
	UpdateTeamColors();
}

void ADFPlayerState::OnRep_TeamColor()
{
	UpdateTeamColors();
}

void ADFPlayerState::UpdateTeamColors()
{
	AController* OwnerController = Cast<AController>(GetOwner());
	if (OwnerController != NULL)
	{
		ADFCharacter* ShooterCharacter = Cast<ADFCharacter>(OwnerController->GetCharacter());
		if (ShooterCharacter != NULL)
		{
			ShooterCharacter->UpdateTeamColors();
		}
	}
}

int32 ADFPlayerState::GetTeamNum() const
{
	return TeamNumber;
}

int32 ADFPlayerState::GetKills() const
{
	return NumKills;
}

int32 ADFPlayerState::GetDeaths() const
{
	return NumDeaths;
}

float ADFPlayerState::GetScore() const
{
	return Score;
}

void ADFPlayerState::ScoreKill(ADFPlayerState* Victim, int32 Points)
{
	NumKills++;
	ScorePoints(Points);
}

void ADFPlayerState::ScoreDeath(ADFPlayerState* KilledBy, int32 Points)
{
	NumDeaths++;
	ScorePoints(Points);
}

void ADFPlayerState::ScorePoints(int32 Points)
{
	ADFGameState* const MyGameState = Cast<ADFGameState>(GetWorld()->GameState);
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

/*void ADFPlayerState::InformAboutKill_Implementation(class ADFPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ADFPlayerState* KilledPlayerState)
{

}

void ADFPlayerState::BroadcastDeath_Implementation(class ADFPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ADFPlayerState* KilledPlayerState)
{

}*/

void ADFPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADFPlayerState, TeamNumber);
	DOREPLIFETIME(ADFPlayerState, NumKills);
	DOREPLIFETIME(ADFPlayerState, NumDeaths);
}