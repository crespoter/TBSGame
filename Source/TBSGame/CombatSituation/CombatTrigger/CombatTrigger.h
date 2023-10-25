// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatTrigger.generated.h"

class ACombatSituation;
class UBoxComponent;
class ADeploymentZone;

/**
 * Combat trigger is obsolete now and is not being used.
 */
UCLASS()
class TBSGAME_API ACombatTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatTrigger();

	UPROPERTY(EditInstanceOnly)
	ACombatSituation* CombatSituation {nullptr};

	UPROPERTY(EditAnywhere)
	UBoxComponent* CombatTrigger {nullptr};
	
	UPROPERTY(EditInstanceOnly)
	ADeploymentZone* DeploymentZone {nullptr};

	UFUNCTION()
	void OnCombatTriggerHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	
protected:
	virtual void BeginPlay() override;
};
