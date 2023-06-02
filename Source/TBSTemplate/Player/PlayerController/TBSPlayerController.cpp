// Fill out your copyright notice in the Description page of Project Settings.
#include "TBSPlayerController.h"
#include "Cheats/TBSCheatManager.h"
#include "Game/TBSGameState.h"

ATBSPlayerController::ATBSPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CheatClass = UTBSCheatManager::StaticClass();
}

void ATBSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
	GameState->GamePhaseChangedEvent.AddDynamic(this, &ATBSPlayerController::HandleGamePhaseChange);
}

void ATBSPlayerController::HandleGamePhaseChange(const EGamePhase GamePhase)
{
	switch (GamePhase)
	{
	case EGamePhase::Battle:
		break;
	case EGamePhase::Exploration:
		break;
	case EGamePhase::Deployment:
		break;
	case EGamePhase::Max:
		break;
	}
}
