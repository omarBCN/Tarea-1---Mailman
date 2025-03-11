// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinding/NavGraph.h"

#include "EdGraphSchema_K2_Actions.h"
#include "ViewportInteractionTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Pathfinding/NodeComponent.h"
#include "Pathfinding/PathfindingUtils.h"

// Direction/Navigation variables definitions
const ANavGraph::FCoords ANavGraph::Down =	FCoords(1, 0);
const ANavGraph::FCoords ANavGraph::Up =	FCoords(-1, 0);
const ANavGraph::FCoords ANavGraph::Right =	FCoords(0, 1);
const ANavGraph::FCoords ANavGraph::Left =	FCoords(0,-1);


// Sets default values
ANavGraph::ANavGraph()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	// Component creation
	PlayerComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerComponent"));
	PlayerComponent->SetupAttachment(RootComponent);
    
	EnemyComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyComponent"));
	EnemyComponent->SetupAttachment(RootComponent);
    
	TargetComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetComponent"));
	TargetComponent->SetupAttachment(RootComponent);
    
	// Load Meshes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    
	if (CubeMesh.Succeeded()) PlayerMeshAsset = CubeMesh.Object;
	if (ConeMesh.Succeeded()) TargetMeshAsset = ConeMesh.Object;
	if (CylinderMesh.Succeeded()) EnemyMeshAsset = CylinderMesh.Object;

	// Set Scale for "Players"
	PlayerComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	EnemyComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	TargetComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	if (!PlayerComponent->GetStaticMesh()) PlayerComponent->SetStaticMesh(PlayerMeshAsset);
	if (!EnemyComponent->GetStaticMesh()) EnemyComponent->SetStaticMesh(EnemyMeshAsset);
	if (!TargetComponent->GetStaticMesh()) TargetComponent->SetStaticMesh(TargetMeshAsset);
}

// Called when the game starts or when spawned
void ANavGraph::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Blocked nodes al begin play: %d"), BlockedNodes.Num());
	
	// Calls Djistra algorithm & stores the path
	// auto ResultingMap = UPathfindingUtils::Dijkstra(this);
	// CurrentPath = UPathfindingUtils::Backtrack(this, ResultingMap);

	// Sets Path Material
	for (auto Node : CurrentPath)
	{
		Node->SetMaterial(0, M_SelectedNode);
	}

	// Set starting players positions 
	if (StartNode)
	{
		PlayerComponent->SetRelativeLocation(StartNode->GetRelativeLocation() + FVector(0, 0, 100.f));
	}
	if (EndNode) //starting enemy positions 
	{
		EnemyComponent->SetRelativeLocation(EndNode->GetRelativeLocation() + FVector(0, 0, 100.f));
	}
		
	// starting target positions 
	RelocateTarget();

	// Generate random obstacles
	GenerateRandomObstacles(0);
	
	
	//Init GameState
	CurrentGameState = EGameState::PlayerTurn;
	CurrentPlayerNode = StartNode;

	SetupPlayerInput();
}

void ANavGraph::UpdateAdjacentNodeInfo()
{
 ///
}


void ANavGraph::SetupPlayerInput()
{
	// Get PlayerController
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (UInputComponent* PlayerInputComponent = PC->InputComponent)
			{
				// Assingns click action
				PlayerInputComponent->BindAction("Click", IE_Pressed, this, &ANavGraph::HandleMouseClick);
			}
		}
	}
}


void ANavGraph::HandleMouseClick()	
{
	if (CurrentGameState != EGameState::PlayerTurn)
		return;
    
	UNodeComponent* ClickedNode = GetNodeUnderCursor();
	if (ClickedNode)
	{
		OnNodeClicked(ClickedNode);
	}
}

UNodeComponent* ANavGraph::GetNodeUnderCursor() const
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
		return nullptr;
    
	// Get mouse position
	FVector WorldLocation, WorldDirection;
	if (PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		// raycast mouse - nodes
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, WorldLocation + WorldDirection * 10000.0f, ECC_Visibility))
		{
			return Cast<UNodeComponent>(HitResult.GetComponent());
		}
	}
    
	return nullptr;
}

UNodeComponent* ANavGraph::GetNodeFromCoords(const FCoords& CurrentCoords) const
{
	if (!IsCoordWithinGraph(CurrentCoords))
		return nullptr;
	
	return GraphNodes[CoordToLinear(CurrentCoords)];
}


void ANavGraph::AddAdjacentNode(UNodeComponent* Node, const FCoords& AdjacentNodeCoords)
{
	if (UNodeComponent* AdjacentNode = GetNodeFromCoords(AdjacentNodeCoords))
	{
		Node->AddAdjacentNode(AdjacentNode);
	}
}

// Sets adjacent nodes by checking up, down, left & right poisitions. If valids (check done by AddAdjacentNode()), adds them as adjacent.
void ANavGraph::SetupAdjacentNodes()
{
	for (int32 NodeIndex = 0; NodeIndex < GraphNodes.Num(); ++NodeIndex )
	{
		const FCoords CurrentCoords = LinearToCoord(NodeIndex);
		
		AddAdjacentNode(GraphNodes[NodeIndex], CurrentCoords + Up);
		AddAdjacentNode(GraphNodes[NodeIndex], CurrentCoords + Right);
		AddAdjacentNode(GraphNodes[NodeIndex], CurrentCoords + Down);
		AddAdjacentNode(GraphNodes[NodeIndex], CurrentCoords + Left);

		UE_LOG(LogTemp, Display, TEXT("Node Coords: %s; Index: %d"), *CurrentCoords.ToString(), NodeIndex);
	}
}

void ANavGraph::RefreshNodeMaterials()
{
	for (auto Node : GraphNodes)
	{
		if (StartNode == Node || EndNode == Node) continue;
		if (BlockedNodes.Contains(Node)) continue;
		Node->SetMaterial(0, M_DefaultNode);
	}
}

void ANavGraph::GenerateRandomObstacles(int32 NumObstacles)
{
	// set all blocked nodes to default material
	for (auto Node : BlockedNodes)
	{
		Node->SetMaterial(0, M_DefaultNode);
	}
	
	// Cleans array 
	BlockedNodes.Empty();
    
	// Nodes that won't be considered to be an obstacle: StartNode & EndNode
	TArray<UNodeComponent*> ExcludedNodes;
	ExcludedNodes.Add(StartNode);
	ExcludedNodes.Add(EndNode);
    
	// Nodes that won't be considered as an obstacle: TargetLocation
	for (UNodeComponent* Node : GraphNodes)
	{
		FVector TargetLocation = TargetComponent->GetRelativeLocation() - FVector(0, 0, 100.f);
		if (FVector::DistSquared(Node->GetRelativeLocation(), TargetLocation) < 100.0f)
		{
			ExcludedNodes.Add(Node);
			break;
		}
	}
    
	// Genrates random obstacles 
	for (int32 i = 0; i < NumObstacles; i++)
	{
		UNodeComponent* ObstacleNode = UPathfindingUtils::GetRandomNode(GraphNodes, ExcludedNodes);
        
		if (ObstacleNode)
		{
			BlockedNodes.Add(ObstacleNode);
			ObstacleNode->SetMaterial(0, M_BlockedNode);
			ExcludedNodes.Add(ObstacleNode);
		}
	}
}

bool ANavGraph::IsNodeWithinMoveRange(UNodeComponent* CurrentNode, const UNodeComponent* TargetNode, int32 MaxDistance, bool IgnoreObstacles)
{
	TArray<UNodeComponent*> Queue;
	TMap<UNodeComponent*, int32> Distance;
    
	Queue.Add(CurrentNode);
	Distance.Add(CurrentNode, 0);
    
	while (Queue.Num() > 0)
	{
		UNodeComponent* Node = Queue[0];
		Queue.RemoveAt(0);
        
		int32 CurrentDistance = Distance[Node];
        
		// Available destiny found
		if (Node == TargetNode)
		{
			return CurrentDistance <= MaxDistance;
		}
        
		// If we haven't exceeded the maximum distanc
		if (CurrentDistance < MaxDistance)
		{
			TArray<UNodeComponent*> TempAdjacentNodes;
			Node->GetAdjacentNodes(TempAdjacentNodes);
            
			for (UNodeComponent* AdjacentNode : TempAdjacentNodes)
			{
				// If it's an obstacle and we're not ignoring obstacles, skip it
				if (!IgnoreObstacles && BlockedNodes.Contains(AdjacentNode))
				{
					continue;
				}
                
				if (!Distance.Contains(AdjacentNode))
				{
					Queue.Add(AdjacentNode);
					Distance.Add(AdjacentNode, CurrentDistance + 1);
				}
			}
		}
	}
	return false;
}

void ANavGraph::OnNodeClicked(UNodeComponent* ClickedNode)
{
	// Game state to verify if Player Tunr
	if (CurrentGameState != EGameState::PlayerTurn)
	{
		return;
	}
    
	// Verify is Node is in range
	if (!IsNodeWithinMoveRange(CurrentPlayerNode, ClickedNode, 2))
	{
		return;
	}
    
	// Exclude Enemy position 
	if (ClickedNode == EndNode)
	{
		return;
	}

	// Exclude obstacles
	if (!IsNodeWithinMoveRange(CurrentPlayerNode, ClickedNode, 2, false))
	{
		return;
	}
    
	// Move player to selected node
	PlayerComponent->SetRelativeLocation(ClickedNode->GetRelativeLocation() + FVector(0, 0, 100.f));
	CurrentPlayerNode = ClickedNode;
    
	// Find target's Node
	UNodeComponent* TargetNode = nullptr;
	FVector TargetLocation = TargetComponent->GetRelativeLocation() - FVector(0, 0, 100.f);
    
	for (UNodeComponent* Node : GraphNodes)
	{
		if (FVector::DistSquared(Node->GetRelativeLocation(), TargetLocation) < 10.0f)
		{
			TargetNode = Node;
			break;
		}
	}
    
	// Check if player has reached the target
	if (ClickedNode == TargetNode)
	{
		// Relocate target after catching it 
		RelocateTarget();
	}
    
	// Switch to IA's turn
	StartEnemyTurn();
}

void ANavGraph::StartEnemyTurn()
{
	CurrentGameState = EGameState::EnemyTurn;
    
	// Timer to make enemy's move visible
	GetWorldTimerManager().SetTimer(EnemyTurnTimer, this, &ANavGraph::ProcessEnemyMove, 1.0f, false);
}

void ANavGraph::ProcessEnemyMove()
{
	// Stores original start & end Nodes
    UNodeComponent* OriginalStart = StartNode;
    UNodeComponent* OriginalEnd = EndNode;
    
    // Switches between start and end nodes: Enemy is Origin and Player, destination
    UNodeComponent* CurrentEnemyNode = EndNode; 
    StartNode = CurrentEnemyNode;               
    EndNode = CurrentPlayerNode;                
    
    // Create path Dijstra & backtrack
    auto ResultingMap = UPathfindingUtils::Dijkstra(this);
    CurrentPath = UPathfindingUtils::Backtrack(this, ResultingMap);
    
    // Restores original nodes
    StartNode = OriginalStart;
    EndNode = OriginalEnd;
    
    // Reset materials from old path
	RefreshNodeMaterials();
    
    // Sets material to new path except to blocked nodes
    for (auto Node : CurrentPath)
    {
    	if (Node != StartNode && Node != EndNode && !BlockedNodes.Contains(Node))
        {
            Node->SetMaterial(0, M_SelectedNode);
        }
    }
    
    // Enemy Move
    if (CurrentPath.Num() >= 2)
    {
   
    	// Invert path since it could return backwards from backtrack
        UNodeComponent* FirstPathNode = CurrentPath[0];
        UNodeComponent* SecondPathNode = CurrentPath[1];
        
        // Verify is array needs to be inverted
        if (FirstPathNode == CurrentPlayerNode)
        {
            Algo::Reverse(CurrentPath); // Reverses array
            SecondPathNode = CurrentPath[1]; // Recalculates after reversing
        }
    	
        UNodeComponent* NextNode = SecondPathNode;
        
        // Update enmy location
        EnemyComponent->SetRelativeLocation(NextNode->GetRelativeLocation() + FVector(0, 0, 100.f));
        EndNode = NextNode; 
        
        // Check if player has ben captured
        if (NextNode == CurrentPlayerNode)
        {
            CurrentGameState = EGameState::GameOver;
            HandleGameOver(false);
            return;
        }
    }
    CurrentGameState = EGameState::PlayerTurn;
}

void ANavGraph::RelocateTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("Blocked nodes before relocate tyarget: %d"), BlockedNodes.Num());
	// reset materials for the entire graph
	for (auto Node : GraphNodes)
	{
		if (Node != StartNode && Node != EndNode)
		{
			Node->SetMaterial(0, M_DefaultNode);
		}
	}
	
	// Creates array with nodes to exclude
	TArray<UNodeComponent*> NodesToExclude;
	NodesToExclude.Add(CurrentPlayerNode);
	NodesToExclude.Add(EndNode);
    
	// Gets a random node excluding the previous array
	UNodeComponent* RandomNode = UPathfindingUtils::GetRandomNode(GraphNodes, NodesToExclude);
    
	if (RandomNode)
	{
		TargetComponent->SetRelativeLocation(RandomNode->GetRelativeLocation() + FVector(0, 0, 100.f));
	}

	// REgenerate obstacles
	BlockedNodes.Empty(); 
	GenerateRandomObstacles(NumRows / 2);

	UE_LOG(LogTemp, Warning, TEXT("Blocked nodes after relocate tyarget: %d"), BlockedNodes.Num());

	auto ResultingMap = UPathfindingUtils::Dijkstra(this);
	CurrentPath = UPathfindingUtils::Backtrack(this, ResultingMap);
    
	for (auto Node : CurrentPath)
	{
		if (Node != StartNode && Node != EndNode && !BlockedNodes.Contains(Node))
		{
			Node->SetMaterial(0, M_SelectedNode);
		}
	}
}

void ANavGraph::HandleGameOver(bool PlayerWon)
{
	if (PlayerWon)
	{
		UE_LOG(LogTemp, Display, TEXT("¡El jugador ha ganado!"));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("El enemigo ha capturado al jugador. Juego terminado."));
	}
}

void ANavGraph::OnConstruction(const FTransform& Transform)		
{
	Super::OnConstruction(Transform);

	// control variable. If false, constructor won't be executed. Default value = True;
	if (!bRunConstruction)
		return;

	bRunConstruction = false;

	// Graph cleaning
	for ( auto Node : GraphNodes)
	{
		if (Node)
		{
			Node->UnregisterComponent();		
			Node->MarkAsGarbage();				// tells Garbage Collector the actor is ready to be collected. Similar to DestroyActor but destruction is not inmediate and is used with UObjects, not Actors.
		}
	}
	GraphNodes.Reset();

	// Graph builder
	for (int32 i = 0; i < NumRows; ++i )
	{
		for (int32 j = 0; j < NumRows; j++ )     
		{
			// Node creation and naming
			UNodeComponent* Node = NewObject<UNodeComponent>(this, *FString::Printf(TEXT("Node_%d_%d"), i, j));

			// store on component list
			GraphNodes.Add(Node);

			// set location
			Node->SetRelativeLocation({i * DistanceBetweenNodes, j * DistanceBetweenNodes, 0.0});

			// attach component to graph
			Node->SetupAttachment(RootComponent);

			// Register component in order to get physics and rendered
			Node->RegisterComponent();

			// Set Mesh and material to Node
			if (NodeMesh)
			{
				Node->SetStaticMesh(NodeMesh);

				if (M_DefaultNode)
				{
					Node->SetMaterial(0, M_DefaultNode);
				}
			}
		}
	}

	// Set Adjacent Nodes
	SetupAdjacentNodes();
}

void ANavGraph::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty->GetName().Equals(GET_MEMBER_NAME_CHECKED(ANavGraph, StartNodeCoords).ToString()))
	{
		FCoords Coord{static_cast<int32>(StartNodeCoords.X), static_cast<int32>(StartNodeCoords.Y)};

		if (IsCoordWithinGraph(Coord))
		{
			const int32 Index = CoordToLinear(Coord);
			StartNode = GraphNodes[Index];	
			StartNode->SetMaterial(0,M_SelectedNode);
		}
	}
	else if (PropertyChangedEvent.MemberProperty->GetName().Equals(GET_MEMBER_NAME_CHECKED(ANavGraph, EndNodeCoords).ToString()))
	{
		FCoords Coord{static_cast<int32>(EndNodeCoords.X), static_cast<int32>(EndNodeCoords.Y)};

		if (IsCoordWithinGraph(Coord))
		{
			const int32 Index = CoordToLinear(Coord);
			EndNode = GraphNodes[Index];
			EndNode->SetMaterial(0,M_SelectedNode);
		}
	}
		
	RefreshNodeMaterials();
}

int32 ANavGraph::CoordToLinear(const FCoords& InCoordinates) const
{
	return InCoordinates.i * NumRows + InCoordinates.j;
}

ANavGraph::FCoords ANavGraph::LinearToCoord(int32 Index) const
{
	return {Index / NumRows , Index % NumRows};
}

// Checks if Node has all adjacent nodes (up, down, left, right)
bool ANavGraph::IsCoordWithinGraph(const FCoords& InCoordsToCheck) const
{
	return InCoordsToCheck.i >= 0 && InCoordsToCheck.j >= 0 && InCoordsToCheck.i < NumRows && InCoordsToCheck.j < NumRows;
}

// Called every frame
void ANavGraph::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

