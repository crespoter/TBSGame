#include "Character/TBSCharacter.h"

#include "ActorComponents/Health/HealthComponent.h"
#include "AI/HeroAIController/HeroAIController.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Game/TBSGameState.h"
#include "GridSystem/GridActor/GridActor.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
ATBSCharacter::ATBSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("Health Component");
}

void ATBSCharacter::DeployCharacterOnGrid(AGridActor* GridActor, const FIntPoint& SourceIndex,
	const FIntPoint& TargetIndex)
{
	// TODO: Release effects on source index
	const FVector TargetLocation = GridActor->GetWorldLocationFromIndex(TargetIndex);
	SetActorLocation(TargetLocation);
	// TODO: Add effects on target index
}


// Called when the game starts or when spawned
void ATBSCharacter::BeginPlay()
{
	Super::BeginPlay();
	AIController = GetWorld()->SpawnActor<AHeroAIController>(AHeroAIController::StaticClass());
	AIController->Possess(this);
	GameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(GameState);
	GameState->GamePhaseChangedEvent.AddDynamic(this, &ThisClass::OnGamePhaseChanged);
	UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent();
	check(PathFollowingComponent);
	PathFollowingComponent->OnRequestFinished.AddUFunction(this, "OnPathFinished");
}

// Called every frame
void ATBSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATBSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ATBSCharacter::OnGamePhaseChanged(const EGamePhase NewGamePhase)
{
	GetMovementComponent()->StopMovementImmediately();
}

void ATBSCharacter::MoveToLocation(const FVector& TargetLocation)
{
	AIController->MoveToLocation(TargetLocation, 0.0f, true,
		true, false, false);
}

void ATBSCharacter::OnPathFinished()
{
	OnPathFinishedDelegate.Broadcast();
}
