//
//            _                                                
//   ___ __ _| |_ ___  __ _  ___  _ __ _   _   ___ _ __  _ __  
//  / __/ _` | __/ _ \/ _` |/ _ \| '__| | | | / __| '_ \| '_ \ 
// | (_| (_| | ||  __/ (_| | (_) | |  | |_| || (__| |_) | |_) |
//  \___\__,_|\__\___|\__, |\___/|_|   \__, (_)___| .__/| .__/ 
//                    |___/            |___/      |_|   |_|    
//
// This file contains functions related to Program Launcher's categories (in tab control)
// 
// Functions included in this file:
// 
//		INT AddCategory(LPCTSTR lpszNewCategoryName)
//		
//		INT RemoveCategory(UINT nCategoryIndex)
//		
//		INT MoveCategory(INT nCategoryIndex, BOOL bLeft)
//		
//		INT RenameCategory(INT nCategoryIndex, LPTSTR lpszNewName)
//		
//		INT LoadCategoryList()
//		
//		INT SaveCategoryList()
// 
//  Copyright ©2021, barty12
//

#include "Program Launcher.h"


CCategory::CCategory(CProgramLauncher* ptrParent, int parentIndex, wstring wsName)
	:
	ptrParent(ptrParent),
	parentIndex(parentIndex),
	wsCategoryName(wsName)
{

}





int CCategory::MoveItem(int index, int newPos){

	shared_ptr<CLauncherItem> temp = vItems[index];
	if(index == newPos){
		return 1;
	}
	else if(index < 0 || newPos < 0 || index >= vItems.size() || newPos >= vItems.size()){
		return 0;
	}
	else if(index < newPos){
		for(int i = index; i < newPos; i++){
			vItems[i] = vItems[i + 1];
		}
	}
	else{
		for(int i = index; i > newPos; i--){
			vItems[i] = vItems[i - 1];
		}
	}
	vItems[newPos] = temp;

	return 1;
}

void CCategory::ReindexItems(int iStart){
	for(size_t i = iStart; i < vItems.size(); ++i){
		vItems.at(i)->parentIndex = i;
	}
}

CLauncherItem* CCategory::GetItem(int index){
	return vItems.at(index).get();
}
int CCategory::GetSelectedItemIndex(){
	return SendMessageW(Launcher->hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
}
int CCategory::GetFocusedItemIndex(){
	return SendMessageW(Launcher->hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_FOCUSED);
}
CLauncherItem* CCategory::GetSelectedItem(){
	return vItems.at(GetSelectedItemIndex()).get();
}



INT_PTR CALLBACK UserInputProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	static wstring* wsName;
	switch(message){
		case WM_INITDIALOG:
		{
			wsName = (wstring*)lParam;
			//center the dialog
			CenterDialog(CMainWnd.GetSafeHwnd(), hDlg);

			//set maximum number of characters to CATEGORY_NAME_LEN
			SendDlgItemMessageW(hDlg, IDC_USERINPUTDLGEDIT, EM_SETLIMITTEXT, (WPARAM)CATEGORY_NAME_LEN, (LPARAM)0);

			if(wsName->size()){
				SetWindowTextW(hDlg, GetString(IDS_RENAME_CATEGORY).c_str());
				SetWindowTextW(GetDlgItem(hDlg, IDC_USERINPUTDLGEDIT), wsName->c_str());
			}

			return TRUE;
		}
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				case IDC_USERINPUTDLGEDIT:
					// Set the default push button to "OK" when the user enters text.
					if(HIWORD(wParam) == EN_CHANGE){
						SendMessageW(hDlg, DM_SETDEFID, (WPARAM)IDOK, (LPARAM)0);
					}
					break;
				case IDOK:
					try{
						// Get number of characters.
						int cchBuffer = SendDlgItemMessageW(hDlg, IDC_USERINPUTDLGEDIT, EM_LINELENGTH, (WPARAM)0, (LPARAM)0);

						if(cchBuffer >= CATEGORY_NAME_LEN){
							throw IDS_NAME_TOO_LONG;
						}
						if(cchBuffer <= 0){
							throw IDS_ENTER_VALID_NAME;
						}

						vector<WCHAR> vBuffer(CATEGORY_NAME_LEN);
						GetWindowTextW(GetDlgItem(hDlg, IDC_USERINPUTDLGEDIT), &vBuffer[0], vBuffer.capacity());
						wstring wsTemp = &vBuffer[0];

						if(wsTemp.find(CATEGORY_NAME_DELIM) != wstring::npos || wsTemp == L"general" || wsTemp == L"appereance" || wsTemp == L"categories"){
							throw IDS_NAME_INVALID_CHARACTERS;
						}
						*wsName = wsTemp;

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
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					return TRUE;
			}
			return FALSE;
	}
	return FALSE;
}
