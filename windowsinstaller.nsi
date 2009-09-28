; Copyright 2008 Jason A. Donenfeld <Jason@zx2c4.com>

SetCompressor /SOLID /FINAL lzma

!define PRODUCT_NAME "Arora"
!define /date PRODUCT_VERSION "Snapshot (%#m-%#d-%#Y)"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\arora.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

!include "MUI.nsh"
!define MUI_ABORTWARNING
!define MUI_ICON ".\src\browser.ico"
!define MUI_UNICON ".\src\browser.ico"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\arora.exe"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME} ${PRODUCT_VERSION} Installer.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section -Icons
  CreateShortCut "$SMPROGRAMS\Arora.lnk" "$INSTDIR\arora.exe"
SectionEnd

Section -UninstallInfo
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\arora.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\arora.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
SectionEnd

Section "Main Components" SEC01
  KillProcDLL::KillProc "arora.exe"
  Sleep 100
  SetOverwrite on

  SetOutPath "$INSTDIR"
  File "arora.exe"
  File "C:\qt-snapshot\bin\QtCore4.dll"
  File "C:\qt-snapshot\bin\QtGui4.dll"
  File "C:\qt-snapshot\bin\QtNetwork4.dll"
  File "C:\qt-snapshot\bin\QtWebKit4.dll" ;"C:\WebKit\WebKitBuild\Release\lib\QtWebKit4.dll"
  File "C:\openssl-0.9.8h\out32dll\ssleay32.dll"
  File "C:\openssl-0.9.8h\out32dll\libeay32.dll"

  SetOutPath "$INSTDIR\locale"
  File "src\.qm\locale\*.qm"

  SetOutPath "$INSTDIR\imageformats"
  File "C:\qt-snapshot\plugins\imageformats\qtiff4.dll"
  File "C:\qt-snapshot\plugins\imageformats\qsvg4.dll"
  File "C:\qt-snapshot\plugins\imageformats\qmng4.dll"
  File "C:\qt-snapshot\plugins\imageformats\qjpeg4.dll"
  File "C:\qt-snapshot\plugins\imageformats\qico4.dll"
  File "C:\qt-snapshot\plugins\imageformats\qgif4.dll"

  SetOutPath "$INSTDIR\iconengines"
  File "C:\qt-snapshot\plugins\iconengines\qsvgicon4.dll"

  SetOutPath "$INSTDIR\codecs"
  File "C:\qt-snapshot\plugins\codecs\qtwcodecs4.dll"
  File "C:\qt-snapshot\plugins\codecs\qkrcodecs4.dll"
  File "C:\qt-snapshot\plugins\codecs\qjpcodecs4.dll"
  File "C:\qt-snapshot\plugins\codecs\qcncodecs4.dll"

  InitPluginsDir
  SetOutPath $PLUGINSDIR
  File "C:\Program Files\Microsoft Visual Studio 8\SDK\v2.0\BootStrapper\Packages\vcredist_x86\vcredist_x86.exe"
  DetailPrint "Installing Visual C++ 2005 Libraries"
  ExecWait '"$PLUGINSDIR\vcredist_x86.exe" /q:a /c:"msiexec /i vcredist.msi /quiet"'
SectionEnd

Section Uninstall
  KillProcDLL::KillProc "arora.exe"
  Sleep 100
  Delete $SMPROGRAMS\Arora.lnk
  RMDir /r "$INSTDIR"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
SectionEnd
BrandingText "arora-browser.org"
