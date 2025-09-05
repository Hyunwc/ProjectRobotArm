// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/RADeliveryCart.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

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
}

void ARADeliveryCart::AddProduct(AActor* Product)
{
	if (!Product)
	{
		return;
	}

	Products.Add(Product);
	// 물건 임시로 사라져 보이게 
	Product->SetActorHiddenInGame(true);
	ProductCapacity++;

	if (ProductCapacity >= MaxCapacity)
	{
		UE_LOG(LogTemp, Warning, TEXT("자 이제 나를 이동하라고 명령해줘!"));
		OnCartFull.Broadcast(this); // Manager가 듣고 이동해라고 명령할 거임
	}

	//if (ProductCapacity >= MaxCapacity)
	//{
	//	MoveToLocation();
	//}
	//Product->AttachToComponent(CartMesh, FAttachmentTransformRules::KeepWorldTransform);
}

void ARADeliveryCart::MoveToLocation(const FVector& NewLocation)
{
	if (AAIController* AICon = GetAICon())
	{
		AICon->MoveToLocation(NewLocation);
	}
}

void ARADeliveryCart::OnArrived()
{
	//ReturnProducts();
	OnCartArrived.Broadcast(this);
}

void ARADeliveryCart::ReturnProducts()
{
	SetState(ECartState::Return);

	GetWorldTimerManager().SetTimer(ReturnTimerHandle, this, &ARADeliveryCart::HandleReturnTick, 1.f, true);

	//for (AActor* Product : Products)
	//{
	//	if (Product)
	//	{
	//		Product->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//		Product->Destroy();
	//	}
	//}
	//
	//Products.Empty();
}

void ARADeliveryCart::BackToLocation(const FVector& NewLocation)
{
	if (AAIController* AICon = GetAICon())
	{
		AICon->MoveToLocation(NewLocation);
	}

	// 도착 시점에 OnCartBacked.Broadcast 호출은 매니저가 트리거 해줘야함
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
		OnCartReturned.Broadcast(this); // 반환 끝을 알림
		return;
	}

	// TODO : 풀매니저에게 반납하는 코드를 작성합니다.
	AActor* Product = Products.Pop();
	if (Product)
	{
		Product->Destroy();
		ProductCapacity--;
	}
}

