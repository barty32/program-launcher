// 
//  ____                                         _                           _                 _     
// |  _ \ _ __ ___   __ _ _ __ __ _ _ __ ___    | |    __ _ _   _ _ __   ___| |__   ___ _ __  | |__  
// | |_) | '__/ _ \ / _` | '__/ _` | '_ ` _ \   | |   / _` | | | | '_ \ / __| '_ \ / _ \ '__| | '_ \ 
// |  __/| | | (_) | (_| | | | (_| | | | | | |  | |__| (_| | |_| | | | | (__| | | |  __/ |  _ | | | |
// |_|   |_|  \___/ \__, |_|  \__,_|_| |_| |_|  |_____\__,_|\__,_|_| |_|\___|_| |_|\___|_| (_)|_| |_|
//                  |___/                                                                            
//
// Main header file for all source files, contains all macros, global variable declarations and function prototypes
// 
//  Copyright ©2021, barty12
// 
// 
// To-do:
// - add tooltips on buttons
// - fix columns in details view
// - add helper ids to dialogs
// - add icons to tab control
// - add icons to menu items
// 
// 
// 
// Changelog:
// v2.0.1: (14.4.2021)
//		- fixed adding button with empty fields
//		- fixed blurry icons
//		- fixed category name delimiters
// 
// v2.0.2: (18.4.2021)
//		- fixed - preferences spin controls doesn't work
//		- (not needed) add enter handler to preferences' edit boxes
//		- improved slider / edit / spin controls in preferences
//		- target program now launches in its directory instead of launching in Program Launcher's directory
//		- aligned items in dialogs
//		- added version info
//		- when you use Browse in Add button dialog Display name is auto-filled if empty
//		- when you use Browse in Add button dialog Absolute paths is checked automatically
//		- fixed category rename
//		- made proper error handlers
// 
// v2.1.0: (21.4.2021)
//		- added icon caching
// 
// v2.1.1: (26.4.2021)
//		- fixed tab order in dialogs
//		- added proper icon
//		- added enter handler in main window
//		- code cleanup
//		- improved ini file functions -> improved performance
//		- rewrited most of functions for better performance
//		- remove category message box now contains category name
//		- fixed string buffer lenghts (used heap)
//		- fixed adding folders
//		- added checking for 16-bit and 64-bit executables
//		- added support for paths with enviromnent variables
// 
// v2.1.2: (29.5.2021)
//		- add category dialog: blocked special characters
//		- add category dialog: fixed title
//		- rename category menu item now works
//		- improved context menu handlers (now works with keyboard)
//		- added columns to details view
//		- moved strings to string table
//		- added accelerators and full keyboard support
//		- fixed scrollbar displaying on left
//		- configuration file and icon cache files are saved to %AppData%\Program Launcher\ 
//			if they cannot be saved to default directory
// 
// v2.1.3:
//		- 
// 
//

#pragma once

//includes
#include "res\resource.h"
#include "framework.h"

//-------------------------------------------------------------------------------------------------------------------------
// 
//                                             Macros
// 
//-------------------------------------------------------------------------------------------------------------------------

#define CATEGORY_NAME_DELIM TEXT(';')
#define CATEGORY_NAME_DELIM_STRING TEXT(";")

#define INI_FILE_NAME "ProgramLauncher.ini"
#define ICON_CACHE_DIRECTORY L"IconCache"

#define DUMMY(x) #x
#define TOSTRING(x) DUMMY(x)

//resource IDs
#define ID_LISTVIEW 65530

//constants
#define MAX_LOADSTRING 100
#define CATEGORY_LIST_LEN 8192
#define CATEGORY_NAME_LEN 128
#define INT_LEN_IN_DIGITS 16
#define INT_STR_LEN 11
#define MAX_ENTRIES_IN_CATEGORY 512
#define MAX_ENTRY_LEN 1100
#define MAX_CATEGORIES 64

#define MAX_KEY_NAME_LEN 64
#define MAX_SECTION_NAME_LEN 128


#define MIN_ICON_SIZE 16
#define MAX_ICON_SIZE 256
#define DEFAULT_ICON_SIZE 32
#define SMALL_ICON_SIZE 16

#define MIN_ICON_SPACING 20
#define MAX_ICON_SPACING 300
#define DEFAULT_HORZ_SPACING 40
#define DEFAULT_VERT_SPACING 64

#define DEFAULT_SHOWAPPNAMES 2
#define DEFAULT_CLOSEAFTERLAUNCH 0
#define DEFAULT_USEICONCACHING 1
#define DEFAULT_WINDOW_WIDTH 600
#define DEFAULT_WINDOW_HEIGHT 500

// ChangeSliderValue() options
#define SE_SLIDER 1
#define SE_EDIT 2 
#define SE_SPIN 3


#define ErrorHandler() ErrorHandlerEx(__LINE__, __FILEW__, __FUNCTIONW__)

#ifdef _DEBUG
//triggers Error handler only if debug mode is active
#define ErrorHandlerDebug() ErrorHandlerEx(__LINE__, __FILEW__, __FUNCTIONW__)
#else
//does nothing in Release mode
#define ErrorHandlerDebug()
#endif

#define TIME_MEASURE(__BLOCK__) \
LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds; \
LARGE_INTEGER Frequency; \
QueryPerformanceFrequency(&Frequency); \
QueryPerformanceCounter(&StartingTime); \
__BLOCK__; \
QueryPerformanceCounter(&EndingTime); \
ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart; \
ElapsedMicroseconds.QuadPart *= 1000000; \
ElapsedMicroseconds.QuadPart /= Frequency.QuadPart; 



#ifdef _DEBUG
#define NOP {int a = 0;}
#else
#define NOP
#endif


#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif


#ifdef __cplusplus
#define GLOBAL extern
#else
#define GLOBAL 
#endif

//-------------------------------------------------------------------------------------------------------------------------
// 
//                                            Global variables
// 
//-------------------------------------------------------------------------------------------------------------------------

GLOBAL HINSTANCE hInst;        // current instance

GLOBAL HWND hwndMain;
GLOBAL HWND hwndTab;
GLOBAL HWND hwndMainListView;
GLOBAL BOOL bAllowEnChange;
GLOBAL INT nCategoryCount;

GLOBAL BOOL fRebuildIconCache;

GLOBAL TCHAR szIniPath[MAX_PATH];
GLOBAL WCHAR szError[16];

GLOBAL LPWSTR lpszLoadString;

GLOBAL TCHAR arrszCategoryList[MAX_CATEGORIES][CATEGORY_NAME_LEN];

//-------------------------------------------------------------------------------------------------------------------------
// 
//                                        Structure declarations
// 
//-------------------------------------------------------------------------------------------------------------------------

// Launcher Button structure
//	Entries:
//		TCHAR szPath[MAX_PATH];
//		TCHAR szPath64[MAX_PATH];
//		TCHAR szPathIcon[MAX_PATH];
//		INT nIconIndex;
//		TCHAR szName[MAX_PATH];
//		INT bAdmin;
typedef struct _LAUNCHERENTRY{
	TCHAR  szPath[MAX_PATH];			//path to the executable
	TCHAR  szPath64[MAX_PATH];			//path to the 64-bit version of the executable
	TCHAR  szPathIcon[MAX_PATH + 5];	//path to the icon
	INT    nIconIndex;					//icon's index
	TCHAR  szName[MAX_PATH];			//name that is displayed under icon / in tooltip
	INT    bAdmin;						//run program as admin
	INT    bAbsolute;					//use absolute paths
}LAUNCHERENTRY, *LPLAUNCHERENTRY;


//-------------------------------------------------------------------------------------------------------------------------
// 
//                                        Function prototypes
// 
//-------------------------------------------------------------------------------------------------------------------------

ATOM MyRegisterClass(HINSTANCE hInstance, LPCTSTR lpszClassName);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, LPCTSTR lpszClassName);

//window and dialog procedures
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK UserInputProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK BtnEditDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK PrefDlgProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

// Ini file functions (ini.c)
//INT GetIniString(LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpIniName);
INT GetSettingString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPTSTR lpszBuffer, DWORD nSize, LPCTSTR lpszDefault);
INT SetSettingString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszString);
UINT GetSettingInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, INT nDefault);
INT SetSettingInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, INT nNumber);
INT GetSettingButtonStruct(LPCTSTR lpszCategoryName, INT nButtonIndex, LPLAUNCHERENTRY lpStruct, BOOL bRequestAbsolute);
INT SetSettingButtonStruct(LPCTSTR lpszCategoryName, INT nButtonIndex, LPLAUNCHERENTRY lpStruct);
INT DeleteSettingKey(LPCTSTR lpszSection, LPCTSTR lpszKeyName);
INT CreateIniFile();

//list view functions
HWND CreateListView(HWND hwndParent);
BOOL UpdateListView(HWND hwndListView, INT nIconSize);
void ResizeListView(HWND hwndListView);
void SwitchListView(HWND hwndListView, DWORD dwView);
BOOL UpdateImageLists(HWND hwndListView, INT nIconSizeOfLargeImageList);
INT DoListViewContextMenu(HWND hwndListView, INT pX, INT pY);

// Category functions (category.c)
INT AddCategory(LPCTSTR lpszNewCategoryName);
INT RemoveCategory(UINT);
INT RenameCategory(INT nCategoryIndex, LPTSTR lpszNewName);
INT MoveCategory(INT nCategoryIndex, BOOL bLeft);
INT GetCategoryCount();
INT GetCategorySectionCount(UINT nCategoryIndex);
INT LoadCategoryList();
INT SaveCategoryList();

//tab control functions
HWND CreateTabControl(HWND);
void ResizeTabControl(HWND hwndTabControl, HWND hwndParent);


// Common funtions (CommonFunctions.c)
INT     RemoveLetterFromPath    (LPWSTR lpszPath);
INT     AddLetterToPath         (LPWSTR pszPath, INT cchSize);
BOOL    Is64BitWindows          ();
void    ForceWindowRepaint      (HWND hWnd);
void    CenterDialog            (HWND hwndParent, HWND hwndDialog);
void    ErrorHandlerEx          (LONG wLine, LPCWSTR lpszFile, LPCWSTR lpszFunctionName);
INT     LaunchProgram           (INT nBtnId, INT nCategoryIndex);
void    UpdateMenu              (HWND hwndListView, HMENU hMenu);
INT     DoTabControlContextMenu (INT iItemId, INT pX, INT pY);
LPWSTR  LoadLocalString         (UINT uID);
INT     GetIconIndexFromPath    (LPWSTR pszIconPath);

//other functions
INT RemoveButton(INT nBtnIndex, INT nCategoryIndex);
INT MoveButton(INT nButtonIndex, BOOL bLeft);



INT ChangeSliderValue(HWND hwndSlider, HWND hwndEditBox, INT GetValueFrom, INT nMaxValue, LPNMUPDOWN lpstSpinValue);


//icon functions
HICON LoadIconForButton(INT nBtnId, INT nCategoryIndex, INT nIconSize);
INT SaveIconFromResource(INT nCategoryIndex, INT nButtonIndex);
INT RebuildIconCache();




