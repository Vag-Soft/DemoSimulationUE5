// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "Components/SceneCaptureComponent2D.h" 
#include "Common/TcpListener.h"
#include "OneCameraStreamer.generated.h"

USTRUCT()
struct FRenderRequest {
	GENERATED_BODY()

	FString Name;
	TArray<FColor> Image;
	FRenderCommandFence RenderFence;

	FRenderRequest() {

	}
};

UCLASS()
class SOCKETPYTHON_API AOneCameraStreamer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOneCameraStreamer();
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

	void RunAsyncSendData(FSocket* Socket, TArray<FColor> Bmp, FString CameraNameTag);


	void SendRenderRequest();



	TQueue<FRenderRequest*> RenderRequestQueue;
	FSocket* ServerSocket;
	FTcpListener* TCPListener;
	TArray<UTextureRenderTarget2D*> RenderTargets;
	UTextureRenderTarget2D* RenderTarget;
	bool streaming = false;

	USceneCaptureComponent2D* SceneCaptureComponent;

	TArray<FVector> CameraPositions;
	TArray<FRotator> CameraRotations;
	int32 lastIndex = 0;
};





class AsyncSendDataClass : public FNonAbandonableTask {
public:
	AsyncSendDataClass(FSocket* ServerSocket, TArray<FColor> Bitmap, FString CameraNameTag);
	~AsyncSendDataClass();

	// Required by UE4!
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSendDataClass, STATGROUP_ThreadPoolAsyncTasks);
	}

protected:
	FSocket* ServerSocket;
	TArray<FColor> Bitmap;
	FString CameraNameTag;

public:
	void DoWork();
};