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

#define DEFINE_GLOBAL
#include "Program Launcher.h"



//-----------------------------------------------------------------------------------------------------------------------------
// 
//                                                   WinMain
// 
//-----------------------------------------------------------------------------------------------------------------------------
INT APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow){

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//hwndMain = nullptr;
	hInst = hInstance;
	Launcher = nullptr;

	// Initialize common controls.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	// Initialize MFC
	if(!AfxWinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow)){
		MessageBoxW(nullptr, L"Fatal Error: MFC initialization failed\n", L"Program Launcher - Error", MB_OK | MB_ICONERROR);
	}

	// Register Window Class
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
	wstring ld(GetString(IDC_PROGRAMLAUNCHER));
	wcex.lpszClassName = ld.c_str();
	wcex.hIconSm = wcex.hIcon;

	if(!RegisterClassExW(&wcex)){
		MessageBoxW(NULL, GetString(IDS_WND_REGISTRATION_FAILED).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}


	// Perform application initialization
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

	CProgramLauncher main;
	Launcher = &main;
	Launcher->Init();

	HACCEL hAccelTable = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDC_PROGRAMLAUNCHER));
	// Main message loop:
	MSG msg;
	while(GetMessageW(&msg, NULL, 0, 0)){
		if(!TranslateAcceleratorW(CMainWnd.GetSafeHwnd(), hAccelTable, &msg)){
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
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
				case IDM_ABOUT:
					DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_ABOUTBOX), hWnd, About, 0);
					break;
				case IDM_FILE_PREF:
					DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_PREFDLG), hWnd, PrefDlgProc, 0);
					break;
				case IDM_CATEGORY_ADDCATEGORY:
					Launcher->NewCategory();
					break;
				case IDM_CATEGORY_RENAMECURRENTCATEGORY:
					Launcher->RenameCategory(Launcher->GetCurrentCategoryIndex());
					break;
				case IDM_CATEGORY_REMOVECURRENTCATEGORY:
					Launcher->RemoveCategory(Launcher->GetCurrentCategoryIndex());
					break;
				case IDM_POPUPCATEGORY_REMOVECATEGORY:
					Launcher->RemoveCategory(nClickedTab);
					break;
				case IDM_POPUPCATEGORY_RENAME:
					Launcher->RenameCategory(nClickedTab);
					break;
				case IDM_CATEGORY_MOVELEFT:
					//MoveCategory(TabCtrl_GetCurSel(hwndTab), TRUE);
					break;
				case IDM_CATEGORY_MOVERIGHT:
					//MoveCategory(TabCtrl_GetCurSel(hwndTab), FALSE);
					break;
				case IDM_POPUPCATEGORY_MOVELEFT:
					//MoveCategory(nClickedTab, TRUE);
					break;
				case IDM_POPUPCATEGORY_MOVERIGHT:
					//MoveCategory(nClickedTab, FALSE);
					break;
				case IDM_BUTTON_ADDNEWBUTTON:
					Launcher->NewItem();
					break;
				case IDM_VIEW_LARGEICONS:
					Launcher->SwitchListView(LVS_ICON);
					break;
				case IDM_VIEW_SMALLICONS:
					Launcher->SwitchListView(LVS_SMALLICON);
					break;
				case IDM_VIEW_LIST:
					Launcher->SwitchListView(LVS_LIST);
					break;
				case IDM_VIEW_DETAILS:
					Launcher->SwitchListView(LVS_REPORT);
					break;
				case IDM_EXIT:
					Launcher->Exit();
					break;
				case IDM_BUTTON_LAUNCH:
					Launcher->GetCurrentCategory()->GetSelectedItem()->Launch();
					break;
				case IDM_BUTTON_EDIT:
					Launcher->GetCurrentCategory()->GetSelectedItem()->Edit();
					break;
				case IDM_BUTTON_MOVELEFT:
					break;
				case IDM_BUTTON_MOVERIGHT:
					break;
				case IDM_BUTTON_REMOVE:
					Launcher->GetCurrentCategory()->GetSelectedItem()->Remove();
				break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_CONTEXTMENU:
			if((HWND)wParam == Launcher->hwndMainListView){
				return Launcher->DoListViewContextMenu(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}

			if((HWND)wParam == Launcher->CTab.GetSafeHwnd() && GET_X_LPARAM(lParam) == -1 && GET_Y_LPARAM(lParam)){
				nClickedTab = Launcher->CTab.GetCurFocus();
				return Launcher->DoTabControlContextMenu(nClickedTab, -1, -1);
			}

			RECT tabItemRect;
			POINT clickedCoords;
			clickedCoords.x = GET_X_LPARAM(lParam);
			clickedCoords.y = GET_Y_LPARAM(lParam);
			ScreenToClient(hWnd, &clickedCoords);

			for(size_t i = 0; i < Launcher->vCategories.size(); i++){
				Launcher->CTab.GetItemRect(i, &tabItemRect);
				if(PtInRect(&tabItemRect, clickedCoords)){
					nClickedTab = i;
					break;
				}
				else{
					nClickedTab = -1;
				}
			}
				
			if(nClickedTab >= 0){
				return Launcher->DoTabControlContextMenu(nClickedTab, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			return DefWindowProcW(hWnd, message, wParam, lParam);
		break;
		case WM_NOTIFY:
		{
			if(Launcher == nullptr) return 0;
			if(((LPNMTVKEYDOWN)lParam)->hdr.hwndFrom == Launcher->CTab.GetSafeHwnd()){
				if(((LPNMHDR)lParam)->code == TCN_SELCHANGE){
					Launcher->UpdateListView();
				}
				else if(((LPNMTVKEYDOWN)lParam)->hdr.code == TCN_KEYDOWN){
					if(((LPNMTVKEYDOWN)lParam)->wVKey == VK_TAB){
						SetFocus(Launcher->hwndMainListView);
					}
				}
			}
			else if(((LPNMITEMACTIVATE)lParam)->hdr.hwndFrom == Launcher->hwndMainListView){
				if(((LPNMITEMACTIVATE)lParam)->hdr.code == NM_DBLCLK){
					try{
						Launcher->GetCurrentCategory()->GetItem(((LPNMITEMACTIVATE)lParam)->iItem)->Launch();
					}
					catch(...){
					}
					return 0;
				}
				else if(((LPNMHDR)lParam)->code == NM_RETURN){
					try{
						Launcher->GetCurrentCategory()->GetSelectedItem()->Launch();
					}
					catch(...){
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
						if(MessageBoxW(hWnd, GetString(IDS_REMOVE_BTN_PROMPT).c_str(), GetString(IDS_REMOVE_BUTTON).c_str(), MB_ICONEXCLAMATION | MB_YESNO) == IDYES){
							INT nBtnIndex = SendMessageW(Launcher->hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
							if(nBtnIndex != -1){
								try{
									Launcher->GetCurrentCategory()->GetItem(nBtnIndex)->Remove();
								}
								catch(...){
								}
							}
						}
					}
					else if(((LPNMLVKEYDOWN)lParam)->wVKey == VK_TAB){
						Launcher->CTab.SetFocus();
					}
					return DefWindowProcW(hWnd, message, wParam, lParam);
				}
			}
		}
		break;
		case WM_SETFOCUS:
			SetFocus(Launcher->hwndMainListView);
			return 0;
		case WM_CREATE:
		{
			//SetPropW(hWnd, L"Launcher", (HANDLE)((LPCREATESTRUCT)lParam)->lpCreateParams);

			//TCHAR string[256];
			//int result = ExpandEnvironmentStrings("%blabla%", string, 256);
			//int error = GetLastError();
			//INT nSize = GetIniString("categories", "Categories", "default",	string, 10, szIniPath);

			//MessageBoxW(hWnd, LoadLocalString(IDS_APP_TITLE), L"Title", MB_ICONINFORMATION | MB_OK);

		}
		break;
		case WM_SIZE:
			Launcher->ResizeTabControl();
			Launcher->ResizeListView();
		break;
		case WM_PAINT:
		{/*
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
		*/}
		break;
		case WM_INITMENUPOPUP:
		{
			//CProgramLauncher* Launcher = (CProgramLauncher*)GetPropW(hWnd, L"Launcher");
			UpdateMenu(Launcher->hwndMainListView, GetMenu(hWnd));
		}
		break;
		case WM_DESTROY:
		{
			//if(fRebuildIconCache){
				//RebuildIconCache();
			//}

			//save current window dimensions to ini file
			//RECT windowSize;
			//GetWindowRect(hWnd, &windowSize);
			//SetSettingInt(TEXT("general"), TEXT("WindowWidth"), windowSize.right - windowSize.left);
			//SetSettingInt(TEXT("general"), TEXT("WindowHeight"), windowSize.bottom - windowSize.top);

			PostQuitMessage(0);
		}
		break;
		default:
			return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch(message){
		case WM_INITDIALOG:
		{
			//center the dialog
			CenterDialog(CMainWnd.GetSafeHwnd(), hDlg);

			WCHAR szExePath[MAX_PATH];
			DWORD  verHandle = 0;
			UINT   size = 0;
			LPBYTE lpBuffer = NULL;

			GetModuleFileNameW(GetModuleHandleW(NULL), szExePath, _countof(szExePath));

			DWORD  verSize = GetFileVersionInfoSizeW(szExePath, &verHandle);
			if(verSize == 0) return (INT_PTR)TRUE;

			LPWSTR verData = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, verSize);
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


