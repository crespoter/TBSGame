#include "GridSystemTypes.h"

DEFINE_LOG_CATEGORY(LogGridSystems);

FGridState::FGridState(const EGridInstanceType InstanceType, const EGridInstanceActivityType ActivityType)
	: InstanceType(InstanceType), ActivityType(ActivityType)
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
}

void FGridState::SetActivityType(const EGridInstanceActivityType InActivityType)
{
	ActivityType = InActivityType;
}

bool FGridState::IsRendered() const
{
	return InstanceType != EGridInstanceType::None;
}

bool FGridState::IsVisuallySameWith(const FGridState& OtherGridState) const
{
	return GridAccessState == OtherGridState.GridAccessState &&
		(FMath::Abs(Height - OtherGridState.Height) < 0.1) &&
		IsRendered() == OtherGridState.IsRendered() &&
		InstanceType == OtherGridState.InstanceType &&
		ActivityType == OtherGridState.ActivityType;
}
