// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridNode.h"
#include "GameFramework/Actor.h"
#include "ResourceActor.generated.h"

UCLASS()
class FIT3094_A1_CODE_API AResourceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResourceActor();

	UPROPERTY(EditAnywhere)
	int ResourceCount;
	
	GridNode::GRID_TYPE ResourceType;
	int XPos;
	int YPos;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
