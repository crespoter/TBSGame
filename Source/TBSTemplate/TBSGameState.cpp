// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSGameState.h"

#include "EngineUtils.h"
#include "GridSystem/GridActor/GridActor.h"

AGridActor* ATBSGameState::GetGridActor() const
{
	return MapGridActor;
}

void ATBSGameState::UpdateGameState(const EGamePhase GamePhase)
{
	if (CurrentPhase != GamePhase)
	{
		CurrentPhase = GamePhase;
		GamePhaseChangedEvent.Broadcast(CurrentPhase);
	}
}

EGamePhase ATBSGameState::GetCurrentGamePhase() const
{
	return CurrentPhase;
}

void ATBSGameState::BeginPlay()
{
	Super::BeginPlay();
	const TActorIterator<AGridActor> It(GetWorld());
	MapGridActor = *It;
	check(MapGridActor);
}
