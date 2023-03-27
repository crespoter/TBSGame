// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSGameState.h"

#include "EngineUtils.h"
#include "Helpers/TBSHelpers.h"
#include "GridSystem/GridActor/GridActor.h"
#include "TBSTemplate/Player/CameraPawn/TBSCameraPawnBase.h"
#include "TBSTemplate/Player/HeroCharacter/HeroCharacter.h"

AGridActor* ATBSGameState::GetGridActor() const
{
	return MapGridActor;
}

ATBSCameraPawnBase* ATBSGameState::GetCameraPawn() const
{
	return CameraPawn;
}

TArray<AHeroCharacter*> ATBSGameState::GetHeroCharacters() const
{
	return HeroCharacters;
}

void ATBSGameState::UpdateGameState(const EGamePhase GamePhase)
{
	if (CurrentPhase != GamePhase)
	{
		CurrentPhase = GamePhase;
		switch(GamePhase)
		{
		case EGamePhase::Battle:
		case EGamePhase::Exploration:
		case EGamePhase::Max:
			break;
		}
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
	
	MapGridActor = TBSHelpers::GetActorOfClass<AGridActor>(GetWorld());
	check(MapGridActor);
	
	CameraPawn = TBSHelpers::GetActorOfClass<ATBSCameraPawnBase>(GetWorld());
	check(CameraPawn);

	HeroCharacters = TBSHelpers::GetActorsOfClass<AHeroCharacter>(GetWorld());
	check(HeroCharacters.Num());
}
