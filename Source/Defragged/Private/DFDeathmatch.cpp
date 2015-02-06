

#include "Defragged.h"
#include "DFDeathMatch.h"

ADFDeathMatch::ADFDeathMatch(const class FPostConstructInitializeProperties& PCIP) : Super(PCIP)
{
	//set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/Players/BP_AntiVirus.BP_AntiVirus'"));

	//Find team based Blueprinted Meshes.
	static ConstructorHelpers::FObjectFinder<UBlueprint> VirusPawnObject(TEXT("Blueprint'/Game/Blueprints/Players/BP_Virus.BP_Virus'"));
	static ConstructorHelpers::FObjectFinder<UBlueprint> AVPawnObject(TEXT("Blueprint'/Game/Blueprints/Players/BP_AntiVirus.BP_AntiVirus'"));
	VirusBP = VirusPawnObject.Object;
	AntiVirusBP = AVPawnObject.Object;

	if (PlayerPawnObject.Object != NULL)
	{
		DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
	}

	NumTeams = 2;
	KillScore = 1;
	DeathScore = -1;
	PlayerControllerClass = ADFPlayerController::StaticClass();
	PlayerStateClass = ADFPlayerState::StaticClass();
	GameStateClass = ADFGameState::StaticClass();
	HUDClass = ADFHUD::StaticClass();

	//Respawn time for the players.
	MinRespawnDelay = 3.0f;
}

void ADFDeathMatch::BeginPlay()
{
	Super::BeginPlay();
}

void ADFDeathMatch::InitNewPlayer(AController* NewPlayer, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options)
{
	Super::InitNewPlayer(NewPlayer, UniqueId, Options);

	//assign team
	ADFPlayerState* NewPlayerState = CastChecked<ADFPlayerState>(NewPlayer->PlayerState);
	const int32 TeamNum = ChooseTeam(NewPlayerState);
	NewPlayerState->SetTeamNum(TeamNum);
}

void ADFDeathMatch::InitGameState()
{
	Super::InitGameState();

	ADFGameState* const MyGameState = Cast<ADFGameState>(GameState);
	if (MyGameState)
	{
		MyGameState->NumTeams = NumTeams;
	}
}

/** SPAWNING */
AActor* ADFDeathMatch::ChoosePlayerStart(AController* Player)
{
	TArray<APlayerStart*> AvailableSpawns;

	for (int32 i = 0; i < PlayerStarts.Num(); i++)
	{
		APlayerStart* TestSpawn = PlayerStarts[i];
		if (IsSpawnPointAllowed(TestSpawn, Player))
		{
			AvailableSpawns.Add(TestSpawn);
		}
	}

	APlayerStart* SpawnPoint = NULL;
	//CHeck if number of preferred spawns is = 0 then pick a random spawn point, for our sake, we will just randomize which team spawn we will be spawning on.
	SpawnPoint = AvailableSpawns[FMath::RandHelper(AvailableSpawns.Num())];

	return SpawnPoint ? SpawnPoint : Super::ChoosePlayerStart(Player);
}

/*Checks the spawn point for matching team number*/
bool ADFDeathMatch::IsSpawnPointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
	if (Player)
	{
		ADFTeamStart* TeamStartPoint = Cast<ADFTeamStart>(SpawnPoint);
		ADFPlayerState* PlayerState = Cast<ADFPlayerState>(Player->PlayerState);

		if (PlayerState && TeamStartPoint && TeamStartPoint->SpawnTeam != PlayerState->GetTeamNum())
			return false;
	}
	return true;
}

bool ADFDeathMatch::ShouldSpawnAtStartSpot(AController* Player)
{
	return false;
}

UClass* ADFDeathMatch::GetDefaultPawnClassForController(AController* InController)
{
	ADFPlayerState* PlayerState = Cast<ADFPlayerState>(InController->PlayerState);
	if (PlayerState->GetTeamNum() == 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Blue, TEXT("Using the Virus"));		
		return AntiVirusBP->GeneratedClass;
	}
	else if (PlayerState->GetTeamNum() == 1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Blue, TEXT("Using the AntiVirus!"));
		return 	VirusBP->GeneratedClass;
	}
	else
		return Super::GetDefaultPawnClassForController(InController);
}

/** DAMAGE AND TEAMS */
bool ADFDeathMatch::CanDealDamage(class ADFPlayerState* Shooter, class ADFPlayerState* DamagedPlayer) const
{
	return Shooter && DamagedPlayer && (DamagedPlayer == Shooter || DamagedPlayer->GetTeamNum() != Shooter->GetTeamNum());
}

int32 ADFDeathMatch::ChooseTeam(ADFPlayerState* ForPlayerState) const
{
	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(NumTeams);

	//get cureent team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ADFPlayerState const* const TestPlayerState = Cast<ADFPlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamNum()))
		{
			TeamBalance[TestPlayerState->GetTeamNum()]++;
		}
	}

	//find least populated one
	int32 BestTeamScore = TeamBalance[0];
	for (int32 i = 1; i < TeamBalance.Num(); i++)
	{
		if (BestTeamScore > TeamBalance[i])
		{
			BestTeamScore = TeamBalance[i];
		}
	}

	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamBalance.Num(); i++)
	{
		if (TeamBalance[i] == BestTeamScore)
		{
			BestTeams.Add(i);
		}
	}

	//get random from best list
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return RandomBestTeam;
}

void ADFDeathMatch::DetermineWinner()
{
	ADFGameState const* const MyGameState = Cast<ADFGameState>(GameState);
	int32 BestScore = MAX_uint32;
	int32 BestTeam = -1;
	int32 NumBestTeams = 1;

	for (int32 i = 0; i < MyGameState->TeamScores.Num(); i++)
	{
		const int32 TeamScore = MyGameState->TeamScores[i];
		if (BestScore < TeamScore)
		{
			BestScore = TeamScore;
			BestTeam = i;
			NumBestTeams = 1;
		}
		else if (BestScore == TeamScore)
		{
			NumBestTeams++;
		}
	}

	WinningTeam = (NumBestTeams == 1) ? BestTeam : NumTeams;
}

bool ADFDeathMatch::IsWinner(class ADFPlayerState* PlayerState) const
{
	return PlayerState && PlayerState->GetTeamNum() == WinningTeam;
}

void ADFDeathMatch::Killed(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
	ADFPlayerState* KillerPlayerState = Killer ? Cast<ADFPlayerState>(Killer->PlayerState) : NULL;
	ADFPlayerState* VictimPlayerState = KilledPlayer ? Cast<ADFPlayerState>(KilledPlayer->PlayerState) : NULL;


	if (KillerPlayerState && KillerPlayerState != VictimPlayerState && KillerPlayerState->GetTeamNum() != VictimPlayerState->GetTeamNum())
	{
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		//		KillerPlayerState->InformAboutKill(KillerPlayerState, DamageType, VictimPlayerState);
	}

	if (VictimPlayerState)
	{
		//Should be DeathScore not KillScore. For some reason it always thinks the KillerPlayerState == VictimPlayerState. This is a quick fix that needs to be changed
		KillerPlayerState->ScoreKill(VictimPlayerState, KillScore);
		//VictimPlayerState->ScoreDeath(KillerPlayerState, KillScore);
		//		VictimPlayerState->BroadcastDeath(KillerPlayerState, DamageType, VictimPlayerState);
	}

}

/*bool ADFDeathMatch::IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const
{
if (Player)
{
AShooterTeamStart* TeamStart = Cast<AShooterTeamStart>(SpawnPoint);
AShooterPlayerState* PlayerState = Cast<AShooterPlayerState>(Player->PlayerState);

if (PlayerState && TeamStart && TeamStart->SpawnTeam != PlayerState->GetTeamNum())
{
return false;
}
}

return Super::IsSpawnpointAllowed(SpawnPoint, Player);
}*/