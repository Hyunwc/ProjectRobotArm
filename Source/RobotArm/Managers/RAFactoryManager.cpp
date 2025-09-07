// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/RAFactoryManager.h"
#include "Machines/RARobotArm.h"
#include "Pawn/RADeliveryCart.h"
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

	for (TActorIterator<ARADeliveryCart> It(GetWorld()); It; ++It)
	{
		ARADeliveryCart* Cart = *It;

		if (Cart)
		{
			Cart->OnCartStatusChanged.AddDynamic(this, &ARAFactoryManager::UpdateCarStatusData);
			Carts.Add(Cart);
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

void ARAFactoryManager::UpdateCarStatusData(ARADeliveryCart* Cart, ECartState NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("팩토리매니저 : 상태를 넘겨줄게요"));
	// 카트 상태를 텍스트로 변환하는 작업
	FText StateText;

	switch (NewState)
	{
	case ECartState::Wait:
		StateText = FText::FromString("Wait..");
		break;
	case ECartState::Loading:
		StateText = FText::FromString("Loading..");
		break;
	case ECartState::Move:
		StateText = FText::FromString("Move..");
		break;
	case ECartState::Return:
		StateText = FText::FromString("Return..");
		break;
	case ECartState::Back:
		StateText = FText::FromString("Comback..");
		break;
	default:
		StateText = FText::FromString("Unknown..");
		break;
	}

	CartStateMap.Data.Add(Cart->GetCartName(), StateText);

	OnUpdateCartStatus.Broadcast(CartStateMap);
}

