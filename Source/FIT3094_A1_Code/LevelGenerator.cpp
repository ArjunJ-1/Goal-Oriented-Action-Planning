// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGenerator.h"

#include "ResourceActor.h"
#include "HomeIsland.h"
#include "Engine/World.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame. 
	PrimaryActorTick.bCanEverTick = true;
	ResourceBlueprint = AResourceActor::StaticClass();
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool ResetMovement = true;

	for(auto Ship : ShipFleet)
	{
		if(!Ship->FinishedMoving)
		{
			ResetMovement = false;
		}
	}

	if(ResetMovement)
	{
		for(auto Ship : ShipFleet)
		{
			Ship->FinishedMoving = false;
		}
	}

	if(GoldActors.Num() < NUM_FOOD)
	{
		SpawnNextGold();
	}
}

void ALevelGenerator::GenerateWorldFromFile(TArray<FString> WorldArrayStrings)
{
	// If empty array exit immediately something is horribly wrong
	if(WorldArrayStrings.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("World Array is empty!"));
		return;
	}

	// Second line is Height (aka Y value)
	FString Height = WorldArrayStrings[1];
	Height.RemoveFromStart("Height ");
	MapSizeY = FCString::Atoi(*Height);
	UE_LOG(LogTemp, Warning, TEXT("Height: %d"), MapSizeY);

	// Third line is Width (aka X value)
	FString Width = WorldArrayStrings[2];
	Width.RemoveFromStart("width ");
	MapSizeX = FCString::Atoi(*Width);
	UE_LOG(LogTemp, Warning, TEXT("Width: %d"), MapSizeX);

	char CharMapArray[MAX_MAP_SIZE][MAX_MAP_SIZE];
	
	// Read through the Map section for create the CharMapArray
	for (int LineNum = 4; LineNum < MapSizeY + 4; LineNum++)
	{
		for (int CharNum = 0; CharNum < WorldArrayStrings[LineNum].Len(); CharNum++)
		{
			CharMapArray[LineNum-4][CharNum] = WorldArrayStrings[LineNum][CharNum];
		}
	}

	GenerateNodeGrid(CharMapArray);
	SpawnWorldActors(CharMapArray);
}

void ALevelGenerator::SpawnWorldActors(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE])
{
	UWorld* World = GetWorld();

	// Make sure that all blueprints are connected. If not then fail
	if(DeepBlueprint && ShallowBlueprint && LandBlueprint && WoodBlueprint && StoneBlueprint && FruitBlueprint && MerchantBlueprint)
	{
		AResourceActor* TempResource = nullptr;
		AHomeIsland* TempHome = nullptr;
		// For each grid space spawn an actor of the correct type in the game world
		for(int x = 0; x < MapSizeX; x++)
		{
			for (int y = 0; y < MapSizeY; y++)
			{
				float XPos = x * GRID_SIZE_WORLD;
				float YPos = y * GRID_SIZE_WORLD;

				FVector Position(XPos, YPos, 0);

				switch (Grid[x][y])
				{
					case '.':
						World->SpawnActor(DeepBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case '@':
						World->SpawnActor(LandBlueprint, &Position, &FRotator::ZeroRotator);
						break;
					case 'H':
						// Home Island actor is spawned in the world. 
						TempHome = Cast<AHomeIsland>(World->SpawnActor(HomeBlueprint, &Position, &FRotator::ZeroRotator));
						TempHome->XPos = x;
						TempHome->YPos = y;
						WorldArray[x][y]->ResourceAtLocation = TempHome;
						break;
					case 'W':
						World->SpawnActor(WoodBlueprint, &Position, &FRotator::ZeroRotator);
						TempResource = Cast<AResourceActor>(World->SpawnActor(ResourceBlueprint, &Position, &FRotator::ZeroRotator));
						TempResource->ResourceType = GridNode::WoodResource;
						TempResource->XPos = x;
						TempResource->YPos = y;
						WorldArray[x][y]->ResourceAtLocation = TempResource;
						break;
					case 'S':
						World->SpawnActor(StoneBlueprint, &Position, &FRotator::ZeroRotator);
						TempResource = Cast<AResourceActor>(World->SpawnActor(ResourceBlueprint, &Position, &FRotator::ZeroRotator));
						TempResource->ResourceType = GridNode::StoneResource;
						TempResource->XPos = x;
						TempResource->YPos = y;
						WorldArray[x][y]->ResourceAtLocation = TempResource;
						break;
					case 'F':
						World->SpawnActor(FruitBlueprint, &Position, &FRotator::ZeroRotator);
						TempResource = Cast<AResourceActor>(World->SpawnActor(ResourceBlueprint, &Position, &FRotator::ZeroRotator));
						TempResource->ResourceType = GridNode::FruitResource;
						TempResource->XPos = x;
						TempResource->YPos = y;
						WorldArray[x][y]->ResourceAtLocation = TempResource;
						break;
					case 'M':
						World->SpawnActor(MerchantBlueprint, &Position, &FRotator::ZeroRotator);
						TempResource = Cast<AResourceActor>(World->SpawnActor(ResourceBlueprint, &Position, &FRotator::ZeroRotator));
						TempResource->ResourceType = GridNode::MerchantResource;
						TempResource->XPos = x;
						TempResource->YPos = y;
						WorldArray[x][y]->ResourceAtLocation = TempResource;
						break;
					default:
						break;
				}
			}
		}
	}

	// Generate Initial Agent Positions
	if(ShipBlueprint)
	{
		for (int i = 0; i < NUM_AGENTS; i++)
		{
			int RandXPos = 0;
			int RandYPos = 0;
			bool isFree = false;

			while (!isFree) {
				RandXPos = FMath::RandRange(0, MapSizeX - 1);
				RandYPos = FMath::RandRange(0, MapSizeY - 1);

				if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::DeepWater && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr)
				{
					isFree = true;
				}
			}

			FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
			AShip* Agent = World->SpawnActor<AShip>(ShipBlueprint, Position, FRotator::ZeroRotator);

			// Give each agent a particular type of resource to collect
			if(i % 4 == 0)
				Agent->ResourceType = GridNode::WoodResource;
			if(i % 4 == 1)
				Agent->ResourceType = GridNode::FruitResource;
			if(i % 4 == 2)
				Agent->ResourceType = GridNode::StoneResource;
			if(i % 4 == 3)
				Agent->ResourceType = GridNode::MerchantResource;
			
			Agent->AddTickPrerequisiteActor(this);
			Agent->Level = this;
			
			Agent->xPos = RandXPos;
			Agent->yPos = RandYPos;
			
			ShipFleet.Add(Agent);
			WorldArray[RandXPos][RandYPos]->ObjectAtLocation = Agent;
		}
	}

	if (GoldBlueprint)
	{
		for(int i = 0; i < NUM_FOOD; i++)
		{
			int RandXPos = 0;
			int RandYPos = 0;
			bool isFree = false;

			while (!isFree) {
				RandXPos = FMath::RandRange(0, MapSizeX - 1);
				RandYPos = FMath::RandRange(0, MapSizeY - 1);

				if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::DeepWater && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr)
				{
					isFree = true;
				}
			}

			FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
			AGold* NewGold = World->SpawnActor<AGold>(GoldBlueprint, Position, FRotator::ZeroRotator);

			WorldArray[RandXPos][RandYPos]->ResourceAtLocation = NewGold;
			GoldActors.Add(NewGold);
		}
	}

	// Set Static Camera Position
	if(Camera)
	{
		FVector CameraPosition = Camera->GetActorLocation();
		
		CameraPosition.X = MapSizeX * 0.5 * GRID_SIZE_WORLD;
		CameraPosition.Y = MapSizeY * 0.5 * GRID_SIZE_WORLD;
		
		Camera->SetActorLocation(CameraPosition);
	}
}

void ALevelGenerator::SpawnNextGold()
{
	UWorld* World = GetWorld();
	// Generate next Gold Position
	if (GoldBlueprint)
	{
		int RandXPos = 0;
		int RandYPos = 0;
		bool isFree = false;

		while (!isFree) {
			RandXPos = FMath::RandRange(0, MapSizeX - 1);
			RandYPos = FMath::RandRange(0, MapSizeY - 1);

			if (WorldArray[RandXPos][RandYPos]->GridType == GridNode::DeepWater && WorldArray[RandXPos][RandYPos]->ObjectAtLocation == nullptr)
			{
				isFree = true;
			}
		}

		FVector Position(RandXPos * GRID_SIZE_WORLD, RandYPos * GRID_SIZE_WORLD, 20);
		AGold* NewGold = World->SpawnActor<AGold>(GoldBlueprint, Position, FRotator::ZeroRotator);

		WorldArray[RandXPos][RandYPos]->ResourceAtLocation = NewGold;
		GoldActors.Add(NewGold);
	}
}

// Generates the grid of nodes used for pathfinding and also for placement of objects in the game world
void ALevelGenerator::GenerateNodeGrid(char Grid[MAX_MAP_SIZE][MAX_MAP_SIZE])
{
	for(int X = 0; X < MapSizeX; X++)
	{
		for(int Y = 0; Y < MapSizeY; Y++)
		{
			WorldArray[X][Y] = new GridNode();
			WorldArray[X][Y]->X = X;
			WorldArray[X][Y]->Y = Y;
			WorldArray[X][Y]->ResourceNum = 0;
			WorldArray[X][Y]->ObjectAtLocation = nullptr;
			WorldArray[X][Y]->ResourceAtLocation = nullptr;

			// Characters as defined from the map file
			switch(Grid[X][Y])
			{
				case '.':
					WorldArray[X][Y]->GridType = GridNode::DeepWater;
					break;
				case '@':
					WorldArray[X][Y]->GridType = GridNode::Land;
					break;
				case 'H':
					WorldArray[X][Y]->GridType = GridNode::Home;
					break;
				case 'W':
					WorldArray[X][Y]->GridType = GridNode::WoodResource;
					WorldArray[X][Y]->ResourceNum = 500;
					break;
				case 'S':
					WorldArray[X][Y]->GridType = GridNode::StoneResource;
					WorldArray[X][Y]->ResourceNum = 500;
					break;
				case 'F':
					WorldArray[X][Y]->GridType = GridNode::FruitResource;
					WorldArray[X][Y]->ResourceNum = 500;
					break;
				case 'M':
					WorldArray[X][Y]->GridType = GridNode::MerchantResource;
					WorldArray[X][Y]->ResourceNum = 500;
					break;
				default:
					break;
			}
		}
	}
}

// Reset all node values (F, G, H & Parent)
void ALevelGenerator::ResetAllNodes()
{
	for (int X = 0; X < MapSizeX; X++)
	{
		for (int Y = 0; Y < MapSizeY; Y++)
		{
			WorldArray[X][Y]->F = 0;
			WorldArray[X][Y]->G = 0;
			WorldArray[X][Y]->H = 0;
			WorldArray[X][Y]->Parent = nullptr;
			WorldArray[X][Y]->IsChecked = false;
		}
	}
}

float ALevelGenerator::CalculateDistanceBetween(GridNode* first, GridNode* second)
{
	FVector distToTarget = FVector(second->X - first->X,
		second->Y - first->Y, 0);
	return distToTarget.Size();
}

void ALevelGenerator::CalculatePath(AShip* TargetShip, GridNode* GoalNode)
{
	GridNode* currentNode = nullptr;
	GridNode* tempNode = nullptr;
	bool isGoalFound = false;
	
	int SearchCount = 0;

	TQueue<GridNode*> nodesToVisit;

	GridNode* StartNode = WorldArray[TargetShip->xPos][TargetShip->yPos];
	if(!GoalNode) {
		GoalNode = CalculateNearestGoal(StartNode->X, StartNode->Y, GridNode::GoldResource);
	}

	int StartXPos = TargetShip->GetActorLocation().X / GRID_SIZE_WORLD;
	int StartYPos = TargetShip->GetActorLocation().Y / GRID_SIZE_WORLD;
	StartNode = WorldArray[StartXPos][StartYPos];
	StartNode->IsChecked = true;
	nodesToVisit.Enqueue(StartNode);

	while (!nodesToVisit.IsEmpty())
	{
		SearchCount++;
		nodesToVisit.Dequeue(currentNode);

		if (currentNode == GoalNode)
		{
			isGoalFound = true;
			break;
		}

		// Check the left neighbour
		// Check to ensure not out of range
		if (currentNode->Y - 1 > 0)
		{
			// Get the Left neighbor from the list
			tempNode = WorldArray[currentNode->X][currentNode->Y - 1];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked && !tempNode->ObjectAtLocation)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}

		// Check the top neighbour
		// Check to ensure not out of range
		if (currentNode->X + 1 < MapSizeX)
		{
			// Get the top neighbor from the list
			tempNode = WorldArray[currentNode->X + 1][currentNode->Y];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked && !tempNode->ObjectAtLocation)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}

		// Check the right neighbour
		// Check to ensure not out of range
		if (currentNode->Y + 1 < MapSizeY)
		{
			// Get the right neighbor from the list
			tempNode = WorldArray[currentNode->X][currentNode->Y + 1];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked && !tempNode->ObjectAtLocation)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}

		// Check the bottom neighbour
		// Check to ensure not out of range
		if (currentNode->X - 1 > 0)
		{
			// Get the bottom neighbor from the list
			tempNode = WorldArray[currentNode->X - 1][currentNode->Y];
			// Check to make sure the node hasnt been visited AND is not closed (A wall)
			if (tempNode->GridType != GridNode::Land && !tempNode->IsChecked && !tempNode->ObjectAtLocation)
			{
				tempNode->IsChecked = true;
				tempNode->Parent = currentNode;
				nodesToVisit.Enqueue(tempNode);
			}
		}
	}

	if (isGoalFound)
	{
		RenderPath(TargetShip, GoalNode);
		TargetShip->GeneratePath = false;
	}
}

void ALevelGenerator::CalculatePath(AShip* TargetShip, GridNode::GRID_TYPE ResourceType)
{
	GridNode* StartNode = WorldArray[TargetShip->xPos][TargetShip->yPos];
	GridNode* GoalNode = CalculateNearestGoal(StartNode->X, StartNode->Y, ResourceType);

	CalculatePath(TargetShip, GoalNode);
}

void ALevelGenerator::ResetPath(AShip* CurrentShip, GridNode* StartNode, GridNode* GoalNode)
{
	for (int i = 0; i < CurrentShip->PathDisplayActors.Num(); i++)
	{
		CurrentShip->PathDisplayActors[i]->Destroy();
	}
	CurrentShip->PathDisplayActors.Empty();

	CurrentShip->Path.Empty();
}

void ALevelGenerator::DetailPath(AShip* CurrentShip, GridNode* StartNode, GridNode* GoalNode)
{
	/*
	//Onscreen Debug (Don't forget the include!)
	GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("Total Cells searched: %d with a path length of: %d and a distance of: %f"), SearchCount, Ship->Path.Num(), CalculateDistanceBetween(StartNode, GoalNode)));
	GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("The difference between the current implemented path and the direct flight path is: %f"), Ship->Path.Num() / CalculateDistanceBetween(StartNode, GoalNode)));
	//Log Debug message (Accessed through Window->Developer Tools->Output Log)
	UE_LOG(LogTemp, Warning, TEXT("Total Cells searched: %d with a path length of: %d and a distance of: %f"), SearchCount, Ship->Path.Num(), CalculateDistanceBetween(StartNode, GoalNode));
	UE_LOG(LogTemp, Warning, TEXT("The difference between the current implemented path and the direct flight path is: %f"), Ship->Path.Num() / CalculateDistanceBetween(StartNode, GoalNode));
	*/
}

void ALevelGenerator::RenderPath(AShip* CurrentShip, GridNode* GoalNode)
{
	UWorld* World = GetWorld();
	GridNode* CurrentNode = GoalNode;
	while (CurrentNode->Parent != nullptr)
	{
		// the current node will store all the ships that reserved the node
		// (Reserved the node by rendering a path on it)
		CurrentNode->ReservedPath.Add(CurrentShip, true);

		FVector Position(CurrentNode->X * GRID_SIZE_WORLD, CurrentNode->Y * GRID_SIZE_WORLD, 20);
		AActor* PDActor = World->SpawnActor<AActor>(PathDisplayBlueprint, Position, FRotator::ZeroRotator);
		CurrentShip->PathDisplayActors.Add(PDActor);

		GridNode* tempNode = new GridNode();
		tempNode->X = CurrentNode->X;
		tempNode->Y = CurrentNode->Y;
		
		CurrentShip->Path.EmplaceAt(0,tempNode);
		
		CurrentNode = CurrentNode->Parent;
	}

	ResetAllNodes();
}

GridNode* ALevelGenerator::CalculateNearestGoal(int XPos, int YPos, GridNode::GRID_TYPE ResourceType)
{
	float ShortestPath = 999999;

	if(YPos < 0 || XPos < 0 || YPos >= MapSizeY || XPos >= MapSizeX)
	{
		return nullptr;
	}

	GridNode* CurrentPosition = WorldArray[XPos][YPos];
	
	if(ResourceType == GridNode::GoldResource)
	{
		AGold* NearestGold = nullptr;
		for(auto Gold: GoldActors)
		{
			int XPos = Gold->GetActorLocation().X / GRID_SIZE_WORLD;
			int YPos = Gold->GetActorLocation().Y / GRID_SIZE_WORLD;
			
			float CurrentPath = CalculateDistanceBetween(CurrentPosition, WorldArray[XPos][YPos]);
			if(CurrentPath < ShortestPath)
			{
				NearestGold = Gold;
				ShortestPath = CurrentPath;
			}
		}

		if(NearestGold)
		{
			int XPos = NearestGold->GetActorLocation().X / GRID_SIZE_WORLD;;
			int YPos = NearestGold->GetActorLocation().Y / GRID_SIZE_WORLD;
			return WorldArray[XPos][YPos];
		}

		return nullptr;
	}

	GridNode* GoalNode = nullptr;
	
	for (int X = 0; X < MapSizeX; X++)
	{
		for (int Y = 0; Y < MapSizeY; Y++)
		{
			// Checks if the resources are still present
			// When resources <= 0 we want to make sure we deposit the resources currently on the ship
			if(WorldArray[X][Y]->GridType == ResourceType
				&& (WorldArray[X][Y]->ResourceNum > 0 || WorldArray[X][Y]->GridType == GridNode::Home
				|| WorldArray[X][Y]->GridType == GridNode::GoldResource)
				)
			{
				float CurrentPath = CalculateDistanceBetween(CurrentPosition, WorldArray[X][Y]);
				if(CurrentPath < ShortestPath)
				{
					GoalNode = WorldArray[X][Y];
					ShortestPath = CurrentPath;
				}
			}
		}
	}
	
	return GoalNode;
}

GridNode* ALevelGenerator::FindGridNode(AActor* ActorResource)
{
	GridNode* TempNode = nullptr;
	for (int X = 0; X < MapSizeX; X++)
	{
		for (int Y = 0; Y < MapSizeY; Y++)
		{
			if(WorldArray[X][Y]->ResourceAtLocation == ActorResource)
			{
				TempNode = WorldArray[X][Y];
			}
		}
	}

	return TempNode;
}

// This function is used to output all nodes on the map that have a single grid type
TArray<GridNode*> ALevelGenerator::FindGridNodesOfType(GridNode::GRID_TYPE GridType)
{
	TArray<GridNode*> AllNodesOfType;
	for (int X = 0; X < MapSizeX; X++)
	{
		for (int Y = 0; Y < MapSizeY; Y++)
		{
			if(WorldArray[X][Y]->GridType == GridType)
			{
				AllNodesOfType.Add(WorldArray[X][Y]);
			}
		}
	}
	return AllNodesOfType;
}

void ALevelGenerator::CollectGold(AGold* CollectedGold)
{
	GoldActors.Remove(CollectedGold);

	for (int X = 0; X < MapSizeX; X++)
	{
		for (int Y = 0; Y < MapSizeY; Y++)
		{
			if(WorldArray[X][Y]->ResourceAtLocation == CollectedGold)
			{
				WorldArray[X][Y]->ResourceAtLocation = nullptr;
			}
		}
	}

	CollectedGold->Destroy();
}

bool ALevelGenerator::IsGoldValid(AGold* CollectedGold)
{
	return GoldActors.Contains(CollectedGold);
}