// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NodeComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PathfindingUtils.generated.h"

class ANavGraph;
/**
 * 
 */
UCLASS()
class AI_2025_API UPathfindingUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
 
public:
	
	static TMap<UNodeComponent*, float> Dijkstra(ANavGraph* InGraph);

	static UNodeComponent* GetShortestNode(const TMap<UNodeComponent*, float>& Map, const TArray<UNodeComponent*>&  Array);

	static UNodeComponent* GetRandomNode(const TArray<UNodeComponent*>& Path, TArray<UNodeComponent*> NodesToExclude = TArray<UNodeComponent*>());
	
	static TArray<UNodeComponent*> Backtrack(const ANavGraph* InGraph, const TMap<UNodeComponent*, float> InMap);
};
