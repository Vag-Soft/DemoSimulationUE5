#include "TCPConnection.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/SceneCapture2D.h" 
#include "Components/SceneCaptureComponent2D.h" 
#include <ImageUtils.h>
#include <Camera/CameraComponent.h>

ATCPConnection::ATCPConnection()
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

void ATCPConnection::BeginPlay()
{
    Super::BeginPlay();

    // Starting the TCP server
    StartTCPServer();

    AttachRenderTargets();
}

void ATCPConnection::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Stopping the TCP server
    StopTCPServer();

    Super::EndPlay(EndPlayReason);
}

void ATCPConnection::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Polling the connected socket for new data
    if (ServerSocket)
    {
        PollSocket();
    }


    SceneCaptureComponent->SetWorldLocation(CameraPositions[lastIndex]);
    SceneCaptureComponent->SetWorldRotation(CameraRotations[lastIndex]);
    lastIndex = ++lastIndex % CameraPositions.Num();

    //if (streaming)
    //{
    //    TMap<FString, TArray<uint8>> CameraImages = TMap<FString, TArray<uint8>>();

    //    for (UTextureRenderTarget2D* RenderTarget : RenderTargets)
    //    {
    //        FTextureRenderTargetResource* RenderResource = RenderTarget->GameThread_GetRenderTargetResource();

    //        TArray<FColor> Bitmap;
    //        //SceneCapture->CaptureScene();
    //        RenderResource->ReadPixels(Bitmap);

    //        // Converting the image to PNG
    //        TArray<uint8> PNGData;
    //        FImageUtils::ThumbnailCompressImageArray(RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight(), Bitmap, PNGData);


    //        CameraImages.Add(RenderTarget->GetName(), PNGData);
    //    }

    //    // Serializing the map into a byte array.
    //    TArray<uint8> SendBuffer;
    //    FMemoryWriter Writer(SendBuffer, true);

    //    // Writing the number of entries.
    //    int32 NumEntries = CameraImages.Num();
    //    Writer << NumEntries;


    //    // Iterate over each entry.
    //    for (TPair<FString, TArray<uint8>> Pair : CameraImages)
    //    {
    //        // Serializing the key (camera name).
    //        FString CameraName = Pair.Key;
    //        FTCHARToUTF8 Converter(*CameraName);
    //        int32 KeyLength = Converter.Length();
    //        Writer << CameraName;

    //        // Serializing the image data.
    //        TArray<uint8> ImageData = Pair.Value;
    //        int32 ImageSize = ImageData.Num();
    //        Writer << ImageSize;

    //        // Writing the raw image bytes.
    //        if (ImageSize > 0)
    //        {
    //            Writer.Serialize((void*)ImageData.GetData(), ImageSize);
    //        }


    //    }


    //    // Sending the serialized data over the socket.
    //    int32 TotalSent = 0;
    //    int32 BufferSize = SendBuffer.Num();
    //    while (TotalSent < BufferSize)
    //    {
    //        int32 BytesSent = 0;
    //        if (!ServerSocket->Send(SendBuffer.GetData() + TotalSent, BufferSize - TotalSent, BytesSent))
    //        {
    //            UE_LOG(LogTemp, Error, TEXT("Failed to send camera images data"));
    //        }
    //        TotalSent += BytesSent;
    //    }
    //}
}


bool ATCPConnection::StartTCPServer()
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
        TCPListener->OnConnectionAccepted().BindUObject(this, &ATCPConnection::HandleConnection);
        return true;
    }
    return false;
}

void ATCPConnection::StopTCPServer()
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
bool ATCPConnection::HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint)
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
void ATCPConnection::PollSocket()
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
void ATCPConnection::HandleRequest(FString Request)
{
    // Sending all the cameras' viewpoints as images to the client
    if (Request == "images")
    {
        //streaming = true;

        TMap<FString, TArray<uint8>> CameraImages = TMap<FString, TArray<uint8>>();

        for (UTextureRenderTarget2D* RenderTarget : RenderTargets)
        {
            FTextureRenderTargetResource* RenderResource = RenderTarget->GameThread_GetRenderTargetResource();

            TArray<FColor> Bitmap;
            //SceneCapture->CaptureScene();
            RenderResource->ReadPixels(Bitmap);

            // Converting the image to PNG
            TArray<uint8> PNGData;
            FImageUtils::ThumbnailCompressImageArray(RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight(), Bitmap, PNGData);


            CameraImages.Add(RenderTarget->GetName(), PNGData);
        }

        // Serializing the map into a byte array.
        TArray<uint8> SendBuffer;
        FMemoryWriter Writer(SendBuffer, true);

        // Writing the number of entries.
        int32 NumEntries = CameraImages.Num();
        Writer << NumEntries;


        // Iterate over each entry.
        for (TPair<FString, TArray<uint8>> Pair : CameraImages)
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
            }
            TotalSent += BytesSent;
        }


    }
}


void ATCPConnection::AttachRenderTargets()
{

    UWorld* World = GetWorld();
    TArray<AActor*> AllCameras = TArray<AActor*>();
    UGameplayStatics::GetAllActorsOfClass(World, ASceneCapture2D::StaticClass(), AllCameras);
    for (AActor* Actor : AllCameras)
    {
        //USceneCaptureComponent2D* SceneCaptureComponent = Cast<ASceneCapture2D>(Actor)->GetCaptureComponent2D();
        SceneCaptureComponent = Cast<ASceneCapture2D>(Actor)->GetCaptureComponent2D();
        UE_LOG(LogTemp, Log, TEXT("SceneCaptureComponent: %s"), *SceneCaptureComponent->GetName());


        // Creating a render target
        UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
        RenderTarget->InitAutoFormat(500, 500);
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