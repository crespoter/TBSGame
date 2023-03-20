// Fill out your copyright notice in the Description page of Project Settings.
#include "GridActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridSystem/GridBlockerVolume/GridBlockerVolume.h"

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = true;
	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("Instanced Meshes"));
	check(InstancedStaticMeshComponent);
	RootComponent = InstancedStaticMeshComponent;
}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
}

void AGridActor::SetInstancedMeshGridColors(const uint16 MeshInstanceIndex, const FColor& EdgeColor,
	const FColor& BackgroundColor)
{
	/*
	 * 0,1,2 => Edge color
	 * 3,4,5 => Background Color
	 * 6 => Edge Opacity
	 * 7 => Background Opacity
	 */
	const TArray<float> DataValues {
		EdgeColor.R / 255.0f, EdgeColor.G / 255.0f, EdgeColor.B / 255.0f,
		BackgroundColor.R  / 255.0f, BackgroundColor.G  / 255.0f, BackgroundColor.B  / 255.0f,
		EdgeColor.A  / 255.0f, BackgroundColor.A  / 255.0f
	};
	
	for (uint8 i = 0; i < DataValues.Num(); i++)
	{
		InstancedStaticMeshComponent->SetCustomDataValue(MeshInstanceIndex, i, DataValues[i], true);
	}
}

FGridColorData& AGridActor::GetColorDataForState(uint8 StateBitFlag) const
{
	return GridData->GridStateColorMap.FindChecked(
		static_cast<EGridUnitStateCombined>(StateBitFlag));
}


void AGridActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    GenerateGrid();
}


void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridActor::GenerateGrid()
{
	if (UNLIKELY(!GridData))
	{
		UE_LOG(LogGridSystems, Error, TEXT("Grid Data asset not available"));
		return;
	}
	if (LIKELY(GridData->GridUnitMesh))
	{
		InstancedStaticMeshComponent->SetStaticMesh(GridData->GridUnitMesh);
		InstancedStaticMeshComponent->ClearInstances();

		const FIntPoint BottomLeftIndex =
			FIntPoint(GridData->GridDimensions.X / 2,  GridData->GridDimensions.Y / 2);
		
		BottomLeft = FVector2f(BottomLeftIndex.X * -GridData->GridUnitSize.X,
			BottomLeftIndex.Y * -GridData->GridUnitSize.Y);
			
		uint16 MeshInstanceIndex = 0;
		FVector2f CurrentPosition = BottomLeft;
		for (uint8 i = 0; i < GridData->GridDimensions.X; ++i)
		{
			for (uint8 j = 0; j < GridData->GridDimensions.Y; ++j)
			{
				// Ray trace from above the tile to find the correct z value.
				FHitResult HitResult;
				
				
				GetWorld()->LineTraceSingleByChannel(HitResult, FVector(CurrentPosition.X, CurrentPosition.Y, RayTraceHeight),
					FVector(CurrentPosition.X, CurrentPosition.Y, -10.0f), GridData->GridPlacementTraceChannel);

				FGridState GridState;
				GridState.GridIndex = FIntPoint(i, j);

				if (HitResult.bBlockingHit)
				{
					if (!Cast<AGridBlockerVolume>(HitResult.GetActor()))
					{
						FTransform MeshTransform;
						MeshTransform.SetLocation(FVector(CurrentPosition.X, CurrentPosition.Y, 10.0f + HitResult.Location.Z));
						MeshTransform.SetScale3D(FVector(GridData->GridUnitScale.X, GridData->GridUnitScale.Y, 1.0f));
						// InstancedStaticMeshComponent->AddInstance(
						// 	MeshTransform
						// , true);

						GridState.GridAccessState = EGridAccessState::Accessible;
						GridState.Height = HitResult.Location.Z;
						// GridState.MeshInstanceIndex = MeshInstanceIndex++;
						// const FGridColorData GridColorData = GetColorDataForState(static_cast<uint8>(EGridUnitState::Default));
						// SetInstancedMeshGridColors(MeshInstanceIndex - 1, GridColorData.EdgeColor, GridColorData.BackgroundColor);
					}
					else
					{
						GridState.GridAccessState = EGridAccessState::Blocked;
					}
				}
				else
				{
					GridState.GridAccessState = EGridAccessState::OutOfMap;
				}
				GridUnitStateMap.Add(FIntPoint(i, j), GridState);
				CurrentPosition.Y += GridData->GridUnitSize.Y;
			}
			CurrentPosition.X += GridData->GridUnitSize.X;
			CurrentPosition.Y = BottomLeft.Y;
		}
	}
	else
	{
		UE_LOG(LogGridSystems, Error, TEXT("Grid Unit Mesh is not set in Grid Actor data"));
	}
}

FIntPoint AGridActor::GetIndexFromLocation(const FVector2f& GridLocation) const
{
	FVector2f FloatingIndex = (GridLocation - BottomLeft) / GridData->GridUnitSize;
	FloatingIndex += FVector2f(0.5f, 0.5f);
	return FIntPoint(FMath::Floor(FloatingIndex.X), FMath::Floor(FloatingIndex.Y));
}

FVector AGridActor::GetWorldLocationFromIndex(const FIntPoint& Idx) const
{
	const FVector2f MapLocation = BottomLeft + FVector2f(Idx.X, Idx.Y) * GridData->GridUnitSize;

	return FVector(MapLocation.X, MapLocation.Y, GridUnitStateMap.FindChecked(Idx).Height);

}

void AGridActor::SetGridUnitAsSelected(const FIntPoint &GridIndex)
{
	DeselectGrid();
	AddStateToTile(GridIndex, EGridUnitState::Selected);
	SelectedIndex = GridIndex;
}

void AGridActor::SetGridUnitAsHovering(const FIntPoint& GridIndex)
{
	if (GridIndex == HoveringIndex) return;
	UnhoverGridUnit();

	AddStateToTile(GridIndex, EGridUnitState::Hover);
	HoveringIndex = GridIndex;
}

void AGridActor::DeselectGrid()
{
	RemoveStateFromTile(SelectedIndex, EGridUnitState::Selected);
	SelectedIndex.X = SelectedIndex.Y = -1;
}

void AGridActor::UnhoverGridUnit()
{
	RemoveStateFromTile(HoveringIndex, EGridUnitState::Hover);
	HoveringIndex.X = HoveringIndex.Y = -1;
}



void AGridActor::AddStateToTile(const FIntPoint& Index, const EGridUnitState GridStateToAdd)
{
	if (IsValidIndex(Index))
	{
		FGridState GridState = GridUnitStateMap.FindChecked(Index);
		GridState.State = GridState.State | static_cast<uint8>(GridStateToAdd);
		GridUnitStateMap.Add(Index, GridState);
		SetTileVisualToState(Index, GridState.State);
	}
}

void AGridActor::RemoveStateFromTile(const FIntPoint& Index, const EGridUnitState GridStateToAdd)
{
	if (IsValidIndex(Index))
	{
		FGridState GridState = GridUnitStateMap.FindChecked(Index);

		GridState.State &= ~(static_cast<uint8>(GridStateToAdd));
		GridUnitStateMap.Add(Index, GridState);
		SetTileVisualToState(Index, GridState.State);
	}
}

void AGridActor::SetTileVisualToState(const FIntPoint& TileIndex, const EGridUnitState GridState)
{
	SetTileVisualToState(TileIndex, static_cast<uint8>(GridState));
}

void AGridActor::SetTileVisualToState(const FIntPoint& TileIndex, const uint8 GridState)
{
	if (IsValidIndex(TileIndex))
	{
		const FGridColorData GridColorData = GetColorDataForState(GridState);
		const uint16 MeshInstanceIndex = GridUnitStateMap.FindChecked(TileIndex).MeshInstanceIndex;
		SetInstancedMeshGridColors(MeshInstanceIndex, GridColorData.EdgeColor, GridColorData.BackgroundColor);
	}
}

bool AGridActor::IsValidIndex(const FIntPoint &GridIndex) const
{
	return GridIndex.X >= 0 && GridIndex.X < GridData->GridDimensions.X &&
		GridIndex.Y >=0 && GridIndex.Y < GridData->GridDimensions.Y;
}
