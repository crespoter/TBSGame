// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatSituation.h"

#include "Character/AICharacter/TBSAICharacter.h"
#include "DeploymentZone/DeploymentZone.h"
#include "Game/TBSGameState.h"
#include "Game/TBSTypes.h"
#include "GridSystem/GridActor/GridActor.h"
#include "GridSystem/GridActor/GridComponents/GridStateComponent.h"

// Sets default values
ACombatSituation::ACombatSituation()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACombatSituation::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
	check(GridActor);
	PlayerParticipants.Reset();
	EnemyParticipants.Reset();
}

void ACombatSituation::StartCombatSituation(ADeploymentZone* InDeploymentZone)
{
	if (!bIsActive)
	{
		bIsActive = true;
		DeploymentZone = InDeploymentZone;
		GameState->StartDeploymentPhase(this);
		GameState->GamePhaseChangedEvent.AddDynamic(this, &ThisClass::OnGamePhaseChanged);
	}
}

ADeploymentZone* ACombatSituation::GetDeploymentZone() const
{
	return DeploymentZone;
}


AGridActor* ACombatSituation::GetGridActor() const
{
	return GridActor;
}

void ACombatSituation::RegisterAICombatant(ATBSAICharacter* AICombatant)
{
	EnemyParticipants.Emplace(AICombatant);
}

bool ACombatSituation::GetIsPlayersTurn() const
{
	// TODO: 
	return true;
}

void ACombatSituation::OnGamePhaseChanged(EGamePhase GamePhase)
{
	switch(GamePhase)
	{
	case EGamePhase::Battle:
		// Trigger the first turn.
		AddEnemiesToGrid();
		TriggerTurn();
		break;
	default:;
	}
}

void ACombatSituation::AddEnemiesToGrid()
{
	check(GridActor);
	for (ATBSAICharacter* Character : EnemyParticipants)
	{
		FIntPoint Idx = Character->SnapToGrid();
		GridActor->GetGridStateComponent()->SetCharacterUnitAtIndex(Idx, Character);
	}
}

void ACombatSituation::TriggerTurn()
{
	NewTurnDelegate.Broadcast();
}
