// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIDataComponent.generated.h"

// Forward declaration
class UNavigationQueryFilter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AI_2025_API UAIDataComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category="AI Data | Objectives", meta=(AllowPrivateAccess=true))
	TArray<AActor*> TargetsToMoveTo;

	//Navigation Filter
	UPROPERTY(EditInstanceOnly, Category="AI Data | Objectives", meta=(AllowPrivateAccess=true))
	TSubclassOf<UNavigationQueryFilter> AgentNavQueryFilter;

public:
	// Constructor
	UAIDataComponent();

	//These getters allow access to "Targets" and QueryFilter since those are private properties
	AActor* GetNextTarget();
	UClass* GetNavQueryFilterClass() const; //Cuando creemos un getter hay que ponerlo como const para evitar que pueda modificar la clase.
	
	
};
