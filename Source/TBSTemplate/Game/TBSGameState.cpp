// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSGameState.h"

#include "EngineUtils.h"
#include "CombatSituation/CombatSituation.h"
#include "TBSTemplate/Game/BattleStateComponent.h"
#include "TBSTemplate/Player/HeroCharacter/HeroCharacter.h"
#include "TBSTemplate/GridSystem/GridActor/GridActor.h"

ATBSGameState::ATBSGameState()
{
	BattleStateComponent = CreateDefaultSubobject<UBattleStateComponent>(TEXT("Battle State Component"));
}

void ATBSGameState::StartDeploymentPhase(ACombatSituation* CombatSituation)
{
	CurrentPhase = EGamePhase::Deployment;
	switch(CurrentPhase)
	{
	case EGamePhase::Deployment:
		{
			// TODO: Remove party members and main character from map
			break;
		}
	}
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
