// framework.h : include file for standard system include files,
// or project specific include files
//

#pragma once

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
#include <afxcview.h>


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
#include <iostream>
#include <fstream>
#include <sstream>

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
