// Fill out your copyright notice in the Description page of Project Settings.
#include "GridActor.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridSystem/GridBlockerVolume/GridBlockerVolume.h"
#include "Components/BoxComponent.h"
#include "CombatSituation/CombatSituation.h"
#include "CombatSituation/DeploymentZone/DeploymentZone.h"
#include "Data/GridVisualData/GridVisualData.h"
#include "Game/TBSGameState.h"
#include "Player/HeroCharacter/HeroCharacter.h"

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
	ClearGridUnits();
	GenerateGrid();
	LoadVisualData();
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
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

void AGridActor::DrawGridInstance(const FIntPoint& GridIndex, const EGridInstanceType InstanceType, const EGridInstanceActivityType ActivityType)
{
	const FGridVisualState VisualState = GridStyleMap.FindChecked(InstanceType)
		.FindChecked(ActivityType);
	
	uint16 MeshInstanceIndex = 0;	
	if (FGridInstanceState* GridInstanceState = GridInstanceMap.Find(GridIndex))
	{
		GridInstanceState->InstanceType = InstanceType;
		MeshInstanceIndex = GridInstanceState->MeshInstanceIndex;
	}
	else
	{
		FTransform MeshTransform;
		const FVector GridUnitLocation = GetWorldLocationFromIndex(GridIndex);
		MeshTransform.SetLocation(GridUnitLocation);
		MeshTransform.SetScale3D(FVector(GridData->GridUnitScale.X, GridData->GridUnitScale.Y, 1.0f));
		InstancedStaticMeshComponent->AddInstance(MeshTransform, true);
		GridInstanceMap.Emplace(GridIndex, {
			InstanceType,
			ActivityType,
			CurrentMaxInstancedMeshIndex
		});
		MeshInstanceIndex = CurrentMaxInstancedMeshIndex;
	}
	
	SetInstancedMeshGridColors(MeshInstanceIndex,
		VisualState.GridColorData.EdgeColor,
		VisualState.GridColorData.BackgroundColor
	);
	
	CurrentMaxInstancedMeshIndex++;
}

void AGridActor::LoadVisualData()
{
	check(GridVisualDataTable);
	TArray<FGridVisualDataRow*> VisualDataRows;
	GridVisualDataTable->GetAllRows<FGridVisualDataRow>(
		TEXT(""), VisualDataRows);
	
	check(VisualDataRows.Num() != 0);

	for (const FGridVisualDataRow* DataRow : VisualDataRows)
	{
		TMap<EGridInstanceActivityType, FGridVisualState>* ActivityStyleMap
			= GridStyleMap.Find(DataRow->GridInstanceType);
		if (!ActivityStyleMap)
		{
			GridStyleMap.Emplace(DataRow->GridInstanceType, {});
			ActivityStyleMap = GridStyleMap.Find(DataRow->GridInstanceType);
		}
		ActivityStyleMap->Emplace(DataRow->ActivityType, DataRow->GridVisualState);
	}
}

void AGridActor::SetGridAsActive(const FIntPoint& Index)
{
	FGridInstanceState* GridInstanceState = GridInstanceMap.Find(Index);
	if (Index == HoveringGridIndex)
	{
		ResetHoveringGrid();
	}
	if (ActiveGridIndex.X != -1)
	{
		ResetActiveGrid();
	}
	check(GridInstanceState);
	GridInstanceState->ActivityType = EGridInstanceActivityType::Active;
	DrawGridInstance(Index, GridInstanceState->InstanceType, EGridInstanceActivityType::Active);
	ActiveGridIndex = Index;
}

void AGridActor::SetGridAsDefault(const FIntPoint& Index)
{
	FGridInstanceState* GridInstanceState = GridInstanceMap.Find(Index);
	check(GridInstanceState);
	GridInstanceState->ActivityType = EGridInstanceActivityType::None;
	DrawGridInstance(Index, GridInstanceState->InstanceType, EGridInstanceActivityType::None);
}

void AGridActor::ResetActiveGrid()
{
	if (ActiveGridIndex.X == -1)
	{
		return;
	}
	
	FGridInstanceState* GridInstanceState =
		GridInstanceMap.Find(ActiveGridIndex);

	if (GridInstanceState)
	{
		GridInstanceState->ActivityType = EGridInstanceActivityType::None;
	}
	DrawGridInstance(ActiveGridIndex, GridInstanceState->InstanceType,
		GridInstanceState->ActivityType);
	
	ActiveGridIndex = {-1, -1};
}

void AGridActor::ResetHoveringGrid()
{
	if (HoveringGridIndex.X == -1)
	{
		return;
	}
	
	FGridInstanceState* GridInstanceState =
		GridInstanceMap.Find(HoveringGridIndex);

	if (GridInstanceState)
	{
		GridInstanceState->ActivityType = EGridInstanceActivityType::None;
	}

	DrawGridInstance(HoveringGridIndex, GridInstanceState->InstanceType,
		GridInstanceState->ActivityType);
	
	HoveringGridIndex = {-1, -1};
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
	GridStateMap.Reset();
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
						GridState.Height = HitResult.ImpactPoint.Z + 5.0f;
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
				GridStateMap.Add(FIntPoint(i, j), GridState);
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

FIntPoint AGridActor::GetIndexFromLocation(const FVector& GridLocation) const
{
	const FVector2f GridLocation2D = FVector2f(GridLocation.X, GridLocation.Y);
	return GetIndexFromLocation(GridLocation2D);
}

void AGridActor::SetCharacterUnitAtIndex(const FIntPoint& GridIndex, ATBSCharacter* Character)
{
	FGridInstanceState* GridInstanceState = GridInstanceMap.Find(GridIndex);
	check(GridInstanceState);
	GridInstanceState->OccupyingUnit = Character;
}

FVector AGridActor::GetWorldLocationFromIndex(const FIntPoint& Idx) const
{
	const FVector2f MapLocation = BottomLeft + FVector2f(Idx.X, Idx.Y) * GridData->GridUnitSize;
	
	return FVector(MapLocation.X, MapLocation.Y, GridStateMap.Find(Idx)->Height);
}


bool AGridActor::IsValidIndex(const FIntPoint &GridIndex) const
{
	if (GridIndex.X >= 0 && GridIndex.X < Dimension.X &&
		GridIndex.Y >= 0 && GridIndex.Y < Dimension.Y)
	{
		const FGridState GridState = GridStateMap.FindChecked(GridIndex);
		return GridState.GridAccessState == EGridAccessState::Accessible;
	}
	return false;
}

void AGridActor::DrawDebugGrid()
{
	LoadVisualData();
	InstancedStaticMeshComponent->ClearInstances();

	InstancedStaticMeshComponent->SetStaticMesh(GridData->GridUnitMesh);

	uint16 MeshInstanceIndex = 0;
	for (uint16 i = 0; i < Dimension.X; i++)
	{
		for (uint16 j = 0; j < Dimension.Y; j++)
		{
			FTransform MeshTransform;
			FVector GridUnitLocation = GetWorldLocationFromIndex(FIntPoint(i, j));
			const FGridState* GridState = GridStateMap.Find(FIntPoint(i, j));

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
			
			GridInstanceMap.Emplace(FIntPoint(i, j), {
				EGridInstanceType::DebugGrid,
				EGridInstanceActivityType::None,
				MeshInstanceIndex
			});
			MeshInstanceIndex++;
		}
	}
	bIsDebugGridActive = true;
}

void AGridActor::ClearGridUnits()
{
	InstancedStaticMeshComponent->ClearInstances();
	GridInstanceMap.Empty();
	bIsDebugGridActive = false;
	CurrentMaxInstancedMeshIndex = 0;
}

void AGridActor::DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx)
{
	ClearGridUnits();
	DrawDebugGrid();
	for (SIZE_T i = BottomLeftIdx.X; i <= TopRightIdx.X; i++)
	{	
		for (SIZE_T j = BottomLeftIdx.Y; j <= TopRightIdx.Y; j++)
		{
			const FGridInstanceState* GridInstanceState =
				GridInstanceMap.Find(FIntPoint(i, j));
			if (GridInstanceState != nullptr)
			{
				DrawGridInstance(FIntPoint(i, j),
					EGridInstanceType::Deployment,
					EGridInstanceActivityType::None);
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
	
	const FIntPoint BottomleftIndex = GetIndexFromLocation(DeploymentBottomLeft);
	const FIntPoint TopRightIndex = GetIndexFromLocation(DeploymentTopRight);
	
	ClearGridUnits();
	
	for (SIZE_T i = BottomleftIndex.X; i <= TopRightIndex.X; i++)
	{
		for (SIZE_T j = BottomleftIndex.Y; j <= TopRightIndex.Y; j++)
		{
			const FGridState* GridState = GridStateMap.Find(FIntPoint(i, j));
			if (GridState->GridAccessState == EGridAccessState::Accessible)
			{
				DrawGridInstance(FIntPoint(i, j),
					EGridInstanceType::Deployment,
					EGridInstanceActivityType::None); 				
			}
		}
	}
	// TODO: Add Characters
	AHeroCharacter* HeroCharacter = GameState->GetMainCharacter();
	check(HeroCharacter);
	HeroCharacter->SetActorLocation(GetWorldLocationFromIndex(TopRightIndex));
	SetCharacterUnitAtIndex(TopRightIndex, HeroCharacter);
}

void AGridActor::HandleHoverOnGrid(const FIntPoint& GridIndex)
{
	if (HoveringGridIndex == GridIndex)
	{
		return;
	}
	
	if (!IsGridIndexHoverable(GridIndex) || GridIndex == ActiveGridIndex)
	{
		ResetHoveringGrid();
		return;
	}
	if (HoveringGridIndex.X != -1)
	{
		ResetHoveringGrid();
	}

	HoveringGridIndex = GridIndex;
	DrawGridInstance(GridIndex, EGridInstanceType::Deployment, EGridInstanceActivityType::Hover);				
}

void AGridActor::HandleGridSelect(const FIntPoint& Index)
{
	const FGridInstanceState* GridInstanceState = GridInstanceMap.Find(Index);
	if (GridInstanceState == nullptr)
	{
		return;
	}
	if (IsGridUnitSelectable(Index, GridInstanceState))
	{
		const EGamePhase GamePhase = GameState->GetCurrentGamePhase();
		switch(GamePhase)
		{
		case EGamePhase::Deployment:
			{
				SetGridAsActive(Index);
				break;
			}
		default: ;
		}
	}
}

bool AGridActor::IsGridUnitSelectable(const FIntPoint& Index) const
{
	const FGridInstanceState* GridInstanceState = GridInstanceMap.Find(Index);
	return GridInstanceState ? IsGridUnitSelectable(Index, GridInstanceState) : false; 
}

bool AGridActor::IsGridUnitSelectable(const FIntPoint& Index, const FGridInstanceState* GridInstanceState) const
{
	if (!IsValidIndex(Index))
	{
		return false;
	}
	
	bool bCanBeSelected = false;
	const EGamePhase GamePhase = GameState->GetCurrentGamePhase();
	switch(GamePhase)
	{
	case EGamePhase::Deployment:
		{
			bCanBeSelected = GridInstanceState->OccupyingUnit &&
				GridInstanceState->ActivityType != EGridInstanceActivityType::Active &&
				GridInstanceState->InstanceType == EGridInstanceType::Deployment;
			break;
		}
	default:;
	}
	return bCanBeSelected;
}

bool AGridActor::IsGridIndexHoverable(const FIntPoint& Index)
{
	const FGridInstanceState* GridInstanceState = GridInstanceMap.Find(Index);

	if (!IsValidIndex(Index) || !GridInstanceState)
	{
		return false;
	}

	bool bCanBeHovered = false;
	const EGamePhase GamePhase = GameState->GetCurrentGamePhase();

	
	
	switch(GamePhase)
	{
	case EGamePhase::Deployment:
		{
			bCanBeHovered = GridInstanceState->ActivityType != EGridInstanceActivityType::Hover
				&& GridInstanceState->InstanceType == EGridInstanceType::Deployment;
			
			break;
		}
	default:;
	}
	return bCanBeHovered;
}
