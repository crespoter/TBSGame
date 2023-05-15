// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatSituation.h"
#include "Components/BoxComponent.h"
#include "Game/TBSGameState.h"

// Sets default values
ACombatSituation::ACombatSituation()
{
	PrimaryActorTick.bCanEverTick = false;
	CombatTriggerCollider = CreateDefaultSubobject<UBoxComponent>("Box component");
	CombatTriggerCollider->SetupAttachment(RootComponent);
	check(CombatTriggerCollider);
}

// Called when the game starts or when spawned
void ACombatSituation::BeginPlay()
{
	Super::BeginPlay();
	check(!NPCCombatantId.IsNone());
	check(GridActor);
	CombatTriggerCollider->OnComponentHit.AddDynamic(this, &ACombatSituation::OnTriggerColliderHit);
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
}

void ACombatSituation::OnTriggerColliderHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	TriggerSituation();
}

void ACombatSituation::TriggerSituation()
{
	
}
