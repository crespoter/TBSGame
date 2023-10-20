// Copyright Crespoter Interactive

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TBSGame//Game/TBSTypes.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTBSHealthUpdatedDelegate,
	int32, OldHealth, int32, NewHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TBSGAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetMaxHealth() const;

	void Damage(const int32 DamageAmount);

	void Heal(const int32 HealAmount);
	
	UPROPERTY(BlueprintAssignable)
	FTBSMulticastDynamicDelegate HealthDepletedDelegate;

	UPROPERTY(BlueprintAssignable)
	FTBSHealthUpdatedDelegate HealthChangedDelegate;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	int32 MaxHealth {10};

	UPROPERTY(EditAnywhere)
	int32 CurrentHealth {0};

};
