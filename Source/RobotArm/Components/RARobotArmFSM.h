// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RARobotArmFSM.generated.h"

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


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTARM_API URARobotArmFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	URARobotArmFSM();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
	ERobotArmState CurrentState;

public:
	UFUNCTION(BlueprintCallable)
	void ChangeState(ERobotArmState NewState);
};
