// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridActor.generated.h"

class ATBSGameState;
class UBoxComponent;

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

	UFUNCTION(BlueprintCallable)
	void SetGridUnitAsSelected(const FIntPoint &GridIndex);

	UFUNCTION(BlueprintCallable)
	void SetGridUnitAsHovering(const FIntPoint &GridIndex);
	
	UFUNCTION(BlueprintCallable)
	void DeselectGrid();

	UFUNCTION(BlueprintCallable)
	void UnhoverGridUnit();

	UFUNCTION(BlueprintCallable)
	bool IsValidIndex(const FIntPoint &GridIndex) const;

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void DrawDebugGrid();

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void ClearGrid();

	UFUNCTION()
	void DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx);

	/**
	 * Updates the visual state of a grid unit. Use RenderVisualGrid method to render as per the state
	*/
	UFUNCTION(BlueprintCallable)
	void UpdateVisualGridState(FIntPoint& Index, const FVisibleGridState& InVisibleGridState);

	UFUNCTION(BlueprintCallable)
	void ClearVisualGridState();

	
	UFUNCTION(BlueprintCallable)
	void ActivateDeploymentGrid();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetInstancedMeshGridColors(const uint16 MeshInstanceIndex,
		const FColor &EdgeColor, const FColor &BackgroundColor);

	void CalculateDimensions();


public:
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Data")
	UGridGenerationData* GridData;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint SelectedIndex {-1};

	UPROPERTY(BlueprintReadOnly)
	FIntPoint HoveringIndex {-1};

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
	TMap<FIntPoint, FGridState> GridUnitStateMap;

	UPROPERTY()
	TMap<FIntPoint, FVisibleGridState> VisibleGridStateMap;

	UPROPERTY()
	bool bIsGridGenerated { false };

	mutable bool bIsDebugGridActive {false};

	UPROPERTY()
	TMap<FIntPoint, uint16> GridIndexToMapIndexMap;
};
