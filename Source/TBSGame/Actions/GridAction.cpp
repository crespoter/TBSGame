// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/GridAction.h"

bool UGridAction::CheckIfValidToInitialize(AGridActor* InGridActor, ATBSCharacter* InInstigator,
	const FIntPoint& InInstigatingIndex) const
{
	return true;
}

bool UGridAction::CheckIfValidToExecute(const FIntPoint& TargetIndex) const
{
	return true;
}

void UGridAction::Cancel()
{
}

void UGridAction::Execute()
{
}

bool UGridAction::Initialize(AGridActor* InGridActor, ATBSCharacter* InInstigator,
                             const FIntPoint& InInstigatingIndex)
{
	check(InGridActor);
	check(InInstigator);
	if (CheckIfValidToInitialize(InGridActor, InInstigator, InInstigatingIndex))
	{
		GridActor = InGridActor;
		Instigator = InInstigator;
		InstigatingIndex = InInstigatingIndex;
		return true;
	}
	return false;
}

void UGridAction::HandleGridSelect(const FIntPoint& GridIndex)
{
}

void UGridAction::HandleGridHover(const FIntPoint& GridIndex)
{
}

bool UGridAction::IsIndexHoverable(const FIntPoint& Index) const
{
	return true;
}

void UGridAction::SetShouldHandleGridHover(const bool bStatus)
{
	bShouldHandleGridHover = bStatus;
}

bool UGridAction::GetShouldHandleGridHover() const
{
	return bShouldHandleGridHover;
}

void UGridAction::FinishExecution(const EActionExecutionStatus ExecutionStatus)
{
	ActionFinishedDelegate.Broadcast(ExecutionStatus);
}

const AGridActor* UGridAction::GetGridActor() const
{
	return GridActor;
}

AGridActor* UGridAction::GetGridActor()
{
	return GridActor;
}

FIntPoint UGridAction::GetInstigatingIndex() const
{
	return InstigatingIndex;
}

const ATBSCharacter* UGridAction::GetInstigator() const
{
	return Instigator;
}

ATBSCharacter* UGridAction::GetInstigator()
{
	return Instigator;
}

