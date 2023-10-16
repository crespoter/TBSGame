// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/GridAction.h"
#include "MoveGridAction.generated.h"

class UGridStateComponent;


enum class EMoveGridActionState : uint8
{
	None,
	Selecting,
	Executing
};


USTRUCT()
struct FGridPath
{
	GENERATED_BODY()
	uint8 Distance {0};
	FIntPoint MovedFrom {-1};
};


/**
 * Grid action that handles movement of a single character.
 */
UCLASS(Blueprintable)
class TBSGAME_API UMoveGridAction : public UGridAction
{
	GENERATED_BODY()
public:

	virtual bool Initialize(AGridActor* InGridActor, ATBSCharacter* InInstigator,
		const FIntPoint& InInstigatingIndex) override;

	virtual bool CheckIfValidToExecute(const FIntPoint& TargetIndex) const override;

	virtual void Cancel() override;

	virtual void Execute() override;

	virtual void HandleGridSelect(const FIntPoint& GridIndex) override;

	virtual void HandleGridHover(const FIntPoint& GridIndex) override;

	virtual bool IsIndexHoverable(const FIntPoint& Index) const override;

	void GetPathTaken(const FIntPoint& EndIndex, TArray<FIntPoint>& OutGridPath,
		bool bShouldExcludeEdges = false, bool bIsReversed = false) const;

	// Grid instance to use to highlight the path.
	UPROPERTY(EditDefaultsOnly)
	EGridInstanceType PathHighlightGridInstance;

private:
	void ResetDistanceMap();

	void CalculateDistanceMap();
	
	void GetValidNeighbours(const FIntPoint& Index, TArray<FIntPoint>& OutValidIndices);

	void DrawAllMoveGrid();
	
	void DrawSingleGridUnit(const FIntPoint& GridIndex, const EGridInstanceActivityType ActivityType = EGridInstanceActivityType::None);

	void ClearPathGrids();

	UFUNCTION()
	void OnInstigatorReachedTarget();

protected:
	virtual void FinishExecution(const EActionExecutionStatus ExecutionStatus) override;

private:
	uint8 MoveSpeed  {0};

	TMap<FIntPoint, FGridPath> GridPathMap;

	UPROPERTY()
	UGridStateComponent* GridStateComponent {nullptr};

	bool bHasFinishedSetup {false};

	// Path to target (in reverse)
	TArray<FIntPoint> SelectedPath;

	EMoveGridActionState CurrentState {EMoveGridActionState::None};

};
