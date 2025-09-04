// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RAType.h"
#include "RATestActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class ROBOTARM_API ARATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARATestActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* Mesh;

	// 물품 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Type")
	EProductType Type;

	

public:
	UFUNCTION(BlueprintCallable)
	EProductType GetProductType() { return Type; }
};
