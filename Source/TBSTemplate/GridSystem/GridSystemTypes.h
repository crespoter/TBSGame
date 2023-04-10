#pragma once
#include "Engine/DataTable.h"
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


UENUM(BlueprintType)
enum class EGridUnitState : uint8
{
	Default,
	Hover
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


UCLASS(BlueprintType)
class TBSTEMPLATE_API UGridGenerationData : public UDataAsset
{
	GENERATED_BODY()
public:
	// Grid unit size after scaling
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2f GridUnitSize {100.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2f GridBottomLeftWorldLocation {0.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2f GridUnitScale {1.0f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIntPoint GridDimensions {10};
	
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

/*
 * Grid state for the sub grid that is rendered on each turn for a hero. This is the state for the rendered grid
 */
USTRUCT(BlueprintType)
struct TBSTEMPLATE_API FVisibleGridState
{
	GENERATED_BODY()
	uint16 MeshInstanceIndex {0};
	FColor DefaultColor;
	FColor HoverColor;
	FColor ActiveColor;
};