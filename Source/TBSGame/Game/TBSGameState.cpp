// Fill out your copyright notice in the Description page of Project Settings.
#include "TBSGameState.h"
#include "EngineUtils.h"
#include "CombatSituation/CombatSituation.h"
#include "CombatSituation/CombatManager/CombatManager.h"
#include "Player/PlayerController/TBSPlayerController.h"
#include "Player/CameraPawn/TBSCameraPawnBase.h"
#include "TBSGame/Character/TBSCharacter.h"
#include "TBSGame/GridSystem/GridActor/GridActor.h"

ATBSGameState::ATBSGameState()
{
}

void ATBSGameState::StartDeploymentPhase(ACombatSituation* CombatSituation)
{
	check(CombatSituation);
	CurrentPhase = EGamePhase::Deployment;
	CurrentActiveCombatSituation = CombatSituation;
	ActiveGridActor = CurrentActiveCombatSituation->GetGridActor();
	check(ActiveGridActor);
	GamePhaseChangedEvent.Broadcast(CurrentPhase);
	CombatSituation->NewTurnDelegate.AddDynamic(this, &ThisClass::OnTurnChanged);
}

void ATBSGameState::StartBattlePhase()
{
	check(CurrentPhase == EGamePhase::Deployment);
	check(CurrentActiveCombatSituation);
	check(ActiveGridActor);
	CurrentPhase = EGamePhase::Battle;	
	GamePhaseChangedEvent.Broadcast(CurrentPhase);
}

EGamePhase ATBSGameState::GetCurrentGamePhase() const
{
	return CurrentPhase;
}

void ATBSGameState::OnTurnChanged()
{
	check(ActiveGridActor);
	if (CurrentActiveCombatSituation->GetIsPlayersTurn())
	{
		// Delegate this to the player controller.
		PlayerController->HandlePlayerTurn();
	}
	else
	{
		//TODO: AI should handle this case.
	}
}


ATBSCharacter* ATBSGameState::GetMainCharacter() const
{
	return MainCharacter;
}

AGridActor* ATBSGameState::GetActiveGridActor() const
{
	return ActiveGridActor;
}

ACombatSituation* ATBSGameState::GetCurrentActiveCombatSituation() const
{
	return CurrentActiveCombatSituation;
}

TArray<ATBSCharacter*> ATBSGameState::GetPlayerParty() const
{
	return PlayerParty;
}

const TArray<ATBSCharacter*>* ATBSGameState::GetPlayerPartyRef() const
{
	return &PlayerParty;
}

TArray<ATBSCharacter*>* ATBSGameState::GetPlayerPartyRef()
{
	return &PlayerParty;
}

ATBSCameraPawnBase* ATBSGameState::GetCameraPawn()
{
	return CameraPawn;
}

void ATBSGameState::BeginPlay()
{
	Super::BeginPlay();
	check(DefaultCharacters.Num() > 0);
	// TODO: Actually spawn main character and party members on map 

	// MainCharacter = *TActorIterator<ATBSCharacter>(GetWorld());
	MainCharacter = GetWorld()->SpawnActor<ATBSCharacter>(DefaultCharacters[0]);
	PlayerParty.Add(MainCharacter);
	check(MainCharacter);

	PlayerController = Cast<ATBSPlayerController>(GetWorld()->GetFirstPlayerController());
	check(PlayerController);

	CameraPawn = *TActorIterator<ATBSCameraPawnBase>(GetWorld());
	check(CameraPawn);

	CombatManager = *TActorIterator<ACombatManager>(GetWorld());
	check(CombatManager);
	
	// Trigger the first combat situation
	CombatManager->TriggerNextCombatSituation();
}
