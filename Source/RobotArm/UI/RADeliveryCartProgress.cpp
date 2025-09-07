// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RADeliveryCartProgress.h"
#include "Components/TextBlock.h"
#include "Managers/RAFactoryManager.h"
#include "Kismet/GameplayStatics.h"

URADeliveryCartProgress::URADeliveryCartProgress(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void URADeliveryCartProgress::NativeConstruct()
{
	Super::NativeConstruct();

	Txt_1stState->SetText(FText::FromString("Wait.."));
	Txt_2ndState->SetText(FText::FromString("Wait.."));
	Txt_3rdState->SetText(FText::FromString("Wait.."));
	
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARAFactoryManager::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		Manager = Cast<ARAFactoryManager>(Found[0]);
	}

	if (Manager)
	{
		Manager->OnUpdateCartStatus.AddDynamic(this, &URADeliveryCartProgress::UpdateCartStatus);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("팩토리 매니저를 찾을 수가 없어!!"));
	}
}

void URADeliveryCartProgress::UpdateCartStatus(FCartStateMap Data)
{
	UE_LOG(LogTemp, Warning, TEXT("카트UI : 나 호출됐다!"));
	if (Txt_1stState)
	{
		Txt_1stState->SetText(Data.Data.FindRef(TEXT("제1카트")));
	}

	if (Txt_2ndState)
	{
		Txt_2ndState->SetText(Data.Data.FindRef(TEXT("제2카트")));
	}

	if (Txt_3rdState)
	{
		Txt_3rdState->SetText(Data.Data.FindRef(TEXT("제3카트")));
	}
}
