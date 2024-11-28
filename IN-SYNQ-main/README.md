# Rhino to Unreal Engine Mesh Synchronization via TCP [AKA IN-SYNQ]

This repository is a **proof of concept** for creating a TCP connection between Rhino/Grasshopper and Unreal Engine. It allows the transmission of mesh data from Grasshopper to Unreal Engine, where the meshes are rendered dynamically using the **Procedural Mesh Module**.

[Watch the Demo Video](https://video.wixstatic.com/video/a00a00_64c7bd89058943f78b43cc540644d094/1080p/mp4/file.mp4)

## How It Works

- **Grasshopper Script (`TCPServer.gh`)**: Acts as a TCP server. It takes a mesh as input and transmits the mesh data over a TCP connection.
- **Unreal Engine Plugin**: Listens for TCP requests containing mesh data and uses the **Procedural Mesh Module** to create and render the mesh in Unreal Engine.

### Current Configuration
- **IP Address**: `127.0.0.1` (localhost)
- **Port**: `8080`

These settings can be modified in both the Grasshopper script and the Unreal Engine plugin to fit your environment.

---

## Installation and Usage

### 1. Grasshopper Script
1. Open `TCPServer.gh` in Grasshopper.
2. Ensure it is set to run **before** starting Unreal Engine.

### 2. Unreal Engine Plugin
1. Copy the plugin folder into your Unreal Engine project’s `Plugins` directory.
2. Open the Unreal Engine project in **Visual Studio** to build the plugin:
   - Right-click on the project in Visual Studio and select **Build**.
3. Once built, launch Unreal Engine.
4. The plugin will automatically attempt to connect to the Grasshopper script (ensure the script is already running).

### 3. Initializing the Mesh
- Use the **"InitializeMesh"** command in Unreal Engine to start polling for mesh data from the Grasshopper TCP server.
- After the connection is established, **recompute the Grasshopper script** to send the mesh data again.
- Note: If Unreal Engine is not focused (e.g., in the background), data transmission might appear frozen until you refocus Unreal Engine.

---

## Known Issues
- Unreal Engine may **crash** when closing if the connection is active.
- The Grasshopper script must be started **before** opening Unreal Engine.
- After establishing the connection, the Grasshopper script may require a **recompute** to send the mesh data.
- Unreal Engine may appear to **freeze** when in the background. Refocusing the application can resume data transmission.
- The plugin does not include binaries or intermediate files and must be built in Visual Studio as outlined above.

---

## Future Improvements

### Short-Term Improvements
- Fix crashes when closing Unreal Engine.
- Allow the Grasshopper script to start after Unreal Engine without issues.
- Resolve Unreal Engine freezing when in the background.

### Long-Term Improvements
1. **Send Multiple Meshes**: Expand functionality to support multiple meshes.
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

