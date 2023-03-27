// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSPlayerController.h"

#include "Cheats/TBSCheatManager.h"
#include "Player/CameraPawn/TBSCameraPawnBase.h"
#include "Player/HeroCharacter/HeroCharacter.h"

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
		Possess(GameState->GetCameraPawn());
		break;
	case EGamePhase::Exploration:
		Possess(GameState->GetHeroCharacters()[0]);
		break;
	case EGamePhase::Max:
		break;
	}
}
