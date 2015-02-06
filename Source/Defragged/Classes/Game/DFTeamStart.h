

#pragma once

#include "GameFramework/PlayerStart.h"
#include "DFTeamStart.generated.h"

/**
 * Custom Player Start class to be used with team oriented gametypes
 */
UCLASS()
class DEFRAGGED_API ADFTeamStart : public APlayerStart
{
	GENERATED_UCLASS_BODY()
	UPROPERTY(EditInstanceOnly, Category = Team)
	uint32 SpawnTeam;
};
