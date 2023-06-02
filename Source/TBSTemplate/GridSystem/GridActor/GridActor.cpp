// Fill out your copyright notice in the Description page of Project Settings.
#include "GridActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridSystem/GridBlockerVolume/GridBlockerVolume.h"
#include "Components/BoxComponent.h"
#include "CombatSituation/CombatSituation.h"
#include "CombatSituation/DeploymentZone/DeploymentZone.h"

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root Node");
	check(RootComponent);
		
	GridArea = CreateDefaultSubobject<UBoxComponent>("Box component");
	GridArea->SetupAttachment(RootComponent);
	check(GridArea);

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("Instanced Meshes"));
	InstancedStaticMeshComponent->SetupAttachment(RootComponent);
	check(InstancedStaticMeshComponent);
}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	check(bIsGridGenerated);
	InstancedStaticMeshComponent->SetStaticMesh(GridData->GridUnitMesh);
	ClearGrid();
	GenerateGrid();
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
	const TArray<float> DataValues 
	{
		EdgeColor.R / 255.0f, EdgeColor.G / 255.0f, EdgeColor.B / 255.0f,
		BackgroundColor.R  / 255.0f, BackgroundColor.G  / 255.0f, BackgroundColor.B  / 255.0f,
		EdgeColor.A  / 255.0f, BackgroundColor.A  / 255.0f
	};
	
	for (uint8 i = 0; i < DataValues.Num(); i++)
	{
		InstancedStaticMeshComponent->SetCustomDataValue(MeshInstanceIndex, i, DataValues[i], true);
	}
}

void AGridActor::CalculateDimensions()
{
	FVector2f Bounds;
	Bounds.X = TopRight.X - BottomLeft.X;
	Bounds.Y = TopRight.Y - BottomLeft.Y;
	Dimension.X = FMath::Floor(Bounds.X / GridData->GridUnitSize.X);
	Dimension.Y = FMath::Floor(Bounds.Y / GridData->GridUnitSize.Y);
}

void AGridActor::DrawGridInstance(const FIntPoint& GridIndex, EGridInstanceType InstanceType)
{
	check(GridIndexToMapIndexMap.Find(GridIndex) == nullptr);
	FGridVisualState GridVisual;// = *GridStyleMap.Find(InstanceType);
	GridVisual.GridColorData.EdgeColor = FColor(0, 0, 255, 255);
	GridVisual.GridColorData.BackgroundColor = FColor(0,0,0,0);
	FTransform MeshTransform;
	const FVector GridUnitLocation = GetWorldLocationFromIndex(GridIndex);

	MeshTransform.SetLocation(GridUnitLocation);
	MeshTransform.SetScale3D(FVector(GridData->GridUnitScale.X, GridData->GridUnitScale.Y, 1.0f));
	InstancedStaticMeshComponent->AddInstance(MeshTransform, true);

	SetInstancedMeshGridColors(CurrentMaxInstancedMeshIndex,
		GridVisual.GridColorData.EdgeColor,
		GridVisual.GridColorData.BackgroundColor
	);
	
	GridIndexToMapIndexMap.Add(GridIndex, CurrentMaxInstancedMeshIndex);
	CurrentMaxInstancedMeshIndex++;
}

void AGridActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}


void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGridActor::GenerateGrid()
{
	check(GridArea);
	FVector Center = GetActorLocation();
	FVector Bounds = GridArea->GetScaledBoxExtent();
	BottomLeft = FVector2f(Center.X, Center.Y) - FVector2f(Bounds.X, Bounds.Y); 
	TopRight = FVector2f(Center.X, Center.Y) + FVector2f(Bounds.X, Bounds.Y);

	CalculateDimensions();
	GridUnitStateMap.Reset();
	if (UNLIKELY(!GridData))
	{
		UE_LOG(LogGridSystems, Error, TEXT("Grid Data asset not available"));
		return;
	}
	if (LIKELY(GridData->GridUnitMesh))
	{
		// Center the bottom left on the bottom left tile
		BottomLeft = BottomLeft  + GridData->GridUnitSize / 2.0f;
		TopRight = TopRight - GridData->GridUnitSize / 2.0f;
		
		FVector2f CurrentPosition = BottomLeft;
		for (uint16 i = 0; i < Dimension.X; ++i)
		{
			for (uint16 j = 0; j < Dimension.Y; ++j)
			{
				// Ray trace from above the tile to find the correct z value.
				FHitResult HitResult;
				
				
				GetWorld()->LineTraceSingleByChannel(HitResult, FVector(CurrentPosition.X, CurrentPosition.Y, RayTraceHeight),
					FVector(CurrentPosition.X, CurrentPosition.Y, -RayTraceHeight), GridData->GridPlacementTraceChannel);

				FGridState GridState;

				if (HitResult.bBlockingHit)
				{
					if (!Cast<AGridBlockerVolume>(HitResult.GetActor()))
					{

						GridState.GridAccessState = EGridAccessState::Accessible;
						GridState.Height = HitResult.ImpactPoint.Z;
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
	bIsGridGenerated = true;
	if (bIsDebugGridActive)
	{
		DrawDebugGrid();
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
	
	return FVector(MapLocation.X, MapLocation.Y, GridUnitStateMap.Find(Idx)->Height);
}


bool AGridActor::IsValidIndex(const FIntPoint &GridIndex) const
{
	return GridIndex.X >= 0 && GridIndex.X < Dimension.X &&
		GridIndex.Y >= 0 && GridIndex.Y < Dimension.Y;
}

void AGridActor::DrawDebugGrid()
{
	// ActivateDeploymentGrid();

	InstancedStaticMeshComponent->ClearInstances();

	InstancedStaticMeshComponent->SetStaticMesh(GridData->GridUnitMesh);

	uint16 MeshInstanceIndex = 0;
	for (uint16 i = 0; i < Dimension.X; i++)
	{
		for (uint16 j = 0; j < Dimension.Y; j++)
		{
			FTransform MeshTransform;
			FVector GridUnitLocation = GetWorldLocationFromIndex(FIntPoint(i, j));
			const FGridState* GridState = GridUnitStateMap.Find(FIntPoint(i, j));

			if (GridState->GridAccessState != EGridAccessState::Accessible)
			{
				GridUnitLocation.Z += 10.0f;
			}
			GridUnitLocation.Z += 5.0f;
			MeshTransform.SetLocation(GridUnitLocation);
			MeshTransform.SetScale3D(FVector(GridData->GridUnitScale.X, GridData->GridUnitScale.Y, 1.0f));
			InstancedStaticMeshComponent->AddInstance(MeshTransform, true);

			FGridColorData GridColorData;
			GridColorData.BackgroundColor = FColor(0, 0, 0, 0);
			if (!GridState)
			{
				GridColorData.EdgeColor = FColor(255, 0 ,0, 255);
				GridColorData.BackgroundColor = FColor(255, 0, 0, 255);
			}
			else
			{
				switch(GridState->GridAccessState)
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
			GridIndexToMapIndexMap.Add(FIntPoint(i, j), MeshInstanceIndex);
			MeshInstanceIndex++;
		}
	}
	bIsDebugGridActive = true;
}

void AGridActor::ClearGrid()
{
	InstancedStaticMeshComponent->ClearInstances();
	GridIndexToMapIndexMap.Empty();
	bIsDebugGridActive = false;
	CurrentMaxInstancedMeshIndex = 0;
}

void AGridActor::DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx)
{
	ClearGrid();
	DrawDebugGrid();
	FGridColorData DeploymentColorData;
	DeploymentColorData.BackgroundColor = FColor(0, 0, 0, 0);
	DeploymentColorData.EdgeColor = FColor(0, 0, 255, 255);
	for (SIZE_T i = BottomLeftIdx.X; i <= TopRightIdx.X; i++)
	{	
		for (SIZE_T j = BottomLeftIdx.Y; j <= TopRightIdx.Y; j++)
		{
			const uint16* IndexRef = GridIndexToMapIndexMap.Find(FIntPoint(i, j));
			if (IndexRef != nullptr)
			{
				SetInstancedMeshGridColors(*IndexRef, DeploymentColorData.EdgeColor, DeploymentColorData.BackgroundColor);
			}
		}
	}
}


void AGridActor::ActivateDeploymentGrid(const ACombatSituation* CurrentCombatSituation)
{
	FVector2f DeploymentBottomLeft;
	FVector2f DeploymentTopRight;
	CurrentCombatSituation->GetDeploymentZone()->GetDeploymentBounds(
		DeploymentBottomLeft, DeploymentTopRight);
	
	const FIntPoint DeploymentBottomLeftIndex = GetIndexFromLocation(DeploymentBottomLeft);
	const FIntPoint DeploymentTopRightIndex = GetIndexFromLocation(DeploymentTopRight);
	
	ClearGrid();
	
	for (int32 i = DeploymentBottomLeftIndex.X; i < DeploymentTopRightIndex.X; i++)
	{
		for (int32 j = DeploymentBottomLeftIndex.Y; j < DeploymentTopRightIndex.Y; j++)
		{
			const FGridState* GridState = GridUnitStateMap.Find(FIntPoint(i, j));
			if (GridState->GridAccessState == EGridAccessState::Accessible)
			{
				UE_LOG(LogTemp, Error, TEXT("DRAWING GRID !!!!"));
				DrawGridInstance(FIntPoint(i, j), EGridInstanceType::Deployment);
				
			}
		}
	}
}