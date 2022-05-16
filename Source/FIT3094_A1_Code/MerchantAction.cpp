// Fill out your copyright notice in the Description page of Project Settings.


#include "MerchantAction.h"
#include "GridNode.h"
#include "LevelGenerator.h"
#include "Ship.h"

MerchantAction::MerchantAction()
{
	Reset();
}

MerchantAction::~MerchantAction()
{
}

bool MerchantAction::IsActionDone(AShip* Ship)
{
	if(RumGathered >= RumToGather)
	{
		return true;
	}
	return false;
}

bool MerchantAction::CheckProceduralPrecondition(AShip* Ship)
{
	if(!Ship)
	{
		return false;
	}

	GoalNode = Ship->Level->CalculateNearestGoal(Ship->xPos, Ship->yPos, GridNode::MerchantResource);

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

bool MerchantAction::PerformAction(AShip* Ship, float DeltaTime)
{
	ActionTime += DeltaTime;
	// Once all the time to collect all 50 resources has passed we increased the gathered count
	// Increase the ships' inventory
	// Decrease the resource deposit in the level
	if(ActionTime >= TimeToCollect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rum Gathered"));
		RumGathered++;
		Ship->TotalResourceCollected++;
		GoalNode->ResourceNum--;
		ActionTime = 0;
	}
	return true;
}

bool MerchantAction::RequiresInRange()
{
	return true;
}

void MerchantAction::Reset()
{
	SetInRange(false);
	Target = nullptr;
	ActionTime = 0;
	RumGathered = 0;
}
