// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TargetPoint.h"
#include "RADeliveryManager.generated.h"

class ARADeliveryCart;

UCLASS()
class ROBOTARM_API ARADeliveryManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARADeliveryManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	// 카트를 관리할 큐
	UPROPERTY(EditAnywhere, Category = "Cart")
	TArray<ARADeliveryCart*> CartQueue;

	// 카트의 최대 수량
	UPROPERTY(EditAnywhere, Category = "Cart")
	int32 MaxCarts;

	// 카트가 물품을 반납하러갈 위치
	UPROPERTY(VisibleAnywhere, Category = "Cart")
	FVector ReturnLocation;

	// 카트가 물품을 받을곳으로 갈 위치
	UPROPERTY(VisibleAnywhere, Category = "Cart")
	FVector LoadingLocation;

	// TODO : 당장은 필요없을 거 같습니다. 나중에 필요시 수정해서 사용.
	//// 카트가 적재상태로 가기 전 위치 백업. 반납한 카트가 이 위치로 와야함.
	//UPROPERTY(VisibleAnywhere, Category = "Cart")
	//FVector PrevCartLocation;

	// FVector가 아닌 TargetPoint참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart")
	ATargetPoint* LoadingTargetPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart")
	ATargetPoint* ReturnTargetPoint;

private:
	FTimerHandle FirstCartTimer;

public:
	// 큐에서 제일 앞의 카트 반환
	UFUNCTION()
	ARADeliveryCart* GetNextCart();

	UFUNCTION()
	void InitializeFirstCart();

	// 작업 완료한 카트를 큐 맨뒤로 이동
	UFUNCTION()
	void RecycleCart(ARADeliveryCart* Cart);

	UFUNCTION()
	void HandleCartCapacityFull(ARADeliveryCart* Cart);

	UFUNCTION()
	void HandleCartArrived(ARADeliveryCart* Cart);

	UFUNCTION()
	void HandleCartReturned(ARADeliveryCart* Cart);

	UFUNCTION()
	void HandleCartCombackCompleted(ARADeliveryCart* Cart);

	UFUNCTION()
	void StartNextLoadCycle();

};
