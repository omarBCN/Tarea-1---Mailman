// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding/PathfindingUtils.h"

#include "Pathfinding/NavGraph.h"
#include "Pathfinding/NodeComponent.h"

TMap<UNodeComponent*, float> UPathfindingUtils::Dijkstra(ANavGraph* InGraph)
{
	TMap<UNodeComponent*, float> DistanceNodesMap;
	TArray<UNodeComponent*> AvailableNodes = InGraph->GetGraphNodes();// Este array podria ser un set -> TSet<UNodeComponent*> AvailableNodes;

	//inicializar la lista
	for (auto Node : AvailableNodes)
	{ 
		if (InGraph->GetStartNode() == Node)
		{
			DistanceNodesMap.Add(Node, 0.f);
		}
		else
		{
			DistanceNodesMap.Add(Node, UE_MAX_FLT ); //UE_MAX_FLT es una macro de unreal que nos da un float con el máximo tamaño representable
		}
	}

	// recorrer los nodos hasta que la lista esté vacia

	while (!AvailableNodes.IsEmpty())
	{
		UNodeComponent* CurrentNode = UPathfindingUtils::GetShortestNode(DistanceNodesMap,AvailableNodes);
		AvailableNodes.Remove(CurrentNode);

		TArray<UNodeComponent*> AdjacentNodes;	//local variable AdjacentNodes
		CurrentNode->GetAdjacentNodes(AdjacentNodes);		// Call GetAdjacentNodes 

		for (auto Node: AdjacentNodes)
		{
			// Diferencia de distancias + Distancia acumulada
			const float Distance = FVector::Dist(CurrentNode->GetRelativeLocation(), Node->GetRelativeLocation()) +
				DistanceNodesMap[CurrentNode];

			// Almacenamos nueva distancia
			if (DistanceNodesMap[Node] > Distance)
			{
				DistanceNodesMap[Node] = Distance;
			}
		}
	}
	return DistanceNodesMap;
}

UNodeComponent* UPathfindingUtils::GetShortestNode(
	const TMap<UNodeComponent*, float> &InDistanceNodeMap,
	const TArray<UNodeComponent*> &InAvailableNodes)
{
	UNodeComponent* BestNode =!InAvailableNodes.IsEmpty() ? InAvailableNodes[0] : nullptr;
	
	for (auto Node : InAvailableNodes)
	{
		if (InDistanceNodeMap[Node] < InDistanceNodeMap[BestNode])
		{
			BestNode = Node;
		}
	}
	return BestNode;
}

// Recorre el path a la inversa, empezando por EndNode y hasta llegar a StartNode, evaluando cada nodo adyacente al Nodo
// por el que pasa (CurrentNode) y añadiendo al array aquel cuyo coste sea menor (via GetShortesNode())
TArray<UNodeComponent*> UPathfindingUtils::Backtrack(const ANavGraph* InGraph,
													 const TMap<UNodeComponent*, float> InMap)
{
	TArray<UNodeComponent*> Path;
	UNodeComponent* CurrentNode = InGraph->GetEndNode();

	while (CurrentNode != InGraph->GetStartNode())
	{
		Path.Add(CurrentNode);

		TArray<UNodeComponent*> AdjacentNodes;
		CurrentNode->GetAdjacentNodes(AdjacentNodes);

		CurrentNode = GetShortestNode(InMap, AdjacentNodes);
	}
	
	Path.Add(CurrentNode);		
	return Path;
}

// Get Random node within Path to spawn Target (and obstacles?)
UNodeComponent* UPathfindingUtils::GetRandomNode(const TArray<UNodeComponent*> &Path, TArray<UNodeComponent*> NodesToExclude)
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}
	// Filter unvalid nodes (startNode, EndNode, etc...)
	TArray<UNodeComponent*> ValidNodes;
	for (auto Node : Path)
	{
		if (!NodesToExclude.Contains(Node))
		{
			ValidNodes.Add(Node);
		}
	}
    
	if (ValidNodes.IsEmpty())
	{
		return nullptr;
	}
	else
	{
		int32 RandomIndex = FMath::RandRange(0, ValidNodes.Num() - 1);
		return ValidNodes[RandomIndex];
	}
}