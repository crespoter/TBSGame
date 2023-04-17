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

	ATBSGameState();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AGridActor* GetGridActor() const;

	FORCEINLINE ATBSCameraPawnBase* GetCameraPawn() const;

	FORCEINLINE TArray<AHeroCharacter*> GetHeroCharacters() const;

	UFUNCTION(BlueprintCallable)
	void UpdateGameState(EGamePhase GamePhase);
	

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGamePhase GetCurrentGamePhase() const;

	UPROPERTY(BlueprintAssignable)
	FonGamePhaseChanged GamePhaseChangedEvent;
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY()
	AGridActor* MapGridActor {nullptr};
	EGamePhase CurrentPhase {EGamePhase::Exploration};

	UPROPERTY()
	ATBSCameraPawnBase* CameraPawn { nullptr };

	UPROPERTY()
	TArray<AHeroCharacter*> HeroCharacters;

	UPROPERTY()
	UBattleStateComponent* BattleStateComponent { nullptr };
};
