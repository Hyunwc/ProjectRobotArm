// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/RAAIController.h"
#include "NavigationSystem.h"

ARAAIController::ARAAIController()
{
	bAttachToPawn = true;
}

void ARAAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ARAAIController::CommandMoveTo(const FVector& TargetLocation)
{
	MoveToLocation(TargetLocation);
}
