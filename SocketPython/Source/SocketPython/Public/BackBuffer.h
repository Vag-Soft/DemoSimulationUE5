// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "Components/SceneCaptureComponent2D.h" 
#include "Common/TcpListener.h"
#include <Camera/CameraComponent.h>
#include "BackBuffer.generated.h"


USTRUCT()
struct FRenderRequest1 {
	GENERATED_BODY()

	int32 cameraIndex;
	TArray<FColor> Image;
	FRenderCommandFence RenderFence;

	FRenderRequest1() {

	}
};


UCLASS()
class SOCKETPYTHON_API ABackBuffer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABackBuffer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool StartTCPServer();
	void StopTCPServer();
	bool HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint);
	void PollSocket();
	void HandleRequest(FString Request);

	void AttachViewport();
	void AttachMeshComps();

	void RunAsyncSendData(FSocket* Socket, TArray<FColor> Bmp, int32 CameraID, int32 Width, int32 Height, TArray<USkeletalMeshComponent*> MeshComps);


	void SendRenderRequest();



	FSocket* ServerSocket;
	FTcpListener* TCPListener;

	TQueue<FRenderRequest1*> RenderRequestQueue;
	FViewport* Viewport;
	bool streaming = false;
	int32 Width;
	int32 Height;

	TArray<FVector> CameraPositions;
	TArray<FRotator> CameraRotations;
	UCameraComponent* FollowCamera;
	int32 lastIndex = 0;

	TArray<USkeletalMeshComponent*> MeshComps;
};





class AsyncSendDataClass1 : public FNonAbandonableTask {
public:
	AsyncSendDataClass1(FSocket* ServerSocket, TArray<FColor> Bitmap, int32 CameraID, int32 Width, int32 Height, TArray<USkeletalMeshComponent*> MeshComps);
	~AsyncSendDataClass1();

	// Required by UE4!
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSendDataClass1, STATGROUP_ThreadPoolAsyncTasks);
	}

protected:
	FSocket* ServerSocket;
	TArray<FColor> Bitmap;
	int32 CameraID;
	int32 Width;
	int32 Height;
	TArray<USkeletalMeshComponent*> MeshComps;

public:
	void DoWork();
};