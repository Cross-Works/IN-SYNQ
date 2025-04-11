#include "Rhino.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ProceduralMeshComponent.h"
#include "Misc/CommandLine.h"
#include "Engine/Engine.h"

void FRhinoModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Rhino Plugin has started!"));

    // Initialize the TCP client
    MyTCPClient = new TCPClient();

    // Try connecting to the server automatically on startup
    if (MyTCPClient->ConnectToServer(TEXT("127.0.0.1"), 8080))
    {
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Connected to the server successfully."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] Failed to connect to the server."));
    }

    // Register console command for InitializeMesh
    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("InitializeMesh"),
        TEXT("Initializes the procedural mesh component"),
        FConsoleCommandDelegate::CreateRaw(this, &FRhinoModule::InitializeMesh),
        ECVF_Default
    );

    IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("StopMeshSync"),
        TEXT("Stops the TCP mesh synchronization"),
        FConsoleCommandDelegate::CreateRaw(this, &FRhinoModule::StopMeshSync),
        ECVF_Default
    );

    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Console command 'StopMeshSync' registered."));


    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Console command 'InitializeMesh' registered."));

}

void FRhinoModule::InitializeMesh()
{
    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Initializing ProceduralMeshComponent."));

    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] No valid world found."));
        return;
    }

    // Create a new actor to attach the mesh to
    AActor* MeshActor = World->SpawnActor<AActor>();
    if (!MeshActor)
    {
        UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] Failed to create actor for mesh."));
        return;
    }

    // Create and attach the ProceduralMeshComponent to the actor
    ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(MeshActor);
    if (ProceduralMeshComponent)
    {
        ProceduralMeshComponent->RegisterComponent();
        ProceduralMeshComponent->AttachToComponent(MeshActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        MeshActor->SetActorLocation(FVector(0, 0, 0));  // Set the actor's location if necessary

        // Reconnect the client if it's null or disconnected
        if (!MyTCPClient)
        {
            MyTCPClient = new TCPClient();
            MyTCPClient->ConnectToServer(TEXT("127.0.0.1"), 8080);
            UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] TCP client re-initialized and reconnected."));
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] TCP client already exists, reusing it."));
        }

        // Assign the mesh component to the TCP client
        MyTCPClient->ProceduralMesh = ProceduralMeshComponent;

        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] ProceduralMeshComponent initialized, attached to actor, and assigned to TCP client."));

        // Set up polling for incoming mesh data from the server
        World->GetTimerManager().SetTimer(PollingTimerHandle, [this]()
            {
                PollForMessages();
            }, 1.0f, true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RhinoPlugin] Failed to initialize ProceduralMeshComponent."));
    }
}

void FRhinoModule::StopMeshSync()
{
    UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Stopping mesh sync."));

    if (MyTCPClient)
    {
        delete MyTCPClient;
        MyTCPClient = nullptr;
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] TCP client disconnected."));
    }

    // Clear polling timer
    if (GEngine)
    {
        UWorld* World = GEngine->GetWorldContexts()[0].World();
        if (World)
        {
            World->GetTimerManager().ClearTimer(PollingTimerHandle);
            UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] Polling timer cleared."));
        }
    }

    // Optionally destroy ProceduralMeshComponent
    if (ProceduralMeshComponent)
    {
        ProceduralMeshComponent->DestroyComponent();
        ProceduralMeshComponent = nullptr;
        UE_LOG(LogTemp, Log, TEXT("[RhinoPlugin] ProceduralMeshComponent destroyed."));
    }
}



void FRhinoModule::PollForMessages()
{
    if (MyTCPClient)
    {
        MyTCPClient->PollForMessages();
    }
}

void FRhinoModule::ShutdownModule()
{
    StopMeshSync(); // Safe cleanup even if user forgets
    if (MyTCPClient)
    {
        MyTCPClient->Disconnect();
        delete MyTCPClient;
        MyTCPClient = nullptr;
    }

    if (ProceduralMeshComponent)
    {
        ProceduralMeshComponent->DestroyComponent();
        ProceduralMeshComponent = nullptr;
    }
}

IMPLEMENT_MODULE(FRhinoModule, Rhino)
