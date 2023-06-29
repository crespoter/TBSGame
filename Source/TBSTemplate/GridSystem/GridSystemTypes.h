#pragma once
#include "GridSystemTypes.Generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGridSystems, Log, All);

class AHeroCharacter;
class ATBSCharacter;


// TODO: Move GamePhase to a new Type file
UENUM()
enum class EGamePhase : uint8
{
	Exploration,
	Deployment,
	Battle,
	Max
};


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
	Deployment,
	DebugGrid,
	None
};

UENUM()
enum class EGridInstanceActivityType
{
	None,
	Hover,
	Active
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


USTRUCT()
struct TBSTEMPLATE_API FGridState
{
	GENERATED_BODY()

	FGridState() = default;
	
	FGridState(EGridInstanceType InstanceType, EGridInstanceActivityType ActivityType, uint16 MeshInstanceIndex);

	FORCEINLINE EGridInstanceType GetInstanceType() const;
	FORCEINLINE EGridInstanceActivityType GetActivityType() const;
	FORCEINLINE void SetInstanceType(const EGridInstanceType InInstanceType);
	FORCEINLINE void SetActivityType(const EGridInstanceActivityType InActivityType);

	
	EGridAccessState GridAccessState {EGridAccessState::Max};

	float Height {0.0f};

	uint16 MeshInstanceIndex {0};

	bool bIsUnitRendered {false};

	UPROPERTY()
	ATBSCharacter* OccupyingUnit {nullptr};

private:
	EGridInstanceType InstanceType {EGridInstanceType::None};
	EGridInstanceActivityType ActivityType {EGridInstanceActivityType::None};
};
