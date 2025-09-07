// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/RAUserWidgetBase.h"
#include "RAType.h"
#include "RADeliveryCartProgress.generated.h"

class UTextBlock;
class ARAFactoryManager;
/**
 * 
 */
UCLASS()
class ROBOTARM_API URADeliveryCartProgress : public URAUserWidgetBase
{
	GENERATED_BODY()

public:
	URADeliveryCartProgress(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	//UPROPERTY(meta = (BindWidget))
	//TArray<UTextBlock*> CartStatusTexts;

	UPROPERTY(VisibleAnywhere)
	ARAFactoryManager* Manager;
	
	// TODO : 시간날때 하드코딩된 부분을 유연하게 고쳐봅니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_1stState;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_2ndState;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_3rdState;

public:
	UFUNCTION()
	void UpdateCartStatus(FCartStateMap Data);
	
};
