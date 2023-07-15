#include "GridSystemTypes.h"

DEFINE_LOG_CATEGORY(LogGridSystems);

FGridState::FGridState(const EGridInstanceType InstanceType, const EGridInstanceActivityType ActivityType,
	const uint16 MeshInstanceIndex) :
	bIsUnitRendered(InstanceType != EGridInstanceType::None)
{
}


EGridInstanceType FGridState::GetInstanceType() const
{
	return InstanceType;
}

EGridInstanceActivityType FGridState::GetActivityType() const
{
	return ActivityType;
}

void FGridState::SetInstanceType(const EGridInstanceType InInstanceType)
{
	InstanceType = InInstanceType;
	bIsUnitRendered = InstanceType != EGridInstanceType::None;
}

void FGridState::SetActivityType(const EGridInstanceActivityType InActivityType)
{
	ActivityType = InActivityType;
}

bool FGridState::IsVisuallySameWith(const FGridState& OtherGridState) const
{
	return GridAccessState == OtherGridState.GridAccessState &&
		(FMath::Abs(Height - OtherGridState.Height) < 0.1) &&
		bIsUnitRendered == OtherGridState.bIsUnitRendered &&
		InstanceType == OtherGridState.InstanceType &&
		ActivityType == OtherGridState.ActivityType;
}
