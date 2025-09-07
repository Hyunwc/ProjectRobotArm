// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/RADeliveryCart.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/RAPoolManager.h"
#include "RATestActor.h"
//#include "AIController.h"


ARADeliveryCart::ARADeliveryCart()
{
	PrimaryActorTick.bCanEverTick = true;

	CartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CartMesh"));
	RootComponent = CartMesh;

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	
	ProductCapacity = 0;
	MaxCapacity = 5;
	CartState = ECartState::Wait;
}

void ARADeliveryCart::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARADeliveryCart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARADeliveryCart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARADeliveryCart::SetState(ECartState NewState)
{
	CartState = NewState;

	UE_LOG(LogTemp, Warning, TEXT("카트 : 상태를 넘겨줄게요"));

	OnCartStatusChanged.Broadcast(this, NewState);
}

void ARADeliveryCart::AddProduct(AActor* Product)
{
	if (!Product)
	{
		return;
	}

	Products.Add(Cast<ARATestActor>(Product));
	// 물건 임시로 사라져 보이게 
	//Product->SetActorHiddenInGame(true);
	ProductCapacity++;

	if (ProductCapacity >= MaxCapacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("자 이제 나를 이동하라고 명령해줘!"));
		OnCartFull.Broadcast(this); // Manager가 듣고 이동해라고 명령할 거임
	}
}

void ARADeliveryCart::MoveToLocation(const FVector& NewLocation)
{
	if (AAIController* AICon = GetAICon())
	{
		//FNavPathSharedPtr NavPath;
		AICon->MoveToLocation(NewLocation);

		if (UPathFollowingComponent* PathComp = AICon->GetPathFollowingComponent())
		{
			PathComp->OnRequestFinished.AddUObject(this, &ARADeliveryCart::HandleMoveCompleted);
		}
	}
}

void ARADeliveryCart::BackToLocation(const FVector& NewLocation)
{
	if (AAIController* AICon = GetAICon())
	{
		CachedCombackLocation = NewLocation;
		const float AcceptanceRadius = 5.0f;
		AICon->MoveToLocation(NewLocation, AcceptanceRadius);

		if (UPathFollowingComponent* PathComp = AICon->GetPathFollowingComponent())
		{
			PathComp->OnRequestFinished.AddUObject(this, &ARADeliveryCart::HandleCombackCompleted);
		}
	}
}

void ARADeliveryCart::OnArrived()
{
	//ReturnProducts();
	OnCartArrived.Broadcast(this);
}

void ARADeliveryCart::HandleMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.Code == EPathFollowingResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cart %s : 도착성공"), *GetName());
		OnCartArrived.Broadcast(this);
	}
}

void ARADeliveryCart::HandleCombackCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.Code == EPathFollowingResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cart %s : 복귀 성공! 다시 명령 대기하겠습니다"), *GetName());
		SetActorLocation(CachedCombackLocation);
		OnCartCombacked.Broadcast(this);
	}
}

void ARADeliveryCart::ReturnProducts()
{
	SetState(ECartState::Return);

	GetWorldTimerManager().SetTimer(ReturnTimerHandle, this, &ARADeliveryCart::HandleReturnTick, 1.f, true);
}

AAIController* ARADeliveryCart::GetAICon() const
{
	return Cast<AAIController>(GetController());
}

void ARADeliveryCart::HandleReturnTick()
{
	if (Products.Num() == 0)
	{
		GetWorldTimerManager().ClearTimer(ReturnTimerHandle);

		ProductCapacity = 0;
		UE_LOG(LogTemp, Warning, TEXT("Cart : 반환 후 용량: %d, 배열 사이즈: %d"), ProductCapacity, Products.Num());
		OnCartReturned.Broadcast(this); // 반환 끝을 알림
		return;
	}

	// TODO : 풀매니저에게 반납하는 코드를 작성합니다.
	OnReturnCartProduct.Broadcast(Products.Pop(), EProductType::Other);
	ProductCapacity--;
}

void ARADeliveryCart::SetLoadingLocation(const FVector& Location)
{
	CacheLoadingLocation = Location;
}

