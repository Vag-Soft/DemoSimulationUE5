// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "Components/SceneCaptureComponent2D.h" 
#include "Common/TcpListener.h"
#include <Camera/CameraComponent.h>
#include "MultipleCameraStreamer.generated.h"


USTRUCT()
struct FRenderRequest2 {
	GENERATED_BODY()

	int32 cameraIndex;
	TArray<FColor> Image;
	FRenderCommandFence RenderFence;

	FRenderRequest2() {

	}
};



UCLASS()
class SOCKETPYTHON_API AMultipleCameraStreamer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultipleCameraStreamer();


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

	void AttachRenderTarget();
	void AttachMeshComps();

	void RunAsyncSendData(FSocket* Socket, TArray<FColor> Bmp, int32 CameraID, int32 Width, int32 Height, TArray<USkeletalMeshComponent*> MeshComps);


	void SendRenderRequest(UTextureRenderTarget2D* RenderTarget, int32 camIndex);



	FSocket* ServerSocket;
	FTcpListener* TCPListener;

	TQueue<FRenderRequest2*> RenderRequestQueue;
	bool streaming = false;
	int32 Width;
	int32 Height;

	int32 lastIndex = 0;

	TArray<USkeletalMeshComponent*> MeshComps;
	TArray<UTextureRenderTarget2D*> RenderTargets;


};

class AsyncSendDataClass2 : public FNonAbandonableTask {
public:
	AsyncSendDataClass2(FSocket* ServerSocket, TArray<FColor> Bitmap, int32 CameraID, int32 Width, int32 Height, TArray<USkeletalMeshComponent*> MeshComps);
	~AsyncSendDataClass2();

	// Required by UE4!
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSendDataClass2, STATGROUP_ThreadPoolAsyncTasks);
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