#include "TCPClient.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "ProceduralMeshComponent.h"
#include "Json.h"

TCPClient::TCPClient()
    : ClientSocket(nullptr), ProceduralMesh(nullptr)
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
            DataBuffer = DataBuffer.RightChop(JsonEnd + 1); // Remove parsed JSON from buffer

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

        TArray<FVector> Vertices;
        TArray<int32> Triangles;

        // Parse vertices array
        if (JsonObject->HasField("vertices"))
        {
            const TArray<TSharedPtr<FJsonValue>> JsonVertices = JsonObject->GetArrayField(TEXT("vertices"));
            for (const TSharedPtr<FJsonValue>& Value : JsonVertices)
            {
                const TArray<TSharedPtr<FJsonValue>> VertexArray = Value->AsArray();
                if (VertexArray.Num() == 3)
                {
                    FVector Vertex(VertexArray[0]->AsNumber(), VertexArray[1]->AsNumber(), VertexArray[2]->AsNumber());
                    Vertices.Add(Vertex);
                    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Parsed vertex: %s"), *Vertex.ToString());
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] Invalid vertex array size: %d"), static_cast<int32>(VertexArray.Num()));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] 'vertices' field not found in JSON."));
        }

        // Parse faces array (triangles or quads)
        if (JsonObject->HasField("faces"))
        {
            const TArray<TSharedPtr<FJsonValue>> JsonFaces = JsonObject->GetArrayField(TEXT("faces"));
            for (const TSharedPtr<FJsonValue>& Value : JsonFaces)
            {
                const TArray<TSharedPtr<FJsonValue>> FaceArray = Value->AsArray();
                if (FaceArray.Num() >= 3)
                {
                    Triangles.Add(FaceArray[0]->AsNumber());
                    Triangles.Add(FaceArray[1]->AsNumber());
                    Triangles.Add(FaceArray[2]->AsNumber());

                    if (FaceArray.Num() == 4) // Handle quad as two triangles
                    {
                        Triangles.Add(FaceArray[0]->AsNumber());
                        Triangles.Add(FaceArray[2]->AsNumber());
                        Triangles.Add(FaceArray[3]->AsNumber());
                    }

                    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Parsed face with vertices: %d, %d, %d"),
                        static_cast<int32>(FaceArray[0]->AsNumber()),
                        static_cast<int32>(FaceArray[1]->AsNumber()),
                        static_cast<int32>(FaceArray[2]->AsNumber()));

                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] Invalid face array size: %d"), static_cast<int32>(FaceArray.Num()));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] 'faces' field not found in JSON."));
        }

        // Create the mesh if data is valid
        if (Vertices.Num() > 0 && Triangles.Num() > 0)
        {
            CreateMesh(Vertices, Triangles);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] Insufficient data to create mesh: %d vertices, %d triangles."),
                static_cast<int32>(Vertices.Num()), static_cast<int32>(Triangles.Num() / 3));

        }

        // Clear the buffer after successful parsing
        DataBuffer.Empty();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[RhinoPlugin] Failed to parse JSON mesh data. Retaining data in buffer for next call."));
    }
}


void TCPClient::CreateMesh(const TArray<FVector>& Vertices, const TArray<int32>& Triangles)
{
    if (!ProceduralMesh) // Check if ProceduralMeshComponent is assigned
    {
        UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] ProceduralMeshComponent is not assigned!"));
        return;
    }

    // Clear any existing mesh data
    ProceduralMesh->ClearAllMeshSections();

    // Create a new mesh section
    ProceduralMesh->CreateMeshSection_LinearColor(
        0,           // Mesh section index
        Vertices,    // Vertices array
        Triangles,   // Triangles array
        TArray<FVector>(),        // Normals (empty array will auto-calculate)
        TArray<FVector2D>(),      // UVs (empty for now)
        TArray<FLinearColor>(),   // Vertex colors
        TArray<FProcMeshTangent>(), // Tangents
        true         // Enable collision
    );

    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Mesh created with %d vertices and %d triangles."), Vertices.Num(), Triangles.Num() / 3);
}
