// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TBSTemplate/Game/TBSTypes.h"
#include "TBSGameState.generated.h"

class UGridConfigurationActorComponent;
class AGridActor;
class ATBSCameraPawnBase;
class AHeroCharacter;
class ACombatSituation;
class ATBSCharacter;
class ATBSPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FonGamePhaseChanged, EGamePhase, GamePhase);


UCLASS()
class TBSTEMPLATE_API ATBSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	ATBSGameState();
	
	UFUNCTION(BlueprintCallable)
	void StartDeploymentPhase(ACombatSituation* CombatSituation);
	
	UFUNCTION(BlueprintCallable)
	void StartBattlePhase();
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGamePhase GetCurrentGamePhase() const;

	UFUNCTION()
	void OnTurnChanged();
	
	FORCEINLINE AHeroCharacter* GetMainCharacter() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AGridActor* GetActiveGridActor() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ACombatSituation* GetCurrentActiveCombatSituation() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<ATBSCharacter*> GetPlayerParty() const;
	FORCEINLINE const TArray<ATBSCharacter*>* GetPlayerPartyRef() const;
	FORCEINLINE TArray<ATBSCharacter*>* GetPlayerPartyRef();

	// Properties
	
	UPROPERTY(BlueprintAssignable)
	FonGamePhaseChanged GamePhaseChangedEvent;

protected:
	virtual void BeginPlay() override;

	EGamePhase CurrentPhase {EGamePhase::Exploration};
	
	UPROPERTY()
	AGridActor* ActiveGridActor {nullptr};

	UPROPERTY()
	AHeroCharacter* MainCharacter {nullptr};
	
	UPROPERTY()
	ACombatSituation* CurrentActiveCombatSituation {nullptr};

	UPROPERTY()
	TArray<ATBSCharacter*> PlayerParty;

	UPROPERTY()
	ATBSPlayerController* PlayerController {nullptr};
};
