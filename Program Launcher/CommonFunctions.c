//CommonFunctions.c
//
//
//
//
//

#include "Program Launcher.h"


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
// @brief Gets number of entries (buttons) in the specified category
// @param nCategoryIndex - Index of the category
// @return Number of entries in the specified category, or 0 if there is no entry, category does not exist or an error occured
//
INT GetCategorySectionCount(UINT nCategoryIndex){
	TCHAR szDummyString[2];

	TCHAR lpszIndex[INT_LEN_IN_DIGITS+6];
	INT i = 0;
	_stprintf_s(lpszIndex, INT_LEN_IN_DIGITS+6, TEXT("%i.Path"), i);

	while(i < MAX_ENTRIES_IN_CATEGORY){
		if(!GetSettingString(arrszCategoryList[nCategoryIndex], lpszIndex, szDummyString, 2, TEXT("x"))){
			return 0;
		}
		if(szDummyString[0] == TEXT('x')){
			break;
		}
		i++;
		_stprintf_s(lpszIndex, INT_LEN_IN_DIGITS+6, TEXT("%i.Path"), i);
	}
	return i;
}









// DEPRECATED, don't use
void ForceWindowRepaint(HWND hWnd){
	//WINDOWPLACEMENT wndPlace;
	//wndPlace.length = sizeof(WINDOWPLACEMENT);

	UpdateImageLists(hwndMainListView, GetSettingInt(TEXT("appereance"), TEXT("IconSize"), DEFAULT_ICON_SIZE));

	//resize window to repaint it
	//GetWindowPlacement(hWnd, &wndPlace);

	//wndPlace.rcNormalPosition.right++;
	//SetWindowPlacement(hWnd, &wndPlace);

	//wndPlace.rcNormalPosition.right--;
	//SetWindowPlacement(hWnd, &wndPlace);

	InvalidateRect(hWnd, NULL, TRUE);

	
	//force list refresh
	//SwitchListView(hwndListView, LVS_SMALLICON);
	//SwitchListView(hwndListView, LVS_ICON);
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

//
// @brief Creates a tab control, sized to fit the specified parent window's client area, and adds some tabs. 
// @param hwndParent - parent window (the application's main window). 
// @return Returns the handle to the tab control. 
// 
HWND CreateTabControl(HWND hwndParent){

	DWORD dwStyle;
	TCITEM tie;

	if(nCategoryCount <= 0){
		return NULL;
	}

	dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP/*| TCS_BOTTOM*/ | TCS_MULTILINE;

	hwndTab = CreateWindowExW(WS_EX_CONTROLPARENT, WC_TABCONTROL, L"", dwStyle, 0, 0, 0, 0, hwndParent, (HMENU)65531, hInst, NULL);

	if(hwndTab == NULL){
		return NULL;
	}
	ResizeTabControl(hwndTab, hwndMain);

	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;

	for(int i = 0; i < nCategoryCount; i++){
		tie.pszText = arrszCategoryList[i];
		TabCtrl_InsertItem(hwndTab, i, &tie);
	}

	return hwndTab;
}

//
// @brief Fit the specified tab control into parent window's client area
// @param hwndTabControl - handle to tab control
// @param hwndParent - handle to parent window (the application's main window). 
// 
void ResizeTabControl(HWND hwndTabControl, HWND hwndParent){
	RECT rc;
	GetClientRect(hwndParent, &rc);

	MoveWindow(hwndTabControl,
		rc.left,
		rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		TRUE);
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
		WCHAR szBuffer[100 + 2 * INT_STR_LEN] = {0};
		_snwprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, L"An error occurred calling FormatMessage().\nError Code: %d\nFormatMessage failure error code: %d", dwLastError, GetLastError());
		MessageBoxW(hwndMain, szBuffer, L"Error", MB_ICONERROR);
		return;
	}

	// Display the error information along with the place the error happened.
	//_sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, TEXT("Generic, Line=%d, File=%s"), /*wLine*/0, /*lpszFile*/"0");
#ifdef _DEBUG
	//display more info if debug mode is active
	lpszBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (wcslen((LPCWSTR)lpvMessage) + 401) * sizeof(WCHAR));
	if(lpszBuffer == NULL) goto FMT_MESSAGE_ERROR;

	_snwprintf_s(lpszBuffer, (wcslen((LPCWSTR)lpvMessage) + 401), _TRUNCATE, L"%s\nError Code: %d\nFile: %s\nFunction: %s\nLine: %i", (LPWSTR)lpvMessage, GetLastError(), lpszFile, lpszFunctionName, wLine);
	MessageBoxW(hwndMain, lpszBuffer, L"Error", MB_ICONERROR | MB_OK);
	HeapFree(GetProcessHeap(), 0, lpszBuffer);
#else
	MessageBoxW(hwndMain, (LPWSTR)lpvMessage, L"Error", MB_ICONERROR | MB_OK);
#endif
	LocalFree(lpvMessage);
}

//
// @brief Launches program based on button index and category
// @param nBtnId - id of the button
// @param nCategoryIndex - id of category
// @return 1 if success, 0 if fail (Returns the handle to the new process)
// 
INT LaunchProgram(INT nBtnId, INT nCategoryIndex){

	TCHAR szDirectory[MAX_PATH];
	LAUNCHERENTRY ButtonProps;

	if(!GetSettingButtonStruct(arrszCategoryList[nCategoryIndex], nBtnId, &ButtonProps, TRUE)){
		ErrorHandlerDebug();
		return 0;
	}

	//expand enviromnent variables
	LPTSTR lpszTempPath = _tcsdup(ButtonProps.szPath);
	ExpandEnvironmentStrings(lpszTempPath, ButtonProps.szPath, MAX_PATH);
	lpszTempPath = _tcsdup(ButtonProps.szPath64);
	ExpandEnvironmentStrings(lpszTempPath, ButtonProps.szPath64, MAX_PATH);
	free(lpszTempPath);

	SHELLEXECUTEINFO shExInfo;
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = 0;//SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = hwndMain;
	if(ButtonProps.bAdmin){
		shExInfo.lpVerb = TEXT("runas");
	}
	else{
		shExInfo.lpVerb = NULL;
	}
	if(Is64BitWindows() && ButtonProps.szPath64[0] != TEXT('0')){
		shExInfo.lpFile = ButtonProps.szPath64;
		_tcscpy_s(szDirectory, MAX_PATH, ButtonProps.szPath64);
		PathRemoveFileSpec(szDirectory);
		shExInfo.lpDirectory = szDirectory;
	}
	else{
		shExInfo.lpFile = ButtonProps.szPath;	// Application to start   
		_tcscpy_s(szDirectory, MAX_PATH, ButtonProps.szPath);
		PathRemoveFileSpec(szDirectory);
		shExInfo.lpDirectory = szDirectory;
	}
	shExInfo.lpParameters = TEXT("");				// Additional parameters
	shExInfo.nShow = SW_SHOW;
	shExInfo.hInstApp = 0;

	if(!ShellExecuteEx(&shExInfo)){
		//ErrorHandler(TEXT("Program Execution Failed"), TEXT("LaunchProgram"));
		ErrorHandler();
		return 0;
	}

	if(GetSettingInt(TEXT("general"), TEXT("CloseAfterLaunch"), DEFAULT_CLOSEAFTERLAUNCH)){
		DestroyWindow(hwndMain);
	}

	return 1;
}



INT_PTR CALLBACK UserInputProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	TCHAR lpszBuffer[CATEGORY_NAME_LEN + 2];
	WORD cchBuffer;
	static int iCatIndex;

	switch(message){
		case WM_INITDIALOG:
		{
			//center the dialog
			CenterDialog(hwndMain, hDlg);

			iCatIndex = lParam;

			// Set the default push button to "Cancel." 
			SendMessage(hDlg, DM_SETDEFID, (WPARAM)IDCANCEL, (LPARAM)0);
			//set maximum number of characters to CATEGORY_NAME_LEN
			SendDlgItemMessage(hDlg, IDC_USERINPUTDLGEDIT, EM_SETLIMITTEXT, (WPARAM)CATEGORY_NAME_LEN, (LPARAM)0);

			if(iCatIndex >= 0){
				SetWindowText(hDlg, LoadLocalString(IDS_RENAME_CATEGORY));
			}

			return TRUE;
		}
		case WM_COMMAND:;
			int wmId = LOWORD(wParam);
			int wmNotify = HIWORD(wParam);

			// Set the default push button to "OK" when the user enters text. 
			if(HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_USERINPUTDLGEDIT){
				SendMessage(hDlg, DM_SETDEFID, (WPARAM)IDOK, (LPARAM)0);
			}

			switch(wmId){
				case IDOK:
					// Get number of characters. 
					cchBuffer = (WORD)SendDlgItemMessage(hDlg, IDC_USERINPUTDLGEDIT, EM_LINELENGTH, (WPARAM)0, (LPARAM)0);

					if(cchBuffer >= CATEGORY_NAME_LEN){

						MessageBox(hDlg, LoadLocalString(IDS_NAME_TOO_LONG), szError, MB_ICONEXCLAMATION | MB_OK);
						return FALSE;
					}
					else if(cchBuffer == 0){

						MessageBox(hDlg, LoadLocalString(IDS_ENTER_VALID_NAME), szError, MB_ICONEXCLAMATION | MB_OK);
						return FALSE;
					}

					// Put the number of characters into first word of buffer. 
					*((LPWORD)lpszBuffer) = cchBuffer;

					// Get the characters. 
					SendDlgItemMessage(hDlg, IDC_USERINPUTDLGEDIT, EM_GETLINE, (WPARAM)0, (LPARAM)lpszBuffer);

					// Null-terminate the string. 
					lpszBuffer[cchBuffer] = 0;

					if(_tcschr(lpszBuffer, TEXT(';')) != NULL){
						MessageBox(hDlg, LoadLocalString(IDS_NAME_INVALID_CHARACTERS), szError, MB_ICONEXCLAMATION | MB_OK);
						return FALSE;
					}

					if(iCatIndex == -1){
						AddCategory(lpszBuffer);
					}
					else{
						RenameCategory(iCatIndex, lpszBuffer);
					}

					EndDialog(hDlg, TRUE);
					return TRUE;
					break;

				case IDCANCEL:
					EndDialog(hDlg, TRUE);
					return TRUE;
					break;
			}
			return 0;
	}
	return FALSE;

	UNREFERENCED_PARAMETER(lParam);
}



LPWSTR LoadLocalString(UINT uID){
	//WCHAR useless[8];
	//INT strLen = LoadStringW(GetModuleHandleW(NULL), uID, useless, 0);
	//int error = GetLastError();

	int nHeapSize = HeapSize(GetProcessHeap(), 0, lpszLoadString);
	LoadStringW(GetModuleHandleW(NULL), uID, lpszLoadString, nHeapSize);
	return lpszLoadString;
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


//
// @brief Updates popup menu before it shows
// @param hwndListView - handle to list view from which get the current view
// @param hMenu - handle to menu
// 
void UpdateMenu(HWND hwndListView, HMENU hMenu){
	UINT  uID = ID_VIEW_LARGEICONS;
	DWORD dwStyle;

	//uncheck all of these guys
	//CheckMenuItem(hMenu, ID_VIEW_LARGEICONS, MF_BYCOMMAND | MF_UNCHECKED);
	//CheckMenuItem(hMenu, ID_VIEW_SMALLICONS, MF_BYCOMMAND | MF_UNCHECKED);
	//CheckMenuItem(hMenu, ID_VIEW_LIST, MF_BYCOMMAND | MF_UNCHECKED);
	//CheckMenuItem(hMenu, ID_VIEW_DETAILS, MF_BYCOMMAND | MF_UNCHECKED);
	int nItemId = ListView_GetNextItem(hwndListView, -1, LVNI_SELECTED);
	int state = 0;

	if(nItemId > ListView_GetItemCount(hwndListView) || nItemId < 0){
		state = 1;
	}
	EnableMenuItem(hMenu, ID_BUTTON_LAUNCH, state);
	EnableMenuItem(hMenu, ID_BUTTON_EDIT, state);
	EnableMenuItem(hMenu, ID_BUTTON_MOVELEFT, state);
	EnableMenuItem(hMenu, ID_BUTTON_MOVERIGHT, state);
	EnableMenuItem(hMenu, ID_BUTTON_REMOVE, state);

	//check the appropriate view menu item
	dwStyle = GetWindowLong(hwndListView, GWL_STYLE);
	switch(dwStyle & LVS_TYPEMASK)   {
		case LVS_ICON:
			uID = ID_VIEW_LARGEICONS;
			break;

		case LVS_SMALLICON:
			uID = ID_VIEW_SMALLICONS;
			break;

		case LVS_LIST:
			uID = ID_VIEW_LIST;
			break;

		case LVS_REPORT:
			uID = ID_VIEW_DETAILS;
			break;
	}
	CheckMenuRadioItem(hMenu, ID_VIEW_DETAILS, ID_VIEW_SMALLICONS, uID, MF_BYCOMMAND | MF_CHECKED);
}



INT DoTabControlContextMenu(INT iItemId, INT pX, INT pY){
	POINT displayCoords;

	if(pX == -1 && pY == -1){
		RECT rcItem;
		TabCtrl_GetItemRect(hwndTab, iItemId, &rcItem);

		displayCoords.x = rcItem.right - (rcItem.right - rcItem.left) / 2;
		displayCoords.y = rcItem.bottom - (rcItem.bottom - rcItem.top) / 2;
		ClientToScreen(hwndTab, &displayCoords);
	}
	else{
		displayCoords.x = pX;
		displayCoords.y = pY;
	}

	HMENU hMenu = LoadMenuW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDR_CATMENU));
	hMenu = GetSubMenu(hMenu, 0);

	//disable useless menu items when clicked outside button
	if(iItemId > TabCtrl_GetItemCount(hwndTab) || iItemId < 0){
		// Handle useless items
		//EnableMenuItem(hMenu, ID_POPUPBTN_EDIT, MF_BYCOMMAND | MF_GRAYED);
		//EnableMenuItem(hMenu, ID_POPUPBTN_LAUNCH, MF_BYCOMMAND | MF_GRAYED);
		//EnableMenuItem(hMenu, ID_POPUPBTN_MOVELEFT, MF_BYCOMMAND | MF_GRAYED);
		//EnableMenuItem(hMenu, ID_POPUPBTN_MOVERIGHT, MF_BYCOMMAND | MF_GRAYED);
		//EnableMenuItem(hMenu, ID_POPUPBTN_REMOVE, MF_BYCOMMAND | MF_GRAYED);
	}
	//SetMenuDefaultItem(hMenu, ID_POPUPBTN_LAUNCH, FALSE);

	TrackPopupMenu(hMenu, 0, displayCoords.x, displayCoords.y, 0, hwndMain, NULL);
	return TRUE;





	//RECT tabItemRect;
	//POINT clickedCoords;
	//POINT displayCoords;
	////INT nClickedTab = -1;
	//
	//clickedCoords.x = GET_X_LPARAM(lParam);
	//clickedCoords.y = GET_Y_LPARAM(lParam);
	//displayCoords.x = GET_X_LPARAM(lParam);
	//displayCoords.y = GET_Y_LPARAM(lParam);
	//ScreenToClient(hWnd, &clickedCoords);
	//
	//for(int i = 0; i < nCategoryCount; i++){
	//	TabCtrl_GetItemRect(hwndTab, i, &tabItemRect);
	//	if(PtInRect(&tabItemRect, clickedCoords)){
	//		nClickedTab = i;
	//		break;
	//	}
	//	else{
	//		nClickedTab = -1;
	//	}
	//}
	//
	//if(nClickedTab >= 0){
	//
	//	//MENUITEMINFO mInfo;
	//
	//	//mInfo.cbSize = sizeof(MENUITEMINFO);
	//	//GetMenuItemInfo(hMenu, ID_POPUPCATEGORY_REMOVECATEGORY, MF_BYCOMMAND, &mInfo);
	//	//mInfo.fMask = MIIM_BITMAP;// | MIIM_DATA;
	//	//mInfo.hbmpItem = HBMMENU_SYSTEM;// HBMMENU_MBAR_CLOSE;
	//	//mInfo.hbmpItem = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAMLAUNCHER), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//	//int error = GetLastError();
	//	//mInfo.dwItemData = (ULONG_PTR)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_PROGRAMLAUNCHER));
	//	//SetMenuItemInfo(hMenu, ID_POPUPCATEGORY_REMOVECATEGORY, MF_BYCOMMAND, &mInfo);
	//}
}

