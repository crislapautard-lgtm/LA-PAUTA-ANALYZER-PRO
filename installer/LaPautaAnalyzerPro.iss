
#define MyAppName "La Pauta Analyzer Pro"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "La Pauta"

[Setup]
AppId={{B8A4C53B-9E2B-4F20-9D65-2A4B7C6D1101}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={commoncf}\VST3
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\La Pauta Analyzer Pro.vst3
OutputDir=..\output
OutputBaseFilename=LaPautaAnalyzerPro_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
MinVersion=10.0

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "staging\La Pauta Analyzer Pro.vst3\*"; DestDir: "{app}\La Pauta Analyzer Pro.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Code]
function InitializeSetup(): Boolean;
begin
  Result := IsWin64;

  if not Result then
    MsgBox(
      'La Pauta Analyzer Pro requiere Windows de 64 bits.',
      mbError,
      MB_OK
    );
end
