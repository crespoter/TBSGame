// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TBSCharacter.h"

class AGridActor;
class ATBSCharacter;

UENUM()
enum class EActionType
{
	None,
	Deploy,
	Move,
	Skill
};

/**
 * Action is any activity done by the player on the grid.
 * eg: Move, Attack, Spells.
 * Action is an interface class and Action classes are expected to extend from it.
 */
class TBSTEMPLATE_API UAction : public UObject
{
public:
	virtual bool CheckIfValidToInitialize(const FIntPoint& Index) const = 0;
	virtual void Cancel() = 0;
	virtual void Execute() = 0;
	
	virtual void Initialize(AGridActor* InGridActor,
		ATBSCharacter* InInstigator,
		const FIntPoint& InInstigatingIndex);
protected:

	AGridActor* GridActor {nullptr};
	ATBSCharacter* Instigator {nullptr};
	FIntPoint InstigatingIndex {-1};
	EActionType ActionType {EActionType::None};

	bool bIsInitialized {false};
};
