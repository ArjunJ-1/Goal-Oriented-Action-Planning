// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AShip;
class FIT3094_A1_CODE_API GOAPAction
{
protected:
	bool InRange;

	// Do Reset in the below section handles calling this function
	// Sub-classes can handle their own additional reset requirements here
	virtual void Reset() = 0;
	
public:
	// List of conditions required for this action to be valid
	TMap<FString, bool> Preconditions;
	// The changes this action had on the world
	TMap<FString, bool> Effects;

	// The cost of action
	// All action have a cost associated with them
	// This can also be changed at runtime
	float ActionCost;

	// often actions will work on a target
	// We store a generic Actor as the target here
	// subclasses can cast this as needed
	AActor* Target;

	// Constructor and Destructor
	GOAPAction();
	~GOAPAction();

	// Reset the action to its base state
	void DoReset();

	// Adding and removing preconditions
	void AddPrecondition(FString Name, bool State);
	void RemovePrecondition(FString Name, bool State);

	// Adding and removing effects
	void AddEffect(FString Name, bool State);
	void RemoveEffect(FString Name, bool State);

	//Set and check weather or not an action is within range
	bool IsInRange() { return InRange; }
	void SetInRange(bool Range) { InRange = Range; }

	// Pure virtaul functions that child classes must implement
	// Weather or not an action has finished executing
	virtual bool IsActionDone(AShip* Ship) = 0;
	// Check procedural precondition at run time. Provided a shit agent
	virtual bool CheckProceduralPrecondition(AShip* Ship) = 0;
	// Performs the action
	// Returns true unless the action is no longer able to be completed
	// This is called each frame until the action is done
	virtual bool PerformAction(AShip* Ship, float DeltaTime) = 0;
	// Weather or not the action requires something in range
	// Some Actions can be performed anywhere. whereas others require specific locations
	// e.g. To collect a resource the ship must be next to it
	virtual bool RequiresInRange() = 0;
};
