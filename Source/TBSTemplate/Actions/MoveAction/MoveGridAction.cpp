// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/MoveAction/MoveGridAction.h"

#include "GridSystem/GridActor/GridActor.h"
#include "GridSystem/GridActor/GridComponents/GridStateComponent.h"

bool UMoveGridAction::Initialize(AGridActor* InGridActor, ATBSCharacter* InInstigator,
                                 const FIntPoint& InInstigatingIndex)
{
	const bool bIsParentInitialized = Super::Initialize(InGridActor, InInstigator, InInstigatingIndex);
	check(bIsParentInitialized);
	// TODO: MoveSpeed should be fetched from the instigator.
	MoveSpeed = 5;
	CalculateDistanceMap();
	DrawAllMoveGrid();
	return true;
}

bool UMoveGridAction::CheckIfValidToExecute(const FIntPoint& TargetIndex) const
{
	// TODO: Check if the move location is reachable.
	return true;
}

void UMoveGridAction::Cancel()
{
	// TODO: Cleanup the grid.
}

void UMoveGridAction::Execute()
{
	// TODO: Move the character to the grid. On finishing movement, trigger onFinishCallback.	
}

void UMoveGridAction::HandleGridSelect(const FIntPoint& GridIndex)
{
	Super::HandleGridSelect(GridIndex);
	// TODO: Highlight the grid if its within reach
}

bool UMoveGridAction::IsIndexHoverable(const FIntPoint& Index) const
{
	return Super::IsIndexHoverable(Index);
	// TODO: return true if hoverable
	// TODO: We should also be handling hovering behaviour on this action
}

void UMoveGridAction::ResetDistanceMap()
{
	OffsetDistanceMap.Empty();
	OffsetDistanceMap.Reserve((MoveSpeed + 2) * (MoveSpeed + 2));
}

void UMoveGridAction::CalculateDistanceMap()
{
	// TODO: Also store the path
	// TODO: Handle Heights
	ResetDistanceMap();
	TQueue<FIntPoint> PendingOffsets;
	TSet<FIntPoint> VisitedOffsets;
	VisitedOffsets.Reserve(MoveSpeed * MoveSpeed);
	PendingOffsets.Enqueue(FIntPoint(0));
	VisitedOffsets.Add(FIntPoint(0));
	OffsetDistanceMap.Add({0}, 0);
	while(!PendingOffsets.IsEmpty())
	{
		FIntPoint CurrentIndex;
		PendingOffsets.Dequeue(CurrentIndex);
		VisitedOffsets.Add(CurrentIndex);
		const uint8 CurrentDistance = OffsetDistanceMap.FindChecked({
			CurrentIndex.X,
			CurrentIndex.Y
		});
		if ((CurrentDistance + 1) >= MoveSpeed)
		{
			continue;
		}
		TArray<FIntPoint> ValidNeighbours;
		GetValidNeighbours(CurrentIndex, ValidNeighbours);
		for (const FIntPoint NeighbourOffset : ValidNeighbours)
		{
			if (!VisitedOffsets.Contains(NeighbourOffset))
			{
				const uint8* DistanceRef = OffsetDistanceMap.Find({NeighbourOffset.X, NeighbourOffset.Y});

				if (!DistanceRef || (CurrentDistance + 1) < *DistanceRef)
				{
					OffsetDistanceMap.Add(NeighbourOffset, CurrentDistance + 1);
					PendingOffsets.Enqueue(NeighbourOffset);
				}
			}
		}
	}
}

void UMoveGridAction::GetValidNeighbours(const FIntPoint& IndexOffset, TArray<FIntPoint>& OutValidIndices)
{
	static const TArray<FIntPoint> PotentialNeighbourSet = {
		{0, 1},
		{1, 0},
		{-1, 0},
		{0, -1}
	};

	OutValidIndices.Empty();
	OutValidIndices.Reserve(4);
	for (const FIntPoint Neighbour : PotentialNeighbourSet)
	{
		FIntPoint GridIndex = GetGridIndexFromOffset(IndexOffset + Neighbour);
		if (GetGridActor()->IsValidIndex(GridIndex))
		{
			FGridState GridState;
			GetGridActor()->GetGridStateComponent()->GetGridUnitState(GridIndex, GridState);
			if (GridState.GridAccessState == EGridAccessState::Accessible && GridState.OccupyingUnit == nullptr)
			{
				OutValidIndices.Add(IndexOffset + Neighbour);
			}
		}
	}
}

void UMoveGridAction::DrawAllMoveGrid()
{
	TArray<FIntPoint> ValidIndices;
	OffsetDistanceMap.GetKeys(ValidIndices);
	for (const FIntPoint& ValidIndex : ValidIndices)
	{
		
		DrawSingleGridUnit(GetInstigatingIndex() + ValidIndex);
	}
}

void UMoveGridAction::DrawSingleGridUnit(const FIntPoint& GridIndex)
{
	FGridState GridState;
	GetGridActor()->GetGridStateComponent()->GetGridUnitState(GridIndex, GridState);
	GridState.SetInstanceType(EGridInstanceType::Movement);
	GridState.SetActivityType(EGridInstanceActivityType::None);
	GetGridActor()->GetGridStateComponent()->AddGridState(GridIndex, GridState);
}

FIntPoint UMoveGridAction::GetGridIndexFromOffset(const FIntPoint& Offset) const
{
	return GetInstigatingIndex() + Offset;
}
