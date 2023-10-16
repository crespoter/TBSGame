// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game/TBSTypes.h"
#include "CombatSituation.generated.h"

class UBoxComponent;
class AGridActor;
class AHeroCharacter;
class ATBSGameState;
class ADeploymentZone;
class ATBSCharacter;

/**
 * The brains of a situation. Controls the rules of a particular engagement
*/
UCLASS()
class TBSGAME_API ACombatSituation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatSituation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	void StartCombatSituation(ADeploymentZone* DeploymentZone);

	UFUNCTION()
	FORCEINLINE ADeploymentZone* GetDeploymentZone() const;
	
	UFUNCTION()
	FORCEINLINE AGridActor* GetGridActor() const;

	UFUNCTION()
	void RegisterAICombatant(ATBSAICharacter* AICombatant);

	UFUNCTION()
	bool GetIsPlayersTurn() const;
	
	UPROPERTY()
	FTBSMulticastDynamicDelegate NewTurnDelegate;
	
private:
	UFUNCTION()
	void OnGamePhaseChanged(EGamePhase GamePhase);

	UFUNCTION()
	void AddEnemiesToGrid();

	UFUNCTION()
	void TriggerTurn();
	
	UPROPERTY()
    TSet<ATBSCharacter*> PlayerParticipants;

	UPROPERTY()
	TSet<ATBSAICharacter*> EnemyParticipants;
	
	UPROPERTY()
	ATBSGameState* GameState {nullptr};

	UPROPERTY()
	ADeploymentZone* DeploymentZone {nullptr};
	
	bool bIsActive {false};

	UPROPERTY(EditInstanceOnly, Category="Grid")
	AGridActor* GridActor {nullptr};

};
