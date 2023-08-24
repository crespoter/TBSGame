// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridSystem/GridSystemTypes.h"
#include "Game/TBSTypes.h"
#include "GridActor.generated.h"

class ATBSGameState;
class UBoxComponent;
class ACombatSituation;
class UGridStateComponent;
class UGridVisualComponent;
class UGridAction;


/**
 * -Only one grid can be active at a time
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

	FORCEINLINE FVector2D GetDimensions() const;
	
	FORCEINLINE const UGridStateComponent* GetGridStateComponent() const;

	FORCEINLINE UGridStateComponent* GetGridStateComponent();
	

	void StartGridAction(const FIntPoint& GridIndex, ATBSCharacter* InstigatingCharacter, UGridAction* GridAction);

private:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void CalculateDimensions();

	UFUNCTION()
	void OnActionFinished(const EActionExecutionStatus ExecutionStatus);

	UFUNCTION()
	void OnGamePhaseChanged(const EGamePhase NewGamePhase);

	UPROPERTY(EditDefaultsOnly)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent {nullptr};

	UPROPERTY(EditDefaultsOnly)
	UGridStateComponent* GridStateComponent {nullptr};

	UPROPERTY(EditDefaultsOnly)
	UGridVisualComponent* GridVisualComponent {nullptr};

	UPROPERTY(EditAnywhere, Category = "Grid Generation")
	UBoxComponent* GridArea {nullptr};

	UPROPERTY(EditDefaultsOnly, Category="Grid Generation")
	float RayTraceHeight = 1000.0f;
	

	UPROPERTY()
	bool bIsGridGenerated { false };
	
	
	UPROPERTY()
	ATBSGameState* GameState {nullptr};

	FVector2f BottomLeft {0};
	FVector2f TopRight {0};
	FVector2D Dimension;

	UPROPERTY()
	UGridAction* CurrentGridAction {nullptr};
};
