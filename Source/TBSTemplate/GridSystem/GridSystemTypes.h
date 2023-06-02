#pragma once
#include "GridSystemTypes.Generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGridSystems, Log, All);

UENUM(BlueprintType)
enum class EGridAccessState : uint8
{
	Accessible,
	Blocked,
	OutOfMap,
	Max
};

UENUM()
enum class EGridInstanceType
{
	Deployment
};


USTRUCT(BlueprintType)
struct FGridColorData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FColor EdgeColor;
	
	UPROPERTY(EditDefaultsOnly)
	FColor BackgroundColor;
};

USTRUCT(BlueprintType)
struct FGridVisualState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGridColorData GridColorData;
	
};

UCLASS(BlueprintType)
class TBSTEMPLATE_API UGridGenerationData : public UDataAsset
{
	GENERATED_BODY()
public:
	// Grid unit size after scaling
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2f GridUnitSize {100.0f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2f GridUnitScale {1.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* GridUnitMesh { nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> GridPlacementTraceChannel { ECollisionChannel::ECC_WorldStatic };
};

/**
 * Grid state for the whole map. Static details are stored here like grid height and static state.
 */
USTRUCT(BlueprintType)
struct TBSTEMPLATE_API FGridState
{
	GENERATED_BODY()
	EGridAccessState GridAccessState {EGridAccessState::Max};
	float Height {0.0f};
};
