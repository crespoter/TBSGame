// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridActor.generated.h"



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
	UFUNCTION(CallInEditor, BlueprintCallable, Category="Grid Generation")
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


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetInstancedMeshGridColors(const uint16 MeshInstanceIndex,
		const FColor &EdgeColor, const FColor &BackgroundColor);

	FGridColorData& GetColorDataForState(uint8 StateBitFlag) const;

	UFUNCTION(BlueprintCallable)
	void AddStateToTile(const FIntPoint& Index, const EGridUnitState GridStateToAdd);
	
	UFUNCTION(BlueprintCallable)
	void RemoveStateFromTile(const FIntPoint& Index, const EGridUnitState GridStateToAdd);

	UFUNCTION(BlueprintCallable)
void SetTileVisualToState(const FIntPoint& TileIndex, const EGridUnitState GridState);

	void SetTileVisualToState(const FIntPoint& TileIndex, const uint8 GridState);

	
public:
	virtual void PostInitializeComponents() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Grid Data")
	UGridGenerationData* GridData;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint SelectedIndex {-1};

	UPROPERTY(BlueprintReadOnly)
	FIntPoint HoveringIndex {-1};

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent {nullptr};

	UPROPERTY()
	FVector2f BottomLeft {0};

	const float RayTraceHeight = 500.0f;

	UPROPERTY()
	TMap<FIntPoint, FGridState> GridUnitStateMap;
	
};
