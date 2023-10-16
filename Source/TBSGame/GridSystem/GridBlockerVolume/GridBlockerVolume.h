// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridBlockerVolume.generated.h"

class UBoxComponent;

UCLASS()
class TBSGAME_API AGridBlockerVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridBlockerVolume();
protected:
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent {nullptr};
};
