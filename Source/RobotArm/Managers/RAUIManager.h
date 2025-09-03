// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAType.h"
#include "RAUIManager.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTARM_API ARAUIManager : public AActor
{
	GENERATED_BODY()
	
public:
	ARAUIManager();

protected:
	virtual void BeginPlay() override;

};
