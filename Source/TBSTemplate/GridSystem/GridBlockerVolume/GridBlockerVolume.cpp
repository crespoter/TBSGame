// Fill out your copyright notice in the Description page of Project Settings.


#include "GridSystem/GridBlockerVolume/GridBlockerVolume.h"

#include "Components/BoxComponent.h"

// Sets default values
AGridBlockerVolume::AGridBlockerVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
}
