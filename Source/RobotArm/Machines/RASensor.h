// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Machines/RAMachineBase.h"
#include "RAType.h"
#include "RATestActor.h"
#include "RASensor.generated.h"

class UBoxComponent;
//class ARATestActor;

// 타입을 감지하여 해당 타입을 가진 로봇암에게 탐색 모드로 들어가라고 알리는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChangeSearch, EProductType, Type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProductDetected, EProductType, Type, ARATestActor*, Actor);
/**
 * 
 */
UCLASS()
class ROBOTARM_API ARASensor : public ARAMachineBase
{
	GENERATED_BODY()

public:
	ARASensor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	// 오버랩을 위한 박스 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* Sensor;

	UPROPERTY(BlueprintAssignable)
	FOnStateChangeSearch OnStateChangeSearch;

	UPROPERTY(BlueprintAssignable)
	FOnProductDetected OnProductDetected;

protected:
	UFUNCTION()
	void OnSensorOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSensorZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
