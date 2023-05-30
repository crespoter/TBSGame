// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeploymentZone.generated.h"

class UBoxComponent;

UCLASS()
class TBSTEMPLATE_API ADeploymentZone : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeploymentZone();

	void GetDeploymentBounds(FVector2f& OutBottomLeft, FVector2f& OutTopRight) const;

	UFUNCTION(CallInEditor)
	void DrawDebugDeploymentZone();
protected:
	UPROPERTY(EditInstanceOnly)
	UBoxComponent* DeploymentArea {nullptr};
};
