// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/GridAction.h"
#include "GridSystem/GridActor/GridComponents/GridStateComponent.h"
#include "DeployGridAction.generated.h"

UCLASS()
class TBSTEMPLATE_API UGridDeployAction : public UGridAction
{
	GENERATED_BODY()
public:
	virtual bool CheckIfValidToInitialize(AGridActor* InGridActor,
		ATBSCharacter* InInstigator,
		const FIntPoint& InInstigatingIndex) const override;
	
	virtual bool CheckIfValidToExecute(const FIntPoint& TargetIndex) const override;

	virtual void Cancel() override;

	virtual void Execute() override;

	virtual bool Initialize(AGridActor* InGridActor, ATBSCharacter* InInstigator,
		const FIntPoint& InInstigatingIndex) override;

private:
	UFUNCTION()
	void DeployFighter();
public:
	virtual void HandleGridSelect(const FIntPoint& GridIndex) override;
private:
	UPROPERTY()
	UGridStateComponent* GridStateComponent {nullptr};

	EGridDirection Direction {EGridDirection::Up};

	FIntPoint SelectedIndex {GridConstants::InvalidIndex};
};


