// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridActor/GridComponents/GridVisualComponent.h"

#include "EngineUtils.h"
#include "GridStateComponent.h"
#include "CombatSituation/DeploymentZone/DeploymentZone.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridSystem/GridActor/GridActor.h"

void UGridVisualComponent::Initialize(UGridStateComponent* InGridStateComponent, UInstancedStaticMeshComponent* InGridInstancedMesh,
                                      AGridActor* InOwningGridActor)
{
	GridStateComponent = InGridStateComponent;
	GridInstancedMesh = InGridInstancedMesh;
	OwningGridActor = InOwningGridActor;
	check(GridStateComponent);
	check(InGridInstancedMesh);
	check(InOwningGridActor);
	bIsInitialized = true;
}

void UGridVisualComponent::DrawDebugGrid()
{
	GridInstancedMesh->ClearInstances();
	GridInstancedMesh->SetStaticMesh(GridStateComponent->GetGridUnitMesh());
	
	uint16 MeshInstanceIndex = 0;
	const FVector2D Dimension = OwningGridActor->GetDimensions();
	for (uint16 i = 0; i < Dimension.X; i++)
	{
		for (uint16 j = 0; j < Dimension.Y; j++)
		{
			FTransform MeshTransform;
			FVector GridUnitLocation = OwningGridActor->GetWorldLocationFromIndex(FIntPoint(i, j));
			FGridState GridInstanceState;
			const bool bIsGridStateFound = GridStateComponent->GetGridUnitState(FIntPoint(i, j), GridInstanceState);
			if (GridInstanceState.GridAccessState != EGridAccessState::Accessible)
			{
				GridUnitLocation.Z += 10.0f;
			}
			
			GridUnitLocation.Z += 5.0f;
			MeshTransform.SetLocation(GridUnitLocation);
			MeshTransform.SetScale3D(FVector(GridStateComponent->GetGridUnitScale().X, GridStateComponent->GetGridUnitScale().Y, 1.0f));
			GridInstancedMesh->AddInstance(MeshTransform, true);

			FGridColorData GridColorData;
			GridColorData.BackgroundColor = FColor(0, 0, 0, 0);
			// TODO: Use debug color info from data file instead of hardcoded colors
			if (!bIsGridStateFound)
			{
				GridColorData.EdgeColor = FColor(255, 0 ,0, 255);
				GridColorData.BackgroundColor = FColor(255, 0, 0, 255);
			}
			else
			{
				switch(GridInstanceState.GridAccessState)
				{
				case EGridAccessState::Accessible:
					{
						GridColorData.EdgeColor = FColor(0, 255 ,0, 255);
						break;
					}
				case EGridAccessState::Blocked:
					{
						GridColorData.EdgeColor = FColor(255, 0 ,0, 255);
						break;
					}
				case EGridAccessState::OutOfMap:
					{
						GridColorData.EdgeColor = FColor(0, 0 ,0, 255);
						break;
					}
				default: ;
				}
			}
			SetInstancedMeshGridColors(MeshInstanceIndex, GridColorData.EdgeColor, GridColorData.BackgroundColor);
			
			GridStateComponent->AddGridState(FIntPoint(i, j), {
				EGridInstanceType::DebugGrid,
				EGridInstanceActivityType::None
			});
			MeshInstanceMap.Emplace(FIntPoint(i,j), MeshInstanceIndex);
			MeshInstanceIndex++;
		}
	}
	
	for (TActorIterator<ADeploymentZone> ActorItr(GetWorld()); ActorItr ;++ActorItr)
	{
		FVector2f DeploymentBottomLeft, DeploymentTopRight;
		ActorItr->GetDeploymentBounds(DeploymentBottomLeft, DeploymentTopRight);
		
		const FIntPoint BottomLeftIdx = OwningGridActor->GetIndexFromLocation(DeploymentBottomLeft);
		const FIntPoint TopRightIdx = OwningGridActor->GetIndexFromLocation(DeploymentTopRight);
		DrawDebugDeploymentZone(BottomLeftIdx, TopRightIdx);
	}
}

void UGridVisualComponent::DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx)
{
	for (SIZE_T i = BottomLeftIdx.X; i <= TopRightIdx.X; i++)
	{	
		for (SIZE_T j = BottomLeftIdx.Y; j <= TopRightIdx.Y; j++)
		{

			OwningGridActor->UpdateGridState(FIntPoint(i, j),
				EGridInstanceType::Deployment,
				EGridInstanceActivityType::None);
			// While on editor, the events may not be registered, so updating the visuals directly
			FGridState GridState;
			if (GridStateComponent->GetGridUnitState(FIntPoint(i, j), GridState))
			{
				UpdateVisualState(GridState, FIntPoint(i,j), EUpdateOpType::Updated);
			}
		}
	}
}

#if WITH_EDITORONLY_DATA

void UGridVisualComponent::ResetEditorDebugGrid()
{
	Reset();
}

void UGridVisualComponent::UpdateVisualState(const FGridState& GridState, const FIntPoint& Index,
	const EUpdateOpType UpdateOp)
{
	OnVisualStateChanged(GridState, Index, UpdateOp);
}

#endif

void UGridVisualComponent::BeginPlay()
{
	Super::BeginPlay();
	check(bIsInitialized);
	GridInstancedMesh->SetStaticMesh(GridStateComponent->GetGridUnitMesh());
	GridStateComponent->GridVisualStateUpdatedDelegate.AddDynamic(this, &UGridVisualComponent::OnVisualStateChanged);
	Reset();
}

void UGridVisualComponent::OnVisualStateChanged(const FGridState& GridState, const FIntPoint& Index,
	const EUpdateOpType UpdateOp)
{
	if (UpdateOp == EUpdateOpType::Reset)
	{
		Reset();
		return;
	}
	const uint16* MeshInstanceIndexRef = MeshInstanceMap.Find(Index);
	const FGridVisualState* VisualState = GridStateComponent->GetGridVisualState(GridState.GetInstanceType(), GridState.GetActivityType());
	if (VisualState)
	{
		uint16 MeshInstanceIndex;
		
		if (!MeshInstanceIndexRef)
		{
			FTransform MeshTransform;
			const FVector GridUnitLocation = OwningGridActor->GetWorldLocationFromIndex(Index);
			MeshTransform.SetLocation(GridUnitLocation);
			MeshTransform.SetScale3D(FVector(GridStateComponent->GetGridUnitScale().X, GridStateComponent->GetGridUnitScale().Y, 1.0f));
			GridInstancedMesh->AddInstance(MeshTransform, true);
			MeshInstanceMap.Add(Index, CurrentMaxInstancedMeshIndex);
			MeshInstanceIndex = CurrentMaxInstancedMeshIndex;
			CurrentMaxInstancedMeshIndex++;
		}
		else
		{
			MeshInstanceIndex = *MeshInstanceIndexRef;
		}
		SetInstancedMeshGridColors(MeshInstanceIndex,
			VisualState->GridColorData.EdgeColor,
			VisualState->GridColorData.BackgroundColor
		);
	}
}

void UGridVisualComponent::Reset()
{
	MeshInstanceMap.Reset();
	GridInstancedMesh->ClearInstances();
	CurrentMaxInstancedMeshIndex = 0;
}

void UGridVisualComponent::SetInstancedMeshGridColors(const uint16 MeshInstanceIndex, const FColor& EdgeColor,
	const FColor& BackgroundColor)
{
	/*
	 * 0,1,2 => Edge color
	 * 3,4,5 => Background Color
	 * 6 => Edge Opacity
	 * 7 => Background Opacity
	 */
	const TArray<float> DataValues 
	{
		EdgeColor.R / 255.0f, EdgeColor.G / 255.0f, EdgeColor.B / 255.0f,
		BackgroundColor.R  / 255.0f, BackgroundColor.G  / 255.0f, BackgroundColor.B  / 255.0f,
		EdgeColor.A  / 255.0f, BackgroundColor.A  / 255.0f
	};
	
	for (uint8 i = 0; i < DataValues.Num(); i++)
	{
		GridInstancedMesh->SetCustomDataValue(MeshInstanceIndex, i, DataValues[i], true);
	}
}