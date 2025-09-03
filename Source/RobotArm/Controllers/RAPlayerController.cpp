// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/RAPlayerController.h"
#include "Camera/RACameraActor.h"
#include "Kismet/GameplayStatics.h"


ARAPlayerController::ARAPlayerController()
{
	bShowMouseCursor = true;
}

void ARAPlayerController::BeginPlay()
{
	Super::BeginPlay();

	EnableInput(this);

	SetInputMode(FInputModeGameAndUI());

	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CameraActorClass, FoundCameras);
	UE_LOG(LogTemp, Warning, TEXT("RAPlayerController : 찾은 카메라 수 : %d"), FoundCameras.Num());
	for (AActor* Actor : FoundCameras)
	{
		ARACameraActor* Camera = Cast<ARACameraActor>(Actor);

		CameraMap.Add(Camera->CameraTag, Camera);
	}

	SetViewTarget(CameraMap.FindRef(TEXT("Main")));	
}

void ARAPlayerController::SwitchCamera(FName CameraTag)
{
	if (CameraMap.Num() == 0)
	{
		return;
	}

	SetViewTargetWithBlend(CameraMap.FindRef(CameraTag), 0.0f);

	OnCameraViewChanged.Broadcast(CameraMap.FindRef(CameraTag)->DisplayName);
}

