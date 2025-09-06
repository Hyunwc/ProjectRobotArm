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

			Cart->SetState(ECartState::Wait);
		}
	}

	if (CartQueue.Num() > 0)
	{
		ARADeliveryCart* FrontCart = CartQueue[0];

		if (FrontCart->GetState() == ECartState::Wait)
		{
			FrontCart->MoveToLocation(LoadingLocation);
			FrontCart->SetState(ECartState::Loading);
		}
	}
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

void ARADeliveryManager::RecycleCart(ARADeliveryCart* Cart)
{
	if (CartQueue.Remove(Cart) > 0)
	{
		CartQueue.Add(Cart);
		Cart->BackToLocation(PrevCartLocation);
	}
}

void ARADeliveryManager::HandleCartCapacityFull(ARADeliveryCart* Cart)
{
	UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 카드 %s가 가득 찼음 반환 시작"), *Cart->GetName());

	// 현재 카트 반환 장소로
	Cart->MoveToLocation(ReturnLocation);
	Cart->SetState(ECartState::Move);

	// 다음 카트 찾아서 적재 장소로
	int32 NextIndex = (CartQueue.IndexOfByKey(Cart) + 1) % CartQueue.Num();

	if (CartQueue.IsValidIndex(NextIndex))
	{
		ARADeliveryCart* NextCart = CartQueue[NextIndex];
		if (NextCart->GetState() == ECartState::Wait)
		{
			UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 카드 %s 적재 장소로 이동 "), *NextCart->GetName());
			// 복귀 장소 캐시
			PrevCartLocation = NextCart->GetActorLocation();
			DrawDebugSphere(GetWorld(), PrevCartLocation, 50.f, 12, FColor::Green);
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
		Cart->SetState(ECartState::Return);
	}
}

void ARADeliveryManager::HandleCartReturned(ARADeliveryCart* Cart)
{
	UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 자 돌아가보실까!"));
	// 큐 맨 뒤로 보내고 대기장소로 복귀
	RecycleCart(Cart);
	Cart->SetState(ECartState::Wait);
}

void ARADeliveryManager::HandleCartCombackCompleted(ARADeliveryCart* Cart)
{
	UE_LOG(LogTemp, Warning, TEXT("DeliveryManager : 카트 %s 복귀 완료, 대기 상태로 전환"), *Cart->GetName());
	//Cart->SetState(ECartState::Wait);
}

