// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherWoodAction.h"
#include "GridNode.h"
#include "LevelGenerator.h"
#include "Ship.h"

GatherWoodAction::GatherWoodAction()
{
	Reset();
}

GatherWoodAction::~GatherWoodAction()
{
}

bool GatherWoodAction::IsActionDone(AShip* Ship)
{
	if(WoodGathered >= WoodToGather)
	{
		return true;
	}
	return false;
}

bool GatherWoodAction::CheckProceduralPrecondition(AShip* Ship)
{
	if(!Ship)
	{
		return false;
	}

	GoalNode = Ship->Level->CalculateNearestGoal(Ship->xPos, Ship->yPos, GridNode::WoodResource);

	if(!GoalNode || !GoalNode->ResourceAtLocation || GoalNode->ResourceNum < 0)
	{
		return false;
	}

	Target = GoalNode->ResourceAtLocation;

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

bool GatherWoodAction::PerformAction(AShip* Ship, float DeltaTime)
{
	ActionTime += DeltaTime;
	// Once all the time to collect all 50 resources has passed we increased the gathered count
	// Increase the ships' inventory
	// Decrease the resource deposit in the level
	if(ActionTime >= TimeToCollect)
	{
		WoodGathered++;
		Ship->TotalResourceCollected++;
		GoalNode->ResourceNum--;
		ActionTime = 0;
	}
	return true;
}

bool GatherWoodAction::RequiresInRange()
{
	return true;
}

void GatherWoodAction::Reset()
{
	SetInRange(false);
	Target = nullptr;
	ActionTime = 0;
	WoodGathered = 0;
}
