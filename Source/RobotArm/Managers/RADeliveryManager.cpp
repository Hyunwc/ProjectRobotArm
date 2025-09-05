// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/RADeliveryManager.h"
#include "Pawn/RADeliveryCart.h"
#include "Kismet/GameplayStatics.h"

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

	//if (CartQueue.Num() > 0)
	//{
	//	ARADeliveryCart* FrontCart = CartQueue[0];
	//
	//	if (FrontCart->GetState() == ECartState::Wait)
	//	{
	//		DispatchLoadingCart();
	//	}
	//}
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

		// 맨 뒤로 보낸 뒤 대기상태로 
		int32 Index = CartQueue.Num() - 1;
		if (WaitingLocations.IsValidIndex(Index))
		{
			Cart->MoveToLocation(WaitingLocations[Index]);
		}
		Cart->SetState(ECartState::Wait);
	}
}

void ARADeliveryManager::DispatchLoadingCart()
{
	if (ARADeliveryCart* Cart = GetNextCart())
	{
		Cart->MoveToLocation(LoadingLocation);
		Cart->SetState(ECartState::Loading);
	}
}

void ARADeliveryManager::DispatchReturnCart()
{
	if (ARADeliveryCart* Cart = GetNextCart())
	{
		Cart->MoveToLocation(ReturnLocation);
		Cart->SetState(ECartState::Return);
	}
}

void ARADeliveryManager::HandleCartCapacityFull(ARADeliveryCart* Cart)
{
	UE_LOG(LogTemp, Warning, TEXT("응 여기로 안들어오거든!"));
	Cart->MoveToLocation(ReturnLocation);
	Cart->SetState(ECartState::Move);
}

void ARADeliveryManager::HandleCartArrived(ARADeliveryCart* Cart)
{
	Cart->ReturnProducts();
	Cart->SetState(ECartState::Return);
}

void ARADeliveryManager::HandleCartReturned(ARADeliveryCart* Cart)
{
	RecycleCart(Cart);
}

