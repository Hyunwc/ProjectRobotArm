// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RASensor.h"
#include "Components/BoxComponent.h"

ARASensor::ARASensor()
{
	Sensor = CreateDefaultSubobject<UBoxComponent>(TEXT("Sensor"));
	Sensor->SetupAttachment(Root);
}

void ARASensor::BeginPlay()
{
}

void ARASensor::Tick(float DeltaTime)
{
}

void ARASensor::OnSensorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 충돌 액터의 Enum값 추출
	//EProductType Type = 
}

void ARASensor::OnSensorZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
