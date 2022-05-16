// Fill out your copyright notice in the Description page of Project Settings.


#include "Ship.h"

#include <concrt.h>
#include <fstream>

#include "CollectRumFromHome.h"
#include "CollectTreasureAction.h"
#include "DepositResources.h"
#include "GatherFruitAction.h"
#include "GatherStoneAction.h"
#include "GatherWoodAction.h"
#include "GOAPPlanner.h"
#include "Gold.h"
#include "LevelGenerator.h"
#include "MerchantAction.h"

// Sets default values
AShip::AShip()
{
 	// Set this actor to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;
	MoveSpeed = 200;
	Tolerance = 10;

	ActionStateMachine = new StateMachine<ACTOR_STATES, AShip>(this, State_Nothing);
	ActionStateMachine->RegisterState(State_Idle, &AShip::OnIdleEnter, &AShip::OnIdleTick, &AShip::OnIdleExit);
	ActionStateMachine->RegisterState(State_Move, &AShip::OnMoveEnter, &AShip::OnMoveTick, &AShip::OnMoveExit);
	ActionStateMachine->RegisterState(State_Action, &AShip::OnActionEnter, &AShip::OnActionTick, &AShip::OnActionExit);
	ActionStateMachine->ChangeState(State_Idle);

	MaxIdleTime = 3;
	CurrentIdleTime = 0;
	
	// Start with the action of collecting gold
	CollectTreasureAction* TreasureAction = new CollectTreasureAction();
	TreasureAction->AddPrecondition("HasMorale", false);
	TreasureAction->AddEffect("HasMorale", true);
	AvailableActions.Add(TreasureAction);

	// Decrease Rum for each action performed. Each ship starts with 75 rum
	NumRum = 75;
}

// Called when the game starts or when spawned
void AShip::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ActionStateMachine->Tick(DeltaTime);
}

void AShip::OnIdleEnter()
{
	// Collect resources if ship does not have any resources
	if(TotalResourceCollected == 0)
		ShipAction();
	
	// Once the resource has been collected, we perform the deposit action to deposit resources in the home Island
	// After the resources have been deposited, the ship can go on another run t collect more resources.
	if(TotalResourceCollected == 0 && DoOnce1)
		DoOnce1 = false;
	if(TotalResourceCollected >= 50 && !DoOnce1)
	{
		DepositResources* DepositAction = new DepositResources();
		DepositAction->AddPrecondition("ResourceCollected", true);
		DepositAction->AddEffect("ResourceCollected", false);
		AvailableActions.Add(DepositAction);
		DoOnce1 = true;
	}

	// Collect rum from home Island 
	if(NumRum > 0 && CollectRum)
		CollectRum = false;
	if(NumRum <= 0 && !CollectRum)
	{
		CollectRumFromHome* CollectRumAction = new CollectRumFromHome();
		CollectRumAction->AddPrecondition("RumBarrels", false);
		CollectRumAction->AddEffect("RumBarrels", true);
		AvailableActions.Add(CollectRumAction);
		CollectRum = true;
	}

	// Only collect gold if morale is <= 40
	if(morale > 40 && CollectGold)
		CollectGold = false;
	if(morale <= 40 && !CollectGold)
	{
		CollectTreasureAction* TreasureAction = new CollectTreasureAction();
		TreasureAction->AddPrecondition("HasMorale", false);
		TreasureAction->AddEffect("HasMorale", true);
		AvailableActions.Add(TreasureAction);
		CollectGold = true;
	}
}

void AShip::OnIdleTick(float Delta_Time)
{
	FinishedMoving = true;
	if(CurrentIdleTime >= MaxIdleTime)
	{
		CurrentIdleTime = 0;

		// in this state we are looking to make a plan
		// First get the current world state
		TMap<FString, bool> WorldState = GetWorldState();
		// get the desired goal state
		TMap<FString, bool> GoalState = GetGoalState();

		// Attempt to make a plan and check success
		if(GOAPPlanner::Plan(this, AvailableActions, CurrentActions, WorldState, GoalState))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s has found a plan. Executing plan"), *GetName());
			ActionStateMachine->ChangeState(State_Action);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s was unable to find a plan. Idling for %f seconds"), *GetName(), MaxIdleTime);
		}
	}
	else
	{
		CurrentIdleTime += Delta_Time;
	}
}

void AShip::OnIdleExit()
{
}

void AShip::OnMoveEnter()
{
	// Entering into the move state check to ensure we have some from of movement
	// If we do not then cancel immediately
	if(CurrentActions.IsEmpty())
	{
		ActionStateMachine->ChangeState(State_Idle);
		return;
	}

	// if current action require an in range check and the target is null, return to planning
	GOAPAction* CurrentAction = *CurrentActions.Peek();
	if(CurrentAction->RequiresInRange() && CurrentAction->Target == nullptr)
	{
		ActionStateMachine->ChangeState(State_Idle);
		return;
	}

	// Else lets make a plan to move to the new action
	if(CurrentAction->RequiresInRange())
	{
		GoalLocation = Level->FindGridNode(CurrentAction->Target);
		if(GoalLocation)
		{
			Level->CalculatePath(this, GoalLocation);
		}
	}
}

void AShip::OnMoveTick(float Delta_Time)
{
	// Get the current action we are executing
	// This will never be null as the CurrentActions must have a count to enter this state
	GOAPAction* CurrentAction = *CurrentActions.Peek();
	// as we move set the previous node object at location to null pointer
	Level->WorldArray[xPos][yPos]->ObjectAtLocation = nullptr;
	// Remove the reserved path as we have moved past it
	Level->WorldArray[xPos][yPos]->ReservedPath.Remove(this);
	//...
	if(!FinishedMoving)
	{
		// Return to planning when action is no longer valid.
		// Check if the resource > 0 so the ship can collect making sure that it's not a gold actor or a home actor
		if(GoalLocation->ResourceAtLocation == nullptr
			|| (GoalLocation->ResourceNum < 50 && !GoalLocation->ResourceAtLocation->IsA(AGold::StaticClass())
				&& !GoalLocation->ResourceAtLocation->IsA(AHomeIsland::StaticClass())))
		{
			Reroute(); // Delete the path and change state to idle
		}
		
		else if(Path.Num() > 0)
		{
			// Get our current location
			FVector CurrentPosition = GetActorLocation();
			
			// Calculate the target position based off the X & Y values inside of path
			const float TargetXPos = Path[0]->X * ALevelGenerator::GRID_SIZE_WORLD;
			const float TargetYPos = Path[0]->Y * ALevelGenerator::GRID_SIZE_WORLD;
			const FVector TargetPosition(TargetXPos, TargetYPos, CurrentPosition.Z);

			// has more than 1 ship reserved this path
			if(Path.Num() > 1)
			{
				// list of ships who have reserved this path
				TMap<AShip*, bool> ReservedPathList = Level->WorldArray[Path[1]->X][Path[1]->Y]->ReservedPath;
				if(ReservedPathList.Num() > 1) 
				{
					// find the ship that's closest to it's goal by checking for the path length
					int MinPathLen = 999999; // temp value holder
					AShip* ShipToMove = nullptr; // The ship that with the shortest path goes first
					// for every ship that reserved the path find the one with the shortest path
					for(auto& Elem: ReservedPathList)
					{
						if(MinPathLen > Elem.Key->Path.Num())
						{
							MinPathLen = Elem.Key->Path.Num();
							ShipToMove = Elem.Key;
						}
					}
					// If the ship has reserved this path but it's not the one that should move then reroute this ship
					if(ReservedPathList.Contains(this) && this != ShipToMove)
						Reroute();
				}
			}
			
			FVector Direction = TargetPosition - CurrentPosition;
			Direction.Normalize();

			CurrentPosition += Direction * MoveSpeed * Delta_Time;
			SetActorLocation(CurrentPosition);

			if(FVector::Dist(CurrentPosition, TargetPosition) <= Tolerance)
			{
				xPos = Path[0]->X;
				yPos = Path[0]->Y;
				// Current node object at location will be this ship
				Level->WorldArray[xPos][yPos]->ObjectAtLocation = this;
				CurrentPosition = TargetPosition;
				Path.RemoveAt(0);
				morale--;
				FinishedMoving = true;
			}
		}
		else
		{
			GoalLocation->ReservedPath.Add(this, true);
			//Level->WorldArray[xPos][yPos]->ObjectAtLocation = this;
			// Set the current action to be in range. We have moved to its location so this is valid
			CurrentAction->SetInRange(true);
			// Change to the Action state. Time to perform our action
			ActionStateMachine->ChangeState(State_Action);
		}
	}
}

void AShip::OnMoveExit()
{
}

void AShip::OnActionEnter()
{
}

void AShip::OnActionTick(float Delta_Time)
{
	// We do not need to move so set this to true
	FinishedMoving = true;
	// if we have no states change to idle and exit immediately
	if(CurrentActions.IsEmpty())
	{
		ActionStateMachine->ChangeState(State_Idle);
		return;
	}

	// Check to see if our action has finished
	GOAPAction* CurrentAction = *CurrentActions.Peek();
	if(CurrentAction->IsActionDone(this))
	{
		// Decrease the morale and rum for each action performed
		morale--;
		// Rum is only decreased when we collect resources
		if(CurrentAction->Target && !CurrentAction->Target->IsA(AGold::StaticClass()) &&
			!CurrentAction->Target->IsA(AHomeIsland::StaticClass()) && NumRum >= 0)
			NumRum--;
		// We have finished with the action. Lets get rid of it
		CurrentActions.Dequeue(CurrentAction);

		// Delete all actors once the goal has been reached
		// These are here to help visualize our path. We are done with them
		for(auto PathObject : PathDisplayActors)
		{
			PathObject->Destroy();
		}
		PathDisplayActors.Empty();
	}

	// if at this point we still have more actions continue the process
	if(!CurrentActions.IsEmpty())
	{
		// get the top of the queue again
		// Technically this is a pointer to a pointer this why we dereference it once
		CurrentAction = *CurrentActions.Peek();

		// Check to see if we need to be within range for an action
		// if no range requirements is needed return true
		// if a range requirement is needed then check to see if we are in range
		// Ternary operators are nasty but they do have their place!
		bool InRange = CurrentAction->RequiresInRange() ? CurrentAction->IsInRange() : true;

		// if we are in range attempt the action
		if(InRange)
		{
			// Attempt to perform the action
			// Actions can fail which is why we need to store the result
			bool IsActionSuccessful = CurrentAction->PerformAction(this, Delta_Time);

			// if we fail the action change to the idle state and report that we had to abort the plan
			if(!IsActionSuccessful)
			{
				ActionStateMachine->ChangeState(State_Idle);
				OnPlanAborted(CurrentAction);
			}
		}
		else
		{
			// At this point we have a  valid action but we are not in range. Commence movement
			ActionStateMachine->ChangeState(State_Move);
		}
	}
	else
	{
		// No Action remaining. return to idle state
		ActionStateMachine->ChangeState(State_Idle);
	}
}

void AShip::OnActionExit()
{
	Level->WorldArray[xPos][yPos]->ReservedPath.Remove(this);
}

TMap<FString, bool> AShip::GetWorldState()
{
	TMap<FString, bool> WorldState;

	WorldState.Emplace("HasMorale", morale > 100);
	WorldState.Emplace("ResourceCollected", TotalResourceCollected >= 50);
	WorldState.Emplace("RumBarrels", NumRum > 0);
	return WorldState;
}

TMap<FString, bool> AShip::GetGoalState()
{
	TMap<FString, bool> GoalState;

	GoalState.Emplace("HasMorale", true);
	return GoalState;
}

void AShip::OnPlanFailed(TMap<FString, bool> FailedGoalStates)
{
}

void AShip::OnPlanAborted(GOAPAction* FailedAction)
{
}

// For each type of ship, append its action into available actions
void AShip::ShipAction()
{
	if(ResourceType == GridNode::WoodResource)
	{
		GatherWoodAction* WoodAction = new GatherWoodAction();
		WoodAction->AddPrecondition("ResourceCollected", false);
		WoodAction->AddEffect("ResourceCollected", true);
		AvailableActions.Add(WoodAction);	
	}
	if(ResourceType == GridNode::FruitResource)
	{
		GatherFruitAction* FruitAction = new GatherFruitAction();
		FruitAction->AddPrecondition("ResourceCollected", false);
		FruitAction->AddEffect("ResourceCollected", true);
		AvailableActions.Add(FruitAction);
	}
	if(ResourceType == GridNode::StoneResource)
	{
		GatherStoneAction* StoneAction = new GatherStoneAction();
		StoneAction->AddPrecondition("ResourceCollected", false);
		StoneAction->AddEffect("ResourceCollected", true);
		AvailableActions.Add(StoneAction);
	}
	if(ResourceType == GridNode::MerchantResource)
	{
		MerchantAction* RumAction = new MerchantAction();
		RumAction->AddPrecondition("ResourceCollected", false);
		RumAction->AddEffect("ResourceCollected", true);
		AvailableActions.Add(RumAction);
	}
}

void AShip::Reroute()
{
	// We Finish moving to the target
	// Remove the action from the CurrentActions queue
	// set the reserved path to false and empty the path
	FinishedMoving = true;
	// for every path node remove the reservation by this ship
	for(auto& Elem: Path)
	{
		Level->WorldArray[Elem->X][Elem->Y]->ReservedPath.Remove(this);
		//Elem->ReservedPath.Remove(this);
	}
	Path.Empty();
			
	// Delete all actors once the goal has been reached
	// These are here to help visualize our path. We are done with them
	for(const auto PathObject : PathDisplayActors)
	{
		PathObject->Destroy();
	}
	PathDisplayActors.Empty();

	// Return back to the idle state to plan
	ActionStateMachine->ChangeState(State_Idle);
}
