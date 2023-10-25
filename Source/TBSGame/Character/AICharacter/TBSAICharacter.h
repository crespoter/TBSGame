// Copyright Crespoter Interactive

#pragma once

#include "CoreMinimal.h"
#include "Character/TBSCharacter.h"
#include "TBSAICharacter.generated.h"

class ACombatSituation;

UCLASS()
class TBSGAME_API ATBSAICharacter : public ATBSCharacter
{
	GENERATED_BODY()
public:
	ATBSAICharacter();

	/**
	 * @brief Snaps the character to the grid in the combat situation
	 * @return The index of the grid.
	 */
	UFUNCTION(CallInEditor, Category="AI Character")
	FIntPoint SnapToGrid();


	

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
#endif

	UPROPERTY(EditInstanceOnly, Category="AI Character")
	ACombatSituation* CombatSituation {nullptr};

};
