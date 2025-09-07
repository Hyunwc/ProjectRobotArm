// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RAType.generated.h"

UENUM(BlueprintType)
enum class ERobotArmState : uint8
{
	Idle,
	Search,
	Attach,
	Carry,
	Dettach,
	Return
};

UENUM(BlueprintType)
enum class EProductType : uint8
{
	Default,
	Food,
	Clothes,
	Electronics,
	Daily,
	Other
};

UENUM(BlueprintType)
enum class ECartState : uint8
{
	Wait,
	Loading,
	Move,
	Return,
	Back
};

USTRUCT(BlueprintType)
struct FClassificationMap
{
	GENERATED_BODY()

	TMap<EProductType, int32> Data;
};

USTRUCT(BlueprintType)
struct FCartStateMap
{
	GENERATED_BODY()

	TMap<FString, FText> Data;
};
/**
 * 
 */
UCLASS()
class ROBOTARM_API URAType : public UObject
{
	GENERATED_BODY()
	
};
