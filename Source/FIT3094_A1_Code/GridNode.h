// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

/**
 * 
 */
class AShip;
class FIT3094_A1_CODE_API GridNode
{

public:

	// Types of grid nodes
	enum GRID_TYPE
	{
		DeepWater,
		Land,
		ShallowWater,
		Home,
		WoodResource,
		StoneResource,
		FruitResource,
		MerchantResource,
		GoldResource
	};


	GridNode();

	float GetTravelCost() const;

	// Position in Grid
	int X;
	int Y;

	// Informed Search Variables
	int G;
	float H;
	float F;

	// Number of resources associated with node
	int ResourceNum;

	// Type of grid space
	GRID_TYPE GridType;
	
	// Pointer to previous Node (Only used in searching)
	GridNode* Parent;

	// Object at current location
	AActor* ObjectAtLocation;
	AActor* ResourceAtLocation;

	// Whether or not the grid square has been checked by the search
	bool IsChecked;
	// Store the path currently reserved by a ship
	TMap<AShip*, bool> ReservedPath;
};
