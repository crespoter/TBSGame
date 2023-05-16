// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HeroAIController.generated.h"

/**
 * 
 */
UCLASS()
class TBSTEMPLATE_API AHeroAIController : public AAIController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
};
