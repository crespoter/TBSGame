// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HeroCharacter/HeroCharacter.h"

#include "AI/HeroAIController/HeroAIController.h"
#include "Game/TBSGameState.h"
#include "GameFramework/PawnMovementComponent.h"


// Sets default values
AHeroCharacter::AHeroCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

// Called every frame
void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}