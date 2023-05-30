// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatSituation.generated.h"

class UBoxComponent;
class AGridActor;
class AHeroCharacter;
class ATBSGameState;
class ADeploymentZone;


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
	void StartCombatSituation(const ADeploymentZone* DeploymentZone);
	
	UPROPERTY(EditInstanceOnly, Category = "Enemies")
	FName NPCCombatantId;	

protected:
	// TODO: Change to TBSCharacter, auto populate based on tags
	// Participants in the order of their speed
	UPROPERTY()
    TArray<AHeroCharacter*> ActiveParticipants;

	UPROPERTY()
	ATBSGameState* GameState {nullptr};
	
	bool bIsActive {false};
};
