// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"    
#include "Components/StaticMeshComponent.h"
#include "NodeComponent.generated.h"

/**
 * 
 */

UCLASS()
class AI_2025_API UNodeComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UNodeComponent*> AdjacentNodes;

public:
	// Constructor
	UNodeComponent();
    
	void GetAdjacentNodes(TArray<UNodeComponent*>& OutAdjacentNodes) const {OutAdjacentNodes = AdjacentNodes;};
    
	void SetAdjacentNodes(const TArray<UNodeComponent*>& InAdjacentNodes) {AdjacentNodes = InAdjacentNodes;};
    
	void AddAdjacentNode(UNodeComponent* Node) {AdjacentNodes.Add(Node);};
};
