// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HISClone.generated.h"

UCLASS()
class HIDEINSYNC_API AHISClone : public AActor
{
	GENERATED_BODY()

public:
	AHISClone();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated)
	int PlayerId;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool bIsFound = false;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* CloneMesh;
	UPROPERTY(EditAnywhere)
	class USphereComponent* AreaSphere;
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* CapsuleCollider;
	UPROPERTY(EditAnywhere)
	class UWidgetComponent* FoundWidget;

	// If binding to a component's OnBeginOverlap, then NEED this signature:
	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:
	FORCEINLINE void SetPlayerId(int Id) { PlayerId = Id; }		// Ensure this is only used once, by the game mode, while setting up the level
	FORCEINLINE int GetPlayerId() { return PlayerId; }

	FORCEINLINE bool IsFound() { return bIsFound; }				// We using this?

	void ShowFoundWidget(bool bShowWidget);
	
	void FoundIgnore();
	UFUNCTION(NetMulticast, Reliable)							// [TODO] Aren't they all like this? Also THINK! Do we need NetMulticast? Or Server, or something else?
	void FoundReset();
	void FoundSpectate();
};
