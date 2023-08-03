// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBSPlayerController.generated.h"

enum class EGamePhase : uint8;
class ATBSGameState;
class AHeroCharacter;
class ATBSCharacter;
class ATBSCameraPawnBase;
class AGridActor;

/**
 * This player controller controls the camera and team movement.
 * Battle actions of the player are protracted through this.
*/
UCLASS()
class TBSTEMPLATE_API ATBSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATBSPlayerController(const FObjectInitializer& ObjectInitializer);

	// Called from game state when its the players turn.
	void HandlePlayerTurn();

	void SetAsSelected(uint8 PartyIndex);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	ATBSGameState* GameState { nullptr };

	UPROPERTY()
	AGridActor* GridActor {nullptr};

	const TArray<ATBSCharacter*>* PlayerParty {nullptr};

	uint8 SelectedIndex {0};

	bool bIsSelected {false};
};