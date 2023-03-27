// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HeroCharacter/HeroCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AHeroCharacter::AHeroCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring arm component");
	check(CameraComponent);
	check(SpringArmComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
}

// Called when the game starts or when spawned
void AHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHeroCharacter::HandleMovementInput(const FInputActionValue& ActionValue)
{
	const FVector2d MovementValues = ActionValue.Get<FVector2d>();

	FVector ForwardDirection = FVector::VectorPlaneProject(CameraComponent->GetForwardVector(), GetActorUpVector());
	ForwardDirection.Normalize();

	FVector RightDirection = FVector::VectorPlaneProject(CameraComponent->GetRightVector(), GetActorUpVector());
	RightDirection.Normalize();
	// FVector ForwardDirection = CameraComponent->GetUpVector().Dot()
	AddMovementInput(ForwardDirection, MovementValues.X);
	AddMovementInput(RightDirection, MovementValues.Y);
}

// Called every frame
void AHeroCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	const APlayerController* PC = Cast<APlayerController>(GetController());
	check(PC);
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	check(EnhancedInputSubsystem);

	EnhancedInputSubsystem->ClearAllMappings();
	check(ExplorationInputMappingContext);
	EnhancedInputSubsystem->AddMappingContext(ExplorationInputMappingContext, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindAction(CharacterMovementAction.Movement,
		ETriggerEvent::Triggered, this, &AHeroCharacter::HandleMovementInput);
}

