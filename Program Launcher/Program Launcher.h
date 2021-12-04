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
// To-do list:
// 
// FIX: remove grid in details view
// ADD: copy/cut/paste handlers
// ADD: launch shortcut
// ADD: search function
// ADD: auto folder scan
// ADD: sort options
// FIX: reordering multiline tabs
// FIX: cancel tab reorder with ESC
// ADD: support for UWP apps
// ADD: helper ids to dialogs
// ADD: helper ids to status bar
// ADD: icons to tab control
// ADD: icons to menu items
// 
// 
// Startup sequence:
// 1. Get ini file location
// 2. Parse ini file
// 3. Load options
// 4. Load categories
// 5. Create and show main window
// 6. Load elements
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
//		- fixed scrollbar displaying on right
//		- configuration file and icon cache files are saved to %AppData%\Program Launcher\ 
//			if they cannot be saved to default directory
// 
// v2.1.3: (6.11.2021)
//		- superseded by 2.2 version
//		- last release written in C
// 
// v2.2:
//		- whole app rewritten from C to object-oriented C++ with MFC
//		- added drag and drop tab reordering
//		- items are loaded in separate thread
//		- added tooltips on buttons (tooltip mode in preferences is now available)
//		- changed default icon size and spacing
//		- added in-place editing of items
//		- changed tab control font
//		- current list view mode is saved on close
//		- list view column widths and order are saved on close
//		- added drag and drop item reordering
//		- introduced new ini file searching algorithm
//		- added status bar
//		- used my new ini parser library
//		- added drop file support
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'pch.h' before including this file for PCH"
#endif

//includes
#ifdef IDC_STATIC
#undef IDC_STATIC
#endif
#include "res\resource.h"

//-------------------------------------------------------------------------------------------------------------------------
// 
//                                             Macros
// 
//-------------------------------------------------------------------------------------------------------------------------

#define CATEGORY_NAME_DELIM L';'
#define CATEGORY_NAME_DELIM_STRING L";"

#define INI_FILE_NAME "ProgramLauncher.ini"
#define ICON_CACHE_DIRECTORY L"IconCache"

//resource IDs
#define ID_LISTVIEW   65530
#define ID_TABCONTROL 65531

//constants
#define CATEGORY_NAME_LEN 128
#define MAX_CATEGORY_SIZE 1024
#define SMALL_ICON_SIZE   16

//min and max values
#define MIN_ICON_SIZE    16
#define MAX_ICON_SIZE    256
#define MIN_ICON_SPACING 20
#define MAX_ICON_SPACING 300

//default values
#define DEFAULT_ICON_SIZE        64
#define DEFAULT_HORZ_SPACING     110
#define DEFAULT_VERT_SPACING     110
#define DEFAULT_SHOWAPPNAMES     2
#define DEFAULT_CLOSEAFTERLAUNCH 0
#define DEFAULT_USEICONCACHING   1
#define DEFAULT_WINDOW_WIDTH     900
#define DEFAULT_WINDOW_HEIGHT    600

//ApplyOptions() options
enum ApplyOptions{
	OPT_NONE     = 0x00,
	OPT_APPNAMES = 0x01,
	OPT_SIZE     = 0x02,
	OPT_SPACING  = 0x04,
	OPT_WINDOW   = 0x08,
	OPT_ALL      = 0xff
};


#define ErrorHandler() ErrorHandlerEx(__LINE__, __FILEW__, __FUNCTIONW__)

#ifdef _DEBUG
//triggers Error handler only if debug mode is active
#define ErrorHandlerDebug() ErrorHandlerEx(__LINE__, __FILEW__, __FUNCTIONW__)
#else
//does nothing in Release mode
#define ErrorHandlerDebug()
#endif

//utility for measuring time
#ifdef _DEBUG
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
#else
#define TIME_MEASURE(__BLOCK__) \
__BLOCK__;
#endif

#define null nullptr

#ifdef LoadIcon
#undef LoadIcon
#endif

#define DUMMY(x) #x
#define TOSTRING(x) DUMMY(x)

#ifdef _DEBUG
#define NOP {int a = 0;}
#else
#define NOP
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#ifdef DECLARE_GLOBAL_VARIABLES
#define GLOBAL
#else
#define GLOBAL extern
#endif

//-------------------------------------------------------------------------------------------------------------------------
// 
//                                             Structures & classes definitions
// 
//-------------------------------------------------------------------------------------------------------------------------

typedef struct _tagElementProperties{
	wstring wsPath;		//path to the executable
	wstring wsPath64;	//path to the 64-bit version of the executable
	wstring wsPathIcon;	//path to the icon
	wstring wsName;		//name that is displayed under icon / in tooltip
	int     iIconIndex;
	bool    bAdmin;		//run program as admin
	bool    bAbsolute;	//use absolute paths
}ElementProps;

typedef struct tagTCORDERCHANGE{
	NMHDR hdr;
	UINT nSrcTab;
	UINT nDstTab;
} NMTCORDERCHANGE, *LPNMTCORDERCHANGE;

class TabControl;
class CProgramLauncher;
	class CLauncherWnd;
		class CCategory;
			class CLauncherItem;

//-------------------------------------------------------------------------------------------------------------------------
// 
//                                            Global variables
// 
//-------------------------------------------------------------------------------------------------------------------------

GLOBAL HINSTANCE hInst;
GLOBAL CProgramLauncher* Launcher;
GLOBAL CLauncherWnd*     CMainWnd;

//-------------------------------------------------------------------------------------------------------------------------
// 
//                                        Function prototypes
// 
//-------------------------------------------------------------------------------------------------------------------------


DWORD WINAPI LoadElements(LPVOID lParam);

// Ini file functions (ini.c)
//INT CreateIniFile();

// Common funtions (CommonFunctions.c)
bool Is64BitWindows      ();
bool IsFileWritable      (const fs::path& filePath);
void ErrorHandlerEx      (LONG wLine, LPCWSTR lpszFile, LPCWSTR lpszFunctionName);
INT  GetIconIndexFromPath(LPWSTR pszIconPath);

int ErrorMsg(const wstring& msg, const wstring& title = L"", UINT type = MB_ICONEXCLAMATION | MB_OK);
int ErrorMsg(UINT msgID, UINT titleID = IDS_ERROR, UINT type = MB_ICONEXCLAMATION | MB_OK);

// icon functions
bool SaveIconFromResource(const fs::path& lpszResourcePath, WORD iIconIndex, const fs::path& lpszResultFileName);

// string functions
wstring ExpandEnvStrings(const wstring& source);
wstring GetString(UINT uID);

// path functions
bool AddLetterToPath(wstring& path);
bool RemoveLetterFromPath(wstring& path);

bool GetElementProperties(const wstring& categoryName, UINT iIndex, ElementProps& lpProps);
bool SaveElementProperties(const wstring& categoryName, UINT iIndex, const ElementProps& lpProps);

//array-to-string conversion
wstring StrJoin(vector<wstring> arrStr, const wchar_t* lpDelim = L",");
wstring StrJoinFromInt(vector<int> arrStr, const wchar_t* lpDelim = L",");
vector<wstring> StrSplit(const wstring& lpSource, const wchar_t lpDelim = L',');
vector<int> StrSplitToInt(const wstring& lpSource, const wchar_t lpDelim = L',');
//-------------------------------------------------------------------------------------------------------------------------
// 
//                                        C++ Classes
// 
//-------------------------------------------------------------------------------------------------------------------------

#define TCN_TABORDERCHANGED (TCN_FIRST - 5)

class TabControl : public CTabCtrl{
public:
	TabControl();
	virtual ~TabControl();

	// Command/Notification Handlers
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd*);

private:
	bool  m_bDragging = false;     // Specifies that whether drag 'n drop is in progress.
	UINT  m_nSrcTab = 0;           // Specifies the source tab that is going to be moved.
	UINT  m_nDstTab = 0;           // Specifies the destination tab (drop position).
	bool  m_bHotTracking = false;  // Specifies the state of whether the tab control has hot tracking enabled.

	CRect m_InsertPosRect = {0,0,0,0};

	CSpinButtonCtrl* m_pSpinCtrl = nullptr;

	// Utility members
	bool DrawIndicator(CPoint point);
public:
bool ReorderTab(UINT nSrcTab, UINT nDstTab);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

// CCustomListCtrl
class CCustomListCtrl : public CListCtrl{
	protected:
	DECLARE_MESSAGE_MAP()
	public:
	afx_msg void OnPaint();
};


class CProgramLauncher : public CWinApp{
	public:
	vector<shared_ptr<CCategory>> vCategories;
	IniParser* ini;
	bool bRebuildIconCache = false;
	
	struct _options{
		UINT ShowAppNames =         DEFAULT_SHOWAPPNAMES;
		bool CloseAfterLaunch =     DEFAULT_CLOSEAFTERLAUNCH;
		UINT WindowWidth =          DEFAULT_WINDOW_WIDTH;
		UINT WindowHeight =         DEFAULT_WINDOW_HEIGHT;
		bool UseIconCaching =       DEFAULT_USEICONCACHING;
		int IconSize =              DEFAULT_ICON_SIZE;
		int IconSpacingHorizontal = DEFAULT_HORZ_SPACING;
		int IconSpacingVertical =   DEFAULT_VERT_SPACING;
	} options;

	CProgramLauncher() noexcept;
	~CProgramLauncher();

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void Save();
	bool NewItem();
	bool NewCategory();
	void ReindexCategories(UINT iStart = 0);
	void RebuildIconCache();

	int GetCurrentCategoryIndex();
	CCategory* GetCategory(UINT index);
	CCategory* GetCurrentCategory();
	int GetSelectedItemIndex();
	int GetFocusedItemIndex();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnAppAbout();
};

class CLauncherWnd : public CFrameWnd{

	public:
	CLauncherWnd() noexcept;
	protected:
	DECLARE_DYNAMIC(CLauncherWnd)

	// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
	virtual ~CLauncherWnd();

	// Tab control
	TabControl CTab;
	bool CreateTabControl();
	void ResizeTabControl();
	bool DoTabControlContextMenu(UINT iItemId, INT pX, INT pY);
	void UpdateMenu(CMenu* menu);

	// List view
	CCustomListCtrl CList;
	bool CreateListView();
	void UpdateListView(bool bUpdateImageList = false);
	void ResizeListView();
	void SwitchListView(DWORD dwView);
	void ResetListViewColumns();
	bool DoListViewContextMenu(INT pX, INT pY);
	void StopDragging(CPoint* point);

	CStatusBar m_statusBar;

	protected:
	int m_clickedTab = -1;
	bool m_bDragging = false;

	// Generated message map functions
	protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

	public:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);


	afx_msg void OnFilePref();
	afx_msg void OnAddCategory();
	afx_msg void OnRemoveCurrentCategory();
	afx_msg void OnRenameCurrentCategory();
	afx_msg void OnExit();
	afx_msg void OnAddNewButton();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonLaunch();
	afx_msg void OnButtonRemove();
	afx_msg void OnRemoveCategory();
	afx_msg void OnRenameCategory();
	afx_msg void OnViewDetails();
	afx_msg void OnViewLargeicons();
	afx_msg void OnViewList();
	afx_msg void OnViewSmallicons();
	afx_msg void OnButtonMoveLeft();
	afx_msg void OnButtonMoveRight();
	afx_msg void OnCategoryMoveLeft();
	afx_msg void OnCategoryMoveRight();
	afx_msg void OnCurCategoryMoveLeft();
	afx_msg void OnCurCategoryMoveRight();
	
	afx_msg void OnResetColumns();
	afx_msg void OnRefresh();
	afx_msg void OnSave();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};



class CCategory{
	public:
	CProgramLauncher* ptrParent;
	UINT parentIndex;

	wstring wsCategoryName;
	vector<shared_ptr<CLauncherItem>> vItems;

	CImageList imSmall;
	CImageList imLarge;

	CCategory(CProgramLauncher* ptrParent, const UINT parentIndex, const wstring& wsName);
	int LoadElements();

	//Actions
	bool Rename();
	bool Remove(bool bAsk = true, bool bKeepItems = true);
	bool Move(UINT newPos, bool bRelative = false);
	bool MoveTab(UINT newPos, bool bRelative = false);

	CLauncherItem* GetItem(UINT index);
	CLauncherItem* GetSelectedItem();

	void UpdateImageLists(int nIconSizeOfLargeImageList = -1);
	void ReindexItems(UINT iStart = 0);
};

class CLauncherItem{
public:
	CCategory* ptrParent;
	UINT parentIndex;

	ElementProps props;

	//don't use
	wstring& wsPath = props.wsPath;			//path to the executable
	wstring& wsPath64 = props.wsPath64;		//path to the 64-bit version of the executable
	wstring& wsPathIcon = props.wsPathIcon;	//path to the icon
	wstring& wsName = props.wsName;			//name that is displayed under icon / in tooltip
	int&     iIconIndex = props.iIconIndex;
	bool&   bAdmin = props.bAdmin;			//run program as admin
	bool&   bAbsolute = props.bAbsolute;	//use absolute paths

	HICON hItemIcon = null;
	HICON hSmallIcon = null;

	CLauncherItem(CCategory* ptrParent, const UINT parentIndex, const wstring& wsName, const wstring& wsPath, const wstring& wsPathIcon = L"", const int iIconIndex = 0, const wstring& wsPath64 = L"", const bool bAdmin = false, const bool bAbsolute = false);
	CLauncherItem(CCategory* ptrParent, const UINT parentIndex, const ElementProps& props);
	~CLauncherItem();
	
	HICON LoadIcon(UINT nIconSize, HICON* phSmall = null);

	bool Edit();
	void Remove(bool bAsk = true);
	bool Move(UINT newPos, bool bRelative = false);
	bool Launch() const;
	void InsertIntoListView();
	void UpdateInListView();
};


#include "Dialogs.h"



