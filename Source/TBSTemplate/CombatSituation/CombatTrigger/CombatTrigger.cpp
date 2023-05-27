// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatTrigger/CombatTrigger.h"

#include "CombatSituation/CombatSituation.h"
#include "Components/BoxComponent.h"

ACombatTrigger::ACombatTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	CombatTrigger = CreateDefaultSubobject<UBoxComponent>("Combat Trigger Component");
	check(CombatTrigger);
	CombatTrigger->SetupAttachment(RootComponent);
}

void ACombatTrigger::OnCombatTriggerHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	check(CombatSituation);
	check(DeploymentZone);
	CombatSituation->StartCombatSituation(DeploymentZone);
}

void ACombatTrigger::BeginPlay()
{
	Super::BeginPlay();
	CombatTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACombatTrigger::OnCombatTriggerHit);
}

