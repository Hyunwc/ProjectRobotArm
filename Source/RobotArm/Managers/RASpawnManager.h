// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RASpawnManager.generated.h"

class ARATestActor;
class ARAConveyor;

UCLASS()
class ROBOTARM_API ARASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ARASpawnManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ARATestActor>> ProductClass;

	UPROPERTY(EditInstanceOnly)
	ARAConveyor* TargetConveyor;

	FTimerHandle SpawnTimer;

public:
	UFUNCTION(BlueprintCallable)
	void SpawnProduct();
};
