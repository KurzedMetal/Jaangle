; Script generated with the Venis Install Wizard

; Define your application name
!define APPNAME "Jaangle music management"
!define APPNAMEANDVERSION "Jaangle 0.96b"

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\Jaangle"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "jsetup.exe"

; Modern interface settings
!include "MUI.nsh"

; MUI Settings
!define MUI_ICON "AppIcon.ico"
!define MUI_UNICON "AppIconUninstall.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange.bmp" ; optional

!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp" ; optional

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\Jaangle.exe"


  
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Bulgarian"
!insertmacro MUI_LANGUAGE "Croatian"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Greek"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "Korean"
!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "NorwegianNynorsk"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "Portuguese"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Romanian"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Serbian"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Swedish"
!insertmacro MUI_LANGUAGE "Turkish"
!insertmacro MUI_LANGUAGE "Ukrainian"
!insertmacro MUI_RESERVEFILE_LANGDLL


!include WinMessages.nsh
 
Function CloseProgram 
  Exch $1
  Push $0
  loop:
    FindWindow $0 $1
    IntCmp $0 0 done
      #SendMessage $0 ${WM_DESTROY} 0 0
      SendMessage $0 ${WM_CLOSE} 0 0
    Sleep 2000 
    Goto loop 
  done: 
  Pop $0 
  Pop $1
FunctionEnd

Section "-Main" MainSection

	; Set Section properties
	SetOverwrite on

	Push "JaangleWndClass"
	Call CloseProgram
	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File /a /r "..\Jaangle\*.*"
	SetOutPath "$INSTDIR\"
	CreateDirectory "$SMPROGRAMS\Jaangle"
	CreateShortCut "$SMPROGRAMS\Jaangle\Jaangle.lnk" "$INSTDIR\Jaangle.exe"
	CreateShortCut "$SMPROGRAMS\Jaangle\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Desktop Icon" DesktopIconSection
	CreateShortCut "$DESKTOP\Jaangle.lnk" "$INSTDIR\Jaangle.exe"
SectionEnd

Section "Quick Launch Icon" QuickLaunchSection
	CreateShortCut "$QUICKLAUNCH\Jaangle.lnk" "$INSTDIR\Jaangle.exe"
SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${DesktopIconSection} "Create a desktop icon"
	!insertmacro MUI_DESCRIPTION_TEXT ${QuickLaunchSection} "Create a Quick Launch icon"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Delete Shortcuts
	Delete "$DESKTOP\Jaangle.lnk"
	Delete "$QUICKLAUNCH\Jaangle.lnk"
	Delete "$SMPROGRAMS\Jaangle\Jaangle.lnk"
	Delete "$SMPROGRAMS\Jaangle\Uninstall.lnk"
	RMDir "$SMPROGRAMS\Jaangle"
	
	


	; Clean up Teen Spirit


	; Remove remaining directories
	RMDir /r "$SMPROGRAMS\Jaangle"
	RMDir /r "$INSTDIR\avs\"
	RMDir /r "$INSTDIR\langs\"
	RMDir /r "$INSTDIR\Plugins\"
	RMDir /r "$INSTDIR\serviceicons\"
	RMDir /r "$INSTDIR\Skins\"
	RMDir /r "$INSTDIR\Storage\"
	Delete $INSTDIR\*.dll
	Delete $INSTDIR\jaangle.exe
	Delete $INSTDIR\CrashReport.exe
	Delete $INSTDIR\music.mdb
	Delete $INSTDIR\Jaangle.ini	
	Delete $INSTDIR\Services.xml	
	Delete $INSTDIR\lasttracks.m3u
	RMDir "$INSTDIR\"

SectionEnd

; On initialization
Function .onInit

	!insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

BrandingText "Jaangle music management"

; eof