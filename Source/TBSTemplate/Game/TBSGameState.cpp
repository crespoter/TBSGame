// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSGameState.h"

#include "EngineUtils.h"
#include "Helpers/TBSHelpers.h"
#include "GridSystem/GridActor/GridActor.h"
#include "TBSTemplate/Player/CameraPawn/TBSCameraPawnBase.h"
#include "TBSTemplate/Player/HeroCharacter/HeroCharacter.h"
#include "TBSTemplate/Game/BattleStateComponent.h"

ATBSGameState::ATBSGameState()
{
	BattleStateComponent = CreateDefaultSubobject<UBattleStateComponent>(TEXT("Battle State Component"));
}

void ATBSGameState::StartDeploymentPhase(ACombatSituation* CombatSituation)
{
	CurrentPhase = EGamePhase::Deployment;
	GamePhaseChangedEvent.Broadcast(CurrentPhase);
}

EGamePhase ATBSGameState::GetCurrentGamePhase() const
{
	return CurrentPhase;
}

void ATBSGameState::BeginPlay()
{
	Super::BeginPlay();
}
