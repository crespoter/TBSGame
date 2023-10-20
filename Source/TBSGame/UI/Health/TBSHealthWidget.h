// Copyright Crespoter Interactive

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TBSHealthWidget.generated.h"

class ATBSCharacter;

UCLASS()
class TBSGAME_API UTBSHealthWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void TBSInitialize(ATBSCharacter* OwnerCharacter);
};
