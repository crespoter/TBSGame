// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HeroKeyMap.generated.h"

/**
 * 
 */
UCLASS()
class TBSTEMPLATE_API UHeroKeyMap : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FName CharacterID {""};
	
	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<AHeroCharacter> CharacterClass;
};
