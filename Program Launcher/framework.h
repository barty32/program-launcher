// framework.h : include file for standard system include files,
// or project specific include files
//

#pragma once
#include <IniParser.h>

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
#endif

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

// MFC Header Files
#include <afxwin.h>  // MFC core and standard components
#include <afxcmn.h>
#include <afxext.h>
#include <afxcview.h>
#include <afxdialogex.h>


// Windows Header Files
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <CommCtrl.h>
#include <ShlObj.h>
#include <commdlg.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <Uxtheme.h>

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
//#include <malloc.h>
//#include <memory.h>

// C++ Header Files
#include <string>
#include <exception>
#include <memory>
#include <vector>
#include <algorithm>
#include <future>
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <filesystem>

using std::vector,
      std::wstring,
      std::to_wstring,
      std::wistringstream,
      std::stoi,
      std::wfstream,
      std::wifstream,
      std::wofstream,
      std::getline,
      std::endl,
      std::unique_ptr, 
      std::shared_ptr,
      std::make_unique,
      std::make_shared;



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
