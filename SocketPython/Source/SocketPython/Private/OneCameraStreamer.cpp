// Fill out your copyright notice in the Description page of Project Settings.


#include "OneCameraStreamer.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/SceneCapture2D.h" 
#include "Components/SceneCaptureComponent2D.h" 
#include <ImageUtils.h>
#include <Camera/CameraComponent.h>
#include <IImageWrapper.h>

// Sets default values
AOneCameraStreamer::AOneCameraStreamer()
{
    // Setting this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    // Setting the server socket and TCP listener to null
    ServerSocket = nullptr;
    TCPListener = nullptr;

    // Setting the camera positions and rotations
    CameraPositions = {
        FVector(0, 0, 1000),
        FVector(3000, 0, 1000)
    };

    CameraRotations = {
        FRotator(-45, 45, 0),
        FRotator(-45, 135, 0)
    };
}

// Called when the game starts or when spawned
void AOneCameraStreamer::BeginPlay()
{
	Super::BeginPlay();

    // Starting the TCP server
    StartTCPServer();

    AttachRenderTarget();
}

void AOneCameraStreamer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Stopping the TCP server
    StopTCPServer();

    Super::EndPlay(EndPlayReason);
}

// Called every frame
void AOneCameraStreamer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (ServerSocket)
    {
        PollSocket();
    }

    if (streaming)
    {
        /*for (int i = 0; i < 3; i++)
        {
            SendRenderRequest();
        }*/
        SendRenderRequest();


        SceneCaptureComponent->SetWorldLocation(CameraPositions[lastIndex]);
        SceneCaptureComponent->SetWorldRotation(CameraRotations[lastIndex]);
        lastIndex = ++lastIndex % CameraPositions.Num();
    }




    while (!RenderRequestQueue.IsEmpty()) {

        // Peek the next RenderRequest from queue
        FRenderRequest* nextRenderRequest = nullptr;
        RenderRequestQueue.Peek(nextRenderRequest);

        if (nextRenderRequest) { //nullptr check

            if (nextRenderRequest->RenderFence.IsFenceComplete()) { 

                if (ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket)
                {
                    return;
                }

                RunAsyncSendData(ServerSocket, nextRenderRequest->Image, nextRenderRequest->Name);

                // Delete the first element from RenderQueue
                RenderRequestQueue.Pop();
                delete nextRenderRequest;
            }
            else
            {
                break;
            }
        }
    }
}


bool AOneCameraStreamer::StartTCPServer()
{
    // Creating an IP address
    FIPv4Address IPAddress = FIPv4Address(127, 0, 0, 1);
    // Creating an endpoint 
    FIPv4Endpoint Endpoint = FIPv4Endpoint(IPAddress, 7777);

    // Creating a socket listener for the connection
    TCPListener = new FTcpListener(Endpoint);
    if (TCPListener)
    {
        // Binding the connection accepted event
        TCPListener->OnConnectionAccepted().BindUObject(this, &AOneCameraStreamer::HandleConnection);
        return true;
    }
    return false;
}

void AOneCameraStreamer::StopTCPServer()
{
    // Closing the TCP listener and server socket
    if (TCPListener)
    {
        delete TCPListener;
        TCPListener = nullptr;
    }
    if (ServerSocket)
    {
        ServerSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);
    }
}

// Handling incoming connections
bool AOneCameraStreamer::HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint)
{
    // Showing a debug message when a client connects
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Client Connected: %s"), *Endpoint.ToString()));

    // Sending a message to the client
    FString Message = "Hello from Unreal Server!";
    FTCHARToUTF8 Convert(*Message);
    int32 BytesSent;

    ServerSocket = Socket;
    ServerSocket->Send((uint8*)Convert.Get(), Convert.Length(), BytesSent);
    ServerSocket->SetNonBlocking(true);

    return true;
}

// Polling the connected socket for new data
void AOneCameraStreamer::PollSocket()
{
    if (!ServerSocket)
    {
        return;
    }

    // Checking if there is any pending data
    uint32 PendingDataSize = 0;
    while (ServerSocket->HasPendingData(PendingDataSize))
    {
        // Receiving the data
        TArray<uint8> ReceivedData;
        ReceivedData.SetNumUninitialized(PendingDataSize);
        ReceivedData.Add(0);
        int32 BytesRead = 0;
        if (ServerSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead))
        {
            // Converting the received data to a string
            FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
            // Showing the received data in a debug message
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Received: %s"), *ReceivedString));
            // Handling the received request
            HandleRequest(ReceivedString);
        }
    }
}

// Handling the received request
void AOneCameraStreamer::HandleRequest(FString Request)
{
    // Sending all the cameras' viewpoints as images to the client
    if (Request == "images")
    {
        streaming = true;
    }
}

void AOneCameraStreamer::AttachRenderTarget()
{
    UWorld* World = GetWorld();
    TArray<AActor*> AllCameras = TArray<AActor*>();
    UGameplayStatics::GetAllActorsOfClass(World, ASceneCapture2D::StaticClass(), AllCameras);
    for (AActor* Actor : AllCameras)
    {
        //USceneCaptureComponent2D* SceneCaptureComponent = Cast<ASceneCapture2D>(Actor)->GetCaptureComponent2D();
        SceneCaptureComponent = Cast<ASceneCapture2D>(Actor)->GetCaptureComponent2D();
        UE_LOG(LogTemp, Warning, TEXT("SceneCaptureComponent: %s"), *SceneCaptureComponent->GetName());


        // Creating a render target
        RenderTarget = NewObject<UTextureRenderTarget2D>();
        RenderTarget->InitAutoFormat(1000, 1000);
        RenderTarget->TargetGamma = 1.4f;
        //RenderTarget->bGPUSharedFlag = true; // Makes it a bit slower i think




        // Assigning the render target to the scene capture component
        SceneCaptureComponent->TextureTarget = RenderTarget;

        // Additional settings
        // Image too dark without it
        SceneCaptureComponent->CaptureSource = SCS_FinalColorLDR;
        //SceneCaptureComponent->FOVAngle = 90.0; does nth
        //SceneCaptureComponent->ShowFlags.SetTemporalAA(true); // sounds good but didnt see difference
        //SceneCaptureComponent->DetailMode = DM_Epic; does nth


    }
}


void AOneCameraStreamer::SendRenderRequest()
{
    FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

    struct FReadSurfaceContext {
        FRenderTarget* SrcRenderTarget;
        TArray<FColor>* OutData;
        FIntRect Rect;
        FReadSurfaceDataFlags Flags;
    };


    FRenderRequest* renderRequest = new FRenderRequest();
    renderRequest->Name = RenderTarget->GetName();

    FReadSurfaceContext readSurfaceContext = {
    renderTargetResource,
    &(renderRequest->Image),
    FIntRect(0,0,renderTargetResource->GetSizeXY().X, renderTargetResource->GetSizeXY().Y),
    FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
    };

    ENQUEUE_RENDER_COMMAND(SceneDrawCompletion)(
        [readSurfaceContext](FRHICommandListImmediate& RHICmdList) {
            RHICmdList.ReadSurfaceData(
                readSurfaceContext.SrcRenderTarget->GetRenderTargetTexture(),
                readSurfaceContext.Rect,
                *readSurfaceContext.OutData,
                readSurfaceContext.Flags
            );
        });

    RenderRequestQueue.Enqueue(renderRequest);


    renderRequest->RenderFence.BeginFence();
}





void AOneCameraStreamer::RunAsyncSendData(FSocket* Socket, TArray<FColor> Bmp, FString CameraNameTag) {
    //UE_LOG(LogTemp, Warning, TEXT("Running Async Task"));
    (new FAutoDeleteAsyncTask<AsyncSendDataClass>(Socket, Bmp, CameraNameTag))->StartBackgroundTask();
}



/*
*******************************************************************
*/

AsyncSendDataClass::AsyncSendDataClass(FSocket* Socket, TArray<FColor> Bmp, FString CameraNameTagg) {
    ServerSocket = Socket;
    Bitmap = Bmp;
    CameraNameTag = CameraNameTagg;
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskCreated"));
}

AsyncSendDataClass::~AsyncSendDataClass() {
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskDone"));
}




void AsyncSendDataClass::DoWork() {
    TMap<FString, TArray<uint8>> CameraImagessssss;

    if (ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket) {
        return;
    }

    TArray<uint8> ImgData;
    FImageUtils::ThumbnailCompressImageArray(1000, 1000, Bitmap, ImgData);

    CameraImagessssss.Add(CameraNameTag, ImgData);



    // Serializing the map into a byte array.
    TArray<uint8> SendBuffer;
    FMemoryWriter Writer(SendBuffer, true);

    // Writing the number of entries.
    int32 NumEntries = CameraImagessssss.Num();
    Writer << NumEntries;


    // Iterate over each entry.
    for (TPair<FString, TArray<uint8>> Pair : CameraImagessssss)
    {
        // Serializing the key (camera name).
        FString CameraName = Pair.Key;
        FTCHARToUTF8 Converter(*CameraName);
        int32 KeyLength = Converter.Length();
        Writer << CameraName;

        // Serializing the image data.
        TArray<uint8> ImageData = Pair.Value;
        int32 ImageSize = ImageData.Num();
        Writer << ImageSize;

        // Writing the raw image bytes.
        if (ImageSize > 0)
        {
            Writer.Serialize((void*)ImageData.GetData(), ImageSize);
        }
    }


    // Sending the serialized data over the socket.
    int32 TotalSent = 0;
    int32 BufferSize = SendBuffer.Num();
    while (TotalSent < BufferSize)
    {
        int32 BytesSent = 0;
        if (!ServerSocket->Send(SendBuffer.GetData() + TotalSent, BufferSize - TotalSent, BytesSent))
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to send camera images data"));
            return;
        }
        TotalSent += BytesSent;
    }
}

