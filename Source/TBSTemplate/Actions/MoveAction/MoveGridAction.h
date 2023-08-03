// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/GridAction.h"
#include "MoveGridAction.generated.h"

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

	virtual bool IsIndexHoverable(const FIntPoint& Index) const override;

private:
	void ResetDistanceMap();

	void CalculateDistanceMap();
	
	void GetValidNeighbours(const FIntPoint& IndexOffset, TArray<FIntPoint>& OutValidIndices);

	void DrawAllMoveGrid();

	void DrawSingleGridUnit(const FIntPoint& GridIndex);

	FIntPoint GetGridIndexFromOffset(const FIntPoint& Offset) const;
	
	uint8 MoveSpeed  {0};

	TMap<FIntPoint, uint8> OffsetDistanceMap;
};
