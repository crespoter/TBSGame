// Copyright Crespoter Interactive

#include "Player/SpawnPoint/TBSSpawnPoint.h"

#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/CapsuleComponent.h"

ATBSSpawnPoint::ATBSSpawnPoint()
{
	GetCapsuleComponent()->InitCapsuleSize(40.0f, 92.0f);
	GetCapsuleComponent()->SetShouldUpdatePhysicsVolume(false);
	
#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

	if (GetGoodSprite())
	{
		GetGoodSprite()->Sprite =
			ConstructorHelpers::FObjectFinderOptional<UTexture2D>(
				TEXT("/Engine/EditorResources/S_Player")).Get();
		GetGoodSprite()->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
	if (GetBadSprite())
	{
		GetBadSprite()->SetVisibility(false);
	}

	if (ArrowComponent)
	{
		ArrowComponent->ArrowColor = FColor(150, 200, 255);
		ArrowComponent->ArrowSize = 1.0f;
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SetupAttachment(GetCapsuleComponent());
		ArrowComponent->bIsScreenSizeScaled = true;
	}
#endif // WITH_EDITORONLY_DATA
}
