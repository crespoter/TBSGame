// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatSituation.h"
#include "DeploymentZone/DeploymentZone.h"
#include "Game/TBSGameState.h"

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

void ACombatSituation::StartCombatSituation(ADeploymentZone* InDeploymentZone)
{
	DeploymentZone = InDeploymentZone;
	if (!bIsActive)
	{
		bIsActive = true;
		GameState->StartDeploymentPhase(this);
	}
}

ADeploymentZone* ACombatSituation::GetDeploymentZone() const
{
	return DeploymentZone;
}
