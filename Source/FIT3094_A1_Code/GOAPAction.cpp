// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAction.h"

GOAPAction::GOAPAction()
{
}

GOAPAction::~GOAPAction()
{
}

void GOAPAction::DoReset()
{
	// Reset Required variables
	InRange = false;
	Target = nullptr;

	// Call this child Version of reset function
	// This is due to polymorphism
	Reset();
}

void GOAPAction::AddPrecondition(FString Name, bool State)
{
	Preconditions.Add(Name, State);
}

void GOAPAction::RemovePrecondition(FString Name, bool State)
{
	Preconditions.Remove(Name);
}

void GOAPAction::AddEffect(FString Name, bool State)
{
	Effects.Add(Name, State);
}

void GOAPAction::RemoveEffect(FString Name, bool State)
{
	Effects.Remove(Name);
}










