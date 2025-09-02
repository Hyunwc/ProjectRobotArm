// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Machines/RAMachineBase.h"
#include "RAType.h"
#include "RARobotArm.generated.h"

class UControlRigComponent;
class USkeletalMeshComponent;
class UBoxComponent;
class URARobotArmFSM;
class ARAConveyor;
class ARATestActor;
class ARASensor;

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

	// 기준점이될 컨트롤릭의 이름
	UPROPERTY(EditAnywhere, Category = "ControlRig")
	FName EndEffectorName;

	// 물건 감지용 박스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* BoxComp;

	// 목표 지점
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RobotArm")
	FTransform TargetTransform;

	// 물건을 집는 속도
	UPROPERTY(EditAnywhere, Category = "RobotArm")
	float GrabSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	FTransform StartTransform;

	// 집을 물건의 트랜스폼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	FTransform GrabTransform;

	// 복귀 지점
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	FTransform ReturnTransform;

	// 로봇암의 FSM
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	URARobotArmFSM* FSM;

	// 집을 액터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RobotArm")
	ARATestActor* GrabActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	ARAConveyor* Conveyor;

	// 자신이 집어야할 물품의 타입
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Type")
	EProductType Type;

	// 목표 타입
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Type")
	EProductType TargetType;

	// 센서 참조
	UPROPERTY(VisibleAnywhere)
	ARASensor* Sensor;

	// 집은 물건을 보낼 컨베이어
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conveyor")
	ARAConveyor* TargetConveyor;

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "RobotArm")
	void StartSearch(EProductType SearchType);

	// FSM 상태 관련 함수들
	void IdleState();
	void SearchState();
	void AttachState();
	void CarryState();
	void DettachState();
	void ReturnState();

	UFUNCTION(BlueprintCallable)
	void MoveToTransform(const FTransform& Destination, float DeltaTime);

private:
	float Alpha;

	float Delta;

};
