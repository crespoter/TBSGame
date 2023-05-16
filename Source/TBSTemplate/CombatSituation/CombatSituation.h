// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatSituation.generated.h"

class UBoxComponent;
class AGridActor;
class AHeroCharacter;
class ATBSGameState;

UCLASS()
class TBSTEMPLATE_API ACombatSituation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatSituation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	void OnTriggerColliderHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UFUNCTION()
	void GetCombatGridBounds(FVector& Center, FVector& Bounds) const;
	
	UFUNCTION()
	void TriggerSituation();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void GenerateGridForSituation();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Trigger")
	UBoxComponent* CombatTriggerCollider {nullptr};

	UPROPERTY(EditInstanceOnly, Category = "Enemies")
	FName NPCCombatantId;	

	UPROPERTY(EditInstanceOnly, Category = "Grid")
	AGridActor* GridActor {nullptr};

protected:
	// TODO: Change to TBSCharacter
	// Participants in the order of their speed
	UPROPERTY()
    TArray<AHeroCharacter*> ActiveParticipants;

	UPROPERTY()
	ATBSGameState* GameState {nullptr};
	
	bool bIsActive {false};
};
