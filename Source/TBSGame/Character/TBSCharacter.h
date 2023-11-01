// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/TBSTypes.h"
#include "GameFramework/Character.h"
#include "TBSCharacter.generated.h"

class AGridActor;

enum class EGamePhase : uint8;
class AHeroAIController;
class ATBSGameState;
class UHealthComponent;
class ACombatSituation;


UCLASS()
class TBSGAME_API ATBSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATBSCharacter();

	/*
	 * Instantly moves a character on a grid
	*/
	void DeployCharacterOnGrid(AGridActor* GridActor, const FIntPoint& SourceIndex, const FIntPoint& TargetIndex);

	/**
	 * @brief Snaps the character to the grid in the combat situation
	 * @return The index of the grid.
	 */
	UFUNCTION(CallInEditor, Category="AI Character")
	FIntPoint SnapToGrid();

	UPROPERTY(EditAnywhere)
	bool bIsAI {false};

	
#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
#endif

	UPROPERTY(EditInstanceOnly, Category="AI Character")
	ACombatSituation* CombatSituation {nullptr};
	
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
	
	void MoveToLocation(const FVector& TargetLocation);

	UPROPERTY()
	FTBSMulticastDynamicDelegate OnPathFinishedDelegate;

protected:
	UFUNCTION()
	void OnHealthChanged(int32 OldHealth, int32 newHealth);

	UFUNCTION()
	void OnHealthDepleted();
	
	UPROPERTY()
	UHealthComponent* HealthComponent {nullptr};
private:

	UFUNCTION()
	void OnPathFinished();
	
	UPROPERTY()
	AHeroAIController* AIController {nullptr};

	UPROPERTY()
	ATBSGameState* GameState {nullptr};
};
