using System;
using System.IO;
using System.Text;
using SD = System.Drawing;

using Rhino;
using Grasshopper.Kernel;

namespace RhinoCodePlatform.Rhino3D.Projects.Plugin.GH
{
  public sealed class AssemblyInfo : GH_AssemblyInfo
  {
    static readonly string s_assemblyIconData = "iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAABGdBTUEAALGPC/xhBQAAAAlwSFlzAAAOwgAADsIBFShKgAAAABhJREFUSEvtwQEBAAAAgiD/r64hQAAAXA0JGAAB0AJbXgAAAABJRU5ErkJggg==";
    static readonly string s_categoryIconData = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAABGdBTUEAALGPC/xhBQAAAAlwSFlzAAAOwgAADsIBFShKgAAAABNJREFUOE9jGAWjYBSMAjBgYAAABBAAAadEfGMAAAAASUVORK5CYII=";

    public static readonly SD.Bitmap PluginIcon = default;
    public static readonly SD.Bitmap PluginCategoryIcon = default;

    static AssemblyInfo()
    {
      if (!s_assemblyIconData.Contains("ASSEMBLY-ICON"))
      {
        using (var aicon = new MemoryStream(Convert.FromBase64String(s_assemblyIconData)))
          PluginIcon = new SD.Bitmap(aicon);
      }

      if (!s_categoryIconData.Contains("ASSEMBLY-CATEGORY-ICON"))
      {
        using (var cicon = new MemoryStream(Convert.FromBase64String(s_categoryIconData)))
          PluginCategoryIcon = new SD.Bitmap(cicon);
      }
    }

    public override Guid Id { get; } = new Guid("5d1ff583-f6be-441b-a4e0-5564a6c5c25b");

    public override string AssemblyName { get; } = "IN-SYNQ.Components";
    public override string AssemblyVersion { get; } = "0.0.1.9104";
    public override string AssemblyDescription { get; } = "Proof of concept for creating a TCP connection between Rhino/Grasshopper and Unreal Engine. It allows the transmission of mesh data from Grasshopper to Unreal Engine, where the meshes are rendered dynamically using the Procedural Mesh Module.";
    public override string AuthorName { get; } = "Nauzet Lopez Hernandez";
    public override string AuthorContact { get; } = "nauzet@cross-works.co.uk";
    public override GH_LibraryLicense AssemblyLicense { get; } = GH_LibraryLicense.unset;
    public override SD.Bitmap AssemblyIcon { get; } = PluginIcon;
  }

  public class ProjectComponentPlugin : GH_AssemblyPriority
  {
    static readonly Guid s_projectId = new Guid("5d1ff583-f6be-441b-a4e0-5564a6c5c25b");
    static readonly string s_projectData = "ew0KICAiaG9zdCI6IHsNCiAgICAibmFtZSI6ICJSaGlubzNEIiwNCiAgICAidmVyc2lvbiI6ICI4LjEzLjI0MzE3XHUwMDJCMTMwMDEiLA0KICAgICJvcyI6ICJ3aW5kb3dzIiwNCiAgICAiYXJjaCI6ICJ4NjQiDQogIH0sDQogICJpZCI6ICI1ZDFmZjU4My1mNmJlLTQ0MWItYTRlMC01NTY0YTZjNWMyNWIiLA0KICAiaWRlbnRpdHkiOiB7DQogICAgIm5hbWUiOiAiSU4tU1lOUSIsDQogICAgInZlcnNpb24iOiAiMC4wLjEtYmV0YSIsDQogICAgInB1Ymxpc2hlciI6IHsNCiAgICAgICJlbWFpbCI6ICJuYXV6ZXRAY3Jvc3Mtd29ya3MuY28udWsiLA0KICAgICAgIm5hbWUiOiAiTmF1emV0IExvcGV6IEhlcm5hbmRleiIsDQogICAgICAiY29tcGFueSI6ICJDcm9zcyBXb3JrcyBMVEQiLA0KICAgICAgImFkZHJlc3MiOiAiT0NUQUdPTiBQT0lOVC4gNSBDSEVBUFNJREUsIFNUIFBBVUxcdTAwMjdTLiBMT05ET04sIEVDMlYgNkFBIiwNCiAgICAgICJjb3VudHJ5IjogIlVuaXRlZCBLaW5nZG9tIiwNCiAgICAgICJ1cmwiOiAiaHR0cHM6Ly93d3cuY3Jvc3Mtd29ya3MuY28udWsvIg0KICAgIH0sDQogICAgImRlc2NyaXB0aW9uIjogIlByb29mIG9mIGNvbmNlcHQgZm9yIGNyZWF0aW5nIGEgVENQIGNvbm5lY3Rpb24gYmV0d2VlbiBSaGluby9HcmFzc2hvcHBlciBhbmQgVW5yZWFsIEVuZ2luZS4gSXQgYWxsb3dzIHRoZSB0cmFuc21pc3Npb24gb2YgbWVzaCBkYXRhIGZyb20gR3Jhc3Nob3BwZXIgdG8gVW5yZWFsIEVuZ2luZSwgd2hlcmUgdGhlIG1lc2hlcyBhcmUgcmVuZGVyZWQgZHluYW1pY2FsbHkgdXNpbmcgdGhlIFByb2NlZHVyYWwgTWVzaCBNb2R1bGUuIiwNCiAgICAiY29weXJpZ2h0IjogIkNvcHlyaWdodCBcdTAwQTkgMjAyNCBDcm9zcyBXb3JrcyBMdGQuIiwNCiAgICAibGljZW5zZSI6ICJBcGFjaGUgTGljZW5zZSAyLjAiLA0KICAgICJ1cmwiOiAiaHR0cHM6Ly93d3cuY3Jvc3Mtd29ya3MuY28udWsvaW4tc3lucSIsDQogICAgImltYWdlIjogew0KICAgICAgImxpZ2h0Ijogew0KICAgICAgICAidHlwZSI6ICJzdmciLA0KICAgICAgICAiZGF0YSI6ICJQSE4yWnlCM2FXUjBhRDBpTkRnaUlHaGxhV2RvZEQwaU5EZ2lJSFpsY25OcGIyNDlJakV1TVNJZ2VHMXNibk05SW1oMGRIQTZMeTkzZDNjdWR6TXViM0puTHpJd01EQXZjM1puSWlCMmFXVjNRbTk0UFNJd0lEQWdORGdnTkRnaUlHWnBiR3c5SW01dmJtVWlJSE4wY205clpUMGlibTl1WlNJZ1ptbHNiQzFrWVhKclBTSnViMjVsSWlCemRISnZhMlV0WkdGeWF6MGlibTl1WlNJZ2MzUnliMnRsTFd4cGJtVmpZWEE5SW5OeGRXRnlaU0lnYzNSeWIydGxMV3hwYm1WcWIybHVQU0p0YVhSbGNpSWdjM1J5YjJ0bExYZHBaSFJvUFNJd0lpQXZQZz09Ig0KICAgICAgfSwNCiAgICAgICJkYXJrIjogew0KICAgICAgICAidHlwZSI6ICJzdmciLA0KICAgICAgICAiZGF0YSI6ICJQSE4yWnlCM2FXUjBhRDBpTkRnaUlHaGxhV2RvZEQwaU5EZ2lJSFpsY25OcGIyNDlJakV1TVNJZ2VHMXNibk05SW1oMGRIQTZMeTkzZDNjdWR6TXViM0puTHpJd01EQXZjM1puSWlCMmFXVjNRbTk0UFNJd0lEQWdORGdnTkRnaUlHWnBiR3c5SW01dmJtVWlJSE4wY205clpUMGlibTl1WlNJZ1ptbHNiQzFrWVhKclBTSnViMjVsSWlCemRISnZhMlV0WkdGeWF6MGlibTl1WlNJZ2MzUnliMnRsTFd4cGJtVmpZWEE5SW5OeGRXRnlaU0lnYzNSeWIydGxMV3hwYm1WcWIybHVQU0p0YVhSbGNpSWdjM1J5YjJ0bExYZHBaSFJvUFNJd0lpQXZQZz09Ig0KICAgICAgfSwNCiAgICAgICJwcm9qZWN0SWNvbiI6IHsNCiAgICAgICAgImxpZ2h0Ijogew0KICAgICAgICAgICJieXRlcyI6ICJpVkJPUncwS0dnb0FBQUFOU1VoRVVnQUFBQmdBQUFBWUNBWUFBQURnZHozNEFBQUFCR2RCVFVFQUFMR1BDL3hoQlFBQUFBbHdTRmx6QUFBT3dnQUFEc0lCRlNoS2dBQUFBQmhKUkVGVVNFdnR3UUVCQUFBQWdpRC9yNjRoUUFBQVhBMEpHQUFCMEFKYlhnQUFBQUJKUlU1RXJrSmdnZz09IiwNCiAgICAgICAgICAid2lkdGgiOiAyNCwNCiAgICAgICAgICAiaGVpZ2h0IjogMjQNCiAgICAgICAgfSwNCiAgICAgICAgImRhcmsiOiB7DQogICAgICAgICAgImJ5dGVzIjogImlWQk9SdzBLR2dvQUFBQU5TVWhFVWdBQUFCZ0FBQUFZQ0FZQUFBRGdkejM0QUFBQUJHZEJUVUVBQUxHUEMveGhCUUFBQUFsd1NGbHpBQUFPd2dBQURzSUJGU2hLZ0FBQUFCaEpSRUZVU0V2dHdRRUJBQUFBZ2lEL3I2NGhRQUFBWEEwSkdBQUIwQUpiWGdBQUFBQkpSVTVFcmtKZ2dnPT0iLA0KICAgICAgICAgICJ3aWR0aCI6IDI0LA0KICAgICAgICAgICJoZWlnaHQiOiAyNA0KICAgICAgICB9LA0KICAgICAgICAiaWNvRGF0YSI6ICJBQUFCQUFFQUdCZ0FBQUVBSUFCMkFBQUFGZ0FBQUlsUVRrY05DaG9LQUFBQURVbElSRklBQUFBWUFBQUFHQWdHQUFBQTRIYzlcdTAwMkJBQUFBQVJuUVUxQkFBQ3hqd3Y4WVFVQUFBQUpjRWhaY3dBQURzSUFBQTdDQVJVb1NvQUFBQUFZU1VSQlZFaEw3Y0VCQVFBQUFJSWcvNlx1MDAyQnVJVUFBQUZ3TkNSZ0FBZEFDVzE0QUFBQUFTVVZPUks1Q1lJST0iDQogICAgICB9LA0KICAgICAgImNhdGVnb3J5SWNvbiI6IHsNCiAgICAgICAgImxpZ2h0Ijogew0KICAgICAgICAgICJieXRlcyI6ICJpVkJPUncwS0dnb0FBQUFOU1VoRVVnQUFBQkFBQUFBUUNBWUFBQUFmOC85aEFBQUFCR2RCVFVFQUFMR1BDL3hoQlFBQUFBbHdTRmx6QUFBT3dnQUFEc0lCRlNoS2dBQUFBQk5KUkVGVU9FOWpHQVdqWUJTTUFqQmdZQUFBQkJBQUFhZEVmR01BQUFBQVNVVk9SSzVDWUlJPSIsDQogICAgICAgICAgIndpZHRoIjogMTYsDQogICAgICAgICAgImhlaWdodCI6IDE2DQogICAgICAgIH0sDQogICAgICAgICJkYXJrIjogew0KICAgICAgICAgICJieXRlcyI6ICJpVkJPUncwS0dnb0FBQUFOU1VoRVVnQUFBQkFBQUFBUUNBWUFBQUFmOC85aEFBQUFCR2RCVFVFQUFMR1BDL3hoQlFBQUFBbHdTRmx6QUFBT3dnQUFEc0lCRlNoS2dBQUFBQk5KUkVGVU9FOWpHQVdqWUJTTUFqQmdZQUFBQkJBQUFhZEVmR01BQUFBQVNVVk9SSzVDWUlJPSIsDQogICAgICAgICAgIndpZHRoIjogMTYsDQogICAgICAgICAgImhlaWdodCI6IDE2DQogICAgICAgIH0NCiAgICAgIH0NCiAgICB9DQogIH0sDQogICJzZXR0aW5ncyI6IHsNCiAgICAiYnVpbGRQYXRoIjogImZpbGU6Ly8vQzovVXNlcnMvTmF1emV0TG9wZXpIZXJuYW5kZXovRGVza3RvcC9YVyBHSC9YVyBQbHVnLWluIFByb2plY3QvSU4tU1lOUS1idWlsZC9yaDgiLA0KICAgICJidWlsZFRhcmdldCI6IHsNCiAgICAgICJob3N0Ijogew0KICAgICAgICAibmFtZSI6ICJSaGlubzNEIiwNCiAgICAgICAgInZlcnNpb24iOiAiOCINCiAgICAgIH0sDQogICAgICAidGl0bGUiOiAiUmhpbm8zRCAoOC4qKSIsDQogICAgICAic2x1ZyI6ICJyaDgiDQogICAgfSwNCiAgICAicHVibGlzaFRhcmdldCI6IHsNCiAgICAgICJ0aXRsZSI6ICJNY05lZWwgWWFrIFNlcnZlciINCiAgICB9DQogIH0sDQogICJjb2RlcyI6IFtdDQp9";
    static readonly dynamic s_projectServer = default;
    static readonly object s_project = default;

    static ProjectComponentPlugin()
    {
      s_projectServer = ProjectInterop.GetProjectServer();
      if (s_projectServer is null)
      {
        RhinoApp.WriteLine($"Error loading Grasshopper plugin. Missing Rhino3D platform");
        return;
      }

      // get project
      dynamic dctx = ProjectInterop.CreateInvokeContext();
      dctx.Inputs["projectAssembly"] = typeof(ProjectComponentPlugin).Assembly;
      dctx.Inputs["projectId"] = s_projectId;
      dctx.Inputs["projectData"] = s_projectData;

      object project = default;
      if (s_projectServer.TryInvoke("plugins/v1/deserialize", dctx)
            && dctx.Outputs.TryGet("project", out project))
      {
        // server reports errors
        s_project = project;
      }
    }

    public override GH_LoadingInstruction PriorityLoad()
    {
      if (AssemblyInfo.PluginCategoryIcon is SD.Bitmap icon)
      {
        Grasshopper.Instances.ComponentServer.AddCategoryIcon("IN-SYNQ", icon);
      }
      Grasshopper.Instances.ComponentServer.AddCategorySymbolName("IN-SYNQ", "IN-SYNQ"[0]);

      return GH_LoadingInstruction.Proceed;
    }

    public static bool TryCreateScript(GH_Component ghcomponent, string serialized, out object script)
    {
      script = default;

      if (s_projectServer is null) return false;

      dynamic dctx = ProjectInterop.CreateInvokeContext();
      dctx.Inputs["component"] = ghcomponent;
      dctx.Inputs["project"] = s_project;
      dctx.Inputs["scriptData"] = serialized;

      if (s_projectServer.TryInvoke("plugins/v1/gh/deserialize", dctx))
      {
        return dctx.Outputs.TryGet("script", out script);
      }

      return false;
    }

    public static void DisposeScript(GH_Component ghcomponent, object script)
    {
      if (script is null)
        return;

      dynamic dctx = ProjectInterop.CreateInvokeContext();
      dctx.Inputs["component"] = ghcomponent;
      dctx.Inputs["project"] = s_project;
      dctx.Inputs["script"] = script;

      if (!s_projectServer.TryInvoke("plugins/v1/gh/dispose", dctx))
        throw new Exception("Error disposing Grasshopper script component");
    }
  }
}
