// Fill out your copyright notice in the Description page of Project Settings.


#include "TBSPlayerController.h"

#include "Cheats/TBSCheatManager.h"

ATBSPlayerController::ATBSPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CheatClass = UTBSCheatManager::StaticClass();
}
