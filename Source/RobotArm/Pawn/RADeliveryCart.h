// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RAType.h"
#include "RADeliveryCart.generated.h"

class UStaticMeshComponent;
class UFloatingPawnMovement;
class AAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCartEventSignature, ARADeliveryCart* , Cart);


UCLASS()
class ROBOTARM_API ARADeliveryCart : public APawn
{
	GENERATED_BODY()

public:
	ARADeliveryCart();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CartMesh;

	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "State")
	ECartState CartState;

	// 물품을 담을 배열
	UPROPERTY(VisibleAnywhere, Category = "Product")
	TArray<AActor*> Products;

	// 현재 용량
	UPROPERTY(VisibleAnywhere, Category = "Product")
	int32 ProductCapacity;

	// 최대 용량
	UPROPERTY(VisibleAnywhere, Category = "Product")
	int32 MaxCapacity;

	// 반납할 때 사용할 타이머(즉시 반납보다는 시간이 지나면서 반납하게 하기 위함
	FTimerHandle ReturnTimerHandle; 

public:
	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartFull; // Capacity가 다 찼을 때

	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartArrived; // 목적지에 도착했을 때

	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartReturned; // 모든 제품 반환 끝났을 때

	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartBacked; // 복귀 완료했을 때

public:
	// 상태 관리
	UFUNCTION()
	void SetState(ECartState NewState);
	
	UFUNCTION()
	ECartState GetState() const { return CartState; }

	// 카드에 물품 담는 함수
	UFUNCTION()
	void AddProduct(AActor* Product);

	// 목표 위치로 이동 지시
	UFUNCTION()
	void MoveToLocation(const FVector& NewLocation);

	// 도착했을 때 호출
	UFUNCTION()
	void OnArrived();

	// 카트 비우기
	UFUNCTION()
	void ReturnProducts();

	// 복귀
	UFUNCTION()
	void BackToLocation(const FVector& NewLocation);

	// 카트가 꽉찼는지 검사하는
	UFUNCTION()
	bool CartIsFull() const { return (ProductCapacity == MaxCapacity); }

	AAIController* GetAICon() const;
	void HandleReturnTick(); // 타이머로 반환
};
