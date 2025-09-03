// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/RAFactoryManager.h"
#include "Machines/RARobotArm.h"
#include "EngineUtils.h"

ARAFactoryManager::ARAFactoryManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// 물품 분류 수량 초기화
	ClassificationMap.Data.Add(EProductType::Clothes, 0);
	ClassificationMap.Data.Add(EProductType::Food, 0);
	ClassificationMap.Data.Add(EProductType::Electronics, 0);
	ClassificationMap.Data.Add(EProductType::Daily, 0);
}

void ARAFactoryManager::BeginPlay()
{
	Super::BeginPlay();
	
	// 월드에 있는 로봇암을 전부 읽어와서 델리게이트 구독 후 삽입
	for (TActorIterator<ARARobotArm> It(GetWorld()); It; ++It)
	{
		ARARobotArm* Arm = *It;

		if (Arm)
		{
			Arm->OnClassficationFinished.AddDynamic(this, &ARAFactoryManager::RecordClassificationData);
			RobotArms.Add(Arm);
		}
	}
}

void ARAFactoryManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARAFactoryManager::RecordClassificationData(EProductType Type)
{
	if (ClassificationMap.Data.Find(Type))
	{
		ClassificationMap.Data[Type]++;
	}

	OnUpdateClassification.Broadcast(ClassificationMap);
}

