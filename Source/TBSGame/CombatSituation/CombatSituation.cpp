// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatSituation.h"

#include "Character/TBSCharacter.h"
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
	check(DeploymentZones.Num() > 0);
}

void ACombatSituation::StartCombatSituation(ADeploymentZone* InDeploymentZone)
{
	if (!bIsActive)
	{
		bIsActive = true;
		SelectedDeploymentZone = InDeploymentZone;
		GameState->StartDeploymentPhase(this);
		GameState->GamePhaseChangedEvent.AddDynamic(this, &ThisClass::OnGamePhaseChanged);
	}
}

ADeploymentZone* ACombatSituation::GetDeploymentZone() const
{
	return SelectedDeploymentZone;
}


AGridActor* ACombatSituation::GetGridActor() const
{
	return GridActor;
}

void ACombatSituation::RegisterAICombatant(ATBSCharacter* AICombatant)
{
	EnemyParticipants.Emplace(AICombatant);
}

bool ACombatSituation::GetIsPlayersTurn() const
{
	// TODO: 
	return true;
}

void ACombatSituation::StartDeploymentSelection()
{
	// TODO: Make ui for selecting the deployment zone and start combat there.

	StartCombatSituation(DeploymentZones[0]);
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
	for (ATBSCharacter* Character : EnemyParticipants)
	{
		FIntPoint Idx = Character->SnapToGrid();
		GridActor->GetGridStateComponent()->SetCharacterUnitAtIndex(Idx, Character);
	}
}

void ACombatSituation::TriggerTurn()
{
	NewTurnDelegate.Broadcast();
}
