// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridVisualComponent.generated.h"

class UGridStateComponent;
class UInstancedStaticMeshComponent;
class AGridActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TBSGAME_API UGridVisualComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridVisualComponent() = default;

	void Initialize(UGridStateComponent* InGridStateComponent,
		UInstancedStaticMeshComponent* InGridInstancedMesh, AGridActor* InOwningGridActor);

#if WITH_EDITORONLY_DATA
	void ResetEditorDebugGrid();
	void UpdateVisualState(const FGridState& GridState, const FIntPoint& Index, const EUpdateOpType UpdateOp);
#endif

	void DrawDebugGrid();

	void DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	
private:
	UPROPERTY()
	UGridStateComponent* GridStateComponent {nullptr};

	UPROPERTY()
	AGridActor* OwningGridActor {nullptr};

	UPROPERTY()
	UInstancedStaticMeshComponent* GridInstancedMesh {nullptr};

	UFUNCTION()
	void OnVisualStateChanged(const FGridState& GridState, const FIntPoint& Index, const EUpdateOpType UpdateOp);

	void Reset();
	
	void SetInstancedMeshGridColors(const uint16 MeshInstanceIndex,
		const FColor &EdgeColor, const FColor &BackgroundColor);


	TMap<FIntPoint, uint16> MeshInstanceMap;

	bool bIsInitialized {false};
	
	uint16 CurrentMaxInstancedMeshIndex {0};

};
