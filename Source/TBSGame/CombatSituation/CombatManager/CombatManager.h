// Copyright Crespoter Interactive

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"


class ACombatSituation;

UCLASS()
class TBSGAME_API ACombatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatManager();
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief Triggers the next combat situation
	 * @return false if there is no more combats to trigger.
	 */
	bool TriggerNextCombatSituation();
	
	UPROPERTY(EditInstanceOnly)
	TArray<ACombatSituation*> CombatSituations; 
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	SIZE_T CurrentIndex {0};
};
