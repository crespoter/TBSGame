// Fill out your copyright notice in the Description page of Project Settings.
#include "GridActor.h"

#include "EngineUtils.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridSystem/GridBlockerVolume/GridBlockerVolume.h"
#include "Components/BoxComponent.h"
#include "CombatSituation/CombatSituation.h"
#include "CombatSituation/DeploymentZone/DeploymentZone.h"
#include "Data/GridVisualData/GridVisualData.h"
#include "Game/TBSGameState.h"
#include "GridComponents/GridStateComponent.h"
#include "Player/HeroCharacter/HeroCharacter.h"

AGridActor::AGridActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root Node");
	check(RootComponent);
		
	GridArea = CreateDefaultSubobject<UBoxComponent>("Box component");
	GridArea->SetupAttachment(RootComponent);
	check(GridArea);

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("Instanced Meshes"));
	InstancedStaticMeshComponent->SetupAttachment(RootComponent);
	check(InstancedStaticMeshComponent);

	GridStateComponent = CreateDefaultSubobject<UGridStateComponent>("Grid Data Component");
	check(GridStateComponent);
}

void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	check(bIsGridGenerated);
	InstancedStaticMeshComponent->SetStaticMesh(GridStateComponent->GetGridUnitMesh());
	ClearGridUnits();
	GenerateGrid();
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
	Dimension.X = FMath::Floor(Bounds.X / GridStateComponent->GetGridUnitSize().X);
	Dimension.Y = FMath::Floor(Bounds.Y / GridStateComponent->GetGridUnitSize().Y);
}

void AGridActor::DrawGridInstance(const FIntPoint& GridIndex, const EGridInstanceType InstanceType, const EGridInstanceActivityType ActivityType)
{
	const FGridVisualState* VisualState = GridStateComponent->GetGridVisualState(InstanceType, ActivityType);

	check(VisualState);
	uint16 MeshInstanceIndex;
	const FGridState* GridInstanceState = GridStateComponent->GetGridUnitState(GridIndex);

	if (!GridInstanceState)
	{
		return;
	}
	FGridState NewGridState = *GridInstanceState;

	if (GridInstanceState->bIsUnitRendered)
	{
		NewGridState.SetInstanceType(InstanceType);
		MeshInstanceIndex = GridInstanceState->MeshInstanceIndex;
	}
	else
	{
		FTransform MeshTransform;
		const FVector GridUnitLocation = GetWorldLocationFromIndex(GridIndex);
		MeshTransform.SetLocation(GridUnitLocation);
		MeshTransform.SetScale3D(FVector(GridStateComponent->GetGridUnitScale().X, GridStateComponent->GetGridUnitScale().Y, 1.0f));
		InstancedStaticMeshComponent->AddInstance(MeshTransform, true);
	
		NewGridState.SetInstanceType(InstanceType);
		NewGridState.SetActivityType(ActivityType);
		NewGridState.MeshInstanceIndex = CurrentMaxInstancedMeshIndex;

		MeshInstanceIndex = CurrentMaxInstancedMeshIndex;
		CurrentMaxInstancedMeshIndex++;
	}
	GridStateComponent->AddGridState(GridIndex, NewGridState);

	
	SetInstancedMeshGridColors(MeshInstanceIndex,
		VisualState->GridColorData.EdgeColor,
		VisualState->GridColorData.BackgroundColor
	);
}



void AGridActor::SetGridAsActive(const FIntPoint& Index)
{
	FGridState NewGridInstanceState = *GridStateComponent->GetGridUnitState(Index);
	if (Index == GridStateComponent->GetHoveringGridIndex())
	{
		ResetHoveringGrid();
	}
	if (GridStateComponent->IsActiveIndexSet())
	{
		ResetActiveGrid();
	}
	
	NewGridInstanceState.SetActivityType(EGridInstanceActivityType::Active);

	GridStateComponent->AddGridState(Index, NewGridInstanceState);
	
	DrawGridInstance(Index, NewGridInstanceState.GetInstanceType(), EGridInstanceActivityType::Active);
	GridStateComponent->SetActiveGridIndex(Index);
}

void AGridActor::SetGridAsDefault(const FIntPoint& Index)
{
	FGridState NewGridInstanceState = *GridStateComponent->GetGridUnitState(Index);
	NewGridInstanceState.SetActivityType(EGridInstanceActivityType::None);
	GridStateComponent->AddGridState(Index, NewGridInstanceState);
	DrawGridInstance(Index, NewGridInstanceState.GetInstanceType(), EGridInstanceActivityType::None);
}

void AGridActor::ResetActiveGrid()
{
	if (!GridStateComponent->IsActiveIndexSet())
	{
		return;
	}
	const FIntPoint ActiveGridIndex = GridStateComponent->GetActiveGridIndex(); 
	FGridState NewGridInstanceState = *GridStateComponent->GetGridUnitState(ActiveGridIndex);


	NewGridInstanceState.SetActivityType(EGridInstanceActivityType::None);

	GridStateComponent->AddGridState(ActiveGridIndex, NewGridInstanceState);
	DrawGridInstance(ActiveGridIndex, NewGridInstanceState.GetInstanceType(),
		NewGridInstanceState.GetActivityType());
	
	GridStateComponent->SetActiveGridIndex({-1, -1});
}

void AGridActor::ResetHoveringGrid()
{
	if (!GridStateComponent->IsHoveringIndexSet())
	{
		return;
	}

	const FIntPoint HoveringIndex = GridStateComponent->GetHoveringGridIndex();
	FGridState GridInstanceState = *GridStateComponent->GetGridUnitState(HoveringIndex);

	GridInstanceState.SetActivityType(EGridInstanceActivityType::None);
	
	DrawGridInstance(HoveringIndex, GridInstanceState.GetInstanceType(),
		GridInstanceState.GetActivityType());

	GridStateComponent->SetHoveringGridIndex({-1, -1 });
}

void AGridActor::GenerateGrid()
{
	FVector Center = GetActorLocation();
	FVector Bounds = GridArea->GetScaledBoxExtent();
	BottomLeft = FVector2f(Center.X, Center.Y) - FVector2f(Bounds.X, Bounds.Y); 
	TopRight = FVector2f(Center.X, Center.Y) + FVector2f(Bounds.X, Bounds.Y);

	CalculateDimensions();
	GridStateComponent->ResetGridState();

	const FVector2f GridUnitSize = GridStateComponent->GetGridUnitSize();

	// Center the bottom left on the bottom left tile
	BottomLeft = BottomLeft  + GridUnitSize / 2.0f;
	TopRight = TopRight - GridUnitSize / 2.0f;
	
	FVector2f CurrentPosition = BottomLeft;
	for (uint16 i = 0; i < Dimension.X; ++i)
	{
		for (uint16 j = 0; j < Dimension.Y; ++j)
		{
			// Ray trace from above the tile to find the correct z value.
			FHitResult HitResult;
			
			
			GetWorld()->LineTraceSingleByChannel(HitResult, FVector(CurrentPosition.X, CurrentPosition.Y, RayTraceHeight),
				FVector(CurrentPosition.X, CurrentPosition.Y, -RayTraceHeight), GridStateComponent->GetGridPlacementTraceChannel());

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
			GridStateComponent->AddGridState(FIntPoint(i, j), GridState);
			CurrentPosition.Y += GridUnitSize.Y;
		}
		CurrentPosition.X += GridUnitSize.X;
		CurrentPosition.Y = BottomLeft.Y;
	}
	bIsGridGenerated = true;
}


FIntPoint AGridActor::GetIndexFromLocation(const FVector2f& GridLocation) const
{
	FVector2f FloatingIndex = (GridLocation - BottomLeft) / GridStateComponent->GetGridUnitSize();
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
	FGridState GridInstanceState = *GridStateComponent->GetGridUnitState(GridIndex);
	GridInstanceState.OccupyingUnit = Character;
	GridStateComponent->AddGridState(GridIndex, GridInstanceState);
}

FVector AGridActor::GetWorldLocationFromIndex(const FIntPoint& Idx) const
{
	const FVector2f MapLocation = BottomLeft + FVector2f(Idx.X, Idx.Y) * GridStateComponent->GetGridUnitSize();
	return FVector(MapLocation.X, MapLocation.Y,  GridStateComponent->GetGridUnitState(Idx)->Height);
}


bool AGridActor::IsValidIndex(const FIntPoint &GridIndex) const
{
	if (GridIndex.X >= 0 && GridIndex.X < Dimension.X &&
		GridIndex.Y >= 0 && GridIndex.Y < Dimension.Y)
	{
		const FGridState* GridState = GridStateComponent->GetGridUnitState(GridIndex);
		return GridState->GridAccessState == EGridAccessState::Accessible;
	}
	return false;
}

void AGridActor::DrawDebugGrid()
{

	GenerateGrid();
	GridStateComponent->LoadVisualData();
	InstancedStaticMeshComponent->ClearInstances();
	InstancedStaticMeshComponent->SetStaticMesh(GridStateComponent->GetGridUnitMesh());
	
	
	uint16 MeshInstanceIndex = 0;
	for (uint16 i = 0; i < Dimension.X; i++)
	{
		for (uint16 j = 0; j < Dimension.Y; j++)
		{
			FTransform MeshTransform;
			FVector GridUnitLocation = GetWorldLocationFromIndex(FIntPoint(i, j));
			const FGridState* GridState = GridStateComponent->GetGridUnitState(FIntPoint(i, j));

			if (GridState->GridAccessState != EGridAccessState::Accessible)
			{
				GridUnitLocation.Z += 10.0f;
			}
			
			GridUnitLocation.Z += 5.0f;
			MeshTransform.SetLocation(GridUnitLocation);
			MeshTransform.SetScale3D(FVector(GridStateComponent->GetGridUnitScale().X, GridStateComponent->GetGridUnitScale().Y, 1.0f));
			InstancedStaticMeshComponent->AddInstance(MeshTransform, true);

			FGridColorData GridColorData;
			GridColorData.BackgroundColor = FColor(0, 0, 0, 0);
			// TODO: Use debug color info from data file instead of hardcoded colors
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
			
			GridStateComponent->AddGridState(FIntPoint(i, j), {
				EGridInstanceType::DebugGrid,
				EGridInstanceActivityType::None,
				MeshInstanceIndex
			});
			MeshInstanceIndex++;
		}
	}
	
	for (TActorIterator<ADeploymentZone> ActorItr(GetWorld()); ActorItr ;++ActorItr)
	{
		FVector2f DeploymentBottomLeft, DeploymentTopRight;
		ActorItr->GetDeploymentBounds(DeploymentBottomLeft, DeploymentTopRight);
		
		DrawDebugDeploymentZone(GetIndexFromLocation(DeploymentBottomLeft), GetIndexFromLocation(DeploymentTopRight));
	}
	bIsDebugGridActive = true;
}

void AGridActor::ClearGridUnits()
{
	InstancedStaticMeshComponent->ClearInstances();
	GridStateComponent->SetVisibleGridUnitsAsHidden();
	bIsDebugGridActive = false;
	CurrentMaxInstancedMeshIndex = 0;
}

void AGridActor::DrawDebugDeploymentZone(FIntPoint BottomLeftIdx, FIntPoint TopRightIdx)
{
	for (SIZE_T i = BottomLeftIdx.X; i <= TopRightIdx.X; i++)
	{	
		for (SIZE_T j = BottomLeftIdx.Y; j <= TopRightIdx.Y; j++)
		{
			const FGridState* GridInstanceState =
				GridStateComponent->GetGridUnitState(FIntPoint(i, j));
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
	
	const FIntPoint BottomLeftIndex = GetIndexFromLocation(DeploymentBottomLeft);
	const FIntPoint TopRightIndex = GetIndexFromLocation(DeploymentTopRight);
	
	ClearGridUnits();

	for (SIZE_T i = BottomLeftIndex.X; i <= TopRightIndex.X; i++)
	{
		for (SIZE_T j = BottomLeftIndex.Y; j <= TopRightIndex.Y; j++)
		{
			const FGridState* GridState = GridStateComponent->GetGridUnitState(FIntPoint(i, j));
			if (GridState->GridAccessState == EGridAccessState::Accessible)
			{
				DrawGridInstance(FIntPoint(i, j),
					EGridInstanceType::Deployment,
					EGridInstanceActivityType::None);
			}
		}
	}
	// TODO: Add other Characters
	AHeroCharacter* HeroCharacter = GameState->GetMainCharacter();
	check(HeroCharacter);
	HeroCharacter->SetActorLocation(GetWorldLocationFromIndex(TopRightIndex));
	SetCharacterUnitAtIndex(TopRightIndex, HeroCharacter);
}

void AGridActor::HandleHoverOnGrid(const FIntPoint& GridIndex)
{
	
	if (GridStateComponent->GetHoveringGridIndex() == GridIndex)
	{
		return;
	}
	
	if (!IsGridIndexHoverable(GridIndex) || GridIndex == GridStateComponent->GetActiveGridIndex())
	{
		ResetHoveringGrid();
		return;
	}
	
	ResetHoveringGrid();
	GridStateComponent->SetHoveringGridIndex(GridIndex);
	DrawGridInstance(GridIndex, EGridInstanceType::Deployment, EGridInstanceActivityType::Hover);				
}

void AGridActor::HandleGridSelect(const FIntPoint& Index)
{
	const FGridState* GridInstanceState = GridStateComponent->GetGridUnitState(Index);
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
	const FGridState* GridInstanceState = GridStateComponent->GetGridUnitState(Index);
	return GridInstanceState ? IsGridUnitSelectable(Index, GridInstanceState) : false; 
}

bool AGridActor::IsGridUnitSelectable(const FIntPoint& Index, const FGridState* GridInstanceState) const
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
				GridInstanceState->GetActivityType() != EGridInstanceActivityType::Active &&
				GridInstanceState->GetInstanceType() == EGridInstanceType::Deployment;
			break;
		}
	default:;
	}
	return bCanBeSelected;
}

bool AGridActor::IsGridIndexHoverable(const FIntPoint& Index)
{
	const FGridState* GridInstanceState = GridStateComponent->GetGridUnitState(Index);

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
			bCanBeHovered = GridInstanceState->GetActivityType() != EGridInstanceActivityType::Hover
				&& GridInstanceState->GetInstanceType() == EGridInstanceType::Deployment;
			
			break;
		}
	default:;
	}
	return bCanBeHovered;
}
