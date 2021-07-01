//
//  ____                                        _                           _                               
// |  _ \ _ __ ___   __ _ _ __ __ _ _ __ ___   | |    __ _ _   _ _ __   ___| |__   ___ _ __ ___ _ __  _ __  
// | |_) | '__/ _ \ / _` | '__/ _` | '_ ` _ \  | |   / _` | | | | '_ \ / __| '_ \ / _ \ '__/ __| '_ \| '_ \ 
// |  __/| | | (_) | (_| | | | (_| | | | | | | | |__| (_| | |_| | | | | (__| | | |  __/ |_| (__| |_) | |_) |
// |_|   |_|  \___/ \__, |_|  \__,_|_| |_| |_| |_____\__,_|\__,_|_| |_|\___|_| |_|\___|_(_)\___| .__/| .__/ 
//                  |___/                                                                      |_|   |_|   
//
//  This file contains functions related to main window (WinMain, init functions and window procedure)
//
//


#include "Program Launcher.h"

#ifdef __cplusplus
// Global Variables:
HINSTANCE hInst;        // current instance

HWND hwndMain;
HWND hwndTab;
HWND hwndMainListView;

BOOL bAllowEnChange;
INT nCategoryCount = 0;

BOOL fRebuildIconCache = FALSE;

TCHAR szIniPath[MAX_PATH];
WCHAR szError[16];

LPWSTR lpszLoadString;

TCHAR arrszCategoryList[MAX_CATEGORIES][CATEGORY_NAME_LEN];

#else
fRebuildIconCache = FALSE;

#endif



//-----------------------------------------------------------------------------------------------------------------------------
// 
//                                                   WinMain
// 
//-----------------------------------------------------------------------------------------------------------------------------
INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow){

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hwndMain = NULL;
	hwndTab = NULL;
	hwndMainListView = NULL;

	// Initialize common controls.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);


	// Initialize global strings
	lpszLoadString = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, 512 * sizeof(WCHAR));

	// Register Window Class
	if(!MyRegisterClass(hInstance, LoadLocalString(IDC_PROGRAMLAUNCHER))){
		MessageBox(NULL, LoadLocalString(IDS_WND_REGISTRATION_FAILED), szError, MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// Perform application initialization
	if(!InitInstance(hInstance, nCmdShow, LoadLocalString(IDC_PROGRAMLAUNCHER))){
		MessageBox(NULL, LoadLocalString(IDS_WND_CREATION_FAILED), szError, MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROGRAMLAUNCHER));

	MSG msg;

	// Main message loop:
	while(GetMessageW(&msg, NULL, 0, 0)){
		if(!TranslateAcceleratorW(hwndMain, hAccelTable, &msg)){
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	HeapFree(GetProcessHeap(), 0, lpszLoadString);

	return (int)msg.wParam;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	static INT nClickedTab = -1;
	switch(message){
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch(wmId){
				case IDM_ABOUT:;
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;
				case IDM_FILE_PREF:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_PREFDLG), hWnd, PrefDlgProc);
					break;
				case ID_CATEGORY_ADDCATEGORY:
					DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_USERINPUTDLG), hWnd, UserInputProc, -1);
					break;
				case ID_CATEGORY_RENAMECURRENTCATEGORY:
					if(!GetCategoryCount()){
						MessageBox(hWnd, LoadLocalString(IDS_NO_CATEGORY), szError, MB_ICONERROR | MB_OK);
						break;
					}
					else{
						DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_USERINPUTDLG), hWnd, UserInputProc, TabCtrl_GetCurSel(hwndTab));
					}
					break;
				case ID_CATEGORY_REMOVECURRENTCATEGORY:
					RemoveCategory(SendMessage(hwndTab, TCM_GETCURSEL, (WPARAM)NULL, (LPARAM)NULL));
					break;
				case ID_POPUPCATEGORY_REMOVECATEGORY:
					RemoveCategory(nClickedTab);
					break;
				case ID_POPUPCATEGORY_RENAME:
					if(!GetCategoryCount()){
						MessageBox(hWnd, LoadLocalString(IDS_NO_CATEGORY), szError, MB_ICONERROR | MB_OK);
						break;
					}
					else{
						DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_USERINPUTDLG), hWnd, UserInputProc, nClickedTab);
					}
					break;
				case ID_CATEGORY_MOVELEFT:
					MoveCategory(TabCtrl_GetCurSel(hwndTab), TRUE);
					break;
				case ID_CATEGORY_MOVERIGHT:
					MoveCategory(TabCtrl_GetCurSel(hwndTab), FALSE);
					break;
				case ID_POPUPCATEGORY_MOVELEFT:
					MoveCategory(nClickedTab, TRUE);
					break;
				case ID_POPUPCATEGORY_MOVERIGHT:
					MoveCategory(nClickedTab, FALSE);
					break;
				case ID_BUTTON_ADDNEWBUTTON:
					DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_BTNSETTINGDLG), hWnd, BtnEditDlgProc, -1);
					break;
				case ID_VIEW_LARGEICONS:
					SwitchListView(hwndMainListView, LVS_ICON);
					break;
				case ID_VIEW_SMALLICONS:
					SwitchListView(hwndMainListView, LVS_SMALLICON);
					break;
				case ID_VIEW_LIST:
					SwitchListView(hwndMainListView, LVS_LIST);
					break;
				case ID_VIEW_DETAILS:
					SwitchListView(hwndMainListView, LVS_REPORT);
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				case ID_BUTTON_LAUNCH:
				{
					int nItemId = SendMessageW(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
					if(nItemId < ListView_GetItemCount(hwndMainListView) || nItemId > 0){
						LaunchProgram(nItemId, TabCtrl_GetCurSel(hwndTab));
					}
				}
				break;
				case ID_BUTTON_EDIT:
				{
					int nItemId = SendMessageW(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
					if(nItemId < ListView_GetItemCount(hwndMainListView) || nItemId > 0){
						DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_BTNSETTINGDLG), hWnd, BtnEditDlgProc, nItemId);
					}
				}
				break;
				case ID_BUTTON_MOVELEFT:
				{
					int nItemId = SendMessageW(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
					if(nItemId < ListView_GetItemCount(hwndMainListView) || nItemId > 0){
						MoveButton(nItemId, TRUE);
					}
				}
				break;
				case ID_BUTTON_MOVERIGHT:
				{
					int nItemId = SendMessageW(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
					if(nItemId < ListView_GetItemCount(hwndMainListView) || nItemId > 0){
						MoveButton(nItemId, FALSE);
					}
				}
				break;
				case ID_BUTTON_REMOVE:
				{
					int nItemId = SendMessageW(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
					if(nItemId < ListView_GetItemCount(hwndMainListView) || nItemId > 0){
						if(MessageBox(hWnd, LoadLocalString(IDS_REMOVE_BTN_PROMPT), L"Remove button", MB_ICONEXCLAMATION | MB_YESNO) == IDYES){
							RemoveButton(nItemId, TabCtrl_GetCurSel(hwndTab));
						}
					}
				}
				break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_CONTEXTMENU:
		{

			if((HWND)wParam == hwndMainListView){
				return DoListViewContextMenu((HWND)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}

			if((HWND)wParam == hwndTab && GET_X_LPARAM(lParam) == -1 && GET_Y_LPARAM(lParam)){
				nClickedTab = TabCtrl_GetCurFocus(hwndTab);
				return DoTabControlContextMenu(nClickedTab, -1, -1);
			}

			RECT tabItemRect;
			POINT clickedCoords;
			clickedCoords.x = GET_X_LPARAM(lParam);
			clickedCoords.y = GET_Y_LPARAM(lParam);
			ScreenToClient(hWnd, &clickedCoords);

			for(int i = 0; i < nCategoryCount; i++){
				TabCtrl_GetItemRect(hwndTab, i, &tabItemRect);
				if(PtInRect(&tabItemRect, clickedCoords)){
					nClickedTab = i;
					break;
				}
				else{
					nClickedTab = -1;
				}
			}
				
			if(nClickedTab >= 0){
				return DoTabControlContextMenu(nClickedTab, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			//clickedCoords.x = GET_X_LPARAM(lParam);
			//clickedCoords.y = GET_Y_LPARAM(lParam);
			//ScreenToClient(hwndListView, &clickedCoords);
			//for(int i = 0; i < GetCategorySectionCount(TabCtrl_GetCurSel(hwndTab)); i++){
			//	ListView_GetItemRect(hwndListView, i, &tabItemRect, LVIR_BOUNDS);
			//	if(PtInRect(&tabItemRect, clickedCoords)){
			//		//MessageBox(NULL, TEXT("context menu"), TEXT("Info"), MB_ICONINFORMATION | MB_OK);
			//		nButtonId = i;
			//		break;
			//	}
			//	else{
			//		nButtonId = -1;
			//	}
			//}
			//if((HWND)wParam == hwndListView && GET_X_LPARAM(lParam) == -1 && GET_Y_LPARAM(lParam) == -1){
			//	nButtonId = SendMessage(hwndListView, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
			//	ListView_GetItemRect(hwndListView, nButtonId, &tabItemRect, LVIR_BOUNDS);
			//
			//	displayCoords.x = tabItemRect.right -(tabItemRect.right - tabItemRect.left) / 2;
			//	displayCoords.y = tabItemRect.bottom -(tabItemRect.bottom - tabItemRect.top) / 2;
			//	ClientToScreen(hwndListView, &displayCoords);
			//}
			//
			//if((HWND)wParam == hwndListView){
			//
			//	HMENU hMenu = LoadMenu(GetInstanceModule(NULL), MAKEINTRESOURCE(IDR_BTNMENU));
			//	hMenu = GetSubMenu(hMenu, 0);
			//	
			//	//remove useless menu items when clicked outside button
			//	if(nButtonId > GetCategorySectionCount(TabCtrl_GetCurSel(hwndTab)) || nButtonId < 0){
			//		DeleteMenu(hMenu, ID_POPUPBTN_EDIT, MF_BYCOMMAND);
			//		DeleteMenu(hMenu, ID_POPUPBTN_LAUNCH, MF_BYCOMMAND);
			//		DeleteMenu(hMenu, ID_POPUPBTN_MOVELEFT, MF_BYCOMMAND);
			//		DeleteMenu(hMenu, ID_POPUPBTN_MOVERIGHT, MF_BYCOMMAND);
			//		DeleteMenu(hMenu, ID_POPUPBTN_REMOVE, MF_BYCOMMAND);
			//	}
			//	UpdateMenu(hwndListView, hMenu);
			//	TrackPopupMenu(hMenu, 0, displayCoords.x, displayCoords.y, 0, hWnd, NULL);
			//}

			return DefWindowProcW(hWnd, message, wParam, lParam);
		}
		break;
		case WM_NOTIFY:
		{
			//if(((LPNMHDR)lParam)->hwndFrom == hwndTab){
			//	if(((LPNMHDR)lParam)->code == NM_RCLICK){
			//		nClickedTab = ((LPNMHDR)lParam)->idFrom;
			//		return DoTabControlContextMenu(nClickedTab, 10, 10);
			//	}
			//}
			if(((LPNMTVKEYDOWN)lParam)->hdr.hwndFrom == hwndTab){
				if(((LPNMHDR)lParam)->code == TCN_SELCHANGE){
					UpdateListView(hwndMainListView, -1);
				}
				else if(((LPNMTVKEYDOWN)lParam)->hdr.code == TCN_KEYDOWN){
					if(((LPNMTVKEYDOWN)lParam)->wVKey == VK_TAB){
						SetFocus(hwndMainListView);
					}
				}
			}
			else if(((LPNMITEMACTIVATE)lParam)->hdr.hwndFrom == hwndMainListView){
				//if(((LPNMITEMACTIVATE)lParam)->hdr.code == LVN_GETDISPINFO){
				//	//plvdi = (NMLVDISPINFO*)lParam;
				//	LAUNCHERENTRY ButtonProps;
				//	if(((NMLVDISPINFO*)lParam)->item.iSubItem < 7 && ((NMLVDISPINFO*)lParam)->item.iSubItem > 0){
				//		GetSettingButtonStruct(arrszCategoryList[TabCtrl_GetCurSel(hwndTab)], ((NMLVDISPINFO*)lParam)->item.iItem, &ButtonProps, MAX_ENTRY_LEN, TRUE);
				//	}
				//	//((NMLVDISPINFO*)lParam)->item.mask = LVIF_DI_SETITEM;
				//	switch(((NMLVDISPINFO*)lParam)->item.iSubItem){
				//		case 1:
				//			((NMLVDISPINFO*)lParam)->item.pszText = ButtonProps.szPath;
				//			break;
				//	
				//		case 2:
				//			if(_tcscmp(ButtonProps.szPath64, TEXT("0"))){
				//				((NMLVDISPINFO*)lParam)->item.pszText = ButtonProps.szPath64;
				//			}
				//			break;
				//	
				//		case 3:
				//			((NMLVDISPINFO*)lParam)->item.pszText = ButtonProps.szPathIcon;
				//			break;
				//	
				//		case 4:;
				//			TCHAR szIconIndex[INT_LEN_IN_DIGITS];
				//			_itot_s(ButtonProps.nIconIndex, szIconIndex, INT_LEN_IN_DIGITS, 10);
				//			((NMLVDISPINFO*)lParam)->item.pszText = szIconIndex;
				//			break;
				//
				//		case 5:
				//			if(ButtonProps.bAdmin){
				//				((NMLVDISPINFO*)lParam)->item.pszText = LoadLocalString(IDS_YES);
				//			}
				//			else{
				//				((NMLVDISPINFO*)lParam)->item.pszText = LoadLocalString(IDS_NO);
				//			}
				//			break;
				//
				//		case 6:
				//			if(ButtonProps.bAbsolute){
				//				((NMLVDISPINFO*)lParam)->item.pszText = LoadLocalString(IDS_YES);
				//			}
				//			else{
				//				((NMLVDISPINFO*)lParam)->item.pszText = LoadLocalString(IDS_NO);
				//			}
				//			break;
				//	
				//		default:
				//			break;
				//	}
				//	return DefWindowProc(hWnd, message, wParam, lParam);
				//}
				if(((LPNMITEMACTIVATE)lParam)->hdr.code == NM_DBLCLK){
					INT iItemIndex = ((LPNMITEMACTIVATE)lParam)->iItem;
					if(iItemIndex >= 0 && iItemIndex < ListView_GetItemCount(hwndMainListView)){
						LaunchProgram(iItemIndex, TabCtrl_GetCurSel(hwndTab));
					}
					return 0;
				}
				else if(((LPNMHDR)lParam)->code == NM_RETURN){
					INT iItemIndex = SendMessage(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
					if(iItemIndex >= 0 && iItemIndex < ListView_GetItemCount(hwndMainListView)){
						LaunchProgram(iItemIndex, TabCtrl_GetCurSel(hwndTab));
					}
					return 0;
				}
				else if(((LPNMLVKEYDOWN)lParam)->hdr.code == LVN_KEYDOWN){
					//if(((LPNMLVKEYDOWN)lParam)->wVKey == VK_RETURN){
					//	INT nBtnIndex = SendMessage(hwndListView, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
					//	if(nBtnIndex != -1){
					//		LaunchProgram(nBtnIndex, TabCtrl_GetCurSel(hwndTab));
					//	}
					//}
					if(((LPNMLVKEYDOWN)lParam)->wVKey == VK_DELETE){
						if(MessageBox(hWnd, LoadLocalString(IDS_REMOVE_BTN_PROMPT), L"Remove button", MB_ICONEXCLAMATION | MB_YESNO) == IDYES){
							INT nBtnIndex = SendMessage(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
							if(nBtnIndex != -1){
								RemoveButton(nBtnIndex, TabCtrl_GetCurSel(hwndTab));
							}
						}
					}
					else if(((LPNMLVKEYDOWN)lParam)->wVKey == VK_TAB){
						SetFocus(hwndTab);
					}
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			//if(((LPNMLVGETINFOTIP)lParam)->hdr.hwndFrom == hwndListView){
			//	if(((LPNMLVGETINFOTIP)lParam)->hdr.code == NM_RCLICK){
			//		
			//		return 0;
			//	}
			//}
		}
		break;
		case WM_SETFOCUS:
			SetFocus(hwndMainListView);
			return 0;
		case WM_CREATE:
		{
			LoadCategoryList();
			

			//TCHAR string[256];
			//int result = ExpandEnvironmentStrings("%blabla%", string, 256);
			//int error = GetLastError();
			//INT nSize = GetIniString("categories", "Categories", "default",	string, 10, szIniPath);

			//MessageBoxW(hWnd, LoadLocalString(IDS_APP_TITLE), L"Title", MB_ICONINFORMATION | MB_OK);

		}
		break;
		case WM_SIZE:
			ResizeTabControl(hwndTab, hWnd);
			ResizeListView(hwndMainListView);
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			if(hwndTab == NULL){
				RECT clientRect;
				GetClientRect(hwndMain, &clientRect);
				TextOut(hdc, clientRect.right / 2 - 200, clientRect.bottom / 2 - 5, LoadLocalString(IDS_FIRST_STARTUP_TIP), 55);
			}

			EndPaint(hWnd, &ps);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		case WM_INITMENUPOPUP:
			UpdateMenu(hwndMainListView, GetMenu(hWnd));
		break;
		case WM_DESTROY:
		{
			if(fRebuildIconCache){
				RebuildIconCache();
			}

			//save current window dimensions to ini file
			RECT windowSize;
			GetWindowRect(hWnd, &windowSize);
			SetSettingInt(TEXT("general"), TEXT("WindowWidth"), windowSize.right - windowSize.left);
			SetSettingInt(TEXT("general"), TEXT("WindowHeight"), windowSize.bottom - windowSize.top);

			PostQuitMessage(0);
		}
		break;
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}


//
// @brief Registers the window class.
// @param hInstance - Handle to current instance
// @return ?
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPCTSTR lpszClassName){

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGRAMLAUNCHER));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_PROGRAMLAUNCHER);
	wcex.lpszClassName = (LPCTSTR)lpszClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PROGRAMLAUNCHER));

	return RegisterClassEx(&wcex);
}

//
// @brief Saves instance handle and creates main window
// @description In this function, we save the instance handle in a global variable and create and display the main program window.
// @param hInstance - Handle to current instance
// @param nCmdShow - Used internally
// @return return TRUE if success, FALSE if fail
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, LPCTSTR lpszClassName){
	hInst = hInstance; // Store instance handle in our global variable

	//save global .ini file path
	//TCHAR szExePath[MAX_PATH];
	//GetModuleFileName(NULL, szExePath, MAX_PATH);
	//PathRemoveFileSpec(szExePath);
	//_sntprintf_s(szIniPath, MAX_PATH, MAX_PATH, TEXT("%s\\%s"), szExePath, );

	GetModuleFileNameW(NULL, szIniPath, _countof(szIniPath));
	PathRemoveFileSpecW(szIniPath);
	PathAppendW(szIniPath, TEXT(INI_FILE_NAME));
	if(!PathFileExistsW(szIniPath)){
		WCHAR szNewPath[MAX_PATH];
		ExpandEnvironmentStringsW(L"%AppData%\\Program Launcher", szNewPath, _countof(szNewPath));
		PathAppendW(szNewPath, TEXT(INI_FILE_NAME));
		if(PathFileExistsW(szNewPath)){
			wcscpy_s(szIniPath, _countof(szIniPath), szNewPath);
		}
	}


	TCHAR szTitle[MAX_LOADSTRING];
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);

	//create main window
	hwndMain = CreateWindowEx(
		/*WS_EX_CONTROLPARENT | */WS_EX_ACCEPTFILES,
		lpszClassName,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		DEFAULT_WINDOW_WIDTH,
		DEFAULT_WINDOW_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if(!hwndMain){
		return FALSE;
	}
	

	//resize main window to values stored in .ini file
	INT nWindowWidth = GetSettingInt(TEXT("general"), TEXT("WindowWidth"), DEFAULT_WINDOW_WIDTH);
	INT nWindowHeight = GetSettingInt(TEXT("general"), TEXT("WindowHeight"), DEFAULT_WINDOW_HEIGHT);

	SetWindowPos(hwndMain, 0, 0, 0, nWindowWidth, nWindowHeight, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);

	if(CreateTabControl(hwndMain) != NULL){
		//create and setup main list view
		CreateListView(hwndMain);
	}

	//show window
	ShowWindow(hwndMain, nCmdShow);
	UpdateWindow(hwndMain);

	return TRUE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch(message){
		case WM_INITDIALOG:
		{
			//center the dialog
			CenterDialog(hwndMain, hDlg);

			WCHAR szExePath[MAX_PATH];
			DWORD  verHandle = 0;
			UINT   size = 0;
			LPBYTE lpBuffer = NULL;

			GetModuleFileNameW(GetModuleHandleW(NULL), szExePath, _countof(szExePath));

			DWORD  verSize = GetFileVersionInfoSizeW(szExePath, &verHandle);
			if(verSize == 0) return (INT_PTR)TRUE;

			LPWSTR verData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, verSize);
			if(verData == NULL) return (INT_PTR)TRUE;

			if(GetFileVersionInfoW(szExePath, 0, verSize, verData)){
				if(VerQueryValueW(verData, L"\\", (VOID FAR * FAR*) & lpBuffer, &size)){
					if(size){
						VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
						if(verInfo->dwSignature == 0xfeef04bd){
							_snwprintf_s(verData, verSize / sizeof(WCHAR), verSize, L"v%d.%d.%d", 
								(verInfo->dwFileVersionMS >> 16) & 0xffff,
								(verInfo->dwFileVersionMS >> 0) & 0xffff,
								(verInfo->dwFileVersionLS >> 16) & 0xffff//,
								//(verInfo->dwFileVersionLS >> 0) & 0xffff
							);
						}
					}
				}
			}

			SetWindowTextW(GetDlgItem(hDlg, IDC_ABOUT_VERSION), verData);
			HeapFree(GetProcessHeap(), 0, verData);
			
			return (INT_PTR)TRUE;
		}
		break;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}


