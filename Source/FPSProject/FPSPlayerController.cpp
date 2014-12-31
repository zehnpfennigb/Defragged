

#include "FPSProject.h"
#include "FPSPlayerController.h"


AFPSPlayerController::AFPSPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


void AFPSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AFPSPlayerController::FailedToSpawnPawn()
{
	GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Yellow, TEXT("Failed to spawn"));
	Super::FailedToSpawnPawn();
}

void AFPSPlayerController::UnFreeze()
{
	//GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Yellow, TEXT("Calling Un Freeze"));
	ServerRestartPlayer();
}

void AFPSPlayerController::PawnPendingDestroy(APawn* p)
{
	//GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Yellow, TEXT("Pawn Pending Destroy"));
	Super::PawnPendingDestroy(p);

}


