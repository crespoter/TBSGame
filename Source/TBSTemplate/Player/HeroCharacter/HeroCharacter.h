// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TBSCharacter.h"
#include "Game/TBSGameState.h"
#include "Player/CameraPawn/TBSCameraPawnBase.h"
#include "HeroCharacter.generated.h"


struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class AHeroAIController;
class ATBSGameState;
class UWidgetComponent;


// Character class for all playable and enemy characters
UCLASS()
class TBSTEMPLATE_API AHeroCharacter : public ATBSCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHeroCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGamePhaseChanged(EGamePhase NewGamePhase);

	UPROPERTY()
	AHeroAIController* AIController {nullptr};

	UPROPERTY()
	ATBSGameState* GameState {nullptr};
};
