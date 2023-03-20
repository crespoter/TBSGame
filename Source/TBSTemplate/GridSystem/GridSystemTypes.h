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
	Default = 0,
	Hover = 1 << 0,
	Selected = 1 << 1,
	ReachableTile = 1 << 2,
};

/*
 * Bitmask enums arent working as expected in data files. I am using hardcoded state combinations
 * that gets converted during runtime.
 * TODO: Find a more elegant way
*/
UENUM(BlueprintType)
enum class EGridUnitStateCombined : uint8
{
	Default = EGridUnitState::Default,
	Hover = EGridUnitState::Hover,
	Selected = EGridUnitState::Selected,
	ReachableTile = EGridUnitState::ReachableTile,
	SelectedAndHoverTile = static_cast<uint8>(EGridUnitState::Selected) | static_cast<uint8>(EGridUnitState :: Hover),
	ReachableAndHover = static_cast<uint8>(EGridUnitState::ReachableTile) | static_cast<uint8>(EGridUnitState :: Hover),
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
	FVector2f GridUnitScale {1.0f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FIntPoint GridDimensions {10};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* GridUnitMesh { nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> GridPlacementTraceChannel { ECollisionChannel::ECC_WorldStatic };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EGridUnitStateCombined, FGridColorData> GridStateColorMap;
};


USTRUCT(BlueprintType)
struct TBSTEMPLATE_API FGridState
{
	GENERATED_BODY()
	FIntPoint GridIndex {0};
	EGridAccessState GridAccessState {EGridAccessState::Max};
	float Height {0.0f};
	uint16 MeshInstanceIndex {0};
	uint8 State {static_cast<uint8>(EGridUnitState::Default)};
};