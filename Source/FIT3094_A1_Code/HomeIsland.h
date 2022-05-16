// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HomeIsland.generated.h"

UCLASS()
class FIT3094_A1_CODE_API AHomeIsland : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHomeIsland();
	int XPos;
	int YPos;

	UPROPERTY(EditAnywhere)
		int NumWood;
	UPROPERTY(EditAnywhere)
		int NumStone;
	UPROPERTY(EditAnywhere)
		int NumFruit;
	UPROPERTY(EditAnywhere)
		int NumRum = 5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
