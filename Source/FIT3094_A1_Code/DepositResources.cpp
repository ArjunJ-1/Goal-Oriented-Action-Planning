// Fill out your copyright notice in the Description page of Project Settings.


#include "DepositResources.h"
#include "GridNode.h"
#include "LevelGenerator.h"
#include "Ship.h"

DepositResources::DepositResources()
{
	Reset();
}

DepositResources::~DepositResources()
{
}

bool DepositResources::IsActionDone(AShip* Ship)
{
	if(ResourceDeposited >= ResourcesToDeposit)
	{
		return true;
	}
	return false;
}

bool DepositResources::CheckProceduralPrecondition(AShip* Ship)
{
	if(!Ship)
	{
		return false;
	}

	GoalNode = Ship->Level->CalculateNearestGoal(Ship->xPos, Ship->yPos, GridNode::Home);
	if(!GoalNode)
	{
		return false;
	}

	Target = GoalNode->ResourceAtLocation; // This is the reference to the home island instead of the resource
	// Cast to the home Island to increase the resource count in the home island
	HomeIsland = Cast<AHomeIsland>(Target);
	if(!HomeIsland)
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

bool DepositResources::PerformAction(AShip* Ship, float DeltaTime)
{
	ActionTime += DeltaTime*2; // Deposit speed is 2X
	// Once all the time to Deposit all 50 resources has passed we increased the Deposit count
	// Decrease the ships' inventory
	if(ActionTime >= TimeToDeposit)
	{
		ResourceDeposited++;
		Ship->TotalResourceCollected--;
		ActionTime = 0;

		// Update all the home nodes simultaneously
		for(auto& elem: AllNodesOfType)
		{
			AActor* TempActor = elem->ResourceAtLocation;
			AHomeIsland* TempHome = Cast<AHomeIsland>(TempActor);
			if(!TempHome)
				return false;
			
			// For each type of resource being deposited, Increase the amount of resource on HomeIsland
			if(Ship->ResourceType == GridNode::WoodResource)
				TempHome->NumWood++;
			if(Ship->ResourceType == GridNode::StoneResource)
				TempHome->NumStone++;
			if(Ship->ResourceType == GridNode::FruitResource)
				TempHome->NumFruit++;
			if(Ship->ResourceType == GridNode::MerchantResource)
				TempHome->NumRum++;
		}
	}
	return true;
}

bool DepositResources::RequiresInRange()
{
	return true;
}

void DepositResources::Reset()
{
	SetInRange(false);
	Target = nullptr;
	ActionTime = 0;
	ResourceDeposited = 0;
}
