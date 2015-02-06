

#pragma once

#include "GameFramework/PlayerState.h"
#include "DFPlayerState.generated.h"

/**
 * Player information that will be replicated for each player
 */
UCLASS()
class DEFRAGGED_API ADFPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	//clear scores and teams
	virtual void Reset() override;

	//set team
	virtual void ClientInitialize(class AController* InController) override;

	//set team and update pawn. updates team color.
	void SetTeamNum(int32 NewTeamNumber);

	//player killed someone
	void ScoreKill(ADFPlayerState* Victim, int32 Points);

	//player died
	void ScoreDeath(ADFPlayerState* KilledBy, int32 Points);

	//get team
	int32 GetTeamNum() const;

	//get kills
	int32 GetKills() const;

	//get deaths
	int32 GetDeaths() const;

	//get score
	float GetScore() const;

	//sends kill to clients
	//UFUNCTION(Reliable, Client)
	//	void InformAboutKill(class ADFPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ADFCharacter* KilledPlayerState);

	//broadcast death to local clients
	//UFUNCTION(Reliable, NetMulticast)
	//	void BroadcastDeath(class ADFPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class ADFCharacter* KilledPlayerState);

	//replicate team colors and update player mesh
	UFUNCTION()
		void OnRep_TeamColor();

protected:

	//set the a new color to the mesh
	void UpdateTeamColors();

	//team number
	UPROPERTY(Transient, ReplicatedUsing = OnRep_TeamColor)
		int32 TeamNumber;

	//number of kills
	UPROPERTY(Transient, Replicated)
		int32 NumKills;

	//deaths
	UPROPERTY(Transient, Replicated)
		int32 NumDeaths;

	//helper to score points
	void ScorePoints(int32 Points);
};
