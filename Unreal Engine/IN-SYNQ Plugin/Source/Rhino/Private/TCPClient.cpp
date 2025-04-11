#include "TCPClient.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "ProceduralMeshComponent.h"
#include "Json.h"

TCPClient::TCPClient()
    : ClientSocket(nullptr)
{
}

TCPClient::~TCPClient()
{
    Disconnect();
}

FSocket* TCPClient::CreateSocket()
{
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    return SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TCPClientSocket"), false);
}

bool TCPClient::ConnectToServer(const FString& ServerAddress, int32 Port)
{
    if (ClientSocket)
    {
        Disconnect();
    }

    ClientSocket = CreateSocket();
    if (!ClientSocket)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create socket."));
        return false;
    }

    TSharedPtr<FInternetAddr> ServerAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    ServerAddr->SetIp(*ServerAddress, bIsValid);
    ServerAddr->SetPort(Port);

    if (!bIsValid)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid IP address."));
        return false;
    }

    bool bConnected = ClientSocket->Connect(*ServerAddr);
    if (bConnected)
    {
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Successfully connected to server %s:%d"), *ServerAddress, Port);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] Failed to connect to server %s:%d"), *ServerAddress, Port);
    }
    return bConnected;
}

void TCPClient::Disconnect()
{
    if (ClientSocket)
    {
        ClientSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
        ClientSocket = nullptr;
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Disconnected from server."));
    }
}

bool TCPClient::SendData(const FString& Message)
{
    if (!ClientSocket) return false;

    int32 BytesSent = 0;
    FTCHARToUTF8 Converter(*Message);
    bool bSuccess = ClientSocket->Send(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length(), BytesSent);

    if (bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Sent %d bytes to server."), BytesSent);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] Failed to send data."));
    }

    return bSuccess;
}

void TCPClient::PollForMessages()
{
    if (!ClientSocket) return;

    uint32 PendingDataSize = 0;
    if (ClientSocket->HasPendingData(PendingDataSize))
    {
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Pending data size: %u"), PendingDataSize);

        TArray<uint8> ReceivedData;
        ReceivedData.SetNumUninitialized(FMath::Min(PendingDataSize, 65507u));

        int32 BytesRead = 0;
        ClientSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), BytesRead);

        // Append received data to DataBuffer
        FString ReceivedString = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceivedData.GetData())));
        DataBuffer += ReceivedString;
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Current DataBuffer content: %s"), *DataBuffer);

        // Check for the presence of a JSON payload
        int32 JsonStart = DataBuffer.Find("{");
        int32 JsonEnd = DataBuffer.Find("}", ESearchCase::IgnoreCase, ESearchDir::FromEnd);

        if (JsonStart != INDEX_NONE && JsonEnd != INDEX_NONE && JsonStart < JsonEnd)
        {
            FString JsonString = DataBuffer.Mid(JsonStart, JsonEnd - JsonStart + 1);
            DataBuffer = DataBuffer.RightChop(JsonEnd + 1);

            ParseAndCreateMesh(JsonString);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Incomplete JSON received, awaiting more data..."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] No pending data."));
    }
}


void TCPClient::ParseAndCreateMesh(const FString& JsonString)
{
    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Attempting to parse JSON: %s"), *JsonString);

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] JSON deserialization successful."));

        // Check for array of meshes
        if (!JsonObject->HasField("meshes"))
        {
            UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] 'meshes' field not found in JSON."));
            return;
        }

        const TArray<TSharedPtr<FJsonValue>> MeshesArray = JsonObject->GetArrayField("meshes");
        int32 MeshIndex = 0;

        for (const TSharedPtr<FJsonValue>& MeshValue : MeshesArray)
        {
            TSharedPtr<FJsonObject> MeshObject = MeshValue->AsObject();
            if (!MeshObject.IsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] Invalid mesh object at index %d"), MeshIndex);
                continue;
            }

            TArray<FVector> Vertices;
            TArray<int32> Triangles;

            // Parse vertices
            const TArray<TSharedPtr<FJsonValue>> JsonVertices = MeshObject->GetArrayField(TEXT("vertices"));
            for (const TSharedPtr<FJsonValue>& VertexValue : JsonVertices)
            {
                const TArray<TSharedPtr<FJsonValue>> VertexArray = VertexValue->AsArray();
                if (VertexArray.Num() == 3)
                {
                    FVector Vertex(VertexArray[0]->AsNumber(), VertexArray[1]->AsNumber(), VertexArray[2]->AsNumber());
                    Vertices.Add(Vertex);
                }
            }

            // Parse faces
            const TArray<TSharedPtr<FJsonValue>> JsonFaces = MeshObject->GetArrayField(TEXT("faces"));
            for (const TSharedPtr<FJsonValue>& FaceValue : JsonFaces)
            {
                const TArray<TSharedPtr<FJsonValue>> FaceArray = FaceValue->AsArray();
                if (FaceArray.Num() >= 3)
                {
                    Triangles.Add(FaceArray[0]->AsNumber());
                    Triangles.Add(FaceArray[1]->AsNumber());
                    Triangles.Add(FaceArray[2]->AsNumber());

                    if (FaceArray.Num() == 4) // Convert quad to 2 triangles
                    {
                        Triangles.Add(FaceArray[0]->AsNumber());
                        Triangles.Add(FaceArray[2]->AsNumber());
                        Triangles.Add(FaceArray[3]->AsNumber());
                    }
                }
            }

            if (Vertices.Num() > 0 && Triangles.Num() > 0)
            {
                FString MeshID = MeshObject->GetStringField(TEXT("id")); 
                CreateMesh(MeshID, Vertices, Triangles); 
                UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Mesh %d (%s) created or updated."), MeshIndex, *MeshID);
            }

            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] Mesh %d has invalid geometry."), MeshIndex);
            }

            MeshIndex++;
        }

        // Clear buffer after success
        DataBuffer.Empty();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] Failed to parse JSON mesh data."));
    }
}



void TCPClient::CreateMesh(const FString& MeshID, const TArray<FVector>& Vertices, const TArray<int32>& Triangles)
{
    if (!GEngine) return;

    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (!World) return;

    AActor* MeshActor = nullptr;

    // Check if this mesh already exists
    if (MeshActorMap.Contains(MeshID))
    {
        MeshActor = MeshActorMap[MeshID];
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Updating existing mesh actor: %s"), *MeshID);
    }
    else
    {
        // Create a new Actor
        MeshActor = World->SpawnActor<AActor>(AActor::StaticClass());
        MeshActorMap.Add(MeshID, MeshActor);
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Created new mesh actor: %s"), *MeshID);
    }

    //Check for existing procedural mesh component or create new
    UProceduralMeshComponent* MeshComp = nullptr;
    TArray<UProceduralMeshComponent*> Components;
    MeshActor->GetComponents<UProceduralMeshComponent>(Components);
    if (Components.Num() > 0)
    {
        MeshComp = Cast<UProceduralMeshComponent>(Components[0]);
    }
    else
    {
        MeshComp = NewObject<UProceduralMeshComponent>(MeshActor);
        MeshComp->RegisterComponent();
        MeshActor->AddInstanceComponent(MeshComp);
    }

    if (!MeshComp) return;

    MeshComp->ClearAllMeshSections();
    MeshComp->CreateMeshSection_LinearColor(
        0,
        Vertices,
        Triangles,
        TArray<FVector>(),        
        TArray<FVector2D>(),       
        TArray<FLinearColor>(),    
        TArray<FProcMeshTangent>(),
        true                       
    );

    // Positioning offset (optional: based on hash to spread things out)
    uint32 Hash = GetTypeHash(MeshID);
    float OffsetX = (Hash % 500) - 250;
    float OffsetY = ((Hash / 1000) % 500) - 250;
    MeshActor->SetActorLocation(FVector(OffsetX, OffsetY, 0));
}


