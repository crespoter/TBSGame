// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TBSGameState.generated.h"

class UGridConfigurationActorComponent;
class AGridActor;
class ATBSCameraPawnBase;
class AHeroCharacter;
class UBattleStateComponent;
class ACombatSituation;

UENUM()
enum class EGamePhase : uint8
{
	Exploration,
	Deployment,
	Battle,
	Max
};

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
	FORCEINLINE EGamePhase GetCurrentGamePhase() const;

	FORCEINLINE UBattleStateComponent* GetBattleStateComponent() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AGridActor* GetGridActor() const
	{
		return GridActor;
	}

	// Properties
	
	UPROPERTY(BlueprintAssignable)
	FonGamePhaseChanged GamePhaseChangedEvent;

protected:
	virtual void BeginPlay() override;

protected:
	EGamePhase CurrentPhase {EGamePhase::Exploration};
	
	UPROPERTY(EditDefaultsOnly)
	UBattleStateComponent* BattleStateComponent { nullptr };

	UPROPERTY()
	AGridActor* GridActor {nullptr}; 
};
