// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RACameraManager.generated.h"


UCLASS()
class ROBOTARM_API ARACameraManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARACameraManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;


};
