// Copyright Crespoter Interactive


#include "CombatSituation/CombatManager/CombatManager.h"

#include "CombatSituation/CombatSituation.h"

// Sets default values
ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACombatManager::BeginPlay()
{
	Super::BeginPlay();
	check(CombatSituations.Num() > 0);
}

// Called every frame
void ACombatManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ACombatManager::TriggerNextCombatSituation()
{
	if (CurrentIndex >= CombatSituations.Num())
	{
		return false;
	}
	CombatSituations[CurrentIndex]->StartDeploymentSelection();
	return true;
}

