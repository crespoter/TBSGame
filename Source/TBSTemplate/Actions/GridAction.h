// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridSystem/GridSystemTypes.h"
#include "GridAction.generated.h"

class AGridActor;
class ATBSCharacter;


/**
 * Action is any activity done by the player on the grid.
 * eg: Move, Attack, Spells.
 * Action is an interface class and Action classes are expected to extend from it.
 */
UCLASS(Abstract)
class TBSTEMPLATE_API UGridAction : public UObject
{
	GENERATED_BODY()
public:
	virtual bool CheckIfValidToInitialize(AGridActor* InGridActor,
		ATBSCharacter* InInstigator,
		const FIntPoint& InInstigatingIndex) const;

	virtual bool CheckIfValidToExecute(const FIntPoint& TargetIndex) const;
	
	virtual void Cancel();
	virtual void Execute();
	

	
	virtual bool Initialize(AGridActor* InGridActor,
		ATBSCharacter* InInstigator,
		const FIntPoint& InInstigatingIndex);

	virtual void HandleGridSelect(const FIntPoint& GridIndex);

	UPROPERTY()
	FGridActionFinishedDelegate ActionFinishedDelegate;

protected:
	virtual void FinishExecution(const EActionExecutionStatus ExecutionStatus);
	
	FORCEINLINE const AGridActor* GetGridActor() const;
	
	FORCEINLINE AGridActor* GetGridActor();

	FORCEINLINE FIntPoint GetInstigatingIndex() const;

	FORCEINLINE const ATBSCharacter* GetInstigator() const;

	FORCEINLINE ATBSCharacter* GetInstigator();
	
private:

	UPROPERTY()
	AGridActor* GridActor {nullptr};
	UPROPERTY()
	ATBSCharacter* Instigator {nullptr};

	FIntPoint InstigatingIndex {-1};

	EActionType ActionType {EActionType::None};

	bool bIsInitialized {false};
};
