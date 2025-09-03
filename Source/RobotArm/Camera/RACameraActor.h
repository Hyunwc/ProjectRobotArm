// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "RACameraActor.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTARM_API ARACameraActor : public ACameraActor
{
	GENERATED_BODY()

public:
	ARACameraActor();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FName CameraTag;
};
