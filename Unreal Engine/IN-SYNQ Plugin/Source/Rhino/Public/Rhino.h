// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "TCPClient.h"
#include "ProceduralMeshComponent.h"

class FRhinoCommands;

class FRhinoModule : public IModuleInterface
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** Polls the server for messages */
    void PollForMessages();

    /** Initializes the Procedural Mesh Component (triggered by the console command or shortcut) */
    void InitializeMesh();
    void StopMeshSync();

private:
    /** TCP client for communicating with the server */
    TCPClient* MyTCPClient;

    /** Procedural Mesh Component for dynamic mesh generation */
    UProceduralMeshComponent* ProceduralMeshComponent;

    /** Timer handle for polling server messages */
    FTimerHandle PollingTimerHandle;
};
