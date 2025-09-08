// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RAType.h"
#include "AIController.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "RADeliveryCart.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UFloatingPawnMovement;
class AAIController;
class ARATestActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCartEventSignature, ARADeliveryCart* , Cart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReturnCartProduct, ARATestActor*, Actor, EProductType, Type);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCartStatusChanged, ARADeliveryCart*, Cart, ECartState, NewState);

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
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CartMesh;

	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "State")
	ECartState CartState;

	// 물품을 담을 배열
	UPROPERTY(VisibleAnywhere, Category = "Product")
	TArray<ARATestActor*> Products;

	// 현재 용량
	UPROPERTY(VisibleAnywhere, Category = "Product")
	int32 ProductCapacity;

	// 최대 용량
	UPROPERTY(VisibleAnywhere, Category = "Product")
	int32 MaxCapacity;

	// 반납할 때 사용할 타이머(즉시 반납보다는 시간이 지나면서 반납하게 하기 위함
	FTimerHandle ReturnTimerHandle; 

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Cart")
	FString CartName;

	// 로봇암이 이 위치를 알 수 있게 캐시
	UPROPERTY(VisibleAnywhere, Category = "Target")
	FVector CacheLoadingLocation;

	// 자신의 복귀 위치 저장
	UPROPERTY(VisibleAnywhere, Category = "Cart")
	FVector WaitLocation;

private:
	UPROPERTY(VisibleAnywhere, Category = "Cart")
	FVector CachedCombackLocation;

public:
	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartFull; // Capacity가 다 찼을 때

	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartArrived; // 목적지에 도착했을 때

	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartReturned; // 모든 제품 반환 끝났을 때

	UPROPERTY(BlueprintAssignable)
	FCartEventSignature OnCartCombacked; // 복귀 완료했을 때

	UPROPERTY(BlueprintAssignable)
	FOnReturnCartProduct OnReturnCartProduct; // 반납할 때 풀매니저 보고 알아서 가져가게.

	UPROPERTY(BlueprintAssignable)
	FOnCartStatusChanged OnCartStatusChanged; // 카트의 상태가 변경됐음을 구독자에게 알림

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

	// 복귀
	UFUNCTION()
	void BackToLocation();

	// 도착했을 때 호출
	UFUNCTION()
	void OnArrived();

	void HandleMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	void HandleCombackCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	// 카트 비우기
	UFUNCTION()
	void ReturnProducts();

	UFUNCTION()
	FString GetCartName() { return CartName; }

	// 카트가 꽉찼는지 검사하는
	UFUNCTION()
	bool CartIsFull() const { return (ProductCapacity == MaxCapacity); }

	AAIController* GetAICon() const;
	void HandleReturnTick(); // 타이머로 반환

	UFUNCTION()
	void SetLoadingLocation(const FVector& Location);

	UFUNCTION()
	FVector GetLoadingLocation() { return CacheLoadingLocation; }
};
