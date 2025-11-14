// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/RAPoolManager.h"
#include "Machines/RAConveyor.h"
#include "Pawn/RADeliveryCart.h"
#include "EngineUtils.h"

ARAPoolManager::ARAPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;

    Size = 10;

    PoolSize.Add(EProductType::Clothes, Size);
    PoolSize.Add(EProductType::Food, Size);
    PoolSize.Add(EProductType::Electronics, Size);
    PoolSize.Add(EProductType::Daily, Size);
    PoolSize.Add(EProductType::Other, Size);
}

void ARAPoolManager::BeginPlay()
{
	Super::BeginPlay();
	
    InitPooling();

    for (TActorIterator<ARAConveyor> It(GetWorld()); It; ++It)
    {
        ARAConveyor* Conveyor = *It;

        if (Conveyor)
        {
            Conveyor->OnReturnProduct.AddDynamic(this, &ARAPoolManager::ReturnPooling);
            Conveyors.Add(Conveyor);
        }
    }

    for (TActorIterator<ARADeliveryCart> It(GetWorld()); It; ++It)
    {
        ARADeliveryCart* Cart = *It;

        if (Cart)
        {
            Cart->OnReturnCartProduct.AddDynamic(this, &ARAPoolManager::ReturnPooling);
            Carts.Add(Cart);
        }
    }

    GetWorldTimerManager().SetTimer(SpawnTimer, this, &ARAPoolManager::SpawnPool, 3.0f, true);
}

void ARAPoolManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARAPoolManager::InitPooling()
{
    for (const auto& Elem : PoolSize)
    {
        EProductType Type = Elem.Key;
        int32 Count = Elem.Value;

        // 해당 키(Type)가 없다면 다음으로
        if (!ProductClasses.Contains(Type))
        {
            continue;
        }

        FPoolMapWrapper PoolMapWrapper;

        for (int32 i = 0; i < Count; i++)
        {
            ARAProduct* NewActor = GetWorld()->SpawnActor<ARAProduct>(ProductClasses[Type], 
                FVector::ZeroVector, FRotator::ZeroRotator);
            NewActor->SetActorHiddenInGame(true);
            PoolMapWrapper.PoolMapArray.Add(NewActor);
        }

        PoolMap.Add(Type, PoolMapWrapper);
    }
}

void ARAPoolManager::SpawnPool()
{
    if (!MainConveyor)
    {
        return;
    }

    UEnum* EnumPtr = StaticEnum<EProductType>();
    
    int32 RandomIndex = FMath::RandRange(1, static_cast<int32>(EnumPtr->GetMaxEnumValue() - 1));

    EProductType RandomType = static_cast<EProductType>(RandomIndex);

    // PoolMap에서 찾을 Key(ProductType)를 추출해야함
    if (MainConveyor)
    {
        MainConveyor->AddProduct(GetPooling(RandomType));
    }
}

ARAProduct* ARAPoolManager::GetPooling(EProductType Type)
{
    if (FPoolMapWrapper* Wrapper = PoolMap.Find(Type))
    {
        if (!Wrapper->PoolMapArray.IsEmpty())
        {
            ARAProduct* PoolActor = Wrapper->PoolMapArray.Last();
            PoolActor->SetActorHiddenInGame(false);
            Wrapper->PoolMapArray.Pop();
            return PoolActor;
        }
    }

    return nullptr;
}

void ARAPoolManager::ReturnPooling(ARAProduct* Actor, EProductType Type)
{
    if (!Actor)
    {
        return;
    }

    if (FPoolMapWrapper* Wrapper = PoolMap.Find(Type))
    {
        Actor->SetActorHiddenInGame(true);
        Wrapper->PoolMapArray.Add(Actor);
    }
}

