

#pragma once

#include "GameFramework/PlayerStart.h"
#include "FPSTeamStart.generated.h"

/**
 * Custom Player Start class to be used with team oriented gametypes
 */
UCLASS()
class FPSPROJECT_API AFPSTeamStart : public APlayerStart
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(EditInstanceOnly, Category = Team)
	uint32 SpawnTeam;
};
