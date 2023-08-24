// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/GridAction.h"
#include "MoveGridAction.generated.h"

class UGridStateComponent;


UENUM()
enum class EGridMovementAction : uint8
{
	None,
	Move,
	Jump
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
UCLASS()
class TBSTEMPLATE_API UMoveGridAction : public UGridAction
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

private:
	void ResetDistanceMap();

	void CalculateDistanceMap();
	
	void GetValidNeighbours(const FIntPoint& IndexOffset, TArray<FIntPoint>& OutValidIndices);

	void DrawAllMoveGrid();

	void DrawSingleGridUnit(const FIntPoint& GridIndex, const EGridInstanceActivityType ActivityType = EGridInstanceActivityType::None);

	FIntPoint GetGridIndexFromOffset(const FIntPoint& Offset) const;
	
	uint8 MoveSpeed  {0};

	TMap<FIntPoint, FGridPath> GridPathMap;

	UPROPERTY()
	UGridStateComponent* GridStateComponent {nullptr};
};
