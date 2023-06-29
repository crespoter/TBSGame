#include "GridSystemTypes.h"

DEFINE_LOG_CATEGORY(LogGridSystems);

FGridState::FGridState(const EGridInstanceType InstanceType, const EGridInstanceActivityType ActivityType,
	const uint16 MeshInstanceIndex) :
	MeshInstanceIndex(MeshInstanceIndex),
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
