// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Deploy/DeployGridAction.h"

#include "Character/TBSCharacter.h"
#include "GridSystem/GridActor/GridActor.h"
#include "GridSystem/GridActor/GridComponents/GridStateComponent.h"

bool UGridDeployAction::CheckIfValidToInitialize(AGridActor* InGridActor,
                                                 ATBSCharacter* InInstigator,
                                                 const FIntPoint& InInstigatingIndex) const
{
	if (InInstigator != nullptr)
	{
		// TODO: Check if the occupying character can be deployed by player
		return true;
	}
	return false;
}

bool UGridDeployAction::CheckIfValidToExecute(const FIntPoint& TargetIndex) const
{
	// Invalid if tile is already occupied.
	FGridState GridState;
	bool bCanExecute = GridStateComponent->GetGridUnitState(TargetIndex, GridState);

	// TODO: Allow units to swap
	bCanExecute &= GridState.OccupyingUnit == nullptr;
	bCanExecute &= GridState.GetInstanceType() == EGridInstanceType::Deployment;
	return bCanExecute;
}


void UGridDeployAction::Cancel()
{
	Super::Cancel();
	if (SelectedIndex != GridConstants::InvalidIndex)
	{
		// When cancelling if the direction choice is active, only cancel the direction prompt.
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
	GetGridActor()->SetGridAsActive(SelectedIndex);
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
		GetGridActor()->SetGridAsActive(GetInstigatingIndex());
	}
	return bIsParentInitialized;
}

void UGridDeployAction::DeployFighter()
{
	// Update SourceGrid to remove character. Ignore if source grid is not set
	if (GetInstigatingIndex() != GridConstants::InvalidIndex)
	{
		FGridState SourceGridState;
		const bool bIsSourceFound = GridStateComponent->GetGridUnitState(GetInstigatingIndex(), SourceGridState);
		check(bIsSourceFound);
		SourceGridState.OccupyingUnit = nullptr;
		GridStateComponent->AddGridState(GetInstigatingIndex(), SourceGridState);
	}
	GetGridActor()->ResetActiveGrid();
	FGridState TargetGridState;
	const bool bIsTargetFound = GridStateComponent->GetGridUnitState(SelectedIndex, TargetGridState);
	check(bIsTargetFound);
	TargetGridState.OccupyingUnit = GetInstigator();
	GridStateComponent->AddGridState(SelectedIndex, TargetGridState);
	GetInstigator()->DeployCharacterOnGrid(GetGridActor(), GetInstigatingIndex(), SelectedIndex);
	FinishExecution(EActionExecutionStatus::Success);
}

void UGridDeployAction::HandleGridSelect(const FIntPoint& GridIndex)
{
	Super::HandleGridSelect(GridIndex);
	SelectedIndex = GridIndex;
	Execute();
}

bool UGridDeployAction::IsIndexHoverable(const FIntPoint& Index) const
{
	FGridState GridState;
	GridStateComponent->GetGridUnitState(Index, GridState);
	return GridState.GetInstanceType() == EGridInstanceType::Deployment
		&& GridState.OccupyingUnit == nullptr;
}
