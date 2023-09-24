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
class TBSTEMPLATE_API ACombatSituation : public AActor
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
	FORCEINLINE uint8 GetCurrentTurnTeamId() const;
	
	UPROPERTY(EditInstanceOnly, Category = "Battle")
	FName NPCCombatantId;

	UFUNCTION()
	FORCEINLINE AGridActor* GetGridActor() const;

	UPROPERTY()
	FMulticastDynamicDelegate NewTurnDelegate;
private:
	UFUNCTION()
	void OnGamePhaseChanged(EGamePhase GamePhase);

	UFUNCTION()
	void TriggerTurn(const bool bIsNextTurn);
	
	UPROPERTY()
    TArray<ATBSCharacter*> ActiveParticipants;

	UPROPERTY()
	ATBSGameState* GameState {nullptr};

	UPROPERTY()
	ADeploymentZone* DeploymentZone {nullptr};
	
	bool bIsActive {false};

	UPROPERTY(EditInstanceOnly, Category="Grid")
	AGridActor* GridActor {nullptr};

	TMap<uint8, TArray<ATBSCharacter*>> TeamUnitMap;

	TArray<uint8> ParticipatingTeams;

	uint8 CurrentTurnTeamIndex {0};
};
