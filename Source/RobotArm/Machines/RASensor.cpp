// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RASensor.h"
#include "Components/BoxComponent.h"
#include "RATestActor.h"

ARASensor::ARASensor()
{
	Sensor = CreateDefaultSubobject<UBoxComponent>(TEXT("Sensor"));
	Sensor->SetupAttachment(Root);
}

void ARASensor::BeginPlay()
{
	Super::BeginPlay();

	Sensor->OnComponentBeginOverlap.AddDynamic(this, &ARASensor::OnSensorOverlapBegin);
}

void ARASensor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARASensor::OnSensorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(15, 1.f, FColor::Emerald, TEXT("오버랩 성공"));
	// 충돌 액터의 Enum값 추출
	ARATestActor* Product = Cast<ARATestActor>(OtherActor);
	EProductType ProductType = Product->GetProductType();

	//FString Mgs = UEnum::GetValueAsString(ProductType);
	GEngine->AddOnScreenDebugMessage(10, 3.f, FColor::Emerald, UEnum::GetValueAsString(ProductType));

	// 해당 물품을 스캔했다고 알림
	OnStateChangeSearch.Broadcast(ProductType);
}

void ARASensor::OnSensorZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
