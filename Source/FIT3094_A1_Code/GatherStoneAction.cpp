// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherStoneAction.h"
#include "GridNode.h"
#include "LevelGenerator.h"
#include "Ship.h"

GatherStoneAction::GatherStoneAction()
{
	Reset();
}

GatherStoneAction::~GatherStoneAction()
{
}

bool GatherStoneAction::IsActionDone(AShip* Ship)
{
	if(StoneGathered >= StoneToGather)
	{
		return true;
	}
	return false;
}

bool GatherStoneAction::CheckProceduralPrecondition(AShip* Ship)
{
	if(!Ship)
	{
		return false;
	}

	GoalNode = Ship->Level->CalculateNearestGoal(Ship->xPos, Ship->yPos, GridNode::StoneResource);

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

bool GatherStoneAction::PerformAction(AShip* Ship, float DeltaTime)
{
	ActionTime += DeltaTime;
	// Once all the time to collect all 50 resources has passed we increased the gathered count
	// Increase the ships' inventory
	// Decrease the resource deposit in the level
	if(ActionTime >= TimeToCollect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stone Gathered"));
		StoneGathered++;
		Ship->TotalResourceCollected++;
		GoalNode->ResourceNum--;
		ActionTime = 0;
	}
	return true;
}

bool GatherStoneAction::RequiresInRange()
{
	return true;
}

void GatherStoneAction::Reset()
{
	SetInRange(false);
	Target = nullptr;
	ActionTime = 0;
	StoneGathered = 0;
}
