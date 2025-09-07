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
#include "RATestActor.h"
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

	StateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("StateWidget"));
	static ConstructorHelpers::FClassFinder<UUserWidget> StateWidgetRef(TEXT("/Game/UI/WBP_RobotArmState.WBP_RobotArmState_C"));
	if (StateWidgetRef.Class)
	{
		StateWidget->SetWidgetClass(StateWidgetRef.Class);
		StateWidget->SetupAttachment(Root);
		StateWidget->SetRelativeLocation(FVector(0.f, 0.f, 400.f));
		StateWidget->SetDrawSize(FVector2D(100.f, 30.f));
		StateWidget->SetWidgetSpace(EWidgetSpace::Screen);
		StateWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	FSM = CreateDefaultSubobject<URARobotArmFSM>(TEXT("FSM"));

	MyType = EProductType::Default;

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

	if (MyType != EProductType::Other)
	{
		// 목표지점 설정
		TargetTransform = TargetConveyor->DettachTransform;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("난 내가 알아서 찾을게"));
	}
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

	if (StateWidget)
	{
		if (URARobotArmStateWidget* ArmWidget = Cast<URARobotArmStateWidget>(StateWidget->GetUserWidgetObject()))
		{
			ArmWidget->UpdateRobotArmState(FSM->CurrentState);
		}
	}
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
	//FSM->ChangeState(ERobotArmState::Search);
}

void ARARobotArm::IdleState()
{
	//GEngine->AddOnScreenDebugMessage(0, 3.f, FColor::Cyan, TEXT("명령 대기 중..."));
}

void ARARobotArm::SearchState()
{
	if (ProductQueue.Num() > 0)
	{
		ARATestActor* Product = ProductQueue[0];

		if (IsValid(Product) && BoxComp->IsOverlappingActor(Product))
		{
			GrabActor = Product;

			// 큐에서 처리할 물건 제거
			ProductQueue.RemoveAt(0);

			GrabTransform = GrabActor->GetOwnerMesh()->GetSocketTransform(TEXT("GrabSocket"));

			Alpha = 0.0f;
			FSM->ChangeState(ERobotArmState::Attach);
		}
		else
		{
			if (!IsValid(Product) || BoxComp->IsOverlappingActor(Product))
			{
				UE_LOG(LogTemp, Warning, TEXT("로봇암 %s: 물건이 유효하지 않거나 범위를 벗어남 "), *GetName());
				ProductQueue.RemoveAt(0);
			}
		}
	}
	else
	{
		FSM->ChangeState(ERobotArmState::Idle);
	}
}

void ARARobotArm::AttachState()
{
	if (Alpha == 0.0f)
	{
		StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);
	}

	// 물건집으러 이동
	MoveToTransform(GrabTransform, Delta);

	if (Alpha >= 1.0f)
	{
		if (GrabActor)
		{
			//GEngine->AddOnScreenDebugMessage(8, 2.f, FColor::Red, TEXT("유효"));
			GrabTransform = GrabActor->GetOwnerMesh()->GetSocketTransform(TEXT("GrabSocket"));
			//GrabTransform = GrabActor->GetActorTransform();

			// 그랩액터가 Other타입이면
			if (GrabActor->GetProductType() == EProductType::Other)
			{
				// 물건 집어서
				GrabActor->AttachToComponent(EndEffectorScene, FAttachmentTransformRules::KeepWorldTransform);

				MoveToTransform(GrabTransform, Delta);

				// 카트에 넣고
				ARADeliveryCart* Cart = DeliveryManager->GetNextCart();

				TargetTransform = FTransform(FRotator::ZeroRotator, Cart->GetLoadingLocation(), FVector(1.f, 1.f, 1.f));
				if (!Cart->CartIsFull())
				{
					Cart->AddProduct(GrabActor);
				}

				Alpha = 0.0f;
				
				FSM->ChangeState(ERobotArmState::Carry);
			}
		
			// 어태치 이 후에도 액터가 스플라인을 따라가지 않게 하기 위해 배열에서 제거
			if (Conveyor)
			{
				Conveyor->RemoveProduct(GrabActor);
			}

			MoveToTransform(GrabTransform, Delta);

			GrabActor->AttachToComponent(EndEffectorScene, FAttachmentTransformRules::KeepWorldTransform);
		
			Alpha = 0.0f;
		
			FSM->ChangeState(ERobotArmState::Carry);
		}
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
	if (GrabActor)
	{
		// 메시 분리
		GrabActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		if (MyType != EProductType::Other)
		{
			// 타겟 컨베이어에서 물품이 다시 이동할 수 있도록 컨베이어의 배열에 더해줌.
			TargetConveyor->AddProduct(GrabActor);
			OnClassficationFinished.Broadcast(GrabActor->GetProductType());
		}

		if (GrabActor->GetProductType() == EProductType::Other)
		{
			GrabActor->SetActorHiddenInGame(true);
		}

		GrabActor = nullptr;

		//StartTransform = ControlRigComponent->GetControlTransform(EndEffectorName, EControlRigComponentSpace::WorldSpace);

		Alpha = 0.0f;
		FSM->ChangeState(ERobotArmState::Return);
	}
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
		if (ProductQueue.Num() > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("로봇암 %s : 아직 %d개나 남았네? 이런!"), *GetName(), ProductQueue.Num());
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
	const FVector S = FVector(1.f);

	// 새로운 트랜스폼으로 컨트롤릭 업데이트
	FTransform NewTransform(R, L, S);

	ControlRigComponent->SetControlTransform(EndEffectorName, NewTransform, EControlRigComponentSpace::WorldSpace);
}

void ARARobotArm::HandleProduct(EProductType SearchType, ARATestActor* Actor)
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

void ARARobotArm::OnRobotArmOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bReadyToGrab || GrabActor || !IsValid(OtherActor))
	{
		return;
	}

	ARATestActor* Product = Cast<ARATestActor>(OtherActor);
	if (!Product)
	{
		return;
	}

	if (Product->GetProductType() == TargetType)
	{
		GrabActor = Product;
		GrabTransform = GrabActor->GetOwnerMesh()->GetSocketTransform(TEXT("GrabSocket"));

		Alpha = 0.0f;

		FSM->ChangeState(ERobotArmState::Attach);

		bReadyToGrab = false;
	}
}
