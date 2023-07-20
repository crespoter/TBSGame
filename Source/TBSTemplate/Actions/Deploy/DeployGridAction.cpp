// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Deploy/DeployGridAction.h"

#include "GridSystem/GridActor/GridActor.h"
#include "GridSystem/GridActor/GridComponents/GridStateComponent.h"

bool UGridDeployAction::CheckIfValidToInitialize(AGridActor* InGridActor,
                                                 ATBSCharacter* InInstigator,
                                                 const FIntPoint& InInstigatingIndex) const
{
	if (InInstigator != nullptr)
	{
		// TODO: Check if the character is of the same team.
		return true;
	}
	return false;
}

bool UGridDeployAction::CheckIfValidToExecute(const FIntPoint& TargetIndex) const
{
	// Invalid if tile is already occupied.
	FGridState GridState;
	const bool bIsFound = GridStateComponent->GetGridUnitState(TargetIndex, GridState);
	return bIsFound && GridState.OccupyingUnit == nullptr;
}


void UGridDeployAction::Cancel()
{
	Super::Cancel();
	if (SelectedIndex != GridConstants::InvalidIndex)
	{
		// When cancelling if the direction choice is active, only cancel the direction prompt.
		// TODO: Remove the direction widget
		SelectedIndex = GridConstants::InvalidIndex;
	}
	else
	{
		FinishExecution(EActionExecutionStatus::Cancelled);
	}
}

void UGridDeployAction::Execute()
{
	Super::Execute();
	// TODO: Create direction widget and listen for direction input which would then call move fighter. hard coded front for now
	Direction = EGridDirection::Up;
	DeployFighter();
}

bool UGridDeployAction::Initialize(AGridActor* InGridActor, ATBSCharacter* InInstigator,
                               const FIntPoint& InInstigatingIndex)
{
	const bool bIsParentInitialized = Super::Initialize(InGridActor, InInstigator, InInstigatingIndex);
	if (bIsParentInitialized)
	{
		check(GetGridActor());
		GridStateComponent =  GetGridActor()->GetGridStateComponent();
		check(GridStateComponent);
		check(GetInstigator());
	}
	return bIsParentInitialized;
}

void UGridDeployAction::DeployFighter()
{
	// Update SourceGrid to remove character. Ignore if source grid is not set
	if (GetInstigatingIndex() == GridConstants::InvalidIndex)
	{
		FGridState SourceGridState;
		const bool bIsSourceFound = GridStateComponent->GetGridUnitState(GetInstigatingIndex(), SourceGridState);
		check(bIsSourceFound);
		SourceGridState.OccupyingUnit = nullptr;
		GridStateComponent->AddGridState(GetInstigatingIndex(), SourceGridState);
	}
	FGridState TargetGridState;
	const bool bIsTargetFound = GridStateComponent->GetGridUnitState(SelectedIndex, TargetGridState);
	check(bIsTargetFound);
	TargetGridState.OccupyingUnit = nullptr;
	GridStateComponent->AddGridState(GetInstigatingIndex(), TargetGridState);
	GetInstigator()->DeployCharacterOnGrid(GetGridActor(), GetInstigatingIndex(), SelectedIndex);
}

void UGridDeployAction::HandleGridSelect(const FIntPoint& GridIndex)
{
	Super::HandleGridSelect(GridIndex);
	SelectedIndex = GridIndex;
	GridStateComponent->SetActiveGridIndex(GridIndex);
	Execute();
}
