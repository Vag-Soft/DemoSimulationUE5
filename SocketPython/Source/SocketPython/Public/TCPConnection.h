// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "Components/SceneCaptureComponent2D.h" 
#include "Common/TcpListener.h"
#include "TCPConnection.generated.h"

UCLASS()
class SOCKETPYTHON_API ATCPConnection : public AActor
{
    GENERATED_BODY()

public:
    ATCPConnection();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    bool StartTCPServer();
    void StopTCPServer();
    bool HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint);
    void PollSocket();

    void HandleRequest(FString Request);

    void AttachRenderTargets();

    FSocket* ServerSocket;
    FTcpListener* TCPListener;
    TArray<UTextureRenderTarget2D*> RenderTargets;
    bool streaming = false;



    USceneCaptureComponent2D* SceneCaptureComponent;

    TArray<FVector> CameraPositions;
    TArray<FRotator> CameraRotations;
    int32 lastIndex = 0;
};
