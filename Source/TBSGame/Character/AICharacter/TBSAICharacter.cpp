// Copyright Crespoter Interactive

#include "Character/AICharacter/TBSAICharacter.h"

#include "CombatSituation/CombatSituation.h"
#include "Components/CapsuleComponent.h"
#include "GridSystem/GridActor/GridActor.h"

ATBSAICharacter::ATBSAICharacter()
{
}

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

#if WITH_EDITOR
void ATBSAICharacter::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	if (CombatSituation && bFinished)
	{
		SnapToGrid();
	}
}
#endif
