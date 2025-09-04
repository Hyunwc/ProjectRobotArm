// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Machines/RAMachineBase.h"
#include "RAType.h"
#include "RAConveyor.generated.h"

class USplineComponent;
class ARATestActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReturnProduct, ARATestActor*, Actor, EProductType, Type);

USTRUCT(BlueprintType)
struct FConveyorProduct
{
	GENERATED_BODY()

	ARATestActor* TestActor;
	float Distance;

	FConveyorProduct() : TestActor(nullptr), Distance(0.f) {}
};
/**
 * 
 */
UCLASS()
class ROBOTARM_API ARAConveyor : public ARAMachineBase
{
	GENERATED_BODY()
	
public:
	ARAConveyor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	// 물품들 이동 경로
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	USplineComponent* Spline;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float MoveSpeed;

	// 스플라인 시작 지점
	UPROPERTY(VisibleAnywhere, Category = "Conveyor")
	FVector SplineStartLocation;

	UPROPERTY(VisibleAnywhere)
	TArray<FConveyorProduct> Products;

	// 나중에 컨베이어가 물건을 옮길 때 필요한 좌표
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Conveyor")
	FTransform DettachTransform;

	// 풀매니저가 등록하여 물품을 풀에 반환하기 위함
	UPROPERTY(BlueprintAssignable)
	FOnReturnProduct OnReturnProduct;
		
public:
	UFUNCTION(BlueprintCallable)
	void ProductSpawn(const TArray<TSubclassOf<ARATestActor>>& ProductClass);

	UFUNCTION(BlueprintCallable)
	void RemoveProduct(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	void AddProduct(AActor* Actor);
};
