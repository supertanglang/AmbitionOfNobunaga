// Fill out your copyright notice in the Description page of Project Settings.

#include "AmbitionOfNobunaga.h"
#include "FlySkillActor.h"
#include "UnrealNetwork.h"

AFlySkillActor::AFlySkillActor(const FObjectInitializer& ObjectInitializer)
	: Super(FObjectInitializer::Get())
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	Scene = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("root0"));
	BulletParticle = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("BulletParticle0"));
	RootComponent = Scene;
	BulletParticle->AttachParent = RootComponent;
	DestroyDelay = 2;
	UseTargetLocation = true;
}

void AFlySkillActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AFlySkillActor, UseTargetLocation)))
	{
		if (UseTargetLocation)
		{
			UseTargetActor = false;
		}
		else
		{
			UseTargetActor = true;
		}
	}
	else if ((PropertyName == GET_MEMBER_NAME_CHECKED(AFlySkillActor, UseTargetActor)))
	{
		if (UseTargetActor)
		{
			UseTargetLocation = false;
		}
		else
		{
			UseTargetLocation = true;
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

// Called when the game starts or when spawned
void AFlySkillActor::BeginPlay()
{
	Super::BeginPlay();
	PrepareDestory = false;
}

// Called every frame
void AFlySkillActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	float move = DeltaTime * MoveSpeed;
	FVector ourpos = GetActorLocation();
	FVector dstpos;
	if (UseTargetLocation)
	{
		dstpos = TargetLocation;
	}
	else
	{
		dstpos = TargetActor->GetActorLocation();;
	}
	float dis = FVector::Dist(ourpos, dstpos);
	if (move >= dis)
	{
		SetActorLocation(dstpos);
	}
	else
	{
		FVector dir = dstpos - ourpos;
		dir.Normalize();
		dir *= move;
		SetActorLocation(ourpos + dir);
	}
	if (dis < 1 && !PrepareDestory)
	{
		BulletParticle->SetActive(false);
		PrepareDestory = true;
		DestoryCount = 0;
	}
	if (PrepareDestory)
	{
		DestoryCount += DeltaTime;
		if (DestoryCount >= DestroyDelay)
		{
			this->Destroy();
		}
	}
}

void AFlySkillActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFlySkillActor, TargetLocation);
	DOREPLIFETIME(AFlySkillActor, TargetActor);
}
