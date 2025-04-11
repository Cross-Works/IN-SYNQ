# Rhino to Unreal Engine Mesh Synchronization via TCP [AKA IN-SYNQ]

This repository is a **proof of concept** for creating a TCP connection between Rhino/Grasshopper and Unreal Engine. It allows the transmission of mesh data from Grasshopper to Unreal Engine, where the meshes are rendered dynamically using the **Procedural Mesh Module**.

[Watch the Demo Video](https://video.wixstatic.com/video/a00a00_64c7bd89058943f78b43cc540644d094/1080p/mp4/file.mp4)

## How It Works

- **Grasshopper Script (`IN-SYNQ_0.0.1.gh`)**: Acts as a TCP server. It takes a mesh as input and transmits the mesh data over a TCP connection.
- **Unreal Engine Plugin**: Listens for TCP requests containing mesh data and uses the **Procedural Mesh Module** to create and render the mesh in Unreal Engine.

### Current Configuration
- **IP Address**: `127.0.0.1` (localhost)
- **Port**: `8080`

These settings can be modified in both the Grasshopper script and the Unreal Engine plugin to fit your environment.

---

## Code Overview

This project consists of three main components:

### 1\. **Grasshopper Script**

The Grasshopper script acts as a TCP server. It takes a mesh as input, serializes its data (vertices and faces), and sends it over a TCP connection. The script is designed for easy integration and modification, allowing you to:

-   Add new components for preprocessing meshes (e.g., normal flipping, combining meshes).
-   Adjust the IP address and port for the TCP connection.

To edit the Grasshopper script, open it in Rhino and modify the logic or parameters as needed. Ensure you recompute the script after making changes to apply them.

### 2\. **Unreal Engine Plugin**

The Unreal Engine plugin handles the reception and rendering of mesh data. It contains the following key files:

#### - **`Rhino.h` and `Rhino.cpp`**

-   Manages the plugin lifecycle (`StartupModule` and `ShutdownModule`).
-   Contains the `InitializeMesh` function to create a procedural mesh component dynamically in the Unreal Engine world.
-   Sets up a timer to poll for incoming data from the Grasshopper TCP server.

#### - **`TCPClient.h` and `TCPClient.cpp`**

-   Handles the TCP connection and message parsing.
-   Includes methods for connecting to the server, sending data, and parsing incoming JSON messages.
-   The `CreateMesh` function converts parsed mesh data (vertices and triangles) into a procedural mesh in Unreal Engine.

#### - **Procedural Mesh Creation**

The procedural mesh component (`UProceduralMeshComponent`) is dynamically created in the Unreal Engine scene. It is attached to a new actor, which allows you to:

-   Customize the actor's location and behavior in the Unreal Engine world.
-   Add materials, collision properties, and other features to the mesh.

### 3\. **JSON Mesh Data Parsing**

Incoming mesh data is serialized as JSON with the following structure:

`{
  "vertices": [[x1, y1, z1], [x2, y2, z2], ...],
  "faces": [[v1, v2, v3], [v4, v5, v6], ...]
}`

The plugin deserializes this JSON, extracts the vertices and faces, and uses them to generate a mesh in Unreal Engine. You can expand the JSON structure to include additional attributes (e.g., normals, colors, UVs) and update the parsing logic in `TCPClient.cpp` to support these features.

---

## Installation and Usage

### Prerequisites

Ensure you have the following software installed (preferably the latest versions):
1. **Unreal Engine**
2. **Visual Studio**
3. **Rhino**
4. **Grasshopper**

### Step 1: Download Project Files

- **Grasshopper Script**: Download the script from the GitHub repository.
- **Unreal Engine Plugin**: Download the plugin files from the same repository.

### Step 2: Set Up the Unreal Engine Project

1. **Create or Use an Existing C++ Project**  
   - Create a new Unreal Engine C++ project or use an existing one.

2. **Add the In-SYNQ Plugin**  
   - Place the plugin folder into your project’s `Plugins` directory.  
   *(If the `Plugins` folder doesn’t exist, create it.)*

3. **Generate Visual Studio Files**  
   - Locate the `.uproject` file inside your project folder.  
   - **Shift+Right Click** on it, select **"Generate Visual Studio Project Files"**.  

4. **Convert Blueprint Projects (Optional)**  
   - To use C++ in a Blueprint project, generate Visual Studio project files and add at least one C++ class.  
   - [Follow this guide for details](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-the-cplusplus-class-wizard-in-unreal-engine).
   - You’ll know it’s successful if you see the `.sln` file in your project folder.

5. **Open the Project in Visual Studio**  
   - Double-click the `.sln` file to open it.  
   - Navigate to `Games/ProjectName/Plugins/` to locate the plugin files for editing.

6. **Build the Project**  
   - Click on the **Build** tab and select **Build Solution**.  
   - Ignore minor errors or warnings as long as the **Build Output** confirms the build completed successfully.

7. **Verify Plugin Installation**  
   - Open Unreal Engine and search for the "Rhino" plugin in the **Plugins Menu**.  
   - Close Unreal Engine after verifying it is present and installed.

### Step 3: Grasshoper Definition

1. Open `IN-SYNQ_0.0.1.gh` in Grasshopper by double-clicking it or opening it through Rhino.
2. Set the mesh you wish to transmit into the mesh input.
3. We have added some components to the definition to facilitate the transmission. Feel free to modify at your own discretion.
4. Click on the toggle to start the TCP server.
5. Ensure the script is running **before launching Unreal Engine**.

**OR**

### Step 3: Open the XW Built IN-SYNQ Grasshopper Component 

1. Install the XW Built IN-SYNQ Grasshopper Component using the `.yak` installer or by dragging the `.gha` file into your Grasshopper library folder.
2. Restart Rhino and Grasshopper.
3. You should now see an **"IN-SYNQ"** tab inside Grasshopper.
4. Select the component to start building your own Grasshopper definition.
5. Ensure you are using port `8080`, or adjust it in the Unreal Engine Plugin settings if needed.
6. **Note:** This is a pre-built component, and the C# script itself cannot be accessed or edited.
7. **Note:** Some fixes to known issues have been added to this component:
   - **Mesh Joint Component:** Allows input of multiple meshes by joining them.
   - **Flip Mesh Component:** Ensures normals are always facing outward.
   - **Data Dam Component:** Introduces a 2-second delay in data transmission to prevent server overload.
8. Ensure the script is running **before launching Unreal Engine**.

### Step 4: Launch the Unreal Engine Plugin

1. Start Unreal Engine after confirming the Grasshopper script is running.
2. The plugin will attempt to connect to Grasshopper automatically via `localhost` on port `8080`.  
   *(Ensure the Grasshopper script is active to establish the connection.)*

### Step 5: Initialize the Mesh

- Verify the connection by checking the console. You can filter the logs by searching for [RhinoPlugin] to view messages from the plugin exclusively.
- Use the **"InitializeMesh"** command in Unreal Engine to start receiving mesh data from the Grasshopper TCP server.
- **Note:** If Unreal Engine is in the background, data transmission may appear frozen. Refocus Unreal Engine to resume.

---

## Known Issues

- **Crashes on Closing:** Unreal Engine may crash when closing if the connection is active. This is likely due to the connection not being properly terminated before the editor shuts down. (✅ Resolved: A `StopMeshSync` console command has been implemented to safely terminate the connection before shutdown.)
- **Grasshopper Script Order:** The Grasshopper script must be started **before** opening Unreal Engine.
- **Recompute Required:** After establishing the connection, the Grasshopper script may require a recompute to send the mesh data.
- **Freezing in Background:** Unreal Engine may appear to freeze when in the background. Refocusing the application can resume data transmission.
- **Plugin Build Requirements:** The plugin does not include binaries or intermediate files and must be built in Visual Studio as outlined above.
- **Axis Misalignment:** Grasshopper’s X and Y axes are flipped compared to Unreal Engine's.
- **Flipped Normals:** Procedurally dynamic meshes in Unreal Engine may occasionally have their normals flipped.
- **Tree Access Limitation:** The current C# script does not support tree access, meaning only individual meshes can be used as input.

---

## Future Improvements

### Short-Term Improvements
- **Connection Termination Command:** Implement a command to terminate the connection gracefully. (✅ Implemented: Use `StopMeshSync` in Unreal Engine to disconnect safely.)
- **Crash Fixes:** Address crashes that occur when closing Unreal Engine with an active connection.
- **Script Initialization Flexibility:** Enable the Grasshopper script to start after Unreal Engine without causing issues.
- **Background Freezing Fix:** Resolve the Unreal Engine freezing issue when running in the background.
- **Axis Alignment:** Add automatic adjustment to align Grasshopper's axes with Unreal Engine's coordinate system.
- **Normal Orientation Fix:** Ensure procedurally dynamic meshes in Unreal Engine consistently render with correct normals.

### Long-Term Improvements
1. **Send Multiple Meshes**: Expand functionality to support multiple meshes. (✅ Implemented: Multiple mesh input from Grasshopper is now supported, each spawning as a separate actor in Unreal Engine.)
2. **Develop Core Bridge**: Implement Web Socket and serialized file-based bridge for robust data transfer.
3. **Two-Way Control**: Enable bi-directional communication, allowing Unreal Engine sliders to adjust Grasshopper scripts in real time.
4. **Multi-User Support**: Enable multiple users to collaborate on LAN and pixel streaming with dedicated Unreal Engine and Rhino instances.
5. **Support Geometry Types**: Translate and send various geometry types (points, curves, surfaces, polysurfaces, solids, SubD, extrusions, meshes) via the Procedural Mesh Module.
6. **Textures and Materials**: Transfer texture and material data from Rhino/Grasshopper to Unreal Engine.
7. **Cameras / Named Views**: Synchronize camera views between Grasshopper and Unreal Engine.
8. **Lights**: Transfer lighting data and settings from Rhino/Grasshopper to Unreal Engine.
9. **Geolocation Support**: Implement origin translation and geolocation capabilities.
10. **Documentation**: Produce comprehensive documentation for developers and users.
11. **Create Distributables**: Package the bridge into easily deployable plugins for Grasshopper and Unreal Engine.

---

## Troubleshooting
- **Connection Issues**: Ensure the IP address and port match between the Grasshopper script and the Unreal Engine plugin.
- **Build Errors**: Make sure you have the required dependencies and Visual Studio setup for Unreal Engine development.
- **Data Not Transmitting**: Verify that Unreal Engine is focused (not in the background) and the Grasshopper script is recomputed after the connection is established.
- **Plugin Not Loading**: Confirm the plugin is placed in the correct `Plugins` folder, has been built successfully, and is installed and enabled in Unreal Engine.

Feel free to fork this repository or contribute by submitting pull requests!

---

## License
This project is licensed under the terms of the [Apache License 2.0](LICENSE).

