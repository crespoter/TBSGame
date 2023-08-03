// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatSituation.h"
#include "DeploymentZone/DeploymentZone.h"
#include "Game/TBSGameState.h"
#include "Game/TBSTypes.h"

// Sets default values
ACombatSituation::ACombatSituation()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACombatSituation::BeginPlay()
{
	Super::BeginPlay();
	check(!NPCCombatantId.IsNone());
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
	check(GridActor);
	// TODO: Collect Combatants and teams in this situation.
}

void ACombatSituation::StartCombatSituation(ADeploymentZone* InDeploymentZone)
{
	if (!bIsActive)
	{
		bIsActive = true;
		DeploymentZone = InDeploymentZone;
		TeamUnitMap.Add(Constants::PlayerTeamId, GameState->GetPlayerParty());
		ParticipatingTeams.AddUnique(Constants::PlayerTeamId);
		// Sorts the teams. Lower team id teams always goes first.
		ParticipatingTeams.Sort();
		// TODO: Move all npc characters to appropriate grid location
		GameState->StartDeploymentPhase(this);
		GameState->GamePhaseChangedEvent.AddDynamic(this, &ThisClass::OnGamePhaseChanged);
	}
	check(ParticipatingTeams.Num() > 0);
}

ADeploymentZone* ACombatSituation::GetDeploymentZone() const
{
	return DeploymentZone;
}

uint8 ACombatSituation::GetCurrentTurnTeamId() const
{
	check(CurrentTurnTeamIndex < ParticipatingTeams.Num());
	return ParticipatingTeams[CurrentTurnTeamIndex];
}

AGridActor* ACombatSituation::GetGridActor() const
{
	return GridActor;
}

void ACombatSituation::OnGamePhaseChanged(EGamePhase GamePhase)
{
	switch(GamePhase)
	{
	case EGamePhase::Battle:
		// Trigger the first turn.
		CurrentTurnTeamIndex = 0;
		TriggerTurn(false);
		break;
	default:;
	}
}

void ACombatSituation::TriggerTurn(const bool bIsNextTurn)
{
	if (bIsNextTurn)
	{
		CurrentTurnTeamIndex = (CurrentTurnTeamIndex + 1) % ParticipatingTeams.Num();
	}
	NewTurnDelegate.Broadcast();
}