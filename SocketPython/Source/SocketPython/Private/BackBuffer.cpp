// Fill out your copyright notice in the Description page of Project Settings.


#include "BackBuffer.h"
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
ABackBuffer::ABackBuffer()
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

	MeshComps = TArray<USkeletalMeshComponent*>();
}

// Called when the game starts or when spawned
void ABackBuffer::BeginPlay()
{
    Super::BeginPlay();
    

    // Starting the TCP server
    StartTCPServer();
    // Storing the viewport
    AttachViewport();
	// Storing the mesh components from all characters
    AttachMeshComps();
}

void ABackBuffer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Stopping the TCP server
    StopTCPServer();

    Super::EndPlay(EndPlayReason);
}

// Called every frame
void ABackBuffer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	// Polling the socket for new data
    if (ServerSocket)
    {
        PollSocket();
    }

	// Making render requests and moving the camera
    if (streaming)
    {
        SendRenderRequest();

        FollowCamera->SetWorldLocation(CameraPositions[lastIndex]);
        FollowCamera->SetWorldRotation(CameraRotations[lastIndex]);
        lastIndex = ++lastIndex % CameraPositions.Num();

    }
    
    // Sending the completed render requests to python
    while (!RenderRequestQueue.IsEmpty()) {

        FRenderRequest1* nextRenderRequest = nullptr;
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


bool ABackBuffer::StartTCPServer()
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
        TCPListener->OnConnectionAccepted().BindUObject(this, &ABackBuffer::HandleConnection);
        return true;
    }
    return false;
}

void ABackBuffer::StopTCPServer()
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
bool ABackBuffer::HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint)
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
void ABackBuffer::PollSocket()
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
void ABackBuffer::HandleRequest(FString Request)
{
    if (Request == "images")
    {
        streaming = true;
    }
}

void ABackBuffer::AttachViewport()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Get the Game Viewport
    Viewport = GEngine->GameViewport->Viewport;
    if (!Viewport) return;

	Width = Viewport->GetSizeXY().X;
	Height = Viewport->GetSizeXY().Y;

    // Get the main camera
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
    if (PlayerCharacter)
    {
        FollowCamera = PlayerCharacter->FindComponentByClass<UCameraComponent>();

        if (FollowCamera)
        {
            UE_LOG(LogTemp, Warning, TEXT("FollowCamera Found: %s"), *FollowCamera->GetName());

            FollowCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            UE_LOG(LogTemp, Warning, TEXT("FollowCamera Detached"));
        }

    }
}

void ABackBuffer::AttachMeshComps()
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


void ABackBuffer::SendRenderRequest()
{
    struct FReadSurfaceContext {
        FRenderTarget* SrcRenderTarget;
        TArray<FColor>* OutData;
        FIntRect Rect;
        FReadSurfaceDataFlags Flags;
    };


    // Creating a new render request
    FRenderRequest1* renderRequest = new FRenderRequest1();
    renderRequest->cameraIndex = lastIndex;

    FReadSurfaceContext readSurfaceContext = {
    Viewport,
    &(renderRequest->Image),
    FIntRect(0,0,Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y),
    FReadSurfaceDataFlags(RCM_UNorm, CubeFace_MAX)
    };

    // Sending to the render thread
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
TArray<uint8> ConvertTransformToBytes(const FTransform& Transform, int32 BoneIndex)
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


// Running the async task to send the data
void ABackBuffer::RunAsyncSendData(FSocket* Socket, TArray<FColor> Bmp, int32 CameraIDD, int32 Width1, int32 Height1, TArray<USkeletalMeshComponent*> MeshCompss) {
    //UE_LOG(LogTemp, Warning, TEXT("Running Async Task"));
    (new FAutoDeleteAsyncTask<AsyncSendDataClass1>(Socket, Bmp, CameraIDD, Width1, Height1, MeshCompss))->StartBackgroundTask();
}



/*
*******************************************************************
*/

AsyncSendDataClass1::AsyncSendDataClass1(FSocket* Socket, TArray<FColor> Bmp, int32 CameraIDD, int32 Width1, int32 Height1, TArray<USkeletalMeshComponent*> MeshCompss) {
    ServerSocket = Socket;
    Bitmap = Bmp;
    CameraID = CameraIDD;
	Width = Width1;
	Height = Height1;
	MeshComps = MeshCompss;
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskCreated"));
}

AsyncSendDataClass1::~AsyncSendDataClass1() {
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskDone"));
}




void AsyncSendDataClass1::DoWork() {
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
            TArray<uint8> BoneBytes = ConvertTransformToBytes(BoneTransform, j);
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

