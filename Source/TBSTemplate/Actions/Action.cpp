// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/Action.h"

UAction::UAction(AGridActor* InGridActor, ATBSCharacter* InInstigator,
	const FIntPoint& InInstigatingIndex, const EActionType InActionType) :
		GridActor(InGridActor),
		Instigator(InInstigator),
		InstigatingIndex(InInstigatingIndex),
		ActionType(InActionType)
{
}

UAction::~UAction()
{
}
