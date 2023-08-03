// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HeroCharacter/HeroCharacter.h"

#include "AI/HeroAIController/HeroAIController.h"
#include "Game/TBSGameState.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/WidgetComponent.h"

//TODO: Do we really need HeroCharacter? TBSCharacter can work as the character class for both AI and player Characters.

// Sets default values
AHeroCharacter::AHeroCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
}

// Called when the game starts or when spawned
void AHeroCharacter::BeginPlay()
{
	// TODO: Move to auto possess only main character
	Super::BeginPlay();
	AIController = GetWorld()->SpawnActor<AHeroAIController>(AHeroAIController::StaticClass());
	AIController->Possess(this);
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
	GameState->GamePhaseChangedEvent.AddDynamic(this, &ThisClass::OnGamePhaseChanged);
}

void AHeroCharacter::OnGamePhaseChanged(EGamePhase NewGamePhase)
{
	GetMovementComponent()->StopMovementImmediately();
}