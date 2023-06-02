// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridActor.generated.h"

class ATBSGameState;
class UBoxComponent;
class ACombatSituation;


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
	bool IsValidIndex(const FIntPoint &GridIndex) const;

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void DrawDebugGrid();

	UFUNCTION(Exec, CallInEditor, Category = "Grid")
	void ClearGrid();
	
	UFUNCTION()
	void DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx);
	
	UFUNCTION(BlueprintCallable)
	void ActivateDeploymentGrid(const ACombatSituation* CurrentCombatSituation);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetInstancedMeshGridColors(const uint16 MeshInstanceIndex,
		const FColor &EdgeColor, const FColor &BackgroundColor);

	void CalculateDimensions();

	void DrawGridInstance(const FIntPoint& GridIndex, EGridInstanceType InstanceType);

public:
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Data")
	UGridGenerationData* GridData;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint SelectedIndex {-1};

	UPROPERTY(BlueprintReadOnly)
	FIntPoint HoveringIndex {-1};

	UPROPERTY(EditDefaultsOnly)
	TMap<EGridInstanceType, FGridVisualState> GridStyleMap;

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
	bool bIsGridGenerated { false };

	mutable bool bIsDebugGridActive {false};
	
	uint16 CurrentMaxInstancedMeshIndex {0};

	UPROPERTY()
	TMap<FIntPoint, uint16> GridIndexToMapIndexMap;
};
