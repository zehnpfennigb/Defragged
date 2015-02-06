

#pragma once

#include "GameFramework/GameMode.h"
#include "DFDeathMatch.generated.h"

/**
* Team Death Match Settings
*/
UCLASS(config = Game)
class DEFRAGGED_API ADFDeathMatch : public AGameMode
{
	GENERATED_UCLASS_BODY()

		//initialize player
		virtual void InitNewPlayer(AController* NewPlayer, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options) override;

	//initialize replicated game data
	virtual void InitGameState() override;

	//can players damage each other?
	virtual bool CanDealDamage(class ADFPlayerState* Shooter, class ADFPlayerState* DamagedPlayer) const;

	virtual void BeginPlay() override;

	/** notify about kills */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn);


protected:
	/** score for kill */
	UPROPERTY(config)
		int32 KillScore;

	/** score for death */
	UPROPERTY(config)
		int32 DeathScore;

	//number of teams
	int32 NumTeams;

	//winning team
	int32 WinningTeam;

	//Pick with least players or random when equal
	int32 ChooseTeam(class ADFPlayerState* PlayerState) const;

	//Check who won
	virtual void DetermineWinner();

	//Check if PlayerState is a winner
	virtual bool IsWinner(class ADFPlayerState* PlayerState) const;

	/** Tests each spawn point for availability to the player */
	virtual bool IsSpawnPointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart(AController* Player) override;

	/** always pick new random spawn */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/** returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController(AController* InController) override;

private:
	float MinSpawnDelay;
	UBlueprint* VirusBP;
	UBlueprint* AntiVirusBP;
};
