// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/RARobotArm.h"
#include "ControlRigComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Components/RARobotArmFSM.h"
#include "Components/WidgetComponent.h"
#include "RAConveyor.h"
#include "RAProduct.h"
#include "RASensor.h"
#include "Kismet/GameplayStatics.h"
#include "UI/RARobotArmStateWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Managers/RADeliveryManager.h"
#include "Pawn/RADeliveryCart.h"

ARARobotArm::ARARobotArm()
{
	PrimaryActorTick.bCanEverTick = true;

	EndEffectorName = "Robot1_End_ctrl";
	GrabSpeed = 0.5f;

	ControlRigComponent = CreateDefaultSubobject<UControlRigComponent>(TEXT("ControlRig"));
	ControlRigComponent->SetupAttachment(Root);

	Skeletal = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal"));
	Skeletal->SetupAttachment(Root);

	EndEffectorScene = CreateDefaultSubobject<USceneComponent>(TEXT("EndEffectorScene"));
	EndEffectorScene->SetupAttachment(Root);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetupAttachment(Root);

	//StateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StateWidget"));
	//static ConstructorHelpers::FClassFinder<UUserWidget> StateWidgetRef(TEXT("/Game/UI/WBP_RobotArmState.WBP_RobotArmState_C"));
	//if (StateWidgetRef.Class)
	//{
	//	StateWidget->SetWidgetClass(StateWidgetRef.Class);
	//	StateWidget->SetupAttachment(Root);
	//	StateWidget->SetRelativeLocation(FVector(0.f, 0.f, 400.f));
	//	StateWidget->SetDrawSize(FVector2D(100.f, 30.f));
	//	StateWidget->SetWidgetSpace(EWidgetSpace::Screen);
	//	StateWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//}

	TargetTransform = FTransform(FRotator::ZeroRotator,  FVector::ZeroVector, FVector(1.f, 1.f, 1.f));
	
	FSM = CreateDefaultSubobject<URARobotArmFSM>(TEXT("FSM"));

	//MyType = EProductType::Default;

	bReadyToGrab = false;
}

void ARARobotArm::BeginPlay()
{
	Super::BeginPlay();

	// TODO : 에디터에서 할당하는 것으로 수정해봅니다.
	TArray<AActor*> FoundConveyor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARAConveyor::StaticClass(), FoundConveyor);
	if (FoundConveyor.Num() > 0)
	{
		Conveyor = Cast<ARAConveyor>(FoundConveyor[0]);
		UE_LOG(LogTemp, Log, TEXT("RobotArm : 컨베이어 찾았습니다"));
	}
	
	TArray<AActor*> FoundSensor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARASensor::StaticClass(), FoundSensor);
	if (FoundSensor.Num() > 0)
	{
		Sensor = Cast<ARASensor>(FoundSensor[0]);
		UE_LOG(LogTemp, Log, TEXT("RobotArm : 센서 찾았습니다"));
	}

	//BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ARARobotArm::OnRobotArmOverlapBegin);

	Sensor->OnStateChangeSearch.AddDynamic(this, &ARARobotArm::StartSearch);
	Sensor->OnProductDetected.AddDynamic(this, &ARARobotArm::HandleProduct);

	StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	ReturnTransform = StartTransform;

	if (TargetDettachPoint)
	{
		TargetTransform = FTransform(FRotator::ZeroRotator, TargetDettachPoint->GetActorLocation(), FVector(1.f, 1.f, 1.f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("나의 타겟 찾기 불가능"));
	}

	//if (MyType != EProductType::Other)
	//{
	//	// 목표지점 설정
	//	TargetTransform = TargetConveyor->DettachTransform;
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("난 내가 알아서 찾을게"));
	//}
}

void ARARobotArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Delta = DeltaTime;

	FTransform EffectorTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);

	EndEffectorScene->SetWorldTransform(EffectorTransform);

	switch (FSM->CurrentState)
	{
	case ERobotArmState::Idle:
		IdleState();
		break;
	case ERobotArmState::Search:
		SearchState();
		break;
	case ERobotArmState::Attach:
		AttachState();
		break;
	case ERobotArmState::Carry:
		CarryState();
		break;
	case ERobotArmState::Dettach:
		DettachState();
		break;
	case ERobotArmState::Return:
		ReturnState();
		break;
	}

	OnRobotArmStateChanged.Broadcast(FSM->CurrentState);
}

// 이 함수 들어오면 Search상태로
void ARARobotArm::StartSearch(EProductType SearchType)
{
	// 자신과 다른 타입이라면 
	if (MyType != SearchType)
	{
		return;
	}

	TargetType = SearchType;
	bReadyToGrab = true;
}

void ARARobotArm::IdleState()
{
	//GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Cyan, TEXT("명령 대기 중..."));
}

void ARARobotArm::SearchState()
{
	// 큐가 비어있다면 다시 대기 상태로
	if (ProductQueue.IsEmpty())
	{
		FSM->ChangeState(ERobotArmState::Idle);
		return;
	}

	ARAProduct* Product = ProductQueue[0];

	// 물건이 유효하고 범위 내에 있는지 확인
	if (IsValid(Product) && BoxComp->IsOverlappingActor(Product))
	{
		GrabActor = Product;
		GrabTransform = GrabActor->GetOwnerMesh()->GetSocketTransform(TEXT("GrabSocket"));

		ProductQueue.RemoveAt(0);
		Alpha = 0.0f;

		FSM->ChangeState(ERobotArmState::Attach);
	}
	//else
	//{
	//	ProductQueue.RemoveAt(0);
	//}
}

void ARARobotArm::AttachState()
{
	// 시작 위치 캐시
	if (Alpha == 0.0f)
	{
		StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	}

	// 물건집으러 이동
	MoveToTransform(GrabTransform, Delta);

	if (Alpha >= 1.0f && IsValid(GrabActor))
	{
		// Other 타입일 경우 
		if (GrabActor->GetProductType() == EProductType::Other)
		{
			CurrentCart = DeliveryManager->GetNextCart();
		}
		GrabActor->AttachToComponent(EndEffectorScene, FAttachmentTransformRules::KeepWorldTransform);
		// 컨베이어에서 제거(스플라인 추적 중지)
		if (Conveyor)
		{
			Conveyor->RemoveProduct(GrabActor);
		}

		Alpha = 0.0f;

		FSM->ChangeState(ERobotArmState::Carry);
	}
}

void ARARobotArm::CarryState()
{
	if (Alpha == 0.0f)
	{
		StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	}

	// 목표 지점으로 이동
	MoveToTransform(TargetTransform, Delta);

	if (Alpha >= 1.0f)
	{
		FSM->ChangeState(ERobotArmState::Dettach);
	}
}

void ARARobotArm::DettachState()
{
	if (!IsValid(GrabActor))
	{
		Alpha = 0.0f;
		FSM->ChangeState(ERobotArmState::Return);
		return;
	}

	// 물건 분리
	GrabActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (MyType != EProductType::Other)
	{
		// 컨베이어에서 물품이 다시 이동할 수 있도록 컨베이에 배열에 더해줌.
		TargetConveyor->AddProduct(GrabActor);
		OnClassficationFinished.Broadcast(GrabActor->GetProductType());
	}
	else if (GrabActor->GetProductType() == EProductType::Other)
	{
		GrabActor->SetActorHiddenInGame(true);

		if (CurrentCart && !CurrentCart->CartIsFull())
		{
			CurrentCart->AddProduct(GrabActor);
		}
	}

	GrabActor = nullptr;
	Alpha = 0.0f;
	FSM->ChangeState(ERobotArmState::Return);
}

void ARARobotArm::ReturnState()
{
	if (Alpha == 0.0f)
	{
		StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	}

	// 복귀 지점으로 이동
	MoveToTransform(ReturnTransform, Delta);

	if (Alpha >= 1.0f)
	{
		Alpha = 0.0f;
		// 복귀 완료 후 큐에 물건이 남아있다면?
		if (!ProductQueue.IsEmpty())
		{
			FSM->ChangeState(ERobotArmState::Search);
		}
		else
		{
			FSM->ChangeState(ERobotArmState::Idle);
		}
	}
}

void ARARobotArm::MoveToTransform(const FTransform& Destination, float DeltaTime)
{
	Alpha = FMath::Clamp(Alpha + DeltaTime * GrabSpeed, 0.0f, 1.0f);

	const FVector L = FMath::Lerp(StartTransform.GetLocation(), Destination.GetLocation(), Alpha);
	const FQuat R = FQuat::Slerp(StartTransform.GetRotation(), Destination.GetRotation(), Alpha).GetNormalized();

	// 새로운 트랜스폼으로 컨트롤릭 업데이트
	FTransform NewTransform(R, L, FVector::OneVector);

	ControlRigComponent->SetControlTransform(EndEffectorName, NewTransform, EControlRigComponentSpace::WorldSpace);
}

void ARARobotArm::HandleProduct(EProductType SearchType, ARAProduct* Actor)
{
	// 자신의 타입과 다르다면
	if (SearchType != MyType)
	{
		return;
	}

	if (!ProductQueue.Contains(Actor))
	{
		ProductQueue.Add(Actor);
		UE_LOG(LogTemp, Warning, TEXT("로봇암 %s : %s 물건이 큐에 추가됨, 현재 큐 크기: %d"),*GetName(), *Actor->GetName(), ProductQueue.Num());
	}

	if (FSM->CurrentState == ERobotArmState::Idle)
	{
		FSM->ChangeState(ERobotArmState::Search);
	}
}

//void ARARobotArm::OnRobotArmOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//{
//	if (!bReadyToGrab || GrabActor || !IsValid(OtherActor))
//	{
//		return;
//	}
//
//	ARAProduct* Product = Cast<ARAProduct>(OtherActor);
//	if (!Product)
//	{
//		return;
//	}
//
//	if (Product->GetProductType() == TargetType)
//	{
//		GrabActor = Product;
//		GrabTransform = GrabActor->GetOwnerMesh()->GetSocketTransform(TEXT("GrabSocket"));
//
//		Alpha = 0.0f;
//
//		FSM->ChangeState(ERobotArmState::Attach);
//
//		bReadyToGrab = false;
//	}
//}
