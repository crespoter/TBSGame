// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TBSGameState.generated.h"

class UGridConfigurationActorComponent;
class AGridActor;


UENUM()
enum class EGamePhase : uint8
{
	Exploration,
	Battle,
	Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FonGamePhaseChanged, EGamePhase, GamePhase);


UCLASS()
class TBSTEMPLATE_API ATBSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AGridActor* GetGridActor() const;

	UFUNCTION(BlueprintCallable)
	void UpdateGameState(EGamePhase GamePhase);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGamePhase GetCurrentGamePhase() const;

	UPROPERTY(BlueprintAssignable)
	FonGamePhaseChanged GamePhaseChangedEvent;
	
protected:
	virtual void BeginPlay() override;
	UPROPERTY()
	AGridActor* MapGridActor {nullptr};
	EGamePhase CurrentPhase {EGamePhase::Exploration};
};
