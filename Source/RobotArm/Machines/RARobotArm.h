// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Machines/RAMachineBase.h"
#include "RARobotArm.generated.h"

class UControlRigComponent;
class USkeletalMeshComponent;
class UBoxComponent;
/**
 * 
 */
UCLASS()
class ROBOTARM_API ARARobotArm : public ARAMachineBase
{
	GENERATED_BODY()
	
public:
	ARARobotArm();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UControlRigComponent* ControlRigComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* Skeletal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* EndEffectorScene;

	// 컨트롤릭의 이름
	UPROPERTY(EditAnywhere, Category = "ControlRig")
	FName EndEffectorName;

	// 물건 감지용 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotArm")
	FTransform TargetTransform;

	// 물건을 집는 속도
	UPROPERTY(EditAnywhere, Category = "RobotArm")
	float GrabSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	FTransform StartTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	FTransform GrabTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	FTransform ReturnTransform;

private:
	// 현재 로봇암 상태
	UPROPERTY()
	AActor* CurrentTargetMesh;

	float Alpha;

	UFUNCTION(CallInEditor, Category = "RobotArm")
	void StartRobotAction();

	// 로봇 동작 상태
	enum class EActionState
	{
		Idle,
		MovingToTarget,
		Grabbing,
		MovingToDestination,
		Dropping,
		Returning
	};

	EActionState CurrentState;

	void UpdateRobotState(float DeltaTime);
	void MoveToTransform(const FTransform& Destination, float DeltaTime);
};
