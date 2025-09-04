// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RAConveyor.h"
#include "Components/SplineComponent.h"
#include "RATestActor.h"
#include "Kismet/GameplayStatics.h"

ARAConveyor::ARAConveyor()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Spline->SetupAttachment(Root);

	MoveSpeed = 100.f;
}

void ARAConveyor::BeginPlay()
{
	Super::BeginPlay();

	if (Spline->GetNumberOfSplinePoints() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("RAConveyor : 스플라인은 2개의 포인트를 필요로 합니다!"));
	}

	SplineStartLocation = Spline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
}

void ARAConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float SplineLength = Spline->GetSplineLength();

	for (int32 i = Products.Num() - 1; i >= 0; i--)
	{
		FConveyorProduct& Prod = Products[i];

		if (IsValid(Prod.TestActor))
		{
			// 거리 업데이트
			Prod.Distance += MoveSpeed * DeltaTime;
			FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(Prod.Distance, ESplineCoordinateSpace::World);
			Prod.TestActor->SetActorLocation(NewLocation);

			// 끝까지 가면 삭제
			if (Prod.Distance > Spline->GetSplineLength())
			{
				// 풀에 반납 및 현재 컨베이어의 배열에서 삭제
				OnReturnProduct.Broadcast(Prod.TestActor, Prod.TestActor->GetProductType());
				Products.RemoveAt(i);
			}
		}
		else
		{
			Products.RemoveAt(i);
		}
	}
}

void ARAConveyor::ProductSpawn(const TArray<TSubclassOf<ARATestActor>>& ProductClass)
{
	if (ProductClass.Num() == 0)
	{
		return;
	}

	int32 Index = FMath::RandRange(0, 3);

	ARATestActor* NewActor = GetWorld()->SpawnActor<ARATestActor>(ProductClass[Index], SplineStartLocation, FRotator::ZeroRotator);
	if (NewActor)
	{
		FConveyorProduct Product;
		Product.TestActor = NewActor;
		Product.Distance = 0.0f;
		Products.Add(Product);
	}
}

void ARAConveyor::RemoveProduct(AActor* Actor)
{
	Products.RemoveAll([Actor](const FConveyorProduct& Prod)
		{
			return Prod.TestActor == Actor;
		});
}

void ARAConveyor::AddProduct(AActor* Actor)
{
	if (Actor)
	{
		FConveyorProduct NewProduct;
		NewProduct.TestActor = Cast<ARATestActor>(Actor);
		NewProduct.Distance = 0.0f;
		Products.Add(NewProduct);
	}
}
