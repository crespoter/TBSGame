// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/CombatSituation.h"
#include "Components/BoxComponent.h"
#include "Game/TBSGameState.h"
#include "GridSystem/GridActor/GridActor.h"

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
	CombatTriggerCollider->OnComponentBeginOverlap.AddDynamic(this, &ACombatSituation::OnTriggerColliderHit);
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);

	check(GridActor);
	GridActor->GenerateGrid(this);
}

void ACombatSituation::OnTriggerColliderHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	TriggerSituation();
}

void ACombatSituation::GetCombatGridBounds(FVector& Center, FVector& Bounds) const
{
	Center = CombatTriggerCollider->GetComponentLocation();
	Bounds = CombatTriggerCollider->GetScaledBoxExtent();
}

void ACombatSituation::TriggerSituation()
{
	if (!bIsActive)
	{
		bIsActive = true;
		GameState->StartDeploymentPhase(this);
	}
}

void ACombatSituation::GenerateGridForSituation()
{
	check(GridActor);
	GridActor->GenerateGrid(this);
}
