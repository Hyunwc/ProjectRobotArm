// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/RAUserWidgetBase.h"
#include "RAType.h"

#include "RAProgress.generated.h"

class UTextBlock;
class ARAFactoryManager;
//struct FClassificationMap;

/**
 * 
 */
UCLASS()
class ROBOTARM_API URAProgress : public URAUserWidgetBase
{
	GENERATED_BODY()

public:
	URAProgress(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere)
	ARAFactoryManager* FactoryManager;
	
public:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//TMap<EProductType, int32> ProgressCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Clothes;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Food;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Electronics;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Daily;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateCount(FClassificationMap Data);
};
