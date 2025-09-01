// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RAConveyor.h"
#include "Components/SplineComponent.h"
#include "RATestActor.h"

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

	for (auto& Product : Products)
	{
		if (!Product.TestActor)
		{
			continue;
		}

		// 거리 업데이트
		Product.DistanceAlongSpline += MoveSpeed * DeltaTime;

		// 끝이면 제거
		if (Product.DistanceAlongSpline > SplineLength)
		{
			Product.TestActor->Destroy();
			Product.TestActor = nullptr;
			continue;
		}

		FVector SplineLocation = Spline->GetLocationAtDistanceAlongSpline(Product.DistanceAlongSpline, ESplineCoordinateSpace::World);

		Product.TestActor->SetActorLocation(SplineLocation);
	}

}

void ARAConveyor::ProductSpawn()
{
	ARATestActor* NewActor = GetWorld()->SpawnActor<ARATestActor>(TestBP, SplineStartLocation, FRotator::ZeroRotator);
	if (NewActor)
	{
		FConveyorProduct Product;
		Product.TestActor = NewActor;
		Product.DistanceAlongSpline = 0.0f;
		Products.Add(Product);
	}
}
