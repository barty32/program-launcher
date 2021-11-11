//CommonFunctions.c
//
//
//
//
//

#include "Program Launcher.h"





int GetElementCount(LPCWSTR lpszCategory){
	int i = 0;
	while(i < MAX_ENTRIES_IN_CATEGORY){
		try{
			ReadIniValue(lpszCategory, wstring(to_wstring(i) + L".Path").c_str());
		}
		catch(const std::runtime_error&){
			break;
		}
		i++;
	}
	return i;
}



//
// @brief Creates a tab control, sized to fit the specified parent window's client area, and adds some tabs. 
// @return 1 ok, 0 fail
// 
int CProgramLauncher::CreateTabControl(){

	if(vCategories.size() <= 0){
		return 0;
	}
	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP/*| TCS_BOTTOM*/ | TCS_MULTILINE;

	if(!CTab.CreateEx(WS_EX_CONTROLPARENT, dwStyle, RECT{0,0,0,0}, &CMainWnd, ID_TABCONTROL)){
		return 0;
	}
	ResizeTabControl();

	for(size_t i = 0; i < vCategories.size(); i++){
		CTab.InsertItem(i, vCategories[i]->wsCategoryName.c_str());
	}
	return 1;
}

//
// @brief Fit the specified tab control into parent window's client area
// @param hwndTabControl - handle to tab control
// @param hwndParent - handle to parent window (the application's main window). 
// 
void CProgramLauncher::ResizeTabControl(){
	RECT rc;
	CMainWnd.GetClientRect(&rc);
	CTab.MoveWindow(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}




wstring ExpandEnvStrings(wstring &source){
	vector<WCHAR> vTemp(source.begin(), source.end());
	vTemp.resize(MAX_PATH, 0);
	ExpandEnvironmentStringsW(source.data(), &vTemp[0], vTemp.size());
	return wstring(&vTemp[0]);
}

wstring RemoveFileSpecFromPath(wstring& path){
	vector<WCHAR> vTemp(path.begin(), path.end());
	PathRemoveFileSpecW(&vTemp[0]);
	return wstring(&vTemp[0]);
	//return wstring{strRes, static_cast<size_t>(cch)};
}

int CProgramLauncher::Exit(){
	CMainWnd.DestroyWindow();
	return 1;
}

int CProgramLauncher::NewItem(){
	shared_ptr<CLauncherItem> pNewItem = make_shared<CLauncherItem>(nullptr, -1, L"", L"");
	if(DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_BTNSETTINGDLG), CMainWnd.GetSafeHwnd(), BtnEditDlgProc, (LPARAM)pNewItem.get())){
		vCategories.at(pNewItem->parentIndex)->vItems.push_back(pNewItem);
		pNewItem->parentIndex = vCategories.at(pNewItem->parentIndex)->vItems.size() - 1;
		pNewItem->InsertIntoListView();
		return 1;
	}
	return 0;
}

int CProgramLauncher::NewCategory(){
	wstring wsNewName;
	if(DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_USERINPUTDLG), CMainWnd.GetSafeHwnd(), UserInputProc, (LPARAM)&wsNewName)){
		vCategories.push_back(make_shared<CCategory>(this, vCategories.size(), wsNewName));
		//TODO: insert tab
		return 1;
	}
	return 0;
}
int CProgramLauncher::RemoveCategory(int index){
	//delete vCategories[index];
	vCategories.erase(vCategories.begin() + index);
	//TODO: remove from tab control
	return 1;
}
int CProgramLauncher::RenameCategory(int index){
	wstring wsName(GetCategory(index)->wsCategoryName);
	if(DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_USERINPUTDLG), CMainWnd.GetSafeHwnd(), UserInputProc, (LPARAM)&wsName)){
		GetCategory(index)->wsCategoryName = wsName;
		//TODO: change tab name
		return 1;
	}
	return 0;
}
int CProgramLauncher::MoveCategory(int index, int newPos){
	//TODO: make algorithm
	return 1;
}


CCategory* CProgramLauncher::GetCategory(int index){
	return vCategories.at(index).get();
}
int CProgramLauncher::GetCurrentCategoryIndex(){
	return CTab.GetCurSel();
}
CCategory* CProgramLauncher::GetCurrentCategory(){
	return vCategories.at(GetCurrentCategoryIndex()).get();
}








wstring GetString(UINT uID){
	LPWSTR strRes = nullptr;
	int cch = LoadStringW(hInst, uID, reinterpret_cast<LPWSTR>(&strRes), 0);
	return wstring{strRes, static_cast<size_t>(cch)};
}

//
// @brief Separates (removes) icon index from specified string
// @param pszIconPath - Path to remove index from
// @return separated icon index
//
INT GetIconIndexFromPath(LPWSTR pszIconPath){

	LPWSTR lpCommaPosition;
	int nLen = wcslen(pszIconPath);
	int iIconIndex = 0;

	for(int i = nLen; i >= 0; i--){
		if(pszIconPath[i] == L','){
			lpCommaPosition = pszIconPath + i;
			iIconIndex = _wtoi(lpCommaPosition + 1);
			*lpCommaPosition = 0;
			break;
		}
	}
	return iIconIndex;
}



void ErrorHandlerEx(LONG wLine, LPCWSTR lpszFile, LPCWSTR lpszFunctionName){

	LPVOID  lpvMessage;
	DWORD   dwError;
	LPWSTR  lpszBuffer;
	DWORD   dwLastError = GetLastError();

	// Allow FormatMessage() to look up the error code returned by GetLastError
	dwError = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPWSTR)&lpvMessage,
		0,
		NULL
	);

	// Check to see if an error occurred calling FormatMessage()
	if(dwError == 0){
	FMT_MESSAGE_ERROR:;
		WCHAR szBuffer[100 + 2 * INT_STR_LEN] ={0};
		_snwprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, L"An error occurred calling FormatMessage().\nError Code: %d\nFormatMessage failure error code: %d", dwLastError, GetLastError());
		MessageBoxW(CMainWnd.GetSafeHwnd(), szBuffer, L"Error", MB_ICONERROR);
		return;
	}

	// Display the error information along with the place the error happened.
	//_sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, TEXT("Generic, Line=%d, File=%s"), /*wLine*/0, /*lpszFile*/"0");
#ifdef _DEBUG
	//display more info if debug mode is active
	lpszBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (wcslen((LPCWSTR)lpvMessage) + 401) * sizeof(WCHAR));
	if(lpszBuffer == NULL) goto FMT_MESSAGE_ERROR;

	_snwprintf_s(lpszBuffer, (wcslen((LPCWSTR)lpvMessage) + 401), _TRUNCATE, L"%s\nError Code: %d\nFile: %s\nFunction: %s\nLine: %i", (LPWSTR)lpvMessage, GetLastError(), lpszFile, lpszFunctionName, wLine);
	MessageBoxW(CMainWnd.GetSafeHwnd(), lpszBuffer, L"Error", MB_ICONERROR | MB_OK);
	HeapFree(GetProcessHeap(), 0, lpszBuffer);
#else
	MessageBoxW(hwndMain, (LPWSTR)lpvMessage, L"Error", MB_ICONERROR | MB_OK);
#endif
	LocalFree(lpvMessage);
}


//
// @brief Updates popup menu before it shows
// @param hwndListView - handle to list view from which get the current view
// @param hMenu - handle to menu
// 
void UpdateMenu(HWND hwndListView, HMENU hMenu){
	UINT  uID = IDM_VIEW_LARGEICONS;
	DWORD dwStyle;

	int nItemId = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
	int state = 0;

	if(nItemId > ListView_GetItemCount(hwndListView) || nItemId < 0){
		state = 1;
	}
	EnableMenuItem(hMenu, IDM_BUTTON_LAUNCH, state);
	EnableMenuItem(hMenu, IDM_BUTTON_EDIT, state);
	EnableMenuItem(hMenu, IDM_BUTTON_MOVELEFT, state);
	EnableMenuItem(hMenu, IDM_BUTTON_MOVERIGHT, state);
	EnableMenuItem(hMenu, IDM_BUTTON_REMOVE, state);

	//check the appropriate view menu item
	dwStyle = GetWindowLong(hwndListView, GWL_STYLE);
	switch(dwStyle & LVS_TYPEMASK)   {
		case LVS_ICON:
			uID = IDM_VIEW_LARGEICONS;
			break;

		case LVS_SMALLICON:
			uID = IDM_VIEW_SMALLICONS;
			break;

		case LVS_LIST:
			uID = IDM_VIEW_LIST;
			break;

		case LVS_REPORT:
			uID = IDM_VIEW_DETAILS;
			break;
	}
	CheckMenuRadioItem(hMenu, IDM_VIEW_DETAILS, IDM_VIEW_SMALLICONS, uID, MF_BYCOMMAND | MF_CHECKED);
}



INT CProgramLauncher::DoTabControlContextMenu(INT iItemId, INT pX, INT pY){
	POINT displayCoords;

	if(pX == -1 && pY == -1){
		RECT rcItem;
		CTab.GetItemRect(iItemId, &rcItem);

		displayCoords.x = rcItem.right - (rcItem.right - rcItem.left) / 2;
		displayCoords.y = rcItem.bottom - (rcItem.bottom - rcItem.top) / 2;
		CTab.ClientToScreen(&displayCoords);
	}
	else{
		displayCoords.x = pX;
		displayCoords.y = pY;
	}

	HMENU hMenu = LoadMenuW(hInst, MAKEINTRESOURCEW(IDR_CATMENU));
	hMenu = GetSubMenu(hMenu, 0);

	TrackPopupMenu(hMenu, 0, displayCoords.x, displayCoords.y, 0, CMainWnd.GetSafeHwnd(), NULL);
	return TRUE;
}


//
// @brief Removes drive letter from specified path
// @param lpszPath - Path to remove letter from
// @return Returns 1 if letter was removed, 0 if not found
//
INT RemoveLetterFromPath(LPWSTR lpszPath){
	WCHAR delim = L':';
	BOOL bFoundDelim = FALSE;
	int i;
	LPWSTR lpNewStart = lpszPath;
	int len;

	if(lpszPath == NULL){
		return 0;
	}

	len = wcslen(lpszPath);

	for(i = 0; i <= len; i++){
		if(lpszPath[i] == 0){
			break;
		}
		if(lpszPath[i] == delim){
			bFoundDelim = TRUE;
			lpNewStart = lpszPath + i + 1;
			break;
		}
	}
	if(bFoundDelim){
		wcscpy_s(lpszPath, len + 1, lpNewStart);
		return 1;
	}
	return 0;
}

//
// @brief Adds drive letter from specified path and expands enviromnent variables
// @param lpszPath - Path to add letter to
// @return Returns 1 if letter was added, 0 if it already exists
//
INT AddLetterToPath(LPWSTR pszPath, INT cchSize){

	WCHAR szDriveLetter[2];
	WCHAR delim = L':';
	BOOL bFoundDelim = FALSE;

	if(pszPath == NULL){
		return 0;
	}
	if(pszPath[0] == L'0'){
		return 1;
	}

	// Get current drive letter
	if(!GetModuleFileNameW(NULL, szDriveLetter, 2)){
		ErrorHandlerDebug();
	}

	// Check for enviromnent variables

	// Get expanded string length by passing a zero buffer size
	int cchExpandedPathSize = ExpandEnvironmentStringsW(pszPath, NULL, 0);

	// Allocate memory for expanded path
	LPWSTR lpszExpandedPath = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchExpandedPathSize * sizeof(WCHAR));
	if(lpszExpandedPath == NULL) return 0;

	// Expand the path
	ExpandEnvironmentStringsW(pszPath, lpszExpandedPath, cchExpandedPathSize);

	// Check if drive letter already exists
	for(int i = 0; i < cchExpandedPathSize; i++){
		if(lpszExpandedPath[i] == 0){
			break;
		}
		if(lpszExpandedPath[i] == delim){
			bFoundDelim = TRUE;
			break;
		}
	}

	if(!bFoundDelim){
		_snwprintf_s(pszPath, cchSize, _TRUNCATE, L"%s:%s", szDriveLetter, lpszExpandedPath);
		return 1;
	}

	wcscpy_s(pszPath, cchSize, lpszExpandedPath);
	return 0;
}


BOOL Is64BitWindows(){
#if defined(_WIN64)
	return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows, so must sniff
	BOOL f64 = FALSE;
	return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
	return FALSE; // Win64 does not support Win16
#endif
}

//
// @brief Centers hwndDialog in hwndParent
// @param hwndParent - Handle to parent window
// @param hwndDialog - Handle to dialog to be centered
//
void CenterDialog(HWND hwndParent, HWND hwndDialog){
	//center the dialog
	RECT rectWindow, rectParent;
	GetWindowRect(hwndDialog, &rectWindow);
	GetWindowRect(hwndParent, &rectParent);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
	int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// make sure that the dialog box never moves outside of the screen
	if(nX < 0) nX = 0;
	if(nY < 0) nY = 0;
	if(nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
	if(nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

	MoveWindow(hwndDialog, nX, nY, nWidth, nHeight, FALSE);
}

