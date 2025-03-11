// Fill out your copyright notice in the Description page of Project Settings.


#include "AICore/AIDataComponent.h"
#include "NavFilters/NavigationQueryFilter.h"


// Sets default values for this component's properties
UAIDataComponent::UAIDataComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

AActor* UAIDataComponent::GetNextTarget()
{
	if (!TargetsToMoveTo.IsEmpty())
		return TargetsToMoveTo[0];
	else
		return nullptr;
}

UClass* UAIDataComponent::GetNavQueryFilterClass() const
{
	return AgentNavQueryFilter;
}



