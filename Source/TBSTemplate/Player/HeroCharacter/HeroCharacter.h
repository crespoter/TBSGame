// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HeroCharacter.generated.h"


struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;



USTRUCT(BlueprintType)
struct FExplorationInputActions
{
	GENERATED_BODY();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Movement { nullptr };
};


UCLASS()
class TBSTEMPLATE_API AHeroCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AHeroCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleMovementInput(const FInputActionValue& ActionValue);

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* CameraComponent { nullptr };

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USpringArmComponent* SpringArmComponent { nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* ExplorationInputMappingContext { nullptr };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FExplorationInputActions CharacterMovementAction;
};
