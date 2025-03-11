// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavGraph.generated.h"

class UNodeComponent;

// GameState
UENUM()
enum class EGameState : uint8
{
	PlayerTurn,
	EnemyTurn,
	GameOver
};

UCLASS()
class AI_2025_API ANavGraph : public AActor
{
	GENERATED_BODY()

	// Coordinates struct
	struct FCoords
	{
		FCoords(int32 _i = 0, int32 _j = 0) : i(_i), j(_j) {}		// constructor 
		int32 i, j;

		// operator "+" overloading to implement FCoords sum
		FCoords operator+(const FCoords& Other) const
		{
			return { i + Other.i, j + Other.j };
		}

		// prints coordinates for debug
		FString ToString() const { return FString::Printf(TEXT("%d,%d"), i, j); }
	};

private:
	UPROPERTY(EditInstanceOnly, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	int32 NumRows = 3;
 
	UPROPERTY(EditInstanceOnly, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	float DistanceBetweenNodes = 100;

	UPROPERTY(EditAnywhere, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	TObjectPtr<UStaticMesh> NodeMesh;

	UPROPERTY(EditAnywhere, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> M_SelectedNode;

	UPROPERTY(EditAnywhere, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> M_BlockedNode;

	UPROPERTY(EditAnywhere, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	TObjectPtr<UMaterialInterface> M_DefaultNode;

	UPROPERTY(VisibleAnywhere, Category="Navigation", meta=(AllowPrivateAccess = true))
	TArray<TObjectPtr<UNodeComponent>> GraphNodes;

	UPROPERTY(EditAnywhere, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	FVector2D StartNodeCoords;
	
	UPROPERTY(EditAnywhere, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	FVector2D EndNodeCoords;
	
	UPROPERTY(EditAnywhere, Category="Navigation | Customization", meta=(AllowPrivateAccess = true))
	bool bRunConstruction = true;

	UPROPERTY()
	TArray<UNodeComponent*> BlockedNodes;

public:
	// Mesh components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Characters")
	UStaticMeshComponent* PlayerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Characters")
	UStaticMeshComponent* EnemyComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Characters")
	UStaticMeshComponent* TargetComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meshes")
	UStaticMesh* PlayerMeshAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meshes")
	UStaticMesh* EnemyMeshAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Meshes")
	UStaticMesh* TargetMeshAsset;
	
	// Sets default values for this actor's properties
	ANavGraph();
	
	FORCEINLINE int GetNumberOfRows() const { return NumRows; }
	FORCEINLINE const TArray<TObjectPtr<UNodeComponent>>& GetGraphNodes() const {return GraphNodes;}

	// Getters for Start & End Nodes 
	FORCEINLINE UNodeComponent* GetStartNode() const {return StartNode;}
	FORCEINLINE UNodeComponent* GetEndNode() const {return EndNode;}


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

private:
	
	// Direction/Navigation variables to operate with adjacent Nodes. Defined in cpp since those are static const member class variables
	static const FCoords Down;
	static const FCoords Up;
	static const FCoords Right; 
	static const FCoords Left;

	// Converts Coordinates to single value index
	FORCEINLINE int32 CoordToLinear(const FCoords& InCoordinates) const;

	// Returns coordinates from indexed Node
	FORCEINLINE FCoords LinearToCoord(int32 Index) const;

	// Checks if Node has all adjacent nodes (up, down, left, right)
	FORCEINLINE bool IsCoordWithinGraph(const FCoords& InCoordsToCheck) const;

	// Returns info about adjacent nodes. Called from Onconstruction();
	void UpdateAdjacentNodeInfo();

	// Adds Adjacent nodes if valid
	void AddAdjacentNode(UNodeComponent* Node, const FCoords& AdjacentNodeCoords);

	// Sets adjacent nodes by checking up, down, left & right poisitions. If valids (check done by AddAdjacentNode()), adds them as adjacent.
	void SetupAdjacentNodes();

	// Refresh Nodes' Materials
	void RefreshNodeMaterials();

	// Checks is player move is valid 
	bool IsNodeWithinMoveRange(UNodeComponent* CurrentNode, const UNodeComponent* TargetNode, int32 MaxDistance, bool IgnoreObstacles = false);

	//Generates Random Obstacles that can be navigated by Enmey but not by player
	void GenerateRandomObstacles(int32 NumObstacles);
	
	UNodeComponent* GetNodeFromCoords(const FCoords& CurrentCoords) const;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void OnNodeClicked(UNodeComponent* ClickedNode);

	// Timer to make change turn visible
	FTimerHandle EnemyTurnTimer;
	
	void StartEnemyTurn();
	
	void ProcessEnemyMove();

	void RelocateTarget();
	
	void HandleGameOver(bool PlayerWon);
	
private:
	UPROPERTY()
	TObjectPtr<UNodeComponent> StartNode;
	UPROPERTY()	
	TObjectPtr<UNodeComponent> EndNode;

	// Path
	TArray<UNodeComponent*> CurrentPath;

	UPROPERTY()
	EGameState CurrentGameState;

	// Current Player position
	UPROPERTY()
	UNodeComponent* CurrentPlayerNode;

protected:
	void SetupPlayerInput();
	
	void HandleMouseClick();
	
	UNodeComponent* GetNodeUnderCursor() const;
};
