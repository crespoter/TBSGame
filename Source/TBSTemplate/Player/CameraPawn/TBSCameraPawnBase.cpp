// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CameraPawn/TBSCameraPawnBase.h"

#include "AIController.h"
#include "Game/TBSGameState.h"
#include "Player/PlayerController/TBSPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "GridSystem/GridActor/GridActor.h"
#include "Player/HeroCharacter/HeroCharacter.h"


// Sets default values
ATBSCameraPawnBase::ATBSCameraPawnBase()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	MainSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	
	
	check(CameraComponent);
	check(SpringArmComponent);
	check(MainSceneComponent);

	
	RootComponent = MainSceneComponent;
	SpringArmComponent->SetupAttachment(MainSceneComponent);
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void ATBSCameraPawnBase::BeginPlay()
{
	Super::BeginPlay();

	TargetActorLocation = GetActorLocation();
	TargetArmDistance = SpringArmComponent->TargetArmLength;

	CachedPlayerController = Cast<ATBSPlayerController>(GetWorld()->GetFirstPlayerController());
	check(CachedPlayerController);

	CachedGameState = Cast<ATBSGameState>(GetWorld()->GetGameState());
	check(CachedGameState);

	CachedGameState->GamePhaseChangedEvent.AddDynamic(this, &ATBSCameraPawnBase::HandleGamePhaseChanged);
}

void ATBSCameraPawnBase::HandleCameraMovementInputEvent(const FInputActionValue& ActionValue)
{
	const FVector3d MovementValues = ActionValue.Get<FVector3d>();
	if (FMath::Abs( MovementValues.X) > 0.01f)
	{
		TargetActorLocation += MovementValues.X * GetActorForwardVector() * CameraMovementSpeed;
	}
	if (FMath::Abs(MovementValues.Y) > 0.01f)
	{
		TargetActorLocation += MovementValues.Y * GetActorRightVector() * CameraMovementSpeed;
	}
	
	if (FMath::Abs(MovementValues.Z) > 0.01f)
	{
		TargetArmDistance =  FMath::Clamp(MinCameraArmLength,
		SpringArmComponent->TargetArmLength + ( MovementValues.Z * CameraZoomSpeed), MaxCameraArmLength);
	}
}

void ATBSCameraPawnBase::HandleSelectInputEvent(const FInputActionValue& ActionValue)
{
	if (CurrentGamePhase == EGamePhase::Battle || CurrentGamePhase == EGamePhase::Deployment)
	{
		FHitResult HitResult;
		CachedPlayerController->GetHitResultUnderCursor(GridChannel, true, HitResult);
		if (CurrentGamePhase == EGamePhase::Exploration)
		{
			// TODO:
			/*
			const AHeroCharacter* Hero = CachedGameState->GetMainCharacter();
			AAIController* AIController = Cast<AAIController>(Hero->GetController());
			AIController->MoveToLocation(HitResult.Location);
			*/
		}
		else if (CurrentGamePhase == EGamePhase::Deployment || CurrentGamePhase == EGamePhase::Battle)
		{
			const FVector2f HitLocation2D(HitResult.Location.X, HitResult.Location.Y);
			const FIntPoint GridIndex = GridActor->GetIndexFromLocation(HitLocation2D);
			if (GridActor->IsValidIndex(GridIndex))
			{
				GridActor->HandleGridSelect(GridIndex);
			}
		}
	}
}

void ATBSCameraPawnBase::HandleExecuteInputEvent(const FInputActionValue& ActionValue)
{
	FHitResult HitResult;
	CachedPlayerController->GetHitResultUnderCursor(GridChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		if (CurrentGamePhase == EGamePhase::Exploration)
		{
			const AHeroCharacter* Hero = CachedGameState->GetMainCharacter();
			AAIController* AIController = Cast<AAIController>(Hero->GetController());
			AIController->MoveToLocation(HitResult.Location);
		}
		else if (CurrentGamePhase == EGamePhase::Deployment || CurrentGamePhase == EGamePhase::Battle)
		{
			const FVector2f HitLocation2D(HitResult.Location.X, HitResult.Location.Y);
			const FIntPoint GridIndex = GridActor->GetIndexFromLocation(HitLocation2D);
			if (GridActor->IsValidIndex(GridIndex))
			{
				// GridActor->HandleGridSelect(GridIndex);
			}
		}
	}
}

void ATBSCameraPawnBase::HandleHover()
{
	if (CurrentGamePhase == EGamePhase::Battle || CurrentGamePhase == EGamePhase::Deployment)
	{
		FHitResult HitResult;
		CachedPlayerController->GetHitResultUnderCursor(GridChannel, true, HitResult);
		if (HitResult.bBlockingHit)
		{
			const FVector2f HitLocation2D(HitResult.Location.X, HitResult.Location.Y);
			const FIntPoint GridIndex = GridActor->GetIndexFromLocation(HitLocation2D);
			if (GridActor->IsValidIndex(GridIndex))
			{
				GridActor->HandleHoverOnGrid(GridIndex);
			}
		}
	}
}

void ATBSCameraPawnBase::HandleGamePhaseChanged(EGamePhase NewGamePhase)
{
	CurrentGamePhase = NewGamePhase;
	if (NewGamePhase == EGamePhase::Deployment)
	{
		GridActor = CachedGameState->GetActiveGridActor();
	}
}

// Called every frame
void ATBSCameraPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle camera location
	SetActorLocation(FMath::VInterpTo(GetActorLocation(),
		TargetActorLocation, DeltaTime, CameraMovementSnapSpeed));

	// Handle Camera Zoom
	SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, TargetArmDistance,
		DeltaTime, CameraZoomSnapSpeed);

	// Tile Hovering
	HandleHover();
}

// Called to bind functionality to input
void ATBSCameraPawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	const APlayerController* PC = Cast<APlayerController>(GetController());
	check(PC);

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	check(EnhancedInputSubsystem);

	EnhancedInputSubsystem->ClearAllMappings();
	check(InputMappingContext);
	EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(PlayerInputActions.SelectAction,
		ETriggerEvent::Triggered, this, &ATBSCameraPawnBase::HandleSelectInputEvent);

	EnhancedInputComponent->BindAction(PlayerInputActions.ExecuteAction,
		ETriggerEvent::Triggered, this, &ATBSCameraPawnBase::HandleExecuteInputEvent);

	EnhancedInputComponent->BindAction(PlayerInputActions.CameraMovementAction,
		ETriggerEvent::Triggered, this, &ATBSCameraPawnBase::HandleCameraMovementInputEvent);
}

