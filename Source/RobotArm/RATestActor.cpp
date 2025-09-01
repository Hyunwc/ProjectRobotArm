// Fill out your copyright notice in the Description page of Project Settings.


#include "RATestActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ARATestActor::ARATestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Type = EProductType::Default;
}

void ARATestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

