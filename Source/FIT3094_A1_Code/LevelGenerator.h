// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ship.h"
#include "CoreMinimal.h"
#include "Gold.h"
#include "GameFramework/Actor.h"
#include "GridNode.h"
#include "LevelGenerator.generated.h"

UCLASS()
class FIT3094_A1_CODE_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

	// Maximum Size for World Map
	static const int MAX_MAP_SIZE = 255;

public:

	// Grid Size in World Units
	static const int GRID_SIZE_WORLD = 100;
	static const int NUM_FOOD = 10;
	static const int NUM_AGENTS = 5;
	
	// Sets default values for this actor's properties
	ALevelGenerator();

	UPROPERTY(BlueprintReadOnly)
		int MapSizeX;
	UPROPERTY(BlueprintReadOnly)
		int MapSizeY;
	
	UPROPERTY()
		TArray<FVector2D> GoldArray;

	// This is a 2D Array for holding nodes for each part of the world
	GridNode* WorldArray[MAX_MAP_SIZE][MAX_MAP_SIZE];

	UPROPERTY()
		TArray<AGold*> GoldActors;

	// Actors for spawning into the world
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> PathDisplayBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> DeepBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> LandBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> ShallowBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> WoodBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> FruitBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> StoneBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> MerchantBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> GoldBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> ShipBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> ResourceBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		TSubclassOf<AActor> HomeBlueprint;
	UPROPERTY(EditAnywhere, Category = "Entities")
		AActor* Camera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SpawnWorldActors(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE]);

	void GenerateNodeGrid(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE]);
	void ResetAllNodes();

	float CalculateDistanceBetween(GridNode* first, GridNode* second);

	void SpawnNextGold();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void GenerateWorldFromFile(TArray<FString> WorldArray);
	
	//void CalculateBFS();
	//int SearchCount = 0;
	//bool IsPathCalculated;
	//GridNode* StartNode;
	//GridNode* GoalNode;

	//void ResetPath();
	//void RenderPath();
	//void DetailPath();
	//void ResetPath();
	bool IsGoldValid(AGold* CollectedGold);
	void CollectGold(AGold* CollectedGold);
	//AShip* Ship;

	// Assignment 2 Additions

	// Variables
	TSet<AShip*> ShipFleet;
	
	// Functions
	void CalculatePath(AShip* TargetShip, GridNode* GoalNode = nullptr);
	void CalculatePath(AShip* TargetShip, GridNode::GRID_TYPE ResourceType);
	GridNode* CalculateNearestGoal(int XPos, int YPos, GridNode::GRID_TYPE ResourceType);
	GridNode* FindGridNode(AActor* ActorResource);
	TArray<GridNode*> FindGridNodesOfType(GridNode::GRID_TYPE GridType);

	void ResetPath(AShip* CurrentShip, GridNode* StartNode, GridNode* GoalNode);
	void DetailPath(AShip* CurrentShip, GridNode* StartNode, GridNode* GoalNode);
	void RenderPath(AShip* CurrentShip, GridNode* GoalNode);
};
