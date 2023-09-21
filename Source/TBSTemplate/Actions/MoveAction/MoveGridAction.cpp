// Fill out your copyright notice in the Description page of Project Settings.
#include "Actions/MoveAction/MoveGridAction.h"
#include "Algo/Reverse.h"
#include "Character/TBSCharacter.h"
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
	SelectedPath.Reserve(MoveSpeed);
	GridPathMap.Reserve((MoveSpeed + 2) * (MoveSpeed + 2));
	CurrentState = EMoveGridActionState::Selecting;
	return true;
}

bool UMoveGridAction::CheckIfValidToExecute(const FIntPoint& TargetIndex) const
{
	return TargetIndex != GetInstigatingIndex() && GridPathMap.Contains(TargetIndex);
}

void UMoveGridAction::Cancel()
{
	ClearPathGrids();
	GridStateComponent->SetVisibleGridUnitsAsHidden();
	ResetDistanceMap();
	SelectedPath.Reset();
	FinishExecution(EActionExecutionStatus::Cancelled);
}

void UMoveGridAction::Execute()
{
	GridStateComponent->SetVisibleGridUnitsAsHidden();
	ResetDistanceMap();
	// [...NextTarget,CurrentPosition]
	if (SelectedPath.Num() >= 2)
	{	
		CurrentState = EMoveGridActionState::Executing;
		check(GetInstigator());
		check(GetGridActor());
		// Move out of current position
		GridStateComponent->SetCharacterUnitAtIndex(SelectedPath.Top(),
			nullptr);
		SelectedPath.Pop();
		GetInstigator()->OnPathFinishedDelegate.AddDynamic(
			this, &ThisClass::OnInstigatorReachedTarget);
		GetInstigator()->MoveToLocation(GetGridActor()->GetWorldLocationFromIndex(
			SelectedPath.Top()));
	}
	else
	{
		FinishExecution(EActionExecutionStatus::Success);
	}
}

void UMoveGridAction::HandleGridSelect(const FIntPoint& GridIndex)
{
	Execute();
}

void UMoveGridAction::HandleGridHover(const FIntPoint& GridIndex)
{
	if (CurrentState != EMoveGridActionState::Selecting || !bHasFinishedSetup)
	{
		return;
	}
	const bool bIsIndexHoverable = IsIndexHoverable(GridIndex);
	const bool bIsSameIndex = GridIndex == GridStateComponent->GetHoveringGridIndex();
	if (!bIsSameIndex)
	{
		ClearPathGrids();
	}
	if (!bIsIndexHoverable || bIsSameIndex)
	{
		return;
	}
	GridStateComponent->UpdateGridStateActivity(
		GridIndex,
		EGridInstanceType::Movement,
		EGridInstanceActivityType::Hover);

	// calculate the path taken and highlight these grids.
	
	GetPathTaken(GridIndex, SelectedPath, true, true);
	SelectedPath.Push(GetInstigatingIndex());

	for (const FIntPoint Index : SelectedPath)
	{
		GridStateComponent->UpdateGridStateActivity(Index,
			EGridInstanceType::Path,
			EGridInstanceActivityType::None);
	}
	SelectedPath.Insert(GridIndex, 0);
}

bool UMoveGridAction::IsIndexHoverable(const FIntPoint& Index) const
{
	return CheckIfValidToExecute(Index);
}


void UMoveGridAction::GetPathTaken(const FIntPoint& EndIndex, TArray<FIntPoint>& OutGridPath,
	bool bShouldExcludeEdges, bool bIsReversed) const
{
	OutGridPath.Empty();
	// Each path stores where it came from. We have to work our way back.
	
	FIntPoint CurrentIndex = EndIndex;
	const FIntPoint StartIndex = GetInstigatingIndex();

	if (bShouldExcludeEdges)
	{
		const FGridPath* GridPath =  GridPathMap.Find(CurrentIndex);
		if (!GridPath)
		{
			return;
		}
		CurrentIndex = GridPath->MovedFrom;
	}
	
	while (CurrentIndex != StartIndex)
	{
		OutGridPath.Emplace(CurrentIndex);
		const FGridPath* GridPath =  GridPathMap.Find(CurrentIndex);
		// There is no path.
		if (!GridPath)
		{
			break;
		}
		CurrentIndex = GridPath->MovedFrom;
	}

	if (!bShouldExcludeEdges)
	{
		OutGridPath.Emplace(StartIndex);
	}

	if (!bIsReversed)
	{
		Algo::Reverse(OutGridPath);
	}
}

void UMoveGridAction::ResetDistanceMap()
{
	GridPathMap.Reset();
}

void UMoveGridAction::CalculateDistanceMap()
{
	// TODO: Handle Heights
	ResetDistanceMap();
	TQueue<FIntPoint> PendingIndexes;
	TSet<FIntPoint> VisitedIndexes;
	VisitedIndexes.Reserve(MoveSpeed * MoveSpeed);
	PendingIndexes.Enqueue(GetInstigatingIndex());
	
	GridPathMap.Emplace(GetInstigatingIndex(), {0, GridConstants::InvalidIndex});

	while(!PendingIndexes.IsEmpty())
	{
		FIntPoint CurrentIndex;
		PendingIndexes.Dequeue(CurrentIndex);
		VisitedIndexes.Add(CurrentIndex);

		const FGridPath CurrentGridPath = GridPathMap.FindChecked(CurrentIndex);
		
		if ((CurrentGridPath.Distance + 1) > MoveSpeed)
		{
			continue;
		}
		TArray<FIntPoint> ValidNeighbours;
		GetValidNeighbours(CurrentIndex, ValidNeighbours);
		for (const FIntPoint Neighbour : ValidNeighbours)
		{
			if (!VisitedIndexes.Contains(Neighbour))
			{
				FGridPath* TargetPathRef = GridPathMap.Find(Neighbour);

				if (!TargetPathRef || (CurrentGridPath.Distance + 1) < TargetPathRef->Distance)
				{
					if (!TargetPathRef)
					{
						GridPathMap.Add(Neighbour, {0, {0}});
						TargetPathRef = GridPathMap.Find(Neighbour);
					}
					check(TargetPathRef);
					TargetPathRef->Distance = CurrentGridPath.Distance + 1;
					TargetPathRef->MovedFrom = CurrentIndex;
					PendingIndexes.Enqueue(Neighbour);
				}
			}
		}
	}
	bHasFinishedSetup = true;
}

void UMoveGridAction::GetValidNeighbours(const FIntPoint& Index, TArray<FIntPoint>& OutValidIndices)
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
		const FIntPoint GridIndex = Index + Neighbour;
		if (GetGridActor()->IsValidIndex(GridIndex))
		{
			FGridState GridState;
			GetGridActor()->GetGridStateComponent()->GetGridUnitState(GridIndex, GridState);
			if (GridState.GridAccessState == EGridAccessState::Accessible && GridState.OccupyingUnit == nullptr)
			{
				OutValidIndices.Add(GridIndex);
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
		DrawSingleGridUnit(ValidIndex);
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

void UMoveGridAction::ClearPathGrids()
{
	for (const FIntPoint& Index : SelectedPath)
	{
		GridStateComponent->UpdateGridStateActivity(Index, EGridInstanceType::Movement,
			EGridInstanceActivityType::None);;
	}
	SelectedPath.Reset();
}

void UMoveGridAction::OnInstigatorReachedTarget()
{
	if (!SelectedPath.IsEmpty())
	{
		GridStateComponent->SetCharacterUnitAtIndex(SelectedPath[SelectedPath.Num() - 1],
			GetInstigator());
	}
	// TODO: check if the character will be forced to stop here. (traps and opp attacks).
	if (SelectedPath.Num() >= 2)
	{
		// Move out of current position
		GridStateComponent->SetCharacterUnitAtIndex(SelectedPath[SelectedPath.Num() - 1],
			nullptr);
		SelectedPath.Pop();
		
		GetInstigator()->MoveToLocation(
			GetGridActor()->GetWorldLocationFromIndex(SelectedPath.Top()));
	}
	else
	{
		SelectedPath.Reset();
		FinishExecution(EActionExecutionStatus::Success);
	}
}

void UMoveGridAction::FinishExecution(const EActionExecutionStatus ExecutionStatus)
{
	Super::FinishExecution(ExecutionStatus);
	GetInstigator()->OnPathFinishedDelegate.RemoveAll(this);
}
