// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAProgress.h"
#include "Components/TextBlock.h"
#include "Managers/RAFactoryManager.h"
#include "Kismet/GameplayStatics.h"

URAProgress::URAProgress(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void URAProgress::NativeConstruct()
{
	Super::NativeConstruct();

	Txt_Clothes->SetText(FText::AsNumber(0));
	Txt_Food->SetText(FText::AsNumber(0));
	Txt_Electronics->SetText(FText::AsNumber(0));
	Txt_Daily->SetText(FText::AsNumber(0));

	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARAFactoryManager::StaticClass(), Found);
	if (Found.Num() > 0)
	{
		FactoryManager = Cast<ARAFactoryManager>(Found[0]);
	}

	if (FactoryManager)
	{
		FactoryManager->OnUpdateClassification.AddDynamic(this, &URAProgress::UpdateCount);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("팩토리 매니저를 찾을 수가 없어!!"));
	}
}

void URAProgress::UpdateCount(FClassificationMap Data)
{
	if (const int32 CountPtr = Data.Data.FindRef(EProductType::Clothes))
	{
		Txt_Clothes->SetText(FText::AsNumber(CountPtr));
	}

	if (const int32 CountPtr = Data.Data.FindRef(EProductType::Food))
	{
		Txt_Food->SetText(FText::AsNumber(CountPtr));
	}

	if (const int32 CountPtr = Data.Data.FindRef(EProductType::Electronics))
	{
		Txt_Electronics->SetText(FText::AsNumber(CountPtr));
	}

	if (const int32 CountPtr = Data.Data.FindRef(EProductType::Daily))
	{
		Txt_Daily->SetText(FText::AsNumber(CountPtr));
	}
}
