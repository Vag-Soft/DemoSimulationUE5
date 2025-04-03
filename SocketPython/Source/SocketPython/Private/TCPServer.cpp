#include "TCPServer.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/SceneCapture2D.h" 
#include "Components/SceneCaptureComponent2D.h" 
#include <ImageUtils.h>
#include <Camera/CameraComponent.h>
#include <IImageWrapper.h>

ATCPServer::ATCPServer()
{
	// Setting this actor to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
	// Setting the server socket and TCP listener to null
    ServerSocket = nullptr;
    TCPListener = nullptr;
}

void ATCPServer::BeginPlay()
{
    Super::BeginPlay();

	// Starting the TCP server
    StartTCPServer();

    AttachRenderTargets();
}

void ATCPServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Stopping the TCP server
    StopTCPServer();

    Super::EndPlay(EndPlayReason);
}

void ATCPServer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //FDateTime StartTime = FDateTime::UtcNow();
    //float TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMicroseconds();
    //UE_LOG(LogTemp, Display, TEXT(" % f"), TimeElapsedInMs)
    // Polling the connected socket for new data
    if (ServerSocket)
    {
        PollSocket();
    }
    
    if (streaming)
    {
        /*for (int i = 0; i < 3; i++)
        {
            CaptureNonBlocking();
        }*/
        CaptureNonBlocking();
    }




    while (!RenderRequestQueue.IsEmpty()) {

        // Peek the next RenderRequest from queue
        FRenderRequestStruct* nextRenderRequest = nullptr;
        RenderRequestQueue.Peek(nextRenderRequest);

        if (nextRenderRequest) { //nullptr check

            if (nextRenderRequest->RenderFence.IsFenceComplete()) { // Check if rendering is done, indicated by RenderFence
                // Load the image wrapper module 
                //IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));


                // Prepare data to be written to disk
                //static TSharedPtr<IImageWrapper> imageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG); //EImageFormat::PNG //EImageFormat::JPEG
                //imageWrapper->SetRaw(nextRenderRequest->Image.GetData(), nextRenderRequest->Image.GetAllocatedSize(), 500, 500, ERGBFormat::BGRA, 8);
                ///*const TArray64<uint8>& ImgData = imageWrapper->GetCompressed(5);*/
                //const TArray<uint8>& ImgData = static_cast<TArray<uint8, FDefaultAllocator>> (imageWrapper->GetCompressed(5));

                /*TArray<uint8> ImgData;
                FImageUtils::ThumbnailCompressImageArray(500, 500, nextRenderRequest->Image, ImgData);

                CameraImages.Add(nextRenderRequest->Name, ImgData);*/



                //if (CameraImages.Num() == 1 && !(ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket))
                //{
                //    //SendData();
                //    RunAsyncSendData(CameraImages, ServerSocket);
                //}

                if (ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket)
                {
                    return;
                }

                RunAsyncSendData(CameraImages, ServerSocket, nextRenderRequest->Image, nextRenderRequest->Name);

                ImgCounter += 1;

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


bool ATCPServer::StartTCPServer()
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
        TCPListener->OnConnectionAccepted().BindUObject(this, &ATCPServer::HandleConnection);
        return true;
    }
    return false;
}

void ATCPServer::StopTCPServer()
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
bool ATCPServer::HandleConnection(FSocket* Socket, const FIPv4Endpoint& Endpoint)
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
void ATCPServer::PollSocket()
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
void ATCPServer::HandleRequest(FString Request)
{
    // Sending all the cameras' viewpoints as images to the client
    if (Request == "images")
    {
        //CaptureNonBlocking();


        streaming = true;

        //TMap<FString, const TArray<uint8>> CameraImages = TMap<FString, const TArray<uint8>>();

        /*for (UTextureRenderTarget2D* RenderTarget : RenderTargets)
        {*/
            //FTextureRenderTargetResource* RenderResource = RenderTarget->GameThread_GetRenderTargetResource();

            //TArray<FColor> Bitmap;
            ////SceneCapture->CaptureScene();
            //RenderResource->ReadPixels(Bitmap);

            //// Converting the image to PNG
            //TArray<uint8> PNGData;
            //FImageUtils::ThumbnailCompressImageArray(RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight(), Bitmap, PNGData);


            //CameraImages.Add(RenderTarget->GetName(), PNGData);
        //}

        //// Serializing the map into a byte array.
        //TArray<uint8> SendBuffer;
        //FMemoryWriter Writer(SendBuffer, true);

        //// Writing the number of entries.
        //int32 NumEntries = CameraImages.Num();
        //Writer << NumEntries;


        //// Iterate over each entry.
        //for (TPair<FString, TArray<uint8>> Pair : CameraImages)
        //{
        //    // Serializing the key (camera name).
        //    FString CameraName = Pair.Key;
        //    FTCHARToUTF8 Converter(*CameraName);
        //    int32 KeyLength = Converter.Length();
        //    Writer << CameraName;

        //    // Serializing the image data.
        //    TArray<uint8> ImageData = Pair.Value;
        //    int32 ImageSize = ImageData.Num();
        //    Writer << ImageSize;

        //    // Writing the raw image bytes.
        //    if (ImageSize > 0)
        //    {
        //        Writer.Serialize((void*)ImageData.GetData(), ImageSize);
        //    }


        //}


        //// Sending the serialized data over the socket.
        //int32 TotalSent = 0;
        //int32 BufferSize = SendBuffer.Num();
        //while (TotalSent < BufferSize)
        //{
        //    int32 BytesSent = 0;
        //    if (!ServerSocket->Send(SendBuffer.GetData() + TotalSent, BufferSize - TotalSent, BytesSent))
        //    {
        //        UE_LOG(LogTemp, Error, TEXT("Failed to send camera images data"));
        //    }
        //    TotalSent += BytesSent;
        //}





















  //      TWeakObjectPtr<ATCPServer> This(this);
  //      TArray<UTextureRenderTarget2D*> RenTars = RenderTargets;

  //      TArray<float> Widths;
		//TArray<float> Heights;
		//TArray<FTextureRenderTargetResource*> RenderResources;
		//for (UTextureRenderTarget2D* RenderTarget : RenderTargets)
		//{
  //          RenderResources.Add(RenderTarget->GameThread_GetRenderTargetResource());
		//	Widths.Add(RenderTarget->GetSurfaceWidth());
		//	Heights.Add(RenderTarget->GetSurfaceHeight());
		//}

  //      ENQUEUE_RENDER_COMMAND(AsyncReadPixelsCommand)(
  //          [This, RenderResources, RenTars, Widths, Heights](FRHICommandListImmediate& RHICmdList)
  //          {
  //              TArray<TArray<FColor>> AllPixelData; // Store readback data for each target
  //              //AllPixelData.SetNum(RenderResources.Num());


  //              for (FTextureRenderTargetResource* RenderResource : RenderResources)
  //              {
  //                  TArray<FColor> PixelData;
  //                  // Define the rectangle covering the entire render target.
  //                  FIntRect Rect(0, 0, RenderResource->GetSizeX(), RenderResource->GetSizeY());
  //                  // Set up readback flags. Adjust flags as needed.
  //                  FReadSurfaceDataFlags ReadDataFlags(RCM_UNorm);
  //                  // Read the surface data from the render target texture.
  //                  RHICmdList.ReadSurfaceData(
  //                      RenderResource->GetRenderTargetTexture(),
  //                      Rect,
  //                      PixelData,
  //                      ReadDataFlags
  //                  );

  //                  AllPixelData.Add(PixelData);

  //              }



  //              for (int32 i = 0; i < AllPixelData.Num(); i++)
  //              {
		//			TArray<FColor> PixelData = AllPixelData[i];
		//			UTextureRenderTarget2D* RenderTarget = RenTars[i];
  //                  AsyncTask(ENamedThreads::GameThread, [This, PixelData, RenderTarget]()
  //                      {
		//					This->ProcessImage1(PixelData, RenderTarget);
  //                      });

  //              }













                //// Offload compression & networking to a worker thread
                //AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [This, Widths, Heights, AllPixelData = MoveTemp(AllPixelData)]() mutable
                //    {
                //        TArray<TArray<uint8>> AllCompressedData;
                //        AllCompressedData.SetNum(AllPixelData.Num());

                //        // Compress each image
                //        for (int32 i = 0; i < AllPixelData.Num(); i++)
                //        {
                //            // Converting the image to PNG
                //            TArray<uint8> PNGData;
                //            FImageUtils::ThumbnailCompressImageArray(Widths[i], Heights[i], AllPixelData[i], AllCompressedData[i]);
                //        }

                //    });

            

                //// Offload TCP sending
                //AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [This, AllCompressedData = MoveTemp(AllCompressedData)]() mutable
                //    {
                //        for (const TArray<uint8>& CompressedData : AllCompressedData)
                //        {
                //            SendCompressedDataOverTCP(CompressedData);
                //        }
                //    });




                //TArray<FColor> PixelData;
                //// Define the rectangle covering the entire render target.
                //FIntRect Rect(0, 0, RenderResource->GetSizeX(), RenderResource->GetSizeY());
                //// Set up readback flags. Adjust flags as needed.
                //FReadSurfaceDataFlags ReadDataFlags(RCM_UNorm);
                //// Read the surface data from the render target texture.
                //RHICmdList.ReadSurfaceData(
                //    RenderResource->GetRenderTargetTexture(),
                //    Rect,
                //    PixelData,
                //    ReadDataFlags
                //);

                //// Switch back to the game thread for operations that require it.
                //AsyncTask(ENamedThreads::GameThread, [This, PixelData, RenderTarget]()
                //    {
                //        // Now it's safe to call game thread-only functions.
                //        if (This.IsValid())
                //        {
                //            //                        // Converting the image to PNG
                //            //                        TArray<uint8> PNGData;
                //            //                        FImageUtils::ThumbnailCompressImageArray(RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight(), PixelData, PNGData);
                //                                    //
                //                                    //This->ProcessImage(RenderTarget->GetName(), PNGData);

                //            This->ProcessImage1(RenderTarget->GetName(), PixelData, RenderTarget);
                //        }
                //    });
            //});
    }
}


void ATCPServer::AttachRenderTargets()
{

    UWorld* World = GetWorld();
    TArray<AActor*> AllCameras = TArray<AActor*>();
    UGameplayStatics::GetAllActorsOfClass(World, ASceneCapture2D::StaticClass(), AllCameras);
    for (AActor* Actor : AllCameras)
    {
        USceneCaptureComponent2D* SceneCaptureComponent = Cast<ASceneCapture2D>(Actor)->GetCaptureComponent2D();
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

void ATCPServer::CaptureNonBlocking()
{
	for (UTextureRenderTarget2D* RenderTarget : RenderTargets)
	{
        FTextureRenderTargetResource* renderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();

        struct FReadSurfaceContext {
            FRenderTarget* SrcRenderTarget;
            TArray<FColor>* OutData;
            FIntRect Rect;
            FReadSurfaceDataFlags Flags;
        };


        FRenderRequestStruct* renderRequest = new FRenderRequestStruct();
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
}

void ATCPServer::ProcessImage(FString name, TArray<uint8> PNGData)
{
	CameraImages.Add(name, PNGData);

    if (CameraImages.Num() == RenderTargets.Num())
    {
        ATCPServer::SendData();
		CameraImages.Empty();
    }
}

void ATCPServer::ProcessImage1(TArray<FColor> PixelData, UTextureRenderTarget2D* RenderTarget )
{
    // Converting the image to PNG
    TArray<uint8> PNGData;
    FImageUtils::ThumbnailCompressImageArray(RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight(), PixelData, PNGData);

    CameraImages.Add(RenderTarget->GetName(), PNGData);

    if (CameraImages.Num() == RenderTargets.Num())
    {
        ATCPServer::SendData();
        CameraImages.Empty();
    }
}

void ATCPServer::ProcessImage2(TArray<TArray<FColor>> AllPixelData)
{
	for (int32 i = 0; i < AllPixelData.Num(); i++)
	{
		UTextureRenderTarget2D* RenderTarget = RenderTargets[i];
		TArray<FColor> PixelData = AllPixelData[i];
		// Converting the image to PNG
		TArray<uint8> PNGData;
		FImageUtils::ThumbnailCompressImageArray(RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight(), PixelData, PNGData);
        CameraImages.Add("RenderTarget->GetName()", PNGData);
	}
    ATCPServer::SendData();
}


void ATCPServer::SendData()
{
	if (CameraImages.Num() != RenderTargets.Num())
	{
		return;
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

















void ATCPServer::RunAsyncSendData(TMap<FString, TArray<uint8>> CameraImagesCopy, FSocket* Socket, TArray<FColor> Bmp, FString CameraNameTag) {
    //UE_LOG(LogTemp, Warning, TEXT("Running Async Task"));
    (new FAutoDeleteAsyncTask<AsyncSendDataTask>(CameraImagesCopy, Socket, Bmp, CameraNameTag))->StartBackgroundTask();
}



/*
*******************************************************************
*/

AsyncSendDataTask::AsyncSendDataTask(TMap<FString, TArray<uint8>> CameraImages, FSocket* Socket, TArray<FColor> Bmp, FString CameraNameTagg) {
    //CameraImagesCopy = CameraImages;
    ServerSocket = Socket;
	Bitmap = Bmp;
    CameraNameTag = CameraNameTagg;
	//UE_LOG(LogTemp, Warning, TEXT("AsyncTaskCreated"));
}

AsyncSendDataTask::~AsyncSendDataTask() {
    //UE_LOG(LogTemp, Warning, TEXT("AsyncTaskDone"));
}




void AsyncSendDataTask::DoWork() {
    TMap<FString, TArray<uint8>> CameraImagessssss;

    if (ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket) {
        return;
    }

    TArray<uint8> ImgData;
    FImageUtils::ThumbnailCompressImageArray(500, 500, Bitmap, ImgData);

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



//void AsyncSendDataTask::DoWork() {
//    
//	if (ServerSocket->GetConnectionState() != ESocketConnectionState::SCS_Connected || ServerSocket == NULL || !ServerSocket) {
//		return;
//	}
//
//
//
//    // Serializing the map into a byte array.
//    TArray<uint8> SendBuffer;
//    FMemoryWriter Writer(SendBuffer, true);
//
//    // Writing the number of entries.
//    int32 NumEntries = CameraImagesCopy.Num();
//    Writer << NumEntries;
//
//
//    // Iterate over each entry.
//    for (TPair<FString, TArray<uint8>> Pair : CameraImagesCopy)
//    {
//        // Serializing the key (camera name).
//        FString CameraName = Pair.Key;
//        FTCHARToUTF8 Converter(*CameraName);
//        int32 KeyLength = Converter.Length();
//        Writer << CameraName;
//
//        // Serializing the image data.
//        TArray<uint8> ImageData = Pair.Value;
//        int32 ImageSize = ImageData.Num();
//        Writer << ImageSize;
//
//        // Writing the raw image bytes.
//        if (ImageSize > 0)
//        {
//            Writer.Serialize((void*)ImageData.GetData(), ImageSize);
//        }
//    }
//
//
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

