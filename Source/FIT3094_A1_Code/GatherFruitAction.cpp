// Fill out your copyright notice in the Description page of Project Settings.

#include "GatherFruitAction.h"
#include "GridNode.h"
#include "LevelGenerator.h"
#include "Ship.h"

GatherFruitAction::GatherFruitAction()
{
	Reset();
}

GatherFruitAction::~GatherFruitAction()
{
}

bool GatherFruitAction::IsActionDone(AShip* Ship)
{
	if(FruitGathered >= FruitToGather)
	{
		return true;
	}
	return false;
}

bool GatherFruitAction::CheckProceduralPrecondition(AShip* Ship)
{
	if(!Ship)
	{
		return false;
	}

	GoalNode = Ship->Level->CalculateNearestGoal(Ship->xPos, Ship->yPos, GridNode::FruitResource);

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

bool GatherFruitAction::PerformAction(AShip* Ship, float DeltaTime)
{
	ActionTime += DeltaTime;
	// Once all the time to collect all 50 resources has passed we increased the gathered count
	// Increase the ships' inventory
	// Decrease the resource deposit in the level
	if(ActionTime >= TimeToCollect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Fruit Gathered"));
		FruitGathered++;
		Ship->TotalResourceCollected++;
		GoalNode->ResourceNum--;
		ActionTime = 0;
	}
	return true;
}

bool GatherFruitAction::RequiresInRange()
{
	return true;
}

void GatherFruitAction::Reset()
{
	SetInRange(false);
	Target = nullptr;
	ActionTime = 0;
	FruitGathered = 0;
}
