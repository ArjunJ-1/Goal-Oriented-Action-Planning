// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Queue.h"
#include "CoreMinimal.h"

/**
 * 
 */
// Avoids Circular Dependencies
class AShip;
class GOAPAction;

// We use a basic struct called GOAP Node when playing
// This functions similarly to our GridNode for pathfinding
// These are used when buildings the search in the planner
struct GOAPNode
{
	// Where did this node come from
	GOAPNode* Parent;
	// The cost so far to reach this point
	float RunningCost;
	// The current state of the world when reaching this point
	// This will be the world state with all previous effects applied
	TMap<FString, bool> State;
	// A node is associated with an action. This stores a reference to that action
	GOAPAction* Action;
};

// The GOAP_Planner class itself
// This is only ever treated as a static class
// This could be improved when making a non-static implementation using object pooling
// The method for handling GOAP_Nodes is pretty rudimentary currently and can be resource intensive
class FIT3094_A1_CODE_API GOAPPlanner
{
public:
	// This unction builds out the node graph until it exhausts all possible paths
	// The agent calling it passed itself through along with its Available Actions
	// and action queue
	// Note: We are using C++ so we MUST be references or pointers otherwise pass by copy
	static bool Plan(AShip* Ship, const TSet<GOAPAction*>& AvailableActions,
					TQueue<GOAPAction*>& PlannedActions, TMap<FString, bool> WorldState,
					TMap<FString, bool> GoalState);

protected:
	// This function build out the node graph unitl it exhausts all possible paths
	// It uses a combination of A* and Brute Force to do this. As such there is room for optimization
	// Calls itself recursively to uild out nodes in the graph and find Goals
	static bool BuildGraphRecursively(TArray<GOAPNode*>& AllNodes, GOAPNode* Parent,
									  TArray<GOAPNode*>& GoalNodes, const TSet<GOAPAction*>& AvailableActions,
									  TMap<FString, bool>& GoalState);

	// Function used for creating a subset of actions with an action removed
	// Used to reduce available actions so they are not called twice
	// Potentially limiting if you want to call the same action twice
	static TSet<GOAPAction*> CreateActionSubset(const TSet<GOAPAction*>& AvailableActions,
												 GOAPAction* RemoveAction);

	// Fucntion compares a set of conditions to a state. Used to both determine if goals are met
	// and also if preconditions match existing world state
	static bool CheckedConditionsInState(TMap<FString, bool>& Conditions,
										 TMap<FString, bool>& State);

	// Combine a current state with the change (aka effects) of an action
	static TMap<FString, bool> PopulateNewState(const TMap<FString, bool>& CurrentState,
												TMap<FString, bool>& Changes);
};
