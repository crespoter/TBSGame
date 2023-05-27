// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatSituation.h"
#include "Components/BoxComponent.h"
#include "Game/TBSGameState.h"
#include "GridSystem/GridActor/GridActor.h"

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
}

void ACombatSituation::StartCombatSituation(const ADeploymentZone* DeploymentZone)
{
	if (!bIsActive)
	{
		bIsActive = true;
		GameState->StartDeploymentPhase(this);

		// TODO: Debug; remove
		GameState->GetGridActor()->DrawDebugGrid();
	}
}
