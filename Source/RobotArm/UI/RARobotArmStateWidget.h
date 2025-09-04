// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/RAUserWidgetBase.h"
#include "RAType.h"
#include "RARobotArmStateWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class ROBOTARM_API URARobotArmStateWidget : public URAUserWidgetBase
{
	GENERATED_BODY()
	

public:
	URARobotArmStateWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_RobotArmState;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateRobotArmState(ERobotArmState NewType);
};
