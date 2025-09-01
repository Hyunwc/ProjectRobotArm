// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RAType.generated.h"


UENUM(BlueprintType)
enum class EProductType : uint8
{
	Food,
	Clothes
};
/**
 * 
 */
UCLASS()
class ROBOTARM_API URAType : public UObject
{
	GENERATED_BODY()
	
};
