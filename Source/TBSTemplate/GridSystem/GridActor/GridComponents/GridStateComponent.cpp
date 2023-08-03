// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridActor/GridComponents/GridStateComponent.h"
#include "Data/GridVisualData/GridVisualData.h"

// Sets default values for this component's properties
UGridStateComponent::UGridStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

UStaticMesh* UGridStateComponent::GetGridUnitMesh()
{
	return GridDataAsset->GridUnitMesh;
}

FVector2f UGridStateComponent::GetGridUnitSize() const
{
	return GridDataAsset->GridUnitSize;
}

FVector2f UGridStateComponent::GetGridUnitScale() const
{
	return GridDataAsset->GridUnitScale;
}

ECollisionChannel UGridStateComponent::GetGridPlacementTraceChannel() const
{
	return GridDataAsset->GridPlacementTraceChannel;
}

FIntPoint UGridStateComponent::GetHoveringGridIndex() const
{
	return HoveringGridIndex;
}

void UGridStateComponent::SetHoveringGridIndex(const FIntPoint& Index)
{
	HoveringGridIndex = Index;
}

FIntPoint UGridStateComponent::GetActiveGridIndex() const
{
	return ActiveGridIndex;
}

void UGridStateComponent::SetActiveGridIndex(const FIntPoint& Index)
{
	ActiveGridIndex = Index;
}

bool UGridStateComponent::IsHoveringIndexSet()
{
	return HoveringGridIndex.X != -1 && HoveringGridIndex.Y != -1;
}

bool UGridStateComponent::IsActiveIndexSet()
{
	return ActiveGridIndex.X != -1 && ActiveGridIndex.Y != -1;
}

const FGridVisualState* UGridStateComponent::GetGridVisualState(const EGridInstanceType InstanceType,
                                                                const EGridInstanceActivityType ActivityType) const
{
	const TMap<EGridInstanceActivityType, FGridVisualState>* ActivityMap = GridStyleMap.Find(InstanceType);
	if (!ActivityMap)
	{
		return nullptr;
	}
	return ActivityMap->Find(ActivityType);
}

bool UGridStateComponent::GetGridUnitState(const FIntPoint& Index, FGridState& OutGridState) const
{
	const FGridState* GridStateRef = GridStateMap.Find(Index);
	if (GridStateRef)
	{
		OutGridState = *GridStateRef;
		return true;
	}
	return false;
}

void UGridStateComponent::AddGridState(const FIntPoint& Index, const FGridState& GridInstanceState)
{
	FGridState* CurrentGridState = GridStateMap.Find(Index);
	EUpdateOpType Op;

	bool bShouldRender;
	
	if (CurrentGridState)
	{
		if (CurrentGridState->IsRendered() && !GridInstanceState.IsRendered())
		{
			RenderedGridIndexes.Remove(Index);
			bShouldRender = true;
		}
		else if (!CurrentGridState->IsRendered() && GridInstanceState.IsRendered())
		{
			RenderedGridIndexes.Add(Index);
			bShouldRender = true;
		}
		else
		{
			bShouldRender = !CurrentGridState->IsVisuallySameWith(GridInstanceState);
		}
		Op = EUpdateOpType::Updated;
		*CurrentGridState = GridInstanceState;
	}
	else
	{
		Op = EUpdateOpType::Created;
		RenderedGridIndexes.Add(Index);
		GridStateMap.Emplace(Index, GridInstanceState);
		bShouldRender = true;
	}

	if (bShouldRender)
	{
		GridVisualStateUpdatedDelegate.Broadcast(GridInstanceState, Index, Op);
	}
	
	GridUpdatedDelegate.Broadcast(GridInstanceState, Index, Op);
}

void UGridStateComponent::SetVisibleGridUnitsAsHidden()
{
	for (const FIntPoint Index : RenderedGridIndexes)
	{
		FGridState* GridState = GridStateMap.Find(Index);
		check(GridState);
		if (GridState->IsRendered())
		{
			GridState->SetInstanceType(EGridInstanceType::None);
			GridUpdatedDelegate.Broadcast(*GridState, Index, EUpdateOpType::Updated);
		}
	}
	RenderedGridIndexes.Reset();
	GridVisualStateUpdatedDelegate.Broadcast(FGridState(), FIntPoint(), EUpdateOpType::Reset);
}

void UGridStateComponent::ResetGridState()
{
	GridStateMap.Reset();
	RenderedGridIndexes.Reset();
	GridVisualStateUpdatedDelegate.Broadcast(FGridState(), FIntPoint(), EUpdateOpType::Reset);
	GridUpdatedDelegate.Broadcast(FGridState(), FIntPoint(), EUpdateOpType::Reset);
}

void UGridStateComponent::LoadVisualData()
{
	check(GridVisualDataTable);
	TArray<FGridVisualDataRow*> VisualDataRows;
	GridVisualDataTable->GetAllRows<FGridVisualDataRow>(
		TEXT(""), VisualDataRows);
	
	check(VisualDataRows.Num() != 0);

	for (const FGridVisualDataRow* DataRow : VisualDataRows)
	{
		TMap<EGridInstanceActivityType, FGridVisualState>* ActivityStyleMap
			= GridStyleMap.Find(DataRow->GridInstanceType);
		if (!ActivityStyleMap)
		{
			GridStyleMap.Emplace(DataRow->GridInstanceType, {});
			ActivityStyleMap = GridStyleMap.Find(DataRow->GridInstanceType);
		}
		ActivityStyleMap->Emplace(DataRow->ActivityType, DataRow->GridVisualState);
	}
}


void UGridStateComponent::BeginPlay()
{
	Super::BeginPlay();
	LoadVisualData();
	check(GridDataAsset);
	check(GridVisualDataTable);
}

