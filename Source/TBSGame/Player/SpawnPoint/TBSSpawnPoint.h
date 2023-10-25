// Copyright Crespoter Interactive

#pragma once

#include "CoreMinimal.h"
#include "Engine/NavigationObjectBase.h"
#include "TBSSpawnPoint.generated.h"


class UArrowComponent;

UCLASS(Blueprintable)
class TBSGAME_API ATBSSpawnPoint : public ANavigationObjectBase
{
	GENERATED_BODY()
public:
	ATBSSpawnPoint();
private:
	
	UPROPERTY()
	UArrowComponent* ArrowComponent {nullptr};
};
