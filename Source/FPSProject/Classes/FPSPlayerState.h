

#pragma once

#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.generated.h"

/**
 * Player information that will be replicated for each player
 */
UCLASS()
class FPSPROJECT_API AFPSPlayerState : public APlayerState
{
	GENERATED_UCLASS_BODY()

	//clear scores and teams
	virtual void Reset() override;

	//set team
	virtual void ClientInitialize(class AController* InController) override;

	//set team and update pawn. updates team color.
	void SetTeamNum(int32 NewTeamNumber);

	//player killed someone
	void ScoreKill(AFPSPlayerState* Victim, int32 Points);

	//player died
	void ScoreDeath(AFPSPlayerState* KilledBy, int32 Points);

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
	//	void InformAboutKill(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSCharacter* KilledPlayerState);

	//broadcast death to local clients
	//UFUNCTION(Reliable, NetMulticast)
	//	void BroadcastDeath(class AFPSPlayerState* KillerPlayerState, const UDamageType* KillerDamageType, class AFPSCharacter* KilledPlayerState);

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
