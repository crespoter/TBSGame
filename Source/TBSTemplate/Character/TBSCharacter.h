// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TBSCharacter.generated.h"

class AGridActor;

enum class EGamePhase : uint8;
class AHeroAIController;
class ATBSGameState;

UCLASS()
class TBSTEMPLATE_API ATBSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATBSCharacter();

	/*
	 * Instantly moves a character on a grid
	*/
	void DeployCharacterOnGrid(AGridActor* GridActor, const FIntPoint& SourceIndex, const FIntPoint& TargetIndex);

	// TODO: Add MoveCharacterOnGrid which calls back after finishing movement and on each grid moved on

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	
	UFUNCTION()
	void OnGamePhaseChanged(const EGamePhase NewGamePhase);

	UFUNCTION()
	void MoveToLocation(const FVector TargetLocation);

	UPROPERTY()
	AHeroAIController* AIController {nullptr};

	UPROPERTY()
	ATBSGameState* GameState {nullptr};

};
