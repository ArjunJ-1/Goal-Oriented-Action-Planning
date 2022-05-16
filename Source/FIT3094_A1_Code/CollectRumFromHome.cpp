// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectRumFromHome.h"
#include "GridNode.h"
#include "LevelGenerator.h"
#include "Ship.h"

CollectRumFromHome::CollectRumFromHome()
{
	Reset();
}

CollectRumFromHome::~CollectRumFromHome()
{
}

bool CollectRumFromHome::IsActionDone(AShip* Ship)
{
	if(RumCollected >= RumToCollect)
	{
		return true;
	}
	return false;
}

bool CollectRumFromHome::CheckProceduralPrecondition(AShip* Ship)
{
	if(!Ship)
	{
		return false;
	}
	
	GoalNode = Ship->Level->CalculateNearestGoal(Ship->xPos, Ship->yPos, GridNode::Home);
	//UE_LOG(LogTemp, Warning, TEXT("Resource Check Home Found X:%d Y:%d"), GoalNode->X, GoalNode->Y);

	if(!GoalNode)
	{
		return false;
	}

	Target = GoalNode->ResourceAtLocation; // This is the reference to the home island instead of the resource
	// Cast to the home Island to increase the resource count in the home island
	HomeIsland = Cast<AHomeIsland>(Target);
	if(!HomeIsland)
		return false;
	// Make sure that the Home has rum available
	if(HomeIsland->NumRum < 1)
		return false;
	AllNodesOfType = Ship->Level->FindGridNodesOfType(GridNode::Home);

	FVector Dist = Ship->GetActorLocation() - Target->GetActorLocation();

	if(Dist.Size() <= 5)
	{
		SetInRange(true);
	}
	else
	{
		SetInRange(false);
	}
	return true;
}

bool CollectRumFromHome::PerformAction(AShip* Ship, float DeltaTime)
{
	ActionTime += DeltaTime;
	// Increase rum collected from the home base, Increase Ships rum count and decrease the rum from home base
	if(ActionTime >= TimeToCollect)
	{
		RumCollected++;
		Ship->NumRum++;
		ActionTime = 0;

		// Update all the home nodes simultaneously
		for(auto& elem: AllNodesOfType)
		{
			AActor* TempActor = elem->ResourceAtLocation;
			AHomeIsland* TempHome = Cast<AHomeIsland>(TempActor);
			if(!TempHome)
				return false;
			
			// For each type of resource being deposited, Decrease the amount of rum on HomeIsland
			if(Ship->ResourceType == GridNode::MerchantResource)
				TempHome->NumRum--;
		}
	}
	return true;
}

bool CollectRumFromHome::RequiresInRange()
{
	return true;
}

void CollectRumFromHome::Reset()
{
	SetInRange(false);
	Target = nullptr;
	ActionTime = 0;
	RumCollected = 0;
}
