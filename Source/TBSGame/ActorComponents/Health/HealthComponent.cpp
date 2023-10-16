// Copyright Crespoter Interactive


#include "ActorComponents/Health/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
}

uint8 UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

uint8 UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UHealthComponent::Damage(const uint8 DamageAmount)
{
	const uint8 OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0, MaxHealth);
	// Notify even if the health values are same. Let the dependents decide
	// how to handle a clamped health value.
	
	HealthChangedDelegate.Broadcast(OldHealth, CurrentHealth);
	if (CurrentHealth == 0)
	{
		HealthDepletedDelegate.Broadcast();
	}
}

void UHealthComponent::Heal(const uint8 HealAmount)
{
	const uint8 OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0, MaxHealth);
	HealthChangedDelegate.Broadcast(OldHealth, CurrentHealth);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}