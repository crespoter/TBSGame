// Fill out your copyright notice in the Description page of Project Settings.
#include "GridActor.h"

#include "EngineUtils.h"
#include "Actions/Deploy/DeployGridAction.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridSystem/GridBlockerVolume/GridBlockerVolume.h"
#include "Components/BoxComponent.h"
#include "CombatSituation/CombatSituation.h"
#include "CombatSituation/DeploymentZone/DeploymentZone.h"
#include "Data/GridVisualData/GridVisualData.h"
#include "Game/TBSGameState.h"
#include "GridComponents/GridStateComponent.h"
#include "GridComponents/GridVisualComponent.h"
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

	GridVisualComponent = CreateDefaultSubobject<UGridVisualComponent>("Grid Visual Component");
	check(GridVisualComponent);
	GridVisualComponent->Initialize(GridStateComponent, InstancedStaticMeshComponent, this);

}

void AGridActor::BeginPlay()
{
	// TODO: Generate grid as a Call in editor function. This way the data can be package into build rather than being generated everytime.

	Super::BeginPlay();
	check(bIsGridGenerated);
	ClearGridUnits();
	GenerateGrid();
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
	GameState->GamePhaseChangedEvent.AddDynamic(this, &ThisClass::AGridActor::OnGamePhaseChanged);
}



void AGridActor::CalculateDimensions()
{
	FVector2f Bounds;
	Bounds.X = TopRight.X - BottomLeft.X;
	Bounds.Y = TopRight.Y - BottomLeft.Y;
	Dimension.X = FMath::Floor(Bounds.X / GridStateComponent->GetGridUnitSize().X);
	Dimension.Y = FMath::Floor(Bounds.Y / GridStateComponent->GetGridUnitSize().Y);
}

void AGridActor::OnActionFinished(const EActionExecutionStatus ExecutionStatus)
{
	CurrentGridAction = nullptr;
}

void AGridActor::OnGamePhaseChanged(const EGamePhase NewGamePhase)
{
	switch(NewGamePhase)
	{
	case EGamePhase::Deployment:
		ActivateDeploymentGrid(GameState->GetCurrentActiveCombatSituation());
		break;
	case EGamePhase::Battle:
		GridStateComponent->SetVisibleGridUnitsAsHidden();
		break;
	default:;
	}
}

void AGridActor::UpdateGridState(const FIntPoint& GridIndex, const EGridInstanceType InstanceType, const EGridInstanceActivityType ActivityType)
{

	FGridState GridInstanceState;
	const bool bIsFound = GridStateComponent->GetGridUnitState(GridIndex, GridInstanceState);
	if (!bIsFound)
	{
		return;
	}
	GridInstanceState.SetInstanceType(InstanceType);
	GridInstanceState.SetActivityType(ActivityType);
	GridStateComponent->AddGridState(GridIndex, GridInstanceState);
}

const UGridStateComponent* AGridActor::GetGridStateComponent() const
{
	return GridStateComponent;
}

UGridStateComponent* AGridActor::GetGridStateComponent()
{
	return GridStateComponent;
}


void AGridActor::SetGridAsActive(const FIntPoint& Index)
{
	FGridState GridInstanceState;
	GridStateComponent->GetGridUnitState(Index, GridInstanceState);
	if (Index == GridStateComponent->GetHoveringGridIndex())
	{
		ResetHoveringGrid();
	}
	if (GridStateComponent->IsActiveIndexSet())
	{
		ResetActiveGrid();
	}
	
	GridInstanceState.SetActivityType(EGridInstanceActivityType::Active);

	GridStateComponent->AddGridState(Index, GridInstanceState);
	
	UpdateGridState(Index, GridInstanceState.GetInstanceType(), EGridInstanceActivityType::Active);
	GridStateComponent->SetActiveGridIndex(Index);
}

void AGridActor::SetGridAsDefault(const FIntPoint& Index)
{
	FGridState GridInstanceState;
	GridStateComponent->GetGridUnitState(Index, GridInstanceState);
	GridInstanceState.SetActivityType(EGridInstanceActivityType::None);
	GridStateComponent->AddGridState(Index, GridInstanceState);
	UpdateGridState(Index, GridInstanceState.GetInstanceType(), EGridInstanceActivityType::None);
}

void AGridActor::ResetActiveGrid()
{
	if (!GridStateComponent->IsActiveIndexSet())
	{
		return;
	}
	const FIntPoint ActiveGridIndex = GridStateComponent->GetActiveGridIndex(); 
	FGridState GridInstanceState;
	GridStateComponent->GetGridUnitState(ActiveGridIndex, GridInstanceState);


	GridInstanceState.SetActivityType(EGridInstanceActivityType::None);
	
	GridStateComponent->AddGridState(ActiveGridIndex, GridInstanceState);
	UpdateGridState(ActiveGridIndex, GridInstanceState.GetInstanceType(),
		GridInstanceState.GetActivityType());
	
	GridStateComponent->SetActiveGridIndex({-1, -1});
}

void AGridActor::ResetHoveringGrid()
{
	if (!GridStateComponent->IsHoveringIndexSet())
	{
		return;
	}

	const FIntPoint HoveringIndex = GridStateComponent->GetHoveringGridIndex();
	FGridState GridInstanceState;
	GridStateComponent->GetGridUnitState(HoveringIndex, GridInstanceState);

	GridInstanceState.SetActivityType(EGridInstanceActivityType::None);
	
	UpdateGridState(HoveringIndex, GridInstanceState.GetInstanceType(),
		GridInstanceState.GetActivityType());

	GridStateComponent->SetHoveringGridIndex({-1, -1 });
}

void AGridActor::StartGridAction(const FIntPoint& GridIndex, ATBSCharacter* InstigatingCharacter, UGridAction* GridAction)
{
	CurrentGridAction = GridAction;
	check(CurrentGridAction);
	CurrentGridAction->Initialize(this, InstigatingCharacter, GridIndex);
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
	FGridState GridInstanceState;
	GridStateComponent->GetGridUnitState(GridIndex, GridInstanceState);
	GridInstanceState.OccupyingUnit = Character;
	GridStateComponent->AddGridState(GridIndex, GridInstanceState);
}

FVector AGridActor::GetWorldLocationFromIndex(const FIntPoint& Idx) const
{
	const FVector2f MapLocation = BottomLeft + FVector2f(Idx.X, Idx.Y) * GridStateComponent->GetGridUnitSize();
	FGridState GridInstanceState;
	GridStateComponent->GetGridUnitState(Idx, GridInstanceState);
	return FVector(MapLocation.X, MapLocation.Y,  GridInstanceState.Height);
}


bool AGridActor::IsValidIndex(const FIntPoint &GridIndex) const
{
	if (GridIndex.X >= 0 && GridIndex.X < Dimension.X &&
		GridIndex.Y >= 0 && GridIndex.Y < Dimension.Y)
	{
		FGridState GridInstanceState;
		GridStateComponent->GetGridUnitState(GridIndex, GridInstanceState);
		return GridInstanceState.GridAccessState == EGridAccessState::Accessible;
	}
	return false;
}

void AGridActor::DrawDebugGrid()
{
	GenerateGrid();
	GridStateComponent->LoadVisualData();
	GridVisualComponent->Initialize(GridStateComponent, InstancedStaticMeshComponent, this);
	GridVisualComponent->ResetEditorDebugGrid();
	GridVisualComponent->DrawDebugGrid();
}

void AGridActor::ClearGridUnits()
{
#if WITH_EDITORONLY_DATA
	GridVisualComponent->ResetEditorDebugGrid();
#endif
	GridStateComponent->SetVisibleGridUnitsAsHidden();
}

FVector2D AGridActor::GetDimensions() const
{
	return Dimension;
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
			FGridState GridInstanceState;
			GridStateComponent->GetGridUnitState(FIntPoint(i, j), GridInstanceState);
			if (GridInstanceState.GridAccessState == EGridAccessState::Accessible)
			{
				UpdateGridState(FIntPoint(i, j),
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
	if (GameState->GetCurrentGamePhase() == EGamePhase::Deployment)
	{
		ResetHoveringGrid();
		if (!IsGridIndexHoverable(GridIndex) || GridIndex == GridStateComponent->GetActiveGridIndex())
		{
			return;
		}
		UpdateGridState(GridIndex, EGridInstanceType::Deployment, EGridInstanceActivityType::Hover);
		GridStateComponent->SetHoveringGridIndex(GridIndex);
	}
}

void AGridActor::HandleGridSelect(const FIntPoint& Index)
{

	if (CurrentGridAction)
	{
		if (CurrentGridAction->CheckIfValidToExecute(Index))
		{
			CurrentGridAction->HandleGridSelect(Index);
		}
	}
	else
	{
		FGridState GridInstanceState;
		const bool bIsFound = GridStateComponent->GetGridUnitState(Index, GridInstanceState);
		if (!bIsFound)
		{
			return;
		}
		if (IsGridUnitSelectable(Index, &GridInstanceState))
		{
			const EGamePhase GamePhase = GameState->GetCurrentGamePhase();
			switch(GamePhase)
			{
			case EGamePhase::Deployment:
				{
					UGridDeployAction* DeployAction = NewObject<UGridDeployAction>();
					if (DeployAction->Initialize(this, GridInstanceState.OccupyingUnit, Index))
					{
						CurrentGridAction = DeployAction;
						CurrentGridAction->ActionFinishedDelegate.AddDynamic(this, &ThisClass::AGridActor::OnActionFinished);
					}
					break;
				}
			default: ;
			}
		}
	}
}

bool AGridActor::IsGridUnitSelectable(const FIntPoint& Index) const
{
	FGridState GridInstanceState;
	const bool bIsFound = GridStateComponent->GetGridUnitState(Index, GridInstanceState);
	return bIsFound ? IsGridUnitSelectable(Index, &GridInstanceState) : false; 
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
	FGridState GridInstanceState;
	const bool bIsFound = GridStateComponent->GetGridUnitState(Index, GridInstanceState);

	if (!IsValidIndex(Index) || !bIsFound)
	{
		return false;
	}

	bool bCanBeHovered = false;
	const EGamePhase GamePhase = GameState->GetCurrentGamePhase();

	
	if (CurrentGridAction)
	{
		bCanBeHovered = CurrentGridAction->IsIndexHoverable(Index);	
	}
	else
	{
		switch(GamePhase)
		{
		case EGamePhase::Deployment:
			{
				// Allow a deployment unit with party unit to be hoverable
				bCanBeHovered = GridInstanceState.GetInstanceType() == EGridInstanceType::Deployment
								&& GridInstanceState.OccupyingUnit != nullptr;
				// TODO: Also check if occupying unit is controllable by player character.
				break;
			}
		default:;
		}
	}
	return bCanBeHovered;
}
