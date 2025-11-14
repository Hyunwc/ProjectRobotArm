// Fill out your copyright notice in the Description page of Project Settings.


#include "RAProduct.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ARAProduct::ARAProduct()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Type = EProductType::Default;
}

void ARAProduct::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARAProduct::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

