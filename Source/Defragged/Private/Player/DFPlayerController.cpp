

#include "Defragged.h"
#include "Player/DFPlayerController.h"


ADFPlayerController::ADFPlayerController(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}


void ADFPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ADFPlayerController::FailedToSpawnPawn()
{
	GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Yellow, TEXT("Failed to spawn"));
	Super::FailedToSpawnPawn();
}

void ADFPlayerController::UnFreeze()
{
	//GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Yellow, TEXT("Calling Un Freeze"));
	ServerRestartPlayer();
}

void ADFPlayerController::PawnPendingDestroy(APawn* p)
{
	//GEngine->AddOnScreenDebugMessage(-1, 300.f, FColor::Yellow, TEXT("Pawn Pending Destroy"));
	Super::PawnPendingDestroy(p);

}


