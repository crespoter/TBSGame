// Copyright Crespoter Interactive

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TBSTemplate/Game/TBSTypes.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTBSHealthUpdatedDelegate,
	uint8, OldHealth, uint8, NewHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TBSTEMPLATE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE uint8 GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE uint8 GetMaxHealth() const;

	void Damage(const uint8 DamageAmount);

	void Heal(const uint8 HealAmount);
	
	UPROPERTY(BlueprintAssignable)
	FTBSMulticastDynamicDelegate HealthDepletedDelegate;

	UPROPERTY(BlueprintAssignable)
	FTBSHealthUpdatedDelegate HealthChangedDelegate;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	uint8 MaxHealth {10};

	UPROPERTY(EditAnywhere)
	uint8 CurrentHealth {0};

};
