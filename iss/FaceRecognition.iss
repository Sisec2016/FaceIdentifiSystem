; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

#define MyAppName "FaceRecognition"
#define MyAppVersion "v1.0.2"
#define MyAppPublisher "��������"
#define MyAppURL "http://www.rmax.com/"
#define MyAppExeName "FaceRecognition.exe"

[Setup]
; ע: AppId��ֵΪ������ʶ��Ӧ�ó���
; ��ҪΪ������װ����ʹ����ͬ��AppIdֵ��
; (�����µ�GUID����� ����|��IDE������GUID��)
AppId={{E3489C5A-E816-4D94-B92F-9D83F9723415}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName=C:\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename=��������ʶ��ϵͳ{#MyAppVersion}
Compression=lzma
ShowLanguageDialog=no
SolidCompression=yes
UsePreviousUserInfo=no
UsePreviousAppDir=no 
UsePreviousGroup=no 
UsePreviousLanguage=no 
UsePreviousSetupType=no 
UsePreviousTasks=no 
DisableDirPage=yes 
DisableFinishedPage=no 
DisableProgramGroupPage=yes 
DisableReadyMemo=yes 
DisableReadyPage=yes 
DisableStartupPrompt=yes 
DisableWelcomePage=no 

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; 		Description: "{cm:CreateDesktopIcon}"; 		GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1
Name: "quicklaunchicon"; 	Description: "{cm:CreateQuickLaunchIcon}"; 	GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\bin\*.dll"; 												DestDir: "{app}"; Flags: ignoreversion; Excludes: "*.d.dll"
Source: "..\bin\facerecog.db"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\feadb.db1"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\feadb.db3"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\*.jpg"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\*.json"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\*.wav"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\*.ini"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\facerecog.db"; 										DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\FaceRecognition.exe"; 								DestDir: "{app}"; Flags: ignoreversion
Source: "..\bin\faceMonitorServer.exe"; 								DestDir: "{app}"; Flags: ignoreversion

Source: "..\bin\sqldrivers\*"; 					  						DestDir: "{app}\sqldrivers"; 	Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\skins\*"; 		Excludes:"*.lib;*.pdb";				DestDir: "{app}\skins"; 		Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\skin\*"; 											DestDir: "{app}\skin"; 		Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\platforms\*"; 											DestDir: "{app}\platforms"; 	Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\photoshop\*"; 										DestDir: "{app}\photoshop"; 	Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\photo\*"; 											DestDir: "{app}\photo"; 	Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\mvp\*"; 												DestDir: "{app}\mvp"; 			Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\log\*"; 												DestDir: "{app}\log"; 			Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\bin\imageformats\*"; 												DestDir: "{app}\imageformats"; 			Flags: ignoreversion recursesubdirs createallsubdirs
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Icons]
Name: "{group}\{#MyAppName}"; 													Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; 								Filename: "{#MyAppURL}"
Name: "{commondesktop}\{#MyAppName}"; 											Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; 	Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon