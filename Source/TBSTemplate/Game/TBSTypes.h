#pragma once
#include "TBSTypes.generated.h"

namespace Constants
{
	inline static uint8 PlayerTeamId = 0;
}

UENUM()
enum class EGamePhase : uint8
{
	Exploration,
	Deployment,
	Battle,
	Max
};

UENUM()
enum class EDirection
{
	Up,
	Down,
	Left,
	Right,
	None
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMulticastDelegate);
