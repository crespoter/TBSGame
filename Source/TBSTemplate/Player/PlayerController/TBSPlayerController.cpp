// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSPlayerController.h"
#include "EngineUtils.h"
#include "Cheats/TBSCheatManager.h"
#include "Game/TBSGameState.h"
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
	// HeroCharacter =  *(TActorIterator<AHeroCharacter>(GetWorld()));
	BattleCameraPawn = *(TActorIterator<ATBSCameraPawnBase>(GetWorld()));
	// check(HeroCharacter);
	check(BattleCameraPawn);
}

void ATBSPlayerController::HandleGamePhaseChange(const EGamePhase GamePhase)
{
	switch (GamePhase)
	{
	case EGamePhase::Battle:
		break;
	case EGamePhase::Exploration:
		Possess(HeroCharacter);
		break;
	case EGamePhase::Deployment:
		Possess(BattleCameraPawn);
		break;
	case EGamePhase::Max:
		break;
	}
}
