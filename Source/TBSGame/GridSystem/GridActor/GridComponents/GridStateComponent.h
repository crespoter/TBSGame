// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridStateComponent.generated.h"

class UDataTable;
class UGridGenerationData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TBSGAME_API UGridStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridStateComponent();
	
	FORCEINLINE UStaticMesh* GetGridUnitMesh();

	FORCEINLINE FVector2f GetGridUnitSize() const;

	FORCEINLINE FVector2f GetGridUnitScale() const;

	FORCEINLINE ECollisionChannel GetGridPlacementTraceChannel() const;

	FORCEINLINE FIntPoint GetHoveringGridIndex() const;
	FORCEINLINE void SetHoveringGridIndex(const FIntPoint& Index);
	
	FORCEINLINE FIntPoint GetActiveGridIndex() const;

	FORCEINLINE bool IsHoveringIndexSet();
	FORCEINLINE bool IsActiveIndexSet();
	
	
	const FGridVisualState* GetGridVisualState(const EGridInstanceType InstanceType, const EGridInstanceActivityType ActivityType) const;

	bool GetGridUnitState(const FIntPoint& Index, FGridState& OutGridState) const;
	
	void AddGridState(const FIntPoint& Index, const FGridState& GridInstanceState);

	void SetVisibleGridUnitsAsHidden();
	
	void ResetGridState();

	void LoadVisualData();

	void ResetActiveGrid();

	void ResetHoveringGrid();

	void UpdateGridStateActivity(const FIntPoint& GridIndex, EGridInstanceType InstanceType, 
	    EGridInstanceActivityType ActivityType);

	void SetGridAsActive(const FIntPoint& Index);

	void SetGridAsDefault(const FIntPoint& Index);

	void SetCharacterUnitAtIndex(const FIntPoint& GridIndex, ATBSCharacter* Character);


	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Data")
	UGridGenerationData* GridDataAsset;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* GridVisualDataTable {nullptr};

	UPROPERTY()
	FGridChangedDelegate GridVisualStateUpdatedDelegate;

	UPROPERTY()
	FGridChangedDelegate GridUpdatedDelegate;

protected:
	virtual void BeginPlay() override;

private:
	
	TMap<EGridInstanceType,
		TMap<EGridInstanceActivityType, FGridVisualState>> GridStyleMap;

	UPROPERTY()
	TMap<FIntPoint, FGridState> GridStateMap;
	
	FIntPoint HoveringGridIndex {-1};
	FIntPoint ActiveGridIndex {-1};

	TSet<FIntPoint> RenderedGridIndexes;
};
