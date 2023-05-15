#include "Game/BattleStateComponent.h"

// Sets default values for this component's properties
UBattleStateComponent::UBattleStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBattleStateComponent::Initialize(ACombatSituation* CombatSituation)
{
}


// Called when the game starts
void UBattleStateComponent::BeginPlay()
{
	Super::BeginPlay();
}
