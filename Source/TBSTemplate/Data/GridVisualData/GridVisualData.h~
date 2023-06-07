// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "TBSTemplate/GridSystem/GridSystemTypes.h"
#include "GridVisualData.generated.h"

USTRUCT()
struct TBSTEMPLATE_API FGridVisualDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly)
	EGridInstanceType GridInstanceType {EGridInstanceType::Max};

	UPROPERTY(EditInstanceOnly)
	EGridInstanceActivityType ActivityType {EGridInstanceActivityType::None};
	
	UPROPERTY(EditInstanceOnly)
	FGridVisualState GridVisualState;
};
