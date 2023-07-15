// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Action.h"

void UAction::Initialize(AGridActor* InGridActor, ATBSCharacter* InInstigator,
	const FIntPoint& InInstigatingIndex)
{
	check(InGridActor);
	check(InInstigator);
	GridActor = InGridActor;
	Instigator = InInstigator;
	InstigatingIndex = InInstigatingIndex;
}
