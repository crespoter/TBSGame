// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TBSCharacter.h"
#include "GameFramework/Actor.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridActor.generated.h"

class ATBSGameState;
class UBoxComponent;
class ACombatSituation;
class UGridStateComponent;


/*
TODO:
	Split this class into
	1. GridDataComponent
	2. GridActor
	3. GridVisualComponent
*/

UCLASS()
class TBSTEMPLATE_API AGridActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridActor();

	// Generates the grid.
	UFUNCTION()
	void GenerateGrid();

	UFUNCTION(BlueprintCallable)
	FIntPoint GetIndexFromLocation(const FVector2f& GridLocation) const;

	FIntPoint GetIndexFromLocation(const FVector& GridLocation) const;

	void SetCharacterUnitAtIndex(const FIntPoint& GridIndex, ATBSCharacter* Character);

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void DrawDebugGrid();

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void ClearGridUnits();

	UFUNCTION(BlueprintCallable)
	FVector GetWorldLocationFromIndex(const FIntPoint& Idx) const;

	/**
	 * @brief Checks if given index is valid for the grid actor. Returns false
	 * for out of bounds and blocked indexes
	 * @param GridIndex Grid index to be checked
	 */
	UFUNCTION(BlueprintCallable)
	bool IsValidIndex(const FIntPoint &GridIndex) const;
	
	

	UFUNCTION(BlueprintCallable)
	void ActivateDeploymentGrid(const ACombatSituation* CurrentCombatSituation);

	UFUNCTION(BlueprintCallable)
	void HandleHoverOnGrid(const FIntPoint& GridIndex);

	void HandleGridSelect(const FIntPoint& Index);

	bool IsGridUnitSelectable(const FIntPoint& Index) const;

	bool IsGridUnitSelectable(const FIntPoint& Index, const FGridState* GridInstanceState) const;

	bool IsGridIndexHoverable(const FIntPoint& Index);

	UFUNCTION()
	void DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx);

private:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetInstancedMeshGridColors(const uint16 MeshInstanceIndex,
		const FColor &EdgeColor, const FColor &BackgroundColor);


	void DrawGridInstance(const FIntPoint& GridIndex, EGridInstanceType InstanceType, EGridInstanceActivityType ActivityType);


	void SetGridAsActive(const FIntPoint& Index);

	void SetGridAsDefault(const FIntPoint& Index);

	void ResetActiveGrid();

	void ResetHoveringGrid();
	
	void CalculateDimensions();


	UPROPERTY(EditDefaultsOnly)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent {nullptr};

	UPROPERTY(EditDefaultsOnly)
	UGridStateComponent* GridStateComponent {nullptr};

	UPROPERTY(EditAnywhere, Category = "Grid Generation")
	UBoxComponent* GridArea {nullptr};

	UPROPERTY(EditDefaultsOnly, Category="Grid Generation")
	float RayTraceHeight = 1000.0f;
	

	UPROPERTY()
	bool bIsGridGenerated { false };

	mutable bool bIsDebugGridActive {false};
	
	uint16 CurrentMaxInstancedMeshIndex {0};
	
	UPROPERTY()
	ATBSGameState* GameState {nullptr};

	FVector2f BottomLeft {0};
	FVector2f TopRight {0};
	FVector2D Dimension;

};
