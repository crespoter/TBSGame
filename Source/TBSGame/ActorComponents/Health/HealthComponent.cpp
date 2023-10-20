// Copyright Crespoter Interactive

#include "ActorComponents/Health/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentHealth = MaxHealth;
}

int32 UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

int32 UHealthComponent::GetMaxHealth() const
{
	return MaxHealth;
}

void UHealthComponent::Damage(const int32 DamageAmount)
{
	const int16 OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0, MaxHealth);
	// Notify even if the health values are same. Let the dependents decide
	// how to handle a clamped health value.
	
	HealthChangedDelegate.Broadcast(OldHealth, CurrentHealth);
	if (CurrentHealth == 0)
	{
		HealthDepletedDelegate.Broadcast();
	}
}

void UHealthComponent::Heal(const int32 HealAmount)
{
	const int16 OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0, MaxHealth);
	HealthChangedDelegate.Broadcast(OldHealth, CurrentHealth);
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}