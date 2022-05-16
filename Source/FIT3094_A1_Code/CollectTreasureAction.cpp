// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectTreasureAction.h"
#include "GridNode.h"
#include "LevelGenerator.h"
#include "Ship.h"

CollectTreasureAction::CollectTreasureAction()
{
	Reset();
}

CollectTreasureAction::~CollectTreasureAction()
{
}

void CollectTreasureAction::Reset()
{
	SetInRange(false);
	Target = nullptr;
	ActionTime = 0;
}

bool CollectTreasureAction::IsActionDone(AShip* Ship)
{
	if(TreasureGathered >= TreasureToGather)
	{
		Ship->morale = 200;
		return true;
	}
	return false;
}

bool CollectTreasureAction::CheckProceduralPrecondition(AShip* Ship)
{
	if(!Ship)
	{
		return false;
	}

	GridNode* GoalNode = Ship->Level->CalculateNearestGoal(Ship->xPos, Ship->yPos, GridNode::GoldResource);

	if(!GoalNode || !GoalNode->ResourceAtLocation)
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

bool CollectTreasureAction::PerformAction(AShip* Ship, float DeltaTime)
{
	ActionTime += DeltaTime;
	AGold* GoldResource = Cast<AGold>(Target);

	if(!GoldResource || !Ship->Level->IsGoldValid(GoldResource))
		return false;

	if(ActionTime >= TimeToCollect)
	{
		
		TreasureGathered++;
		UE_LOG(LogTemp, Warning, TEXT("Ship: %s Treassure Gathered: %d"), *Ship->GetName(), TreasureGathered);
		Ship->Level->CollectGold(GoldResource);
		Target = nullptr;
	}
	return true;
}

bool CollectTreasureAction::RequiresInRange()
{
	return true;
}
