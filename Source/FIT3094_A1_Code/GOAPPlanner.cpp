// Fill out your copyright notice in the Description page of Project Settings.

#include "GOAPPlanner.h"
#include "GOAPAction.h"
#include "Ship.h"


bool GOAPPlanner::Plan(AShip* Ship, const TSet<GOAPAction*>& AvailableActions, TQueue<GOAPAction*>& PlannedActions,
                       TMap<FString, bool> WorldState, TMap<FString, bool> GoalState)
{
	// Create an array to old all nodes generated during planning
	TArray<GOAPNode*> AllNodes;

	// Reset all action states
	for (auto Action: AvailableActions)
	{
		Action->DoReset();
	}

	// Clear planned action queue
	PlannedActions.Empty();

	// Get usable actions
	TSet<GOAPAction*> UsableActions;
	for (auto Action: AvailableActions)
	{
		if(Action->CheckProceduralPrecondition(Ship))
		{
			UsableActions.Add(Action);
		}
	}

	// Build a tree of actions
	TArray<GOAPNode*> GoalNodes;

	// Define starting node
	GOAPNode* Start = new GOAPNode;
	Start->Parent = nullptr;
	Start->RunningCost = 0;
	Start->State = WorldState;
	Start->Action = nullptr;

	// Call BuildGraph function (returns true or false depending on if path is found)
	bool IsSuccessbul = BuildGraphRecursively(AllNodes, Start, GoalNodes, UsableActions, GoalState);

	if(!IsSuccessbul)
	{
		UE_LOG(LogTemp, Warning, TEXT("GoalPlanner: No Plan Found"));
		return false;
	}

	// Get the cheapest goal
	GOAPNode* CheapestNode = nullptr;
	for(auto Node: GoalNodes)
	{
		if(CheapestNode)
		{
			if(Node->RunningCost < CheapestNode->RunningCost)
			{
				CheapestNode = Node;
			}
		}
		else
		{
			CheapestNode = Node;
		}
	}

	// Work back from cheapest goal to start
	TArray<GOAPAction*> Results;
	GOAPNode* CurrentNode = CheapestNode;

	while (CurrentNode)
	{
		if(CurrentNode->Action)
		{
			Results.Insert(CurrentNode->Action, 0);
		}
		CurrentNode = CurrentNode->Parent;
	}

	// Build Queue from result
	for (auto Action: Results)
	{
		PlannedActions.Enqueue(Action);
	}

	// Clean up nodes
	AllNodes.Empty();

	return true;
}

bool GOAPPlanner::BuildGraphRecursively(TArray<GOAPNode*>& AllNodes, GOAPNode* Parent, TArray<GOAPNode*>& GoalNodes,
	const TSet<GOAPAction*>& AvailableActions, TMap<FString, bool>& GoalState)
{
	// For every action that is available
	for (auto Action: AvailableActions)
	{
		// Check to see if the preconditions of a state allow it to run
		if(CheckedConditionsInState(Action->Preconditions, Parent->State))
		{
			// Create a new updated world state based on current state and action effects
			TMap<FString, bool> CurrentState = PopulateNewState(Parent->State, Action->Effects);

			// Create a new GOAP Node object for this action and position in graph
			GOAPNode* Node = new GOAPNode;
			Node->Parent = Parent;
			Node->RunningCost = Parent->RunningCost + Action->ActionCost;
			Node->State = CurrentState;
			Node->Action = Action;

			// Add it to our main list of nodes for deletion later
			AllNodes.Add(Node);

			// Check conditions to see if we have a match for our goal
			if(CheckedConditionsInState(GoalState, CurrentState))
			{
				// Found Goal
				GoalNodes.Add(Node);

				// Return true and break loop. We do not need to recur further
				return true;
			}
			else
			{
				// Create a new subset of available actions without the current state
				TSet<GOAPAction*> ActionSubset = CreateActionSubset(AvailableActions, Action);

				// Call this function recursively until we are out of actions or find a goal
				if(BuildGraphRecursively(AllNodes, Node, GoalNodes, ActionSubset, GoalState))
				{
					return true;
				}
			}
		}
	}

	// if we find nothing return false
	return false;
}

TSet<GOAPAction*> GOAPPlanner::CreateActionSubset(const TSet<GOAPAction*>& AvailableActions,
	GOAPAction* RemoveAction)
{
	TSet<GOAPAction*> NewActionSet;

	for	(auto Action: AvailableActions)
	{
		if(Action != RemoveAction)
		{
			NewActionSet.Add(Action);
		}
	}

	return NewActionSet;
}

bool GOAPPlanner::CheckedConditionsInState(TMap<FString, bool>& Conditions, TMap<FString, bool>& State)
{
	 for (auto Condition: Conditions)
	 {
	 	bool* CurrentStateCondition = State.Find(Condition.Key);

	 	if(CurrentStateCondition)
	 	{
	 		if(Condition.Value != *CurrentStateCondition)
	 		{
	 			return false;
	 		}
	 	}
	    else
	    {
		    return false;
	    }
	 }

	return true;
}

TMap<FString, bool> GOAPPlanner::PopulateNewState(const TMap<FString, bool>& CurrentState,
	TMap<FString, bool>& Changes)
{
	TMap<FString, bool> NewState = CurrentState;

	for (auto Pairs: Changes)
	{
		NewState.Add(Pairs.Key, Pairs.Value);
	}

	return NewState;
}

