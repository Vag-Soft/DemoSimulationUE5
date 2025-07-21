// Fill out your copyright notice in the Description page of Project Settings.


#include "MultipleCameraStreamer.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/SceneCapture2D.h" 
#include "Components/SceneCaptureComponent2D.h" 
#include <ImageUtils.h>
#include <Camera/CameraComponent.h>
#include <IImageWrapper.h>
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"

// Sets default values
AMultipleCameraStreamer::AMultipleCameraStreamer()
{
    // Setting this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    // Setting the server socket and TCP listener to null
    ServerSocket = nullptr;
    TCPListener = nullptr;

	// Setting the resolution of the render targets
	Width = 500;
	Height = 500;

    MeshComps = TArray<USkeletalMeshComponent*>();

}

// Called when the game starts or when spawned
void AMultipleCameraStreamer::BeginPlay()
{
	Super::BeginPlay();


    // Starting the TCP server
    StartTCPServer();
	// Storing the render targets
    AttachRenderTarget();
	// Storing the mesh components from all characters
    AttachMeshComps();
	
}


void AMultipleCameraStreamer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Stopping the TCP server
    StopTCPServer();

    Super::EndPlay(EndPlayReason);
}

// Called every frame
void AMultipleCameraStreamer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Polling the socket for new data
    if (ServerSocket)
    {
        PollSocket();
    }

    // Making render requests
    if (streaming)
    {
        for (int i = 0; i < RenderTargets.Num(); i++)
        {
            SendRenderRequest(RenderTargets[i], i);
        }
    }



    // Sending the completed render requests to python
    while (!RenderRequestQueue.IsEmpty()) {

        FRenderRequest2* nextRenderRequest = nullptr;
        RenderRequestQueue.Peek(nextRenderRequest);

        if (nextRenderRequest) { 

            if (nextRenderRequest->RenderFence.IsFenceComplete()) {

                if (ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket)
                {
                    return;
                }

                RunAsyncSendData(ServerSocket, nextRenderRequest->Image, nextRenderRequest->cameraIndex, Width, Height, MeshComps);

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


bool AMultipleCameraStreamer::StartTCPServer()
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
        TCPListener->OnConnectionAccepted().BindUObject(this, &AMultipleCameraStreamer::HandleConnection);
        return true;
    }
    return false;
}

void AMultipleCameraStreamer::StopTCPServer()
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
bool AMultipleCameraStreamer::HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint)
{
    // Showing a debug message when a client connects
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Client Connected: %s"), *Endpoint.ToString()));

    // Sending a message to the client
    FString Message = "Hello from Unreal Server!";
    FTCHARToUTF8 Convert(*Message);
    int32 BytesSent;

    ServerSocket = Socket;
    //ServerSocket->Send((uint8*)Convert.Get(), Convert.Length(), BytesSent);
    ServerSocket->SetNonBlocking(true);

	for (int32 i = 0; i < AllCameras.Num(); i++)
	{
        int32 cameraIndex = i;
        ServerSocket->Send((uint8*)&cameraIndex, sizeof(int32), BytesSent);

        double locX = AllCameras[i]->GetActorLocation().X;
        double locY = AllCameras[i]->GetActorLocation().Y;
        double locZ = AllCameras[i]->GetActorLocation().Z;
        ServerSocket->Send((uint8*)&locX, sizeof(double), BytesSent);
        ServerSocket->Send((uint8*)&locY, sizeof(double), BytesSent);
        ServerSocket->Send((uint8*)&locZ, sizeof(double), BytesSent);

        double pitch = AllCameras[i]->GetActorRotation().Pitch;
        double yaw = AllCameras[i]->GetActorRotation().Yaw;
        double roll = AllCameras[i]->GetActorRotation().Roll;
        ServerSocket->Send((uint8*)&pitch, sizeof(double), BytesSent);
        ServerSocket->Send((uint8*)&yaw, sizeof(double), BytesSent);
        ServerSocket->Send((uint8*)&roll, sizeof(double), BytesSent);
	}

    return true;
}

// Polling the connected socket for new data
void AMultipleCameraStreamer::PollSocket()
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
void AMultipleCameraStreamer::HandleRequest(FString Request)
{
    if (Request == "images")
    {
        streaming = true;
    }
}

void AMultipleCameraStreamer::AttachRenderTarget()
{
    UWorld* World = GetWorld();
    this->AllCameras = TArray<AActor*>();
    UGameplayStatics::GetAllActorsOfClass(World, ASceneCapture2D::StaticClass(), AllCameras);
    for (AActor* Actor : AllCameras)
    {
        USceneCaptureComponent2D* SceneCaptureComponent = Cast<ASceneCapture2D>(Actor)->GetCaptureComponent2D();
        UE_LOG(LogTemp, Warning, TEXT("SceneCaptureComponent: %s"), *SceneCaptureComponent->GetName());


        // Creating a render target
        UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
        RenderTarget->InitAutoFormat(Width, Height);
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

		RenderTargets.Add(RenderTarget);
    }
}

void AMultipleCameraStreamer::AttachMeshComps()
{
    // Get all characters in the scene
    UWorld* World = GetWorld();
    TArray<AActor*> AllCharacters = TArray<AActor*>();
    UGameplayStatics::GetAllActorsOfClass(World, ACharacter::StaticClass(), AllCharacters);
    for (AActor* Actor : AllCharacters)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character: %s"), *Actor->GetName());

        MeshComps.Add(Cast<USkeletalMeshComponent>(Actor->GetComponentByClass(USkeletalMeshComponent::StaticClass())));
    }
}




void AMultipleCameraStreamer::SendRenderRequest(UTextureRenderTarget2D* RenderTarget, int32 camIndex)
{
    FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

    struct FReadSurfaceContext {
        FRenderTarget* SrcRenderTarget;
        TArray<FColor>* OutData;
        FIntRect Rect;
        FReadSurfaceDataFlags Flags;
    };


    // Creating a new render request
    FRenderRequest2* renderRequest = new FRenderRequest2();
    renderRequest->cameraIndex = camIndex;

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

// Converting the transform to a byte array
TArray<uint8> ConvertTransformToBytes1(const FTransform& Transform, int32 BoneIndex)
{
    TArray<uint8> Bytes;

    Bytes.Append(reinterpret_cast<uint8*>(&BoneIndex), sizeof(int32));


    float LocationX = Transform.GetLocation().X;
    Bytes.Append(reinterpret_cast<uint8*>(&LocationX), sizeof(float));

    float LocationY = Transform.GetLocation().Y;
    Bytes.Append(reinterpret_cast<uint8*>(&LocationY), sizeof(float));

    float LocationZ = Transform.GetLocation().Z;
    Bytes.Append(reinterpret_cast<uint8*>(&LocationZ), sizeof(float));


    float RotationX = Transform.GetRotation().X;
    Bytes.Append(reinterpret_cast<uint8*>(&RotationX), sizeof(float));

    float RotationY = Transform.GetRotation().Y;
    Bytes.Append(reinterpret_cast<uint8*>(&RotationY), sizeof(float));

    float RotationZ = Transform.GetRotation().Z;
    Bytes.Append(reinterpret_cast<uint8*>(&RotationZ), sizeof(float));

    float RotationW = Transform.GetRotation().W;
    Bytes.Append(reinterpret_cast<uint8*>(&RotationW), sizeof(float));


    return Bytes;
}



void AMultipleCameraStreamer::RunAsyncSendData(FSocket* Socket, TArray<FColor> Bmp, int32 CameraIDD, int32 Width1, int32 Height1, TArray<USkeletalMeshComponent*> MeshCompss) {
    //UE_LOG(LogTemp, Warning, TEXT("Running Async Task"));
    (new FAutoDeleteAsyncTask<AsyncSendDataClass2>(Socket, Bmp, CameraIDD, Width1, Height1, MeshCompss))->StartBackgroundTask();
}



/*
*******************************************************************
*/

AsyncSendDataClass2::AsyncSendDataClass2(FSocket* Socket, TArray<FColor> Bmp, int32 CameraIDD, int32 Width1, int32 Height1, TArray<USkeletalMeshComponent*> MeshCompss) {
    ServerSocket = Socket;
    Bitmap = Bmp;
    CameraID = CameraIDD;
    Width = Width1;
    Height = Height1;
    MeshComps = MeshCompss;
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskCreated"));
}

AsyncSendDataClass2::~AsyncSendDataClass2() {
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskDone"));
}




void AsyncSendDataClass2::DoWork() {
    if (ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket) {
        return;
    }


    // Compressing the image to PNG
    TArray<uint8> ImgData;
    FImageUtils::ThumbnailCompressImageArray(Width, Height, Bitmap, ImgData);




    // Serializing the map into a byte array.
    TArray<uint8> SendBuffer;
    FMemoryWriter Writer(SendBuffer, true);

    // Writing the ID of the camera
    Writer << CameraID;


    // Writing the image size
    int32 ImageSize = ImgData.Num();
    Writer << ImageSize;

    // Writing the raw image bytes.
    if (ImageSize > 0)
    {
        Writer.Serialize((void*)ImgData.GetData(), ImageSize);
    }






    // Sending number of characters
    int32 NumMeshComps = MeshComps.Num();
    Writer << NumMeshComps;



    // Get all bone names
    TArray<FName> BoneNames;
    MeshComps[0]->GetBoneNames(BoneNames);

    // Sending the number of bones
    int32 NumBones = BoneNames.Num();
    Writer << NumBones;


    for (int i = 0; i < NumMeshComps; i++)
    {
        // Sending the index of the character
        Writer << i;


        USkeletalMeshComponent* MeshComp = MeshComps[i];
        // Prepare the byte array
        TArray<uint8> AllBoneTransformsInBytes;
        // Loop through all bones and add their transforms
        for (int32 j = 0; j < NumBones; j++)
        {


            FTransform BoneTransform = MeshComp->GetBoneTransform(BoneNames[j], ERelativeTransformSpace::RTS_World);
            TArray<uint8> BoneBytes = ConvertTransformToBytes1(BoneTransform, j);
            AllBoneTransformsInBytes.Append(BoneBytes);
            //UE_LOG(LogTemp, Log, TEXT("Bone: %s | Location: %s | Rotation: %s | %d"), *BoneNames[i].ToString(), *BoneTransform.GetLocation().ToString(), *BoneTransform.GetRotation().ToString(), i);
        }

        // Sending the bone transforms
        Writer.Serialize((void*)AllBoneTransformsInBytes.GetData(), AllBoneTransformsInBytes.Num());
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

