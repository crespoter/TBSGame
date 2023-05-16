// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TBSPlayerController.generated.h"

class ATBSGameState;
class AHeroCharacter;
class ATBSCameraPawnBase;

UCLASS()
class TBSTEMPLATE_API ATBSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ATBSPlayerController(const FObjectInitializer& ObjectInitializer);
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleGamePhaseChange(const EGamePhase GamePhase);
	
protected:
	UPROPERTY()
	ATBSGameState* GameState { nullptr };

	UPROPERTY()
	AHeroCharacter* HeroCharacter {nullptr};

	UPROPERTY()
	ATBSCameraPawnBase* BattleCameraPawn {nullptr};
};
