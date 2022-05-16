// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GOAPAction.h"
#include "CoreMinimal.h"

/**
 * 
 */
class GridNode;
class FIT3094_A1_CODE_API MerchantAction: public GOAPAction
{
public:
	MerchantAction();
	~MerchantAction();

	bool IsActionDone(AShip* Ship) override;
	bool CheckProceduralPrecondition(AShip* Ship) override;
	bool PerformAction(AShip* Ship, float DeltaTime) override;
	bool RequiresInRange() override;

private:
	void Reset();
	// How many treasure need to be gathered for the action to be completed
	const int RumToGather = 5;
	// How long does it take to gather a treasure. Actions are not instant they can take time
	const int TimeToCollect = 1;
	// how many treasures have been gathered so far. This will either be 0 or 1 at all times
	int RumGathered = 0;
	// How much time has elapsed inside this action so far. This is used to determine if the action completes
	float ActionTime;
	// Goal node
	GridNode* GoalNode;
};
