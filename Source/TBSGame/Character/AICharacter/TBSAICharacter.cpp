// Copyright Crespoter Interactive

#include "Character/AICharacter/TBSAICharacter.h"

#include "CombatSituation/CombatSituation.h"
#include "Components/CapsuleComponent.h"
#include "GridSystem/GridActor/GridActor.h"

FIntPoint ATBSAICharacter::SnapToGrid()
{
	check(CombatSituation);
	AGridActor* GridActor = CombatSituation->GetGridActor();
	check(GridActor);
	if (!GridActor->GetIsGridGenerated())
	{
		GridActor->GenerateGrid();
	}
	FIntPoint GridIndex;
	FVector GridLocation = GridActor->GetNearestGridLocation(GetActorLocation(), GridIndex);
	GridLocation.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(GridLocation);
	return GridIndex;
}

void ATBSAICharacter::BeginPlay()
{
	Super::BeginPlay();
	check(CombatSituation);
	CombatSituation->RegisterAICombatant(this);
}
