// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/RADeliveryCart.h"
#include "Components/SceneComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Managers/RAPoolManager.h"
#include "RATestActor.h"
//#include "AIController.h"


ARADeliveryCart::ARADeliveryCart()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	CartMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CartMesh"));
	CartMesh->SetupAttachment(Root);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	
	ProductCapacity = 0;
	MaxCapacity = 3;
	CartState = ECartState::Wait;
}

void ARADeliveryCart::BeginPlay()
{
	Super::BeginPlay();
	
	// 현재 위치를 복귀 위치로
	WaitLocation = GetActorLocation();
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
		AICon->MoveToLocation(NewLocation);

		if (UPathFollowingComponent* PathComp = AICon->GetPathFollowingComponent())
		{
			// 기존 델리게이트 전부 제거
			PathComp->OnRequestFinished.Clear();

			PathComp->OnRequestFinished.AddUObject(this, &ARADeliveryCart::HandleMoveCompleted);
		}
	}
}

void ARADeliveryCart::BackToLocation()
{
	if (AAIController* AICon = GetAICon())
	{
		const float AcceptanceRadius = 5.0f;
		AICon->MoveToLocation(WaitLocation, AcceptanceRadius);

		if (UPathFollowingComponent* PathComp = AICon->GetPathFollowingComponent())
		{
			// 기존 델리게이트 전부 제거
			PathComp->OnRequestFinished.Clear();

			PathComp->OnRequestFinished.AddUObject(this, &ARADeliveryCart::HandleCombackCompleted);
		}
	}
}

void ARADeliveryCart::OnArrived()
{
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
		//SetState(ECartState::Wait);
		SetActorLocation(WaitLocation); // 어긋나는 위치 보정
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

	// 이 델리게이트를 등록한 풀매니저 호출
	OnReturnCartProduct.Broadcast(Products.Pop(), EProductType::Other);
	ProductCapacity--;
}

void ARADeliveryCart::SetLoadingLocation(const FVector& Location)
{
	CacheLoadingLocation = Location;
}

