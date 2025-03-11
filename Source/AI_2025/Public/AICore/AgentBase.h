// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI_2025Character.h"
#include "AgentBase.generated.h"

// forward declaration 
class UAIDataComponent;

UCLASS()
class AI_2025_API AAgentBase : public AAI_2025Character
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="AI Data", meta = (AllowPrivateAccess = "true"))
	UAIDataComponent* AIDataComponent;
	
public:
	AAgentBase();
};
