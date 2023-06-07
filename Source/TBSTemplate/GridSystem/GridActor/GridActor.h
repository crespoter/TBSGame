// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridActor.generated.h"

class ATBSGameState;
class UBoxComponent;
class ACombatSituation;
class UDataTable;

UCLASS()
class TBSTEMPLATE_API AGridActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridActor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Generates the grid.
	UFUNCTION(CallInEditor)
	void GenerateGrid();

	UFUNCTION(BlueprintCallable)
	FIntPoint GetIndexFromLocation(const FVector2f& GridLocation) const;

	UFUNCTION(BlueprintCallable)
	FVector GetWorldLocationFromIndex(const FIntPoint& Idx) const;

	/**
	 * @brief Checks if given index is valid for the grid actor. Returns false
	 * for out of bounds and blocked indexes
	 * @param GridIndex Grid index to be checked
	 */
	UFUNCTION(BlueprintCallable)
	bool IsValidIndex(const FIntPoint &GridIndex) const;

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void DrawDebugGrid();

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void ClearGridUnits();
	
	UFUNCTION()
	void DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx);
	
	UFUNCTION(BlueprintCallable)
	void ActivateDeploymentGrid(const ACombatSituation* CurrentCombatSituation);

	UFUNCTION(BlueprintCallable)
	void HandleHoverOnGrid(const FIntPoint& GridIndex);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetInstancedMeshGridColors(const uint16 MeshInstanceIndex,
		const FColor &EdgeColor, const FColor &BackgroundColor);

	void CalculateDimensions();

	void DrawGridInstance(const FIntPoint& GridIndex, EGridInstanceType InstanceType, EGridInstanceActivityType ActivityType);

	void LoadVisualData();
	
public:
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Data")
	UGridGenerationData* GridData;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint SelectedIndex {-1};

	UPROPERTY(BlueprintReadOnly)
	FIntPoint HoveringIndex {-1};

	UPROPERTY(EditDefaultsOnly)
	UDataTable* GridVisualDataTable {nullptr};

	TMap<EGridInstanceType,
		TMap<EGridInstanceActivityType, FGridVisualState>> GridStyleMap;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent {nullptr};


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid Generation")
	UBoxComponent* GridArea {nullptr};

	UPROPERTY()
	FVector2f BottomLeft {0};
	FVector2f TopRight {0};
	FVector2D Dimension;


	const float RayTraceHeight = 1000.0f;

	UPROPERTY()
	TMap<FIntPoint, FGridState> GridStateMap;

	UPROPERTY()
	TMap<FIntPoint, FGridInstanceState> GridInstanceMap;

	UPROPERTY()
	bool bIsGridGenerated { false };

	mutable bool bIsDebugGridActive {false};
	
	uint16 CurrentMaxInstancedMeshIndex {0};

	FIntPoint HoveringGridIndex {-1};
};
