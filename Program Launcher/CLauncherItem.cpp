
#include "pch.h"
#include "Program Launcher.h"


CLauncherItem::CLauncherItem(CCategory* ptrParent, UINT parentIndex, wstring wsName, wstring wsPath, wstring wsPathIcon, int iIconIndex, wstring wsPath64, bool bAdmin, bool bAbsolute)
	:
	ptrParent(ptrParent),
	parentIndex(parentIndex),
	wsName(wsName),
	wsPath(wsPath),
	wsPathIcon(wsPathIcon),
	iIconIndex(iIconIndex),
	wsPath64(wsPath64),
	bAdmin(bAdmin),
	bAbsolute(bAbsolute)
{}

CLauncherItem::CLauncherItem(CCategory* ptrParent, UINT parentIndex, const ElementProps& props)
	:
	ptrParent(ptrParent),
	parentIndex(parentIndex),
	wsName(props.wsName),
	wsPath(props.wsPath),
	wsPathIcon(props.wsPathIcon),
	iIconIndex(props.iIconIndex),
	wsPath64(props.wsPath64),
	bAdmin(props.bAdmin),
	bAbsolute(props.bAbsolute)
{}


CLauncherItem::~CLauncherItem(){
	//Launcher->CList.DeleteItem(parentIndex);
	try{
		if(ptrParent && ptrParent->imLarge.m_hImageList && ptrParent->imSmall.m_hImageList){
			ptrParent->imLarge.Remove(parentIndex);
			ptrParent->imSmall.Remove(parentIndex);
		}
	}catch(...){}
}


bool CLauncherItem::Edit(){
	if(DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_BTNSETTINGDLG), CMainWnd->GetSafeHwnd(), BtnEditDlgProc, (LPARAM)this)){
		HICON hSmall;
		ptrParent->imLarge.Replace(parentIndex, this->LoadIcon(MAKELPARAM(Launcher->options.IconSize, SMALL_ICON_SIZE), &hSmall));
		ptrParent->imSmall.Replace(parentIndex, hSmall);
		this->UpdateInListView();
		return true;
	}
	return false;
}

void CLauncherItem::Remove(bool bAsk){
	if(!bAsk || CMainWnd->MessageBoxW(std::format(GetString(IDS_REMOVE_BTN_PROMPT), wsName).c_str(), GetString(IDS_REMOVE_BUTTON).c_str(), MB_ICONEXCLAMATION | MB_YESNO) == IDYES){
		CCategory* category = ptrParent;
		int index = parentIndex;
		category->vItems.erase(category->vItems.begin() + index);
		category->ReindexItems(index);
		CMainWnd->UpdateListView();
	}
}


bool CLauncherItem::Move(UINT newPos, bool bRelative){
	if(bRelative){
		newPos += parentIndex;
	}

	shared_ptr<CLauncherItem> temp = ptrParent->vItems[parentIndex];
	if(parentIndex == newPos){
		return true;
	} 
	else if(newPos >= ptrParent->vItems.size()){
		return false;
	} 
	else if(parentIndex < newPos){
		for(UINT i = parentIndex; i < newPos; i++){
			ptrParent->vItems[i] = ptrParent->vItems[i + 1];
		}
	} 
	else{
		for(UINT i = parentIndex; i > newPos; i--){
			ptrParent->vItems[i] = ptrParent->vItems[i - 1];
		}
	}
	ptrParent->vItems[newPos] = temp;
	ptrParent->ReindexItems();

	CMainWnd->UpdateListView(true);

	return true;
}


//
// @brief Launches program based on button index and category
// @param nBtnId - id of the button
// @param nCategoryIndex - id of category
// @return 1 if success, 0 if fail (Returns the handle to the new process)
// 
bool CLauncherItem::Launch(){

	//expand enviromnent variables
	wstring wsExpPath = ExpandEnvStrings(wsPath);
	wstring wsExpPath64 = ExpandEnvStrings(wsPath64);

	SHELLEXECUTEINFO shExInfo;
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = 0;//SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = CMainWnd->GetSafeHwnd();
	shExInfo.lpVerb = bAdmin ? L"runas" : nullptr;
	if(Is64BitWindows() && wsPath64.size()){
		shExInfo.lpFile = wsPath64.c_str();
		wstring path(RemoveFileSpecFromPath(wsPath64));
		shExInfo.lpDirectory = path.c_str();
	}
	else{
		shExInfo.lpFile = wsPath.c_str();
		wstring path(RemoveFileSpecFromPath(wsPath));
		shExInfo.lpDirectory = path.c_str();
	}
	shExInfo.lpParameters = L"";// Additional parameters
	shExInfo.nShow = SW_SHOW;
	shExInfo.hInstApp = 0;

	if(!ShellExecuteExW(&shExInfo)){
		if(GetLastError() != ERROR_CANCELLED){
			ErrorHandler();
		}
		return false;
	}

	if(Launcher->options.CloseAfterLaunch){
		Launcher->ExitInstance();
	}
	return true;
}



void CLauncherItem::InsertIntoListView(){
	CMainWnd->CList.InsertItem(parentIndex, L"", parentIndex);
	this->UpdateInListView();
}


void CLauncherItem::UpdateInListView(){

	for(int i = 0; i < 7; i++){
		//Add Subitems to columns
		wstring wsText;
		switch(i){
			case 0:// Name
				wsText = wsName;
				break;
			case 1:// Path
				wsText = wsPath;
				break;
			case 2:// 64-bit Path
				if(!wsPath64.empty()){
					wsText = wsPath64;
				}
				break;
			case 3:// Path to icon
				wsText = wsPathIcon;
				break;
			case 4:// Icon index
				wsText = to_wstring(iIconIndex);
				break;
			case 5:// Run as admin
				wsText = GetString(bAdmin ? IDS_YES : IDS_NO);
				break;
			case 6:// Absolute paths
				wsText = GetString(bAbsolute ? IDS_YES : IDS_NO);
				break;
		}
		CMainWnd->CList.SetItemText(parentIndex, i, wsText.c_str());
	}
}



INT_PTR CALLBACK BtnEditDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	static CLauncherItem* ptrItem;
	switch(message){
		case WM_INITDIALOG:
			//center the dialog
			CenterDialog(CMainWnd->GetSafeHwnd(), hDlg);

			ptrItem = (CLauncherItem*)lParam;

			for(size_t i = 0; i < Launcher->vCategories.size(); i++){
				ComboBox_AddString(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), Launcher->GetCategory(i)->wsCategoryName.c_str());
			}
			//set the category to currently selected tab
			ComboBox_SetCurSel(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), Launcher->GetCurrentCategoryIndex());
			//set maximum number of characters to MAX_PATH-1
			SendDlgItemMessageW(hDlg, IDC_BTNDLGNAMEEDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH - 1, (LPARAM)0);
			SendDlgItemMessageW(hDlg, IDC_BTNDLGPATHEDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH - 1, (LPARAM)0);
			SendDlgItemMessageW(hDlg, IDC_BTNDLGPATH64EDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH - 1, (LPARAM)0);
			SendDlgItemMessageW(hDlg, IDC_BTNDLGICONEDIT, EM_SETLIMITTEXT, (WPARAM)MAX_PATH + 4, (LPARAM)0);

			if(ptrItem->parentIndex == -1){
				SetWindowTextW(hDlg, GetString(IDS_NEW_BUTTON).c_str());
			}
			else{
				SetWindowTextW(hDlg, GetString(IDS_EDIT_BUTTON).c_str());
				//load existing data
				//disable category select combobox
				ComboBox_Enable(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT), 0);
				//path
				Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), ptrItem->wsPath.c_str());
				//64-bit path if exists
				if(!ptrItem->wsPath.empty()){
					Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLG64CHECK), 1);
					Edit_Enable(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), 1);
					Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), ptrItem->wsPath64.c_str());
					Button_Enable(GetDlgItem(hDlg, IDC_BTNDLGPATH64BROWSE), 1);
				}
				//icon path
				Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), wstring(ptrItem->wsPathIcon + L',' + to_wstring(ptrItem->iIconIndex)).c_str());
				//name
				Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), ptrItem->wsName.c_str());
				//admin
				Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLGADMINCHECK), ptrItem->bAdmin);
				//absolute
				Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLGABSCHECK), ptrItem->bAbsolute);
			}
			return TRUE;
		case WM_COMMAND:
		{
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
						WCHAR szFileName[MAX_PATH] = L"";

						ZeroMemory(&ofn, sizeof(ofn));

						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hDlg;
						ofn.lpstrFilter = L"Executables (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
						ofn.lpstrFile = szFileName;
						ofn.nMaxFile = MAX_PATH;
						ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON;
						ofn.lpstrDefExt = L"exe";

						OFN_RETRY:
						if(GetOpenFileNameW(&ofn)){
							//check for compatibility
							DWORD dwExeType = 0;
							if(GetBinaryType(szFileName, &dwExeType)){
								switch(dwExeType){
									//case SCS_32BIT_BINARY:
									//	break;
									case SCS_DOS_BINARY:
										if(MessageBoxW(hDlg, GetString(IDS_DOS_BINARY).c_str(), TEXT("MS-DOS Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
											goto OFN_RETRY;
										}
										break;
									case SCS_WOW_BINARY:
										if(MessageBoxW(hDlg, GetString(IDS_WOW_BINARY).c_str(), TEXT("16-bit Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
											goto OFN_RETRY;
										}
										break;
									case SCS_64BIT_BINARY:
										if(MessageBoxW(hDlg, GetString(IDS_64BIT_BINARY).c_str(), TEXT("64-bit Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
											goto OFN_RETRY;
										}
										break;
								}
							}

							//set the result to the path edit control
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), szFileName);

							//fill name edit box if it's empty
							WCHAR szAppName[MAX_PATH];
							if(!Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), szAppName, MAX_PATH)){
								DWORD dwLen;
								LPTSTR lpVI;
								dwLen = GetFileVersionInfoSizeW(szFileName, NULL);
								if(dwLen != 0){
									lpVI = (LPWSTR)GlobalAlloc(GPTR, dwLen);
									if(lpVI){
										WORD* langInfo;
										UINT cbLang;
										WCHAR tszVerStrName[128];
										LPVOID lpt;
										UINT cbBufSize = 0;
										GetFileVersionInfoW(szFileName, 0, dwLen, lpVI);
										//Get the Product Name
										//First, to get string information, we need to get language information.
										VerQueryValueW(lpVI, L"\\VarFileInfo\\Translation", (LPVOID*)&langInfo, &cbLang);
										//Prepare the label -- default lang is bytes 0 & 1 of langInfo
										_snwprintf_s(tszVerStrName, 128, _TRUNCATE, L"\\StringFileInfo\\% 04x% 04x\\% s", langInfo[0], langInfo[1], L"ProductName");
										//Get the string from the resource data
										if(VerQueryValueW(lpVI, tszVerStrName, &lpt, &cbBufSize)){
											Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), (LPCWSTR)lpt);
										}
										//Cleanup
										GlobalFree((HGLOBAL)lpVI);
									}
								}
							}
							//tick the absolute paths checkbox 
							WCHAR szDriveLetter[2];
							GetModuleFileNameW(nullptr, szDriveLetter, 2);
							Button_SetCheck(GetDlgItem(hDlg, IDC_BTNDLGABSCHECK), (szFileName[0] != szDriveLetter[0]));
							//change icon path according to exe path
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), wstring(wstring(szFileName) + L",0").c_str());
						}
					}
				}
				break;
				case IDC_BTNDLGPATHEDIT:
				{
					if(wmNotify == EN_CHANGE){
						WCHAR szFileName[MAX_PATH];
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), szFileName, MAX_PATH);
						//check if the file exists
						DWORD dwAttrib = GetFileAttributesW(szFileName);

						if(!(dwAttrib == INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY))){
							//change icon path according to exe path
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), wstring(wstring(szFileName) + L",0").c_str());
						}
					}
				}
				break;
				case IDC_BTNDLGPATH64BROWSE:
				{
					if(wmNotify == BN_CLICKED){
						OPENFILENAMEW ofn;
						WCHAR szFileName[MAX_PATH] = L"";

						ZeroMemory(&ofn, sizeof(ofn));

						ofn.lStructSize = sizeof(ofn);
						ofn.hwndOwner = hDlg;
						ofn.lpstrFilter = L"Executables (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
						ofn.lpstrFile = szFileName;
						ofn.nMaxFile = MAX_PATH;
						ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
						ofn.lpstrDefExt = L"exe";

						if(GetOpenFileNameW(&ofn)){
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), szFileName);
						}
					}
				}
				break;
				case IDC_BTNDLGICONEDIT:
				{
					if(wmNotify == EN_CHANGE){
						//display icon preview if the icon path is changed
						WCHAR szPathIcon[MAX_PATH + 5];
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szPathIcon, MAX_PATH + 5);
						INT nIconIndex = GetIconIndexFromPath(szPathIcon);
						HICON hIcon = ExtractIconW(hInst, szPathIcon, nIconIndex);
						Static_SetIcon(GetDlgItem(hDlg, IDC_BTNDLGICONPREV), hIcon);
					}
				}
				break;
				case IDC_BTNDLGICONPICK:
				{
					if(wmNotify == BN_CLICKED){
						WCHAR szIconPathW[MAX_PATH + 5];
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), szIconPathW, MAX_PATH + 5);
						INT iIconIndex = GetIconIndexFromPath(szIconPathW);
						if(PickIconDlg(hDlg, szIconPathW, MAX_PATH, &iIconIndex)){
							Edit_SetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), wstring(wstring(szIconPathW) + L',' + to_wstring(iIconIndex)).c_str());
						}
					}
				}
				break;
				case IDOK:
				try{
					if(!Edit_GetTextLength(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT))){
						throw IDS_ENTER_VALID_NAME;
					}
					if(!Edit_GetTextLength(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT))){
						throw IDS_ENTER_VALID_PATH;
					}
					if(!Edit_GetTextLength(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT))){
						throw IDS_ENTER_VALID_PATH;
					}
					WCHAR buf[MAX_PATH + 5];
					//store path
					Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGPATHEDIT), buf, _countof(buf));
					ptrItem->wsPath = buf;
					//store 64-bit path
					if(Button_GetCheck(GetDlgItem(hDlg, IDC_BTNDLG64CHECK))){
						Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGPATH64EDIT), buf, _countof(buf));
						ptrItem->wsPath64 = buf;
					}
					//store name
					Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGNAMEEDIT), buf, _countof(buf));
					ptrItem->wsName = buf;
					//store icon path
					Edit_GetText(GetDlgItem(hDlg, IDC_BTNDLGICONEDIT), buf, _countof(buf));
					//store icon index
					ptrItem->iIconIndex = GetIconIndexFromPath(buf);
					ptrItem->wsPathIcon = buf;
					ptrItem->bAdmin = IsDlgButtonChecked(hDlg, IDC_BTNDLGADMINCHECK);
					ptrItem->bAbsolute = IsDlgButtonChecked(hDlg, IDC_BTNDLGABSCHECK);

					if(ptrItem->parentIndex == -1){
						ptrItem->parentIndex = ComboBox_GetCurSel(GetDlgItem(hDlg, IDC_BTNDLGCATSELECT));
					}
					
					EndDialog(hDlg, TRUE);
					return TRUE;
				}
				catch(int code){
					MessageBoxW(hDlg, GetString(code).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION | MB_OK);
					return FALSE;
				}
				catch(...){
					MessageBoxW(hDlg, GetString(IDS_ERROR).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION | MB_OK);
					return FALSE;
				}
				break;
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					return TRUE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

