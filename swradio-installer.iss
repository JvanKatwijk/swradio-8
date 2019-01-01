
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "swradio-8"
#define MyAppVersion "1.0"
#define MyAppPublisher "Lazy Chair Computing"
#define MyAppURL "https://github.com/JvanKatwijk/swradio-8"
#define MyAppExeName "swradio-8.0.exe";

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId= {{47070962-772F-4D00-B28C-B5ED98DC6F2B}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile=E:\sdr-j-development\windows-swradio\COPYRIGHT.this_software
InfoBeforeFile=E:\sdr-j-development\windows-swradio\preamble.txt
OutputBaseFilename=setup-swradio
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "E:\sdr-j-development\windows-swradio\swradio-8.0.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "E:\sdr-j-development\windows-swradio\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "E:\sdr-j-development\SDRplay_RSP_API-Windows-2.13.1.exe"; DestDir: "{app}"; AfterInstall : install_sdrplayApi

[Icons]
Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[code]
procedure install_sdrplayApi;
var
    resultCode : Integer;
begin
   Exec (ExpandConstant('{app}\SDRplay_RSP_API-Windows-2.13.1.exe'), '', '', SW_SHOWNORMAL,
    ewWaitUntilTerminated, ResultCode)
end;
