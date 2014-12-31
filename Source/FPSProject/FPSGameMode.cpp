

#include "FPSProject.h"


AFPSGameMode::AFPSGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
    //set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/Players/BP_Virus.BP_Virus'"));
    if(PlayerPawnObject.Object != NULL)
    {
        DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
    }
    
    HUDClass = AFPSHUD::StaticClass();
	PlayerControllerClass = AFPSPlayerController::StaticClass();
	PlayerStateClass = AFPSPlayerState::StaticClass();
	//SpectatorClass = AShooterSpectatorPawn::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();

	MinRespawnDelay = 5.0f;
}

void AFPSGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

/** Returns game session class to use 
TSubclassOf<AGameSession> AFPSGameMode::GetGameSessionClass() const
{
	return AFPSGameSession::StaticClass();
}
*/

void AFPSGameMode::DefaultTimer()
{
	Super::DefaultTimer();

	// don't update timers for Play In Editor mode, it's not real match
	if (GetWorld()->IsPlayInEditor())
	{
		// start match if necessary.
		if (GetMatchState() == MatchState::WaitingToStart)
		{
			StartMatch();
		}
		return;
	}

	AFPSGameState* const MyGameState = Cast<AFPSGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime > 0 && !MyGameState->bTimerPaused)
	{
		MyGameState->RemainingTime--;

		if (MyGameState->RemainingTime <= 0)
		{
			if (GetMatchState() == MatchState::WaitingPostMatch)
			{
				RestartGame();
			}
			else if (GetMatchState() == MatchState::InProgress)
			{
				FinishMatch();
			}
			else if (GetMatchState() == MatchState::WaitingToStart)
			{
				StartMatch();
			}
		}
	}
}

void AFPSGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AFPSGameState* const MyGameState = Cast<AFPSGameState>(GameState);

	MyGameState->RemainingTime = RoundTime;

	// notify players
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
		if (PC)
		{
			//PC->ClientGameStarted();
		}
	}

	// probably needs to be done somewhere else when shootergame goes multiplayer
	//TriggerRoundStartForLocalPlayers();
}

void AFPSGameMode::FinishMatch()
{
	AFPSGameState* const MyGameState = Cast<AFPSGameState>(GameState);
	if (IsMatchInProgress())
	{
		EndMatch();
		DetermineMatchWinner();

		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AFPSPlayerState* PlayerState = Cast<AFPSPlayerState>((*It)->PlayerState);
			const bool bIsWinner = IsWinner(PlayerState);

			(*It)->GameHasEnded(NULL, bIsWinner);
		}

		// probably needs to be done somewhere else when shootergame goes multiplayer
//		TriggerRoundEndForLocalPlayers();

		// lock all pawns
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			(*It)->TurnOff();
		}

		// set up to restart the match
		MyGameState->RemainingTime = TimeBetweenMatches;
	}
}

// Not sure if we need the next 3 methods since they are for local players but it says
// they need revisiting when the game goes multiplayer...
/*
void AFPSGameMode::TriggerRoundStartForLocalPlayers()
{
	// Send start match event, this will set the CurrentMap stat.
	const auto Events = Online::GetEventsInterface();
	const auto Identity = Online::GetIdentityInterface();

	if (Events.IsValid() && Identity.IsValid())
	{
		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
			ULocalPlayer* LocalPlayer = PC ? Cast<ULocalPlayer>(PC->Player) : nullptr;
			if (LocalPlayer)
			{
				int32 UserIndex = LocalPlayer->ControllerId;
				if (UserIndex != -1)
				{
					FOnlineEventParms Params;
					Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1));
					Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0));

					if (PC->PlayerState->UniqueId.IsValid())
					{
						Events->TriggerEvent(*PC->PlayerState->UniqueId, TEXT("PlayerSessionStart"), Params);
					}
				}
			}
		}
	}
}

void AFPSGameMode::TriggerRoundEndForLocalPlayers()
{
	// Send start match event, this will set the CurrentMap stat.
	const auto Events = Online::GetEventsInterface();
	const auto Identity = Online::GetIdentityInterface();

	FOnlineEventParms Params;
	FString MapName = *FPackageName::GetShortName(GetWorld()->PersistentLevel->GetOutermost()->GetName());

	if (Events.IsValid() && Identity.IsValid())
	{
		// notify players
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AFPSPlayerController* PC = Cast<AFPSPlayerController>(*It);
			ULocalPlayer* LocalPlayer = PC ? Cast<ULocalPlayer>(PC->Player) : nullptr;
			if (LocalPlayer)
			{
				int32 UserIndex = LocalPlayer->ControllerId;
				if (UserIndex != -1)
				{
					// round end
					{
						FOnlineEventParms Params;
						Params.Add(TEXT("GameplayModeId"), FVariantData((int32)1));
						Params.Add(TEXT("DifficultyLevelId"), FVariantData((int32)0));
						Params.Add(TEXT("ExitStatusId"), FVariantData((int32)0));

						if (PC->PlayerState->UniqueId.IsValid())
						{
							Events->TriggerEvent(*PC->PlayerState->UniqueId, TEXT("PlayerSessionEnd"), Params);
						}
					}
				}
			}
		}
	}
}

void AFPSGameMode::RequestFinishAndExitToMainMenu()
{
	FString RemoteReturnReason = NSLOCTEXT("NetworkErrors", "HostHasLeft", "Host has left the game.").ToString();
	FString LocalReturnReason(TEXT(""));

	FinishMatch();

	UShooterGameInstance* const GI = Cast<UShooterGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->RemoveSplitScreenPlayers();
	}

	APlayerController* LocalPrimaryController = nullptr;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* Controller = *Iterator;
		if (Controller && !Controller->IsLocalController())
		{
			Controller->ClientReturnToMainMenu(RemoteReturnReason);
		}
		else
		{
			LocalPrimaryController = Controller;
		}
	}

	if (LocalPrimaryController != NULL)
	{
		LocalPrimaryController->ClientReturnToMainMenu(LocalReturnReason);
	}
}

*/

void AFPSGameMode::DetermineMatchWinner()
{
	// nothing to do here
}

bool AFPSGameMode::IsWinner(class AFPSPlayerState* PlayerState) const
{
	return false;
}

void AFPSGameMode::PreLogin(const FString& Options, const FString& Address, const TSharedPtr<FUniqueNetId>& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	AFPSGameState* const MyGameState = Cast<AFPSGameState>(GameState);
	const bool bMatchIsOver = MyGameState && MyGameState->HasMatchEnded();
	const FString EndGameError = TEXT("Match is over!");

	ErrorMessage = bMatchIsOver ? *EndGameError : GameSession->ApproveLogin(Options);
}

void AFPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AFPSPlayerController* NewPC = Cast<AFPSPlayerController>(NewPlayer);
	/*
	// update spectator location for client
	
	if (NewPC && NewPC->GetPawn() == NULL)
	{
		NewPC->ClientSetSpectatorCamera(NewPC->GetSpawnLocation(), NewPC->GetControlRotation());
	}
	*/

	// start warmup if needed
	AFPSGameState* const MyGameState = Cast<AFPSGameState>(GameState);
	if (MyGameState && MyGameState->RemainingTime == 0)
	{
		const bool bWantsMatchWarmup = !GetWorld()->IsPlayInEditor();
		if (bWantsMatchWarmup && WarmupTime > 0)
		{
			MyGameState->RemainingTime = WarmupTime;
		}
		else
		{
			MyGameState->RemainingTime = 0.0f;
		}
	}

	// notify new player if match is already in progress
	if (NewPC && IsMatchInProgress())
	{
//		NewPC->ClientGameStarted();
//		NewPC->ClientStartOnlineGame();
	}
}

void AFPSGameMode::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn, const UDamageType* DamageType)
{
	AFPSPlayerState* KillerPlayerState = Killer ? Cast<AFPSPlayerState>(Killer->PlayerState) : NULL;
	AFPSPlayerState* VictimPlayerState = KilledPlayer ? Cast<AFPSPlayerState>(KilledPlayer->PlayerState) : NULL;
	

	if (KillerPlayerState && KillerPlayerState != VictimPlayerState)
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		
//		KillerPlayerState->InformAboutKill(KillerPlayerState, DamageType, VictimPlayerState);
	}

	if (VictimPlayerState)
	{
		VictimPlayerState->ScoreDeath(KillerPlayerState, DeathScore);
//		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType, VictimPlayerState);
	}
}

float AFPSGameMode::ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float ActualDamage = Damage;

	AFPSCharacter* DamagedPawn = Cast<AFPSCharacter>(DamagedActor);
	if (DamagedPawn && EventInstigator)
	{
		AFPSPlayerState* DamagedPlayerState = Cast<AFPSPlayerState>(DamagedPawn->PlayerState);
		AFPSPlayerState* InstigatorPlayerState = Cast<AFPSPlayerState>(EventInstigator->PlayerState);

		// disable friendly fire
		if (!CanDealDamage(InstigatorPlayerState, DamagedPlayerState))
		{
			ActualDamage = 0.0f;
		}

		// scale self instigated damage
		if (InstigatorPlayerState == DamagedPlayerState)
		{
			ActualDamage *= DamageSelfScale;
		}
	}

	return ActualDamage;
}

bool AFPSGameMode::CanDealDamage(class AFPSPlayerState* DamageInstigator, class AFPSPlayerState* DamagedPlayer) const
{
	return true;
}

bool AFPSGameMode::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

UClass* AFPSGameMode::GetDefaultPawnClassForController(AController* InController)
{
	return Super::GetDefaultPawnClassForController(InController);
}

AActor* AFPSGameMode::ChoosePlayerStart(AController* Player)
{
	TArray<APlayerStart*> PreferredSpawns;
	TArray<APlayerStart*> FallbackSpawns;

	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* TestSpawn = PlayerStarts[i];
		if (IsSpawnpointAllowed(TestSpawn, Player))
		{
			if (IsSpawnpointPreferred(TestSpawn, Player))
			{
				PreferredSpawns.Add(TestSpawn);
			}
			else
			{
				FallbackSpawns.Add(TestSpawn);
			}
		}
	}

	APlayerStart* BestStart = NULL;
	if (PreferredSpawns.Num() > 0)
	{
		BestStart = PreferredSpawns[FMath::RandHelper(PreferredSpawns.Num())];
	}
	else if (FallbackSpawns.Num() > 0)
	{
		BestStart = FallbackSpawns[FMath::RandHelper(FallbackSpawns.Num())];
	}

	return BestStart ? BestStart : Super::ChoosePlayerStart(Player);
}

bool AFPSGameMode::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	AFPSTeamStart* ShooterSpawnPoint = Cast<AFPSTeamStart>(SpawnPoint);
	/*
	if (ShooterSpawnPoint)
	{
		AFPSAIController* AIController = Cast<AFPSAIController>(Player);
		if (ShooterSpawnPoint->bNotForBots && AIController)
		{
			return false;
		}

		if (ShooterSpawnPoint->bNotForPlayers && AIController == NULL)
		{
			return false;
		}
	}
	*/
	return true;
}

bool AFPSGameMode::IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const
{
	ACharacter* MyPawn = Player ? Cast<ACharacter>(Player->GetPawn()) : NULL;
	if (MyPawn)
	{
		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
		{
			ACharacter* OtherPawn = Cast<ACharacter>(*It);
			if (OtherPawn && OtherPawn != MyPawn)
			{
				const float CombinedHeight = (MyPawn->CapsuleComponent->GetScaledCapsuleHalfHeight() + OtherPawn->CapsuleComponent->GetScaledCapsuleHalfHeight()) * 2.0f;
				const float CombinedRadius = MyPawn->CapsuleComponent->GetScaledCapsuleRadius() + OtherPawn->CapsuleComponent->GetScaledCapsuleRadius();
				const FVector OtherLocation = OtherPawn->GetActorLocation();

				// check if player start overlaps this pawn
				if (FMath::Abs(SpawnLocation.Z - OtherLocation.Z) < CombinedHeight && (SpawnLocation - OtherLocation).Size2D() < CombinedRadius)
				{
					return false;
				}
			}
		}
	}

	return true;
}