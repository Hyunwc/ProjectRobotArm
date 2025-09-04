// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAType.h"
#include "RATestActor.h"
#include "RAPoolManager.generated.h"

class ARAConveyor;
//class ARATestActor;

USTRUCT(BlueprintType)
struct FPoolMapWrapper
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<ARATestActor*> PoolMapArray;
};

UCLASS()
class ROBOTARM_API ARAPoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARAPoolManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	// 액터를 저장할 풀
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	TMap<EProductType, FPoolMapWrapper> PoolMap;

	// 풀 사이즈 정의
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	TMap<EProductType, int32> PoolSize;

	// 상품 블루프린트를 담을 Map
	UPROPERTY(EditAnywhere, Category = "Pool")
	TMap<EProductType, TSubclassOf<ARATestActor>> ProductClasses;

	UPROPERTY(EditAnywhere, Category = "Conveyor")
	TArray<ARAConveyor*> Conveyors;

	UPROPERTY(EditInstanceOnly, Category = "Conveyor")
	ARAConveyor* MainConveyor;

	FTimerHandle SpawnTimer;

private:
	int32 Size;

public:
	UFUNCTION()
	void InitPooling();

	UFUNCTION()
	void SpawnPool();

	UFUNCTION()
	ARATestActor* GetPooling(EProductType Type);

	UFUNCTION()
	void ReturnPooling(ARATestActor* Actor, EProductType Type);
	
};
