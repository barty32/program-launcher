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
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
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
