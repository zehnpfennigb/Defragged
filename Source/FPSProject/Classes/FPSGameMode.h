

#pragma once

#include "OnlineIdentityInterface.h"
#include "GameFramework/GameMode.h"
#include "FPSGameMode.generated.h"

/**
 * 
 */
UCLASS(config = Game)
class FPSPROJECT_API AFPSGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()

	/** Initialize the game. This is called before actors' PreInitializeComponents. */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string. */
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<class FUniqueNetId>& UniqueId, FString& ErrorMessage) override;

	/** starts match warmup */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart(AController* Player) override;

	/** always pick new random spawn */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/** returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController(AController* InController) override;

	/** prevents friendly fire */
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	/** notify about kills */
	virtual void Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType);

	/** can players damage each other? */
	virtual bool CanDealDamage(class AFPSPlayerState* DamageInstigator, class AFPSPlayerState* DamagedPlayer) const;

	/** update remaining time */
	virtual void DefaultTimer() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;

	/** spawns default bot */
	class AShooterBot* SpawnBot(FVector SpawnLocation, FRotator SpawnRotation);

protected:

	/** delay between first player login and starting match */
	UPROPERTY(config)
		int32 WarmupTime;

	/** match duration */
	UPROPERTY(config)
		int32 RoundTime;

	UPROPERTY(config)
		int32 TimeBetweenMatches;

	/** score for kill */
	UPROPERTY(config)
		int32 KillScore;

	/** score for death */
	UPROPERTY(config)
		int32 DeathScore;

	/** scale for self instigated damage */
	UPROPERTY(config)
		float DamageSelfScale;

	/** Triggers round start event for local players. Needs revising when shootergame goes multiplayer */
	//void TriggerRoundStartForLocalPlayers();

	/** Triggers round end event for local players. Needs revising when shootergame goes multiplayer */
	//void TriggerRoundEndForLocalPlayers();

	/** check who won */
	virtual void DetermineMatchWinner();

	/** check if PlayerState is a winner */
	virtual bool IsWinner(class AFPSPlayerState* PlayerState) const;

	/** check if player can use spawnpoint */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** check if player should use spawnpoint */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;

	/** Returns game session class to use */
	//virtual TSubclassOf<AGameSession> GetGameSessionClass() const override;

public:

	/** finish current match and lock players */
	UFUNCTION(exec)
		void FinishMatch();

	/*Finishes the match and bumps everyone to main menu.*/
	void RequestFinishAndExitToMainMenu();

	/*
	UPROPERTY()
		TArray<class AShooterPickup*> LevelPickups;
	*/
};
