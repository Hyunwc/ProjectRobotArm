// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RAAIController.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTARM_API ARAAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ARAAIController();

	virtual void OnPossess(APawn* InPawn) override;

	// 특정 위치로 카트 이동
	void CommandMoveTo(const FVector& TargetLocation);
};
