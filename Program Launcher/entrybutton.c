

#include "Program Launcher.h"




//
// @brief Removes specified button (entry)
// @param nBtnIndex - Index of button to remove
// @param nCategoryIndex - Index of category
// @return separated icon index
//
INT RemoveButton(INT nBtnIndex, INT nCategoryIndex){
	LAUNCHERENTRY tempStruct;

	int i = nBtnIndex;
	INT cCategorySectionCount = GetCategorySectionCount(nCategoryIndex);
	while(i < MAX_ENTRIES_IN_CATEGORY){
		GetSettingButtonStruct(arrszCategoryList[nCategoryIndex], i + 1, &tempStruct, FALSE);
		if(tempStruct.szPath[0] == 0){
			break;
		}
		if(i >= cCategorySectionCount - 1){
			break;
		}
		SetSettingButtonStruct(arrszCategoryList[nCategoryIndex], i++, &tempStruct);
	}

	SetSettingButtonStruct(arrszCategoryList[nCategoryIndex], i, NULL);

	RebuildIconCache();
	UpdateListView(hwndMainListView, -1);
	return 1;
}

INT MoveButton(INT nButtonIndex, BOOL bLeft){
	LAUNCHERENTRY ButtonProps;
	LAUNCHERENTRY ButtonTemp;
	INT nCurrentSel = TabCtrl_GetCurSel(hwndTab);
	if(bLeft){
		if(nButtonIndex < 1){
			return 0;
		}
		GetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex, &ButtonProps, FALSE);
		GetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex - 1, &ButtonTemp, FALSE);
		SetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex, &ButtonTemp);
		SetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex - 1, &ButtonProps);
	}
	else{
		if(nButtonIndex >= GetCategorySectionCount(nCurrentSel) - 1){
			return 0;
		}
		GetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex, &ButtonProps, FALSE);
		GetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex + 1, &ButtonTemp, FALSE);
		SetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex, &ButtonTemp);
		SetSettingButtonStruct(arrszCategoryList[nCurrentSel], nButtonIndex + 1, &ButtonProps);
	}
	RebuildIconCache();
	UpdateListView(hwndMainListView, -1);

	return 1;
}



INT_PTR CALLBACK BtnEditDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	static int iBtnId;
	switch(message){
		case WM_INITDIALOG:
		{
			//center the dialog
			CenterDialog(hwndMain, hDlg);

			if(!GetCategoryCount()){
				MessageBox(hDlg, LoadLocalString(IDS_NO_CATEGORY), szError, MB_ICONERROR | MB_OK);
				EndDialog(hDlg, TRUE);
			}


			//TCHAR categoryList[CATEGORY_LIST_LEN];
			INT nCurrentTabIndex = TabCtrl_GetCurSel(hwndTab);

			iBtnId = lParam;

			//// Set the default push button to "Cancel." 
				//SendMessage(hDlg, DM_SETDEFID, (WPARAM)IDCANCEL, (LPARAM)0);
			INT cCatCount = GetCategoryCount();
			for(int i = 0; i < cCatCount; i++){
				SendMessage(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), CB_ADDSTRING, (WPARAM)0, (LPARAM)arrszCategoryList[i]);
			}

			//set the category to currently selected tab
			SendMessage(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), CB_SETCURSEL, (WPARAM)nCurrentTabIndex, (LPARAM)NULL);


			//set maximum number of characters to MAX_PATH-1
			SendDlgItemMessage(hDlg, IDC_BTNDLGNAMEEDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH - 1, (LPARAM)0);
			SendDlgItemMessage(hDlg, IDC_BTNDLGPATHEDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH - 1, (LPARAM)0);
			SendDlgItemMessage(hDlg, IDC_BTNDLGPATH64EDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH - 1, (LPARAM)0);
			SendDlgItemMessage(hDlg, IDC_BTNDLGICONEDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH + 4, (LPARAM)0);

			if(iBtnId == -1){
				SetWindowText(hDlg, LoadLocalString(IDS_NEW_BUTTON));
			}
			else{
				SetWindowText(hDlg, LoadLocalString(IDS_EDIT_BUTTON));
				//load existing data
				LAUNCHERENTRY buttonProps;
				GetSettingButtonStruct(arrszCategoryList[nCurrentTabIndex], iBtnId, &buttonProps, TRUE);

				//disable category select combobox
				ComboBox_Enable(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), 0);

				//path
				Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), buttonProps.szPath);

				//64-bit path if exists
				if(buttonProps.szPath64[0] != TEXT('0')){//_tcslen(buttonProps.szPath64) > 1
					Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLG64CHECK), 1);
					Edit_Enable(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), 1);
					Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), buttonProps.szPath64);
					Button_Enable(GetDlgItem(hDlg, IDC_BTNDLGPATH64BROWSE), 1);
				}

				//icon path
				TCHAR szIconPath[MAX_PATH + 5];
				_stprintf_s(szIconPath, MAX_PATH + 5, TEXT("%s,%i"), buttonProps.szPathIcon, buttonProps.nIconIndex);
				Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szIconPath);

				//name
				Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), buttonProps.szName);

				//admin
				Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLGADMINCHECK), buttonProps.bAdmin);

				//absolute
				Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLGABSCHECK), buttonProps.bAbsolute);

			}
			return TRUE;
		}
		case WM_COMMAND:
		{
			// Set the default push button to "OK" when the user enters text. 
			//if(HIWORD(wParam) == EN_CHANGE && LOWORD(wParam) == IDC_USERINPUTDLGEDIT){

			//	SendMessage(hDlg, DM_SETDEFID, (WPARAM)IDOK, (LPARAM)0);
			//}
			int wmId = LOWORD(wParam);
			int wmNotify = HIWORD(wParam);

			switch(wmId){
				case IDC_BTNDLG64CHECK:
					if(wmNotify == BN_CLICKED){
						Edit_Enable(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), IsDlgButtonChecked(hDlg, IDC_BTNDLG64CHECK));
						Button_Enable(GetDlgItem(hDlg, IDC_BTNDLGPATH64BROWSE), IsDlgButtonChecked(hDlg, IDC_BTNDLG64CHECK));
					}
					break;
				case IDC_BTNDLGPATHBROWSE:
				{
					if(wmNotify == BN_CLICKED){
						OPENFILENAME ofn;
						TCHAR szFileName[MAX_PATH] = TEXT("");

						ZeroMemory(&ofn, sizeof(ofn));

						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hDlg;
						ofn.lpstrFilter = TEXT("Executables (*.exe)\0*.exe\0All Files (*.*)\0*.*\0");
						ofn.lpstrFile = szFileName;
						ofn.nMaxFile = MAX_PATH;
						ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON;
						ofn.lpstrDefExt = TEXT("exe");

						OFN_RETRY:
						if(GetOpenFileName(&ofn)){
							//check for compatibility
							DWORD dwExeType = 0;
							if(GetBinaryType(szFileName, &dwExeType)){
								switch(dwExeType){
									//case SCS_32BIT_BINARY:
									//	break;
									case SCS_DOS_BINARY:
										if(MessageBox(hDlg, LoadLocalString(IDS_DOS_BINARY), TEXT("MS-DOS Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
											goto OFN_RETRY;
										}
										break;
									case SCS_WOW_BINARY:
										if(MessageBox(hDlg, LoadLocalString(IDS_WOW_BINARY), TEXT("16-bit Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
											goto OFN_RETRY;
										}
										break;
//#ifndef _WIN64
									case SCS_64BIT_BINARY:
										if(MessageBox(hDlg, LoadLocalString(IDS_64BIT_BINARY), TEXT("64-bit Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
											goto OFN_RETRY;
										}
										break;
//#endif
								}
							}


							//set the result to the path edit control
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), szFileName);

							//fill name edit box if it's empty
							TCHAR szAppName[MAX_PATH];
							if(!Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), szAppName, MAX_PATH)){
								DWORD dwLen;
								LPTSTR lpVI;
								dwLen = GetFileVersionInfoSize(szFileName, NULL);
								if(dwLen != 0){
									lpVI = (LPTSTR)GlobalAlloc(GPTR, dwLen);
									if(lpVI){
										WORD* langInfo;
										UINT cbLang;
										TCHAR tszVerStrName[128];
										LPVOID lpt;
										UINT cbBufSize = 0;

										GetFileVersionInfo(szFileName, 0, dwLen, lpVI);

										//Get the Product Name
										//First, to get string information, we need to get language information.
										VerQueryValue(lpVI, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&langInfo, &cbLang);
										//Prepare the label -- default lang is bytes 0 & 1 of langInfo
										_stprintf_s(tszVerStrName, 128, TEXT("\\StringFileInfo\\% 04x% 04x\\% s"), langInfo[0], langInfo[1], TEXT("ProductName"));
										//Get the string from the resource data
										if(VerQueryValue(lpVI, tszVerStrName, &lpt, &cbBufSize)){
											Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), (LPCTSTR)lpt);
										}
										//Cleanup
										GlobalFree((HGLOBAL)lpVI);
									}
								}
							}

							//tick the absolute paths checkbox 
							TCHAR szDriveLetter[2];
							GetModuleFileName(NULL, szDriveLetter, 2);
							if(szFileName[0] != szDriveLetter[0]){
								Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLGABSCHECK), 1);
							}
							else{
								Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLGABSCHECK), 0);
							}

							//change icon path according to exe path
							TCHAR szIconPath[MAX_PATH + 5];
							_stprintf_s(szIconPath, MAX_PATH + 5, TEXT("%s,%i"), szFileName, 0);
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szIconPath);

						}
					}
				}
				break;
				case IDC_BTNDLGPATHEDIT:
				{
					if(wmNotify == EN_CHANGE){
						TCHAR szFileName[MAX_PATH];
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), szFileName, MAX_PATH);
						//check if the file exists
						DWORD dwAttrib = GetFileAttributes(szFileName);

						if(dwAttrib == INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)){

						}
						else{
							//change icon path according to exe path
							TCHAR szIconPath[MAX_PATH + 5];
							_stprintf_s(szIconPath, MAX_PATH + 5, TEXT("%s,%i"), szFileName, 0);
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szIconPath);
						}
					}
				}
				break;
				case IDC_BTNDLGPATH64BROWSE:
				{
					if(wmNotify == BN_CLICKED){
						OPENFILENAME ofn;
						TCHAR szFileName[MAX_PATH] = TEXT("");

						ZeroMemory(&ofn, sizeof(ofn));

						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hDlg;
						ofn.lpstrFilter = TEXT("Executables (*.exe)\0*.exe\0All Files (*.*)\0*.*\0");
						ofn.lpstrFile = szFileName;
						ofn.nMaxFile = MAX_PATH;
						ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
						ofn.lpstrDefExt = TEXT("exe");

						if(GetOpenFileName(&ofn)){
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), szFileName);
						}
					}
				}
				break;
				case IDC_BTNDLGICONEDIT:
				{
					if(wmNotify == EN_CHANGE){
						//display icon preview if the icon path is changed
						TCHAR szPathIcon[MAX_PATH + 5];
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szPathIcon, MAX_PATH + 5);
						INT nIconIndex = GetIconIndexFromPath(szPathIcon);
						HICON hIcon = ExtractIcon(hInst, szPathIcon, nIconIndex);
						Static_SetIcon(GetDlgItem(hDlg, IDC_BTNDLGICONPREV), hIcon);
					}
				}
				break;
				case IDC_BTNDLGICONPICK:
				{
					if(wmNotify == BN_CLICKED){
						WCHAR szIconPathW[MAX_PATH + 5];
						TCHAR szIconPathA[MAX_PATH + 5];
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szIconPathA, MAX_PATH + 5);
						INT iIconIndex = GetIconIndexFromPath(szIconPathA);
#ifdef _UNICODE
						wcscpy_s(szIconPathW, MAX_PATH, szIconPathA);
#else
						MultiByteToWideChar(CP_UTF8, 0, szIconPathA, MAX_PATH, szIconPathW, MAX_PATH);
#endif
						if(PickIconDlg(hDlg, szIconPathW, MAX_PATH, &iIconIndex)){
#ifdef _UNICODE
							wcscpy_s(szIconPathA, MAX_PATH, szIconPathW);
#else
							WideCharToMultiByte(CP_UTF8, 0, szIconPathW, MAX_PATH, szIconPathA, MAX_PATH, NULL, NULL);
#endif
							_stprintf_s(szIconPathA, MAX_PATH + 5, TEXT("%s,%i"), szIconPathA, iIconIndex);
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szIconPathA);
						}
					}
				}
				break;
				case IDOK:
				{

					LAUNCHERENTRY structResult;

					if(!Edit_GetTextLength(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT))){
						MessageBox(hDlg, LoadLocalString(IDS_ENTER_VALID_NAME), szError, MB_ICONEXCLAMATION | MB_OK);
						return TRUE;
					}
					else if(!Edit_GetTextLength(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT))){
						MessageBox(hDlg, LoadLocalString(IDS_ENTER_VALID_PATH), szError, MB_ICONEXCLAMATION | MB_OK);
						return TRUE;
					}
					else if(!Edit_GetTextLength(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT))){
						MessageBox(hDlg, LoadLocalString(IDS_ENTER_VALID_PATH), szError, MB_ICONEXCLAMATION | MB_OK);
						return TRUE;
					}

					Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), structResult.szPath, MAX_PATH);
					if(Button_GetCheck(GetDlgItem(hDlg, IDC_BTNDLG64CHECK))){
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), structResult.szPath64, MAX_PATH);
					}
					else{
						_tcscpy_s(structResult.szPath64, MAX_PATH, TEXT("0"));
					}
					Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), structResult.szName, MAX_PATH);
					Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), structResult.szPathIcon, MAX_PATH + 5);
					structResult.nIconIndex = GetIconIndexFromPath(structResult.szPathIcon);
					structResult.bAdmin = (INT)IsDlgButtonChecked(hDlg, IDC_BTNDLGADMINCHECK);
					structResult.bAbsolute = (INT)IsDlgButtonChecked(hDlg, IDC_BTNDLGABSCHECK);

					if(!structResult.bAbsolute){
						RemoveLetterFromPath(structResult.szPath);
						RemoveLetterFromPath(structResult.szPath64);
						RemoveLetterFromPath(structResult.szPathIcon);
					}



					INT nNewButtonId;
					if(iBtnId == -1){
						//nCategorySectionCount = GetCategorySectionCount((UINT)SendMessage(hwndTab, TCM_GETCURSEL, (WPARAM)0, (LPARAM)0));
						nNewButtonId = GetCategorySectionCount((UINT)SendMessage(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), CB_GETCURSEL, (WPARAM)0, (LPARAM)0));
					}
					else{
						nNewButtonId = iBtnId;
					}

					TCHAR szSection[CATEGORY_NAME_LEN];
					ComboBox_GetText(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), szSection, CATEGORY_NAME_LEN);
					if(szSection[0] == 0){
						MessageBox(hDlg, LoadLocalString(IDS_SELECT_ONE_CATEGORY), szError, MB_ICONEXCLAMATION | MB_OK);
						return TRUE;
					}

					SetSettingButtonStruct(szSection, nNewButtonId, &structResult);

					int nCurrentCategory = TabCtrl_GetCurSel(hwndTab);
					//if(iBtnId > 0){
					//	RebuildIconCache();
					//}
					SaveIconFromResource(nCurrentCategory, nNewButtonId);

					HIMAGELIST himlSmall;
					HIMAGELIST himlLarge;
					HICON hIcon;

					himlSmall = ListView_GetImageList(hwndMainListView, LVSIL_SMALL);
					himlLarge = ListView_GetImageList(hwndMainListView, LVSIL_NORMAL);


					//set up the small image list
					hIcon = LoadIconForButton(nNewButtonId, nCurrentCategory, SMALL_ICON_SIZE);
					if(!hIcon){
						hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DEFAULT));
					}
					int ret1 = ImageList_AddIcon(himlSmall, hIcon);

					int size;
					ImageList_GetIconSize(himlLarge, &size, &size);

					//set up the large image list
					hIcon = LoadIconForButton(nNewButtonId, nCurrentCategory, size);
					if(!hIcon){
						hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DEFAULT));
					}
					int ret2 = ImageList_AddIcon(himlLarge, hIcon);

					LVITEMW lvItem;
					ZeroMemory(&lvItem, sizeof(lvItem));
					if(!GetSettingInt(TEXT("general"), TEXT("ShowAppNames"), DEFAULT_SHOWAPPNAMES)){
						lvItem.mask = LVIF_IMAGE;
					}
					else{
						lvItem.mask = LVIF_IMAGE | LVIF_TEXT;
					}
					
					
					lvItem.pszText = structResult.szName;
					lvItem.iItem = nNewButtonId;
					lvItem.iImage = nNewButtonId;
					int ret = ListView_InsertItem(hwndMainListView, &lvItem);
					
					for(int k = 1; k < 7; k++){
						//Add Subitems to columns
						WCHAR szBuf[INT_STR_LEN];
						LPWSTR lpText;
						switch(k){
							case 1:// Path
								lpText = structResult.szPath;
								break;
							case 2:// 64-bit Path
								if(structResult.szPath64[0] == L'0'){
									lpText = L"";
								}
								else{
									lpText = structResult.szPath64;
								}
								break;
							case 3:// Path to icon
								lpText = structResult.szPathIcon;
								break;
							case 4:// Icon index
								_itow_s(structResult.nIconIndex, szBuf, _countof(szBuf), 10);
								lpText = szBuf;
								break;
							case 5:// Run as admin
								if(structResult.bAdmin){
									lpText = LoadLocalString(IDS_YES);
								}
								else{
									lpText = LoadLocalString(IDS_NO);
								}
								break;
							case 6:// Absolute paths
								if(structResult.bAbsolute){
									lpText = LoadLocalString(IDS_YES);
								}
								else{
									lpText = LoadLocalString(IDS_NO);
								}
								break;
							default:
								lpText = szError;
								break;
						}
						ListView_SetItemText(hwndMainListView, nNewButtonId, k, lpText);
					}
					
					EndDialog(hDlg, TRUE);
					return TRUE;
				}
				break;
				case IDCANCEL:
					EndDialog(hDlg, TRUE);
					return TRUE;
					break;
			}
			return 0;
		}
	}
	return FALSE;

	UNREFERENCED_PARAMETER(lParam);
}

