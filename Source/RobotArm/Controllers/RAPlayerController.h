// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RAPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCameraViewChanged, FText, NewDisplay);

class ARACameraActor;

/**
 * 
 */
UCLASS()
class ROBOTARM_API ARAPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARAPlayerController();
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category = "Camera")
	TSubclassOf<ARACameraActor> CameraActorClass;

	UPROPERTY(VisibleAnywhere)
	TMap<FName, ARACameraActor*> CameraMap;

	UPROPERTY(EditInstanceOnly, Category = "Camera")
	ARACameraActor* CurrentCamera;

	UPROPERTY(BlueprintAssignable)
	FOnCameraViewChanged OnCameraViewChanged;

public:
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SwitchCamera(FName CameraTag);
};
