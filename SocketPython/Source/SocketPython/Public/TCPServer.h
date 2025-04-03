#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "Components/SceneCaptureComponent2D.h" 
#include "Common/TcpListener.h"
#include "TCPServer.generated.h"


USTRUCT()
struct FRenderRequestStruct {
    GENERATED_BODY()

    FString Name;
    TArray<FColor> Image;
    FRenderCommandFence RenderFence;

    FRenderRequestStruct() {

    }
};


UCLASS()
class SOCKETPYTHON_API ATCPServer : public AActor
{
    GENERATED_BODY()

public:
    ATCPServer();
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    TQueue<FRenderRequestStruct*> RenderRequestQueue;

protected:
    int ImgCounter = 0;

private:
    bool StartTCPServer();
    void StopTCPServer();
    bool HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint);
    void PollSocket();
    void HandleRequest(FString Request);


    void AttachRenderTargets();


    void CaptureNonBlocking();


    void ProcessImage(FString name, TArray<uint8> PNGData);
    void ProcessImage1(TArray<FColor> PixelData, UTextureRenderTarget2D* RenderTarget);
    void ProcessImage2(TArray<TArray<FColor>> AllPixelData);
	void SendData();

    void RunAsyncSendData(TMap<FString, TArray<uint8>> CameraImagesCopy, FSocket* Socket, TArray<FColor> Bitmap, FString CameraNameTag);



    FSocket* ServerSocket;
    FTcpListener* TCPListener;
	TArray<UTextureRenderTarget2D*> RenderTargets;
	bool streaming = false;

	TMap<FString, TArray<uint8>> CameraImages;


};



class AsyncSendDataTask : public FNonAbandonableTask {
public:
    AsyncSendDataTask(TMap<FString, TArray<uint8>> CameraImagesCopy, FSocket* ServerSocket, TArray<FColor> Bitmap, FString CameraNameTag);
    ~AsyncSendDataTask();

    // Required by UE4!
    FORCEINLINE TStatId GetStatId() const {
        RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSendDataTask, STATGROUP_ThreadPoolAsyncTasks);
    }

protected:
    TMap<FString, TArray<FColor>> CameraImagesCopy;
    FSocket* ServerSocket;
	TArray<FColor> Bitmap;
	FString CameraNameTag;

public:
    void DoWork();
};