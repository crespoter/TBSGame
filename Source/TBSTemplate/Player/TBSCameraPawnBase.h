// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "TBSCameraPawnBase.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class ATBSPlayerController;
class ATBSGameState;
enum class EGamePhase : uint8;

USTRUCT(BlueprintType)
struct FPlayerInputActions
{
	GENERATED_BODY();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* CameraMovementAction { nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* SelectAction { nullptr };
};
UCLASS()
class TBSTEMPLATE_API ATBSCameraPawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATBSCameraPawnBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void HandleCameraMovementInputEvent(const FInputActionValue& ActionValue);

	void HandleSelectInputEvent(const FInputActionValue& ActionValue);

	void HandleHover();

	UFUNCTION()
	void HandleGamePhaseChanged(EGamePhase NewGamePhase);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* CameraComponent { nullptr };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USpringArmComponent* SpringArmComponent { nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FPlayerInputActions PlayerInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float CameraMovementSnapSpeed { 1.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float CameraMovementSpeed { 1.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float CameraZoomSnapSpeed { 1.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float CameraZoomSpeed { 1.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float MinCameraArmLength { 600.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	float MaxCameraArmLength { 2000.0f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* MainSceneComponent { nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TEnumAsByte<ECollisionChannel> GridChannel {ECollisionChannel::ECC_WorldStatic};
	
protected:
	
	FVector TargetActorLocation { 0.0f };

	float TargetArmDistance { 0.0f };

	UPROPERTY()
	ATBSPlayerController* CachedPlayerController { nullptr };

	UPROPERTY()
	ATBSGameState* CachedGameState {nullptr};
};
