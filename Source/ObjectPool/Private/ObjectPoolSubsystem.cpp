// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPoolSubsystem.h"

bool UObjectPoolSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	TArray<UClass*> ChildClasses;
	GetDerivedClasses(StaticClass(), ChildClasses);
	
	return ChildClasses.Num() == 0;
}

void UObjectPoolSubsystem::CreateObjectPool(FObjectPoolConfig ObjectPoolConfig)
{
	if (ObjectPools.Contains(ObjectPoolConfig.ObjectPoolClass.Get()))
	{
		UE_LOG(LogObjectPool, Warning, TEXT("%s has already in the object pools, operation canceled"), *ObjectPoolConfig.ObjectPoolClass->GetName())
		return;
	}

#if WITH_EDITOR
	UObjectPool* PoolObject = UObjectPool::CreateObjectPool(this, ObjectPoolConfig, true);
#else
	UObjectPool* PoolObject = UObjectPool::CreateObjectPool(this, ObjectPoolConfig, false);
#endif

	ObjectPools.Add(ObjectPoolConfig.ObjectPoolClass.Get(), PoolObject);
}

AActor* UObjectPoolSubsystem::AllocateByStaticClass(UClass* InClass)
{
	if (!ObjectPools.Contains(InClass))
	{
		UE_LOG(LogObjectPool, Warning, TEXT("%s does not exists in the pool subsystem"), *InClass->GetName())
		return nullptr;
	}

	return ObjectPools[InClass]->Allocate();
}

AActor* UObjectPoolSubsystem::AllocateBySubclass(const TSubclassOf<AActor> InActorClass)
{
	return AllocateByStaticClass(InActorClass.Get());
}

bool UObjectPoolSubsystem::RecycleByQueue(UClass* InClass)
{
	if (!ObjectPools.Contains(InClass))
	{
		UE_LOG(LogObjectPool, Warning, TEXT("%s does not exists in the pool subsystem"), *InClass->GetName())
		return false;
	}
	return ObjectPools[InClass]->Recycle();
}

bool UObjectPoolSubsystem::RecycleByActor(AActor* InActor)
{
	if (!ObjectPools.Contains(InActor->GetClass()))
	{
		UE_LOG(LogObjectPool, Warning, TEXT("%s does not exists in the pool subsystem"), *InActor->GetName())
		return false;
	}
	return ObjectPools[InActor->GetClass()]->Recycle(InActor);
}

bool UObjectPoolSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == (EWorldType::PIE | EWorldType::Game);
}
