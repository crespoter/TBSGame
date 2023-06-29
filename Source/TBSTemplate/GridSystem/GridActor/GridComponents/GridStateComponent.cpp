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

const FGridState* UGridStateComponent::GetGridUnitState(const FIntPoint& Index) const
{
	return GridStateMap.Find(Index);
}

void UGridStateComponent::AddGridState(const FIntPoint& Index, const FGridState& GridInstanceState)
{
	const FGridState* OldGridState = GridStateMap.Find(Index);
	if (OldGridState)
	{
		if (OldGridState->bIsUnitRendered && !GridInstanceState.bIsUnitRendered)
		{
			RenderedGridsCache.Remove(Index);
		}
		else if (!OldGridState->bIsUnitRendered && GridInstanceState.bIsUnitRendered)
		{
			RenderedGridsCache.Add(Index);
		}
	}
	GridStateMap.Emplace(Index, GridInstanceState);
}

void UGridStateComponent::SetVisibleGridUnitsAsHidden()
{
	for (const FIntPoint Index : RenderedGridsCache)
	{
		FGridState* GridState = GridStateMap.Find(Index);
		check(GridState);
		if (GridState->bIsUnitRendered)
		{
			GridState->SetInstanceType(EGridInstanceType::None);
		}
	}
	RenderedGridsCache.Reset();
}

void UGridStateComponent::ResetGridState()
{
	GridStateMap.Reset();
	RenderedGridsCache.Reset();
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


