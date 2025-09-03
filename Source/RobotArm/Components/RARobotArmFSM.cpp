// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RARobotArmFSM.h"

URARobotArmFSM::URARobotArmFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentState = ERobotArmState::Idle; 
}


void URARobotArmFSM::BeginPlay()
{
	Super::BeginPlay();

}


void URARobotArmFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URARobotArmFSM::ChangeState(ERobotArmState NewState)
{
	CurrentState = NewState;

	// 변경된 상태 출력
	FString StateMsg = UEnum::GetValueAsString(CurrentState);
	//GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Cyan, StateMsg);
}

