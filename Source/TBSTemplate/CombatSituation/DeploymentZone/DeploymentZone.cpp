// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatSituation/DeploymentZone/DeploymentZone.h"

#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "GridSystem/GridActor/GridActor.h"

ADeploymentZone::ADeploymentZone()
{
	PrimaryActorTick.bCanEverTick = false;
	DeploymentArea = CreateDefaultSubobject<UBoxComponent>("Deployment Area");
	DeploymentArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADeploymentZone::GetDeploymentBounds(FVector2f& OutBottomLeft, FVector2f& OutTopRight) const
{
	const FVector BoxExtents = DeploymentArea->GetScaledBoxExtent();
	const FVector2f Center = FVector2f(GetActorLocation().X, GetActorLocation().Y);
	OutBottomLeft = FVector2f(Center.X - BoxExtents.X, Center.Y - BoxExtents.Y);
	OutTopRight = FVector2f(Center.X + BoxExtents.X, Center.Y + BoxExtents.Y); 
}

void ADeploymentZone::DrawDebugDeploymentZone()
{
	FVector2f BottomLeft;
	FVector2f TopRight;
	GetDeploymentBounds(BottomLeft, TopRight);
	AGridActor* GridActor =  *TActorIterator<AGridActor>(GetWorld());
	if (!GridActor)
	{
		UE_LOG(LogGridSystems, Error, TEXT("Grid actor object not found in world"));
		return;
	}

	GridActor->DrawDebugDeploymentZone(
		GridActor->GetIndexFromLocation(BottomLeft),
		GridActor->GetIndexFromLocation(TopRight)
	);
}
