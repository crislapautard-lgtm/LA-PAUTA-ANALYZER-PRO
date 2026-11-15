; ============================================================
;  Instalador de LA PAUTA ANALYZER PRO (VST3, Windows 64 bits)
;  Compilar con Inno Setup 6
; ============================================================

#define MyAppName      "La Pauta Analyzer Pro"
#define MyAppVersion   "1.0.0"
#define MyAppPublisher "La Pauta"
#define MyVst3Folder   "La Pauta Analyzer Pro.vst3"

[Setup]
AppId={{B7A1C3E2-5D4F-4A8B-9C6E-2F1D8E7A3B10}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
DirExistsWarning=no
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=installer_output
OutputBaseFilename=La-Pauta-Analyzer-Pro-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayName={#MyAppName}
CloseApplications=no

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Files]
; Carpeta VST3 generada por la compilación
Source: "build\LaPautaAnalyzerPro_artefacts\Release\VST3\{#MyVst3Folder}\*"; \
    DestDir: "{commoncf64}\VST3\{#MyVst3Folder}"; \
    Flags: ignoreversion recursesubdirs createallsubdirs

[UninstallDelete]
; Borra el tamaño guardado de la ventana
Type: filesandordirs; Name: "{userappdata}\La Pauta\Analyzer Pro"

[Messages]
spanish.FinishedLabel=La Pauta Analyzer Pro se instaló en la carpeta VST3 de Windows.%nAbre tu DAW y vuelve a escanear los plugins si no aparece.
