// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/RADeliveryManager.h"
#include "Pawn/RADeliveryCart.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ARADeliveryManager::ARADeliveryManager()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxCarts = 3;
}

void ARADeliveryManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundCarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARADeliveryCart::StaticClass(), FoundCarts);

	for (AActor* Actor : FoundCarts)
	{
		if (ARADeliveryCart* Cart = Cast<ARADeliveryCart>(Actor))
		{
			CartQueue.Add(Cart);

			Cart->OnCartFull.AddDynamic(this, &ARADeliveryManager::HandleCartCapacityFull);
			Cart->OnCartArrived.AddDynamic(this, &ARADeliveryManager::HandleCartArrived);
			Cart->OnCartReturned.AddDynamic(this, &ARADeliveryManager::HandleCartReturned);
			Cart->OnCartCombacked.AddDynamic(this, &ARADeliveryManager::HandleCartCombackCompleted);

			Cart->SetState(ECartState::Wait);
		}
	}

	if (LoadingTargetPoint)
	{
		LoadingLocation = LoadingTargetPoint->GetActorLocation();
	}

	if (ReturnTargetPoint)
	{
		ReturnLocation = ReturnTargetPoint->GetActorLocation();
	}


	GetWorldTimerManager().SetTimer(FirstCartTimer, this, &ARADeliveryManager::InitializeFirstCart, 1.f, false);
}

void ARADeliveryManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ARADeliveryCart* ARADeliveryManager::GetNextCart()
{
	if (CartQueue.Num() > 0)
	{
		return CartQueue[0];
	}

	return nullptr;
}

void ARADeliveryManager::InitializeFirstCart()
{
	if (CartQueue.Num() > 0)
	{
		ARADeliveryCart* FrontCart = CartQueue[0];

		if (FrontCart->GetState() == ECartState::Wait)
		{
			// 카트의 적재장소를 캐시
			FrontCart->SetLoadingLocation(LoadingLocation);
			FrontCart->MoveToLocation(LoadingLocation);
			FrontCart->SetState(ECartState::Loading);
		}
	}

	GetWorldTimerManager().ClearTimer(FirstCartTimer);
}

void ARADeliveryManager::RecycleCart(ARADeliveryCart* Cart)
{
	if (CartQueue.Remove(Cart) > 0)
	{
		CartQueue.Add(Cart);
		Cart->BackToLocation();
	}
}

void ARADeliveryManager::HandleCartCapacityFull(ARADeliveryCart* Cart)
{
	UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 카드 %s가 가득 찼음 반환 시작"), *Cart->GetName());

	int32 IndexOfFullCart = CartQueue.IndexOfByKey(Cart);
	if (IndexOfFullCart != INDEX_NONE)
	{
		CartQueue.RemoveAt(IndexOfFullCart);
		CartQueue.Add(Cart);
	}

	// 현재 카트 반환 장소로
	Cart->MoveToLocation(ReturnLocation);
	Cart->SetState(ECartState::Move);

	if (CartQueue.Num() > 0)
	{
		ARADeliveryCart* NextCart = CartQueue[0];
		if (NextCart->GetState() == ECartState::Wait)
		{
			UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 카드 %s 적재 장소로 이동 "), *NextCart->GetName());
			// 복귀 장소 캐시
			//PrevCartLocation = NextCart->GetActorLocation();
			// 카트의 적재장소를 캐시
			NextCart->SetLoadingLocation(LoadingLocation);
			NextCart->MoveToLocation(LoadingLocation);
			NextCart->SetState(ECartState::Loading);
		}
	}
}

void ARADeliveryManager::HandleCartArrived(ARADeliveryCart* Cart)
{
	if (Cart->GetState() == ECartState::Move)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 카드 %s 반납 시작"), *Cart->GetName());
		// 타이머 기반으로 반납 시작
		Cart->ReturnProducts();
		//Cart->SetState(ECartState::Return);
	}
}

void ARADeliveryManager::HandleCartReturned(ARADeliveryCart* Cart)
{
	UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 자 돌아가보실까!"));
	// 큐 맨 뒤로 보내고 대기장소로 복귀
	//RecycleCart(Cart);
	Cart->BackToLocation();
	//Cart->SetState(ECartState::Wait);
}

void ARADeliveryManager::HandleCartCombackCompleted(ARADeliveryCart* Cart)
{
	UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 카트 %s 복귀 완료, 대기 상태로 전환"), *Cart->GetName());
	Cart->SetState(ECartState::Wait);
	//StartNextLoadCycle();
}

void ARADeliveryManager::StartNextLoadCycle()
{
	if (CartQueue.Num() > 0)
	{
		ARADeliveryCart* NextCart = CartQueue[0];

		if (NextCart->GetState() == ECartState::Wait)
		{
			NextCart->SetLoadingLocation(LoadingLocation);
			NextCart->MoveToLocation(LoadingLocation);
			NextCart->SetState(ECartState::Loading);
		}
	}
}

