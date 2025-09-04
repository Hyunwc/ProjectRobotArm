// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RARobotArmStateWidget.h"
#include "Components/TextBlock.h"

URARobotArmStateWidget::URARobotArmStateWidget(const FObjectInitializer& ObjectInitializer)
{
}

void URARobotArmStateWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URARobotArmStateWidget::UpdateRobotArmState(ERobotArmState NewType)
{
	if (Txt_RobotArmState)
	{
		FText Text;
		switch (NewType)
		{
		case ERobotArmState::Idle:
			Text = FText::FromString(TEXT("대기 중.."));
			break;
		case ERobotArmState::Search:
			Text = FText::FromString(TEXT("탐색 중.."));
			break;
		case ERobotArmState::Attach:
			Text = FText::FromString(TEXT("집는 중.."));
			break;
		case ERobotArmState::Carry:
			Text = FText::FromString(TEXT("이동 중.."));
			break;
		case ERobotArmState::Dettach:
			Text = FText::FromString(TEXT("놓는 중.."));
			break;
		case ERobotArmState::Return:
			Text = FText::FromString(TEXT("복귀 중.."));
			break;
		default:
			break;
		}
		Txt_RobotArmState->SetText(Text);
	}
}
