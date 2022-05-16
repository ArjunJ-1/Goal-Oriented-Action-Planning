// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GridNode.h"
#include "CoreMinimal.h"
#include "StateMachine.h"
#include "GOAPAction.h"
#include "GameFramework/Actor.h"
#include "Ship.generated.h"

class ALevelGenerator;

UCLASS()
class FIT3094_A1_CODE_API AShip : public AActor
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Stats")
		float MoveSpeed;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float Tolerance;

	// Valid states the state machine can be in
	enum ACTOR_STATES
	{
		State_Nothing,
		State_Idle,
		State_Move,
		State_Action
	};

public:
	// Sets default values for this actor's properties
	AShip();
	bool GeneratePath = true;
	bool FinishedMoving = false;
	bool DoOnce1 = false;
	bool CollectGold = false;
	bool CollectRum = false;
	TArray<GridNode*> Path;
	ALevelGenerator* Level;
	int morale = 100;

	int xPos;
	int yPos;

	GridNode::GRID_TYPE ResourceType;

	UPROPERTY()
		TArray<AActor*> PathDisplayActors;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// state machine state Idle functions
	void OnIdleEnter();
	void OnIdleTick(float Delta_Time);
	void OnIdleExit();

	// state machine state move functions
	void OnMoveEnter();
	void OnMoveTick(float Delta_Time);
	void OnMoveExit();

	// state machine state Action functions
	void OnActionEnter();
	void OnActionTick(float Delta_Time);
	void OnActionExit();

	StateMachine<ACTOR_STATES, AShip>* ActionStateMachine;

	// Use a timer to ensure that we call the timer every 3 seconds
	float MaxIdleTime;
	float CurrentIdleTime;

	// resources that the ship will be collecting
	int NumStones;
	int NumWood;
	int NumFruit;
	UPROPERTY(EditAnywhere)
		int TotalResourceCollected;
	UPROPERTY(EditAnywhere)
		int NumRum;

	// A list of all available action this agent can do
	TSet<GOAPAction*> AvailableActions;
	// The set of current actions the agent is planning to use
	TQueue<GOAPAction*> CurrentActions;
	GridNode* GoalLocation;

	// function for getting the current world state
	TMap<FString, bool> GetWorldState();
	// function for creating a goal state we want to achieve
	TMap<FString, bool> GetGoalState();

	// a plan was unable to be found. handle this behaviour (most likely idle for a bit)
	void OnPlanFailed(TMap<FString, bool> FailedGoalStates);
	// a plan was aborted midway through.. likely an issue of some kind prevented execution
	void OnPlanAborted(GOAPAction* FailedAction);
	// Randomly append action the ship must carry out
	void ShipAction();
	// Reroute the ship and deletes the path
	void Reroute();
};
