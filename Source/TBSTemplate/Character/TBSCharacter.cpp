// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TBSCharacter.h"

// Sets default values
ATBSCharacter::ATBSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATBSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

