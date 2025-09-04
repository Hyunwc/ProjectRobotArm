// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/RAPoolManager.h"
#include "Machines/RAConveyor.h"
#include "EngineUtils.h"
//#include "RATestActor.h"

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
            UE_LOG(LogTemp, Warning, TEXT("PoolManager: 해당 타입은 ProductClasses에 존재하지 않아요"));
            continue;
        }

        FPoolMapWrapper PoolMapWrapper;

        for (int32 i = 0; i < Count; i++)
        {
            ARATestActor* NewActor = GetWorld()->SpawnActor<ARATestActor>(ProductClasses[Type], FVector::ZeroVector, FRotator::ZeroRotator);
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
    
    //int32 RandomIndex = 1 + FMath::RandHelper(EnumPtr->NumEnums() - 2);
    int32 RandomIndex = FMath::RandRange(1, static_cast<int32>(EnumPtr->GetMaxEnumValue() - 1));

    UE_LOG(LogTemp, Warning, TEXT("PoolManager: 랜덤값 %d"), RandomIndex);

    EProductType RandomType = static_cast<EProductType>(RandomIndex);

    // PoolMap에서 찾을 Key(ProductType)를 추출해야함
    if (MainConveyor)
    {
        MainConveyor->AddProduct(GetPooling(RandomType));
    }
}

ARATestActor* ARAPoolManager::GetPooling(EProductType Type)
{
    if (FPoolMapWrapper* Wrapper = PoolMap.Find(Type))
    {
        if (Wrapper->PoolMapArray.Num() > 0)
        {
            ARATestActor* PoolActor = Wrapper->PoolMapArray[0];
            PoolActor->SetActorHiddenInGame(false);
            Wrapper->PoolMapArray.RemoveAt(0);
            return PoolActor;
        }
    }

    return nullptr;
}

void ARAPoolManager::ReturnPooling(ARATestActor* Actor, EProductType Type)
{
    if (!Actor)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("PoolManager: 지금부터 반납을 시작하겠습니다"));

    if (FPoolMapWrapper* Wrapper = PoolMap.Find(Type))
    {
        Actor->SetActorHiddenInGame(true);
        Wrapper->PoolMapArray.Add(Actor);
    }
    UE_LOG(LogTemp, Warning, TEXT("PoolManager: 반납을 실패해습니다만"));
}

