// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAMachineBase.generated.h"

class USceneComponent;
class UStaticMeshComponent;

// TODO : 공장 가동 시작을 알릴 델리게이트를 추가합니다.

UCLASS()
class ROBOTARM_API ARAMachineBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ARAMachineBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;
};
