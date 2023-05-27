// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSGameState.h"

#include "EngineUtils.h"
#include "TBSTemplate/Game/BattleStateComponent.h"
#include "TBSTemplate/GridSystem/GridActor/GridActor.h"

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

UBattleStateComponent* ATBSGameState::GetBattleStateComponent() const
{
	return BattleStateComponent;
}

void ATBSGameState::BeginPlay()
{
	Super::BeginPlay();
	GridActor = *TActorIterator<AGridActor>(GetWorld());
	check(GridActor);
}
