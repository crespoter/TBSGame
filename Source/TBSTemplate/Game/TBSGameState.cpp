// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSGameState.h"

#include "EngineUtils.h"
#include "CombatSituation/CombatSituation.h"
#include "TBSTemplate/Player/HeroCharacter/HeroCharacter.h"
#include "TBSTemplate/GridSystem/GridActor/GridActor.h"

ATBSGameState::ATBSGameState()
{
}

void ATBSGameState::StartDeploymentPhase(ACombatSituation* CombatSituation)
{
	CurrentPhase = EGamePhase::Deployment;
	CurrentActiveCombatSituation = CombatSituation;
	
	GridActor->ActivateDeploymentGrid(CombatSituation);
	
	GamePhaseChangedEvent.Broadcast(CurrentPhase);
}

EGamePhase ATBSGameState::GetCurrentGamePhase() const
{
	return CurrentPhase;
}


AHeroCharacter* ATBSGameState::GetMainCharacter() const
{
	return MainCharacter;
}

void ATBSGameState::BeginPlay()
{
	Super::BeginPlay();
	GridActor = *TActorIterator<AGridActor>(GetWorld());
	check(GridActor);
	// TODO: Actually spawn main character and party members on map
	MainCharacter = *TActorIterator<AHeroCharacter>(GetWorld());
	check(MainCharacter);
}
