#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"
#include "ProceduralMeshComponent.h"

class RHINO_API TCPClient
{
public:
    TCPClient();
    ~TCPClient();

    // Connects to the server at a specified IP address and port
    bool ConnectToServer(const FString& ServerAddress, int32 Port);

    // Disconnects from the server
    void Disconnect();

    // Sends a string message to the server
    bool SendData(const FString& Message);

    // Periodically check for incoming messages
    void PollForMessages();

    // Parses JSON data and creates the mesh from it
    void ParseAndCreateMesh(const FString& JsonString);

    // Helper method to create the mesh from vertices and triangles
    void CreateMesh(const FString& MeshID, const TArray<FVector>& Vertices, const TArray<int32>& Triangles);

    // Pointer to ProceduralMeshComponent for mesh creation
    UProceduralMeshComponent* ProceduralMesh; // Make this public or add a setter method

private:
    // Pointer to the socket used for connection
    FSocket* ClientSocket;

    // Helper function to initialize and configure the socket
    FSocket* CreateSocket();

    // Accumulates received data until a complete JSON message is available
    FString DataBuffer; 

    // Track how many meshes we've created (can be removed later if using IDs exclusively)
    int32 MeshesCreated = 0;

    // Add this line to track spawned mesh actors by ID
    TMap<FString, AActor*> MeshActorMap;

};
