#pragma once

class TBSHelpers
{
public:
	template <typename T> static T* GetActorOfClass(UWorld* World)
	{
		const TActorIterator<T> ActorIt(World);
		return *ActorIt;
	}

	template <typename T> static TArray<T*> GetActorsOfClass(UWorld* World)
	{
		TArray<T*> ActorList;
		for (TActorIterator<T> ActorIt(World); ActorIt; ++ActorIt)
		{
			ActorList.Add(*ActorIt);
		}
		return ActorList;
	}
};
