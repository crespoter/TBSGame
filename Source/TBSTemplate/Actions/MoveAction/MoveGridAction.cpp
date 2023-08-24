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
	SetShouldHandleGridHover(true);
	GridStateComponent = GetGridActor()->GetGridStateComponent();
	check(GridStateComponent);
	return true;
}

bool UMoveGridAction::CheckIfValidToExecute(const FIntPoint& TargetIndex) const
{
	const FIntPoint OffsetIndex = TargetIndex - GetInstigatingIndex();
	return OffsetIndex != GetInstigatingIndex() && GridPathMap.Contains(OffsetIndex);
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
	GridStateComponent->SetGridAsActive(GridIndex);
}

void UMoveGridAction::HandleGridHover(const FIntPoint& GridIndex)
{
	GridStateComponent->ResetHoveringGrid();
	if (!IsIndexHoverable(GridIndex) || GridIndex == GridStateComponent->GetActiveGridIndex())
	{
		return;
	}
	GridStateComponent->UpdateGridStateActivity(GridIndex, EGridInstanceType::Movement, EGridInstanceActivityType::Hover);
	GridStateComponent->SetHoveringGridIndex(GridIndex);
}

bool UMoveGridAction::IsIndexHoverable(const FIntPoint& Index) const
{
	return CheckIfValidToExecute(Index);
}


void UMoveGridAction::ResetDistanceMap()
{
	GridPathMap.Empty();
	GridPathMap.Reserve((MoveSpeed + 2) * (MoveSpeed + 2));
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
	GridPathMap.Emplace({0}, {0, 0});
	while(!PendingOffsets.IsEmpty())
	{
		FIntPoint CurrentOffset;
		PendingOffsets.Dequeue(CurrentOffset);
		VisitedOffsets.Add(CurrentOffset);

		const FGridPath CurrentGridPath = GridPathMap.FindChecked({
			CurrentOffset.X,
			CurrentOffset.Y
		});
		
		if ((CurrentGridPath.Distance + 1) > MoveSpeed)
		{
			continue;
		}
		TArray<FIntPoint> ValidNeighbours;
		GetValidNeighbours(CurrentOffset, ValidNeighbours);
		for (const FIntPoint NeighbourOffset : ValidNeighbours)
		{
			if (!VisitedOffsets.Contains(NeighbourOffset))
			{
				FGridPath* TargetPathRef = GridPathMap.Find({NeighbourOffset.X, NeighbourOffset.Y});

				if (!TargetPathRef || (CurrentGridPath.Distance + 1) < TargetPathRef->Distance)
				{
					if (!TargetPathRef)
					{
						GridPathMap.Add(NeighbourOffset, {0, {0}});
						TargetPathRef = GridPathMap.Find(NeighbourOffset);
					}
					check(TargetPathRef);
					TargetPathRef->Distance = CurrentGridPath.Distance + 1;
					TargetPathRef->MovedFrom = CurrentOffset;
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
	GridPathMap.GetKeys(ValidIndices);
	for (const FIntPoint& ValidIndex : ValidIndices)
	{
		DrawSingleGridUnit(GetInstigatingIndex() + ValidIndex);
	}
	// Draw the main grid as selected.
	DrawSingleGridUnit(GetInstigatingIndex(), EGridInstanceActivityType::Active);
	GetGridActor()->GetGridStateComponent()->SetGridAsActive(GetInstigatingIndex());
}

void UMoveGridAction::DrawSingleGridUnit(const FIntPoint& GridIndex, const EGridInstanceActivityType ActivityType)
{
	FGridState GridState;
	GetGridActor()->GetGridStateComponent()->GetGridUnitState(GridIndex, GridState);
	GridState.SetInstanceType(EGridInstanceType::Movement);
	GridState.SetActivityType(ActivityType);
	GetGridActor()->GetGridStateComponent()->AddGridState(GridIndex, GridState);
}

FIntPoint UMoveGridAction::GetGridIndexFromOffset(const FIntPoint& Offset) const
{
	return GetInstigatingIndex() + Offset;
}
