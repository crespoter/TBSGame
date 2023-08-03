// Fill out your copyright notice in the Description page of Project Settings.
#include "TBSPlayerController.h"

#include "Actions/MoveAction/MoveGridAction.h"
#include "Character/TBSCharacter.h"
#include "Cheats/TBSCheatManager.h"
#include "Game/TBSGameState.h"
#include "GridSystem/GridActor/GridActor.h"

ATBSPlayerController::ATBSPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// We dont have any cheats rn, but TBSCheatManager can be used to develop cheats later on.
	CheatClass = UTBSCheatManager::StaticClass();
}

void ATBSPlayerController::HandlePlayerTurn()
{
	if (!GridActor)
	{
		GridActor = GameState->GetActiveGridActor();
		check(GridActor);
	}
	// Set main character to be selected on turn start.
	SetAsSelected(0);
}

void ATBSPlayerController::SetAsSelected(uint8 PartyIndex)
{
	check(PlayerParty && PlayerParty->IsValidIndex(PartyIndex));
	ATBSCharacter* SelectedCharacter = PlayerParty->operator[](PartyIndex);
	check(SelectedCharacter);
	const FIntPoint CharacterGridIndex = GridActor->GetIndexFromLocation(SelectedCharacter->GetActorLocation());
	UMoveGridAction* MoveGridAction = NewObject<UMoveGridAction>();
	GridActor->StartGridAction(CharacterGridIndex, SelectedCharacter, MoveGridAction);
}

void ATBSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
	PlayerParty = GameState->GetPlayerPartyRef();
	check(PlayerParty && PlayerParty->Num() > 0);
}
