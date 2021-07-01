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

//
// @brief Gets number of categories (returns nCategoryCount global variable)
// @return Number of categories if success (nCategoryCount)
//
INT GetCategoryCount(){

	//TCHAR categoryList[128];
	//UINT nCategoryCount = 0;
	//INT nLen;
	//
	//nLen = GetSettingString(TEXT("categories"), TEXT("Categories"), categoryList, CATEGORY_LIST_LEN, TEXT("0"));
	//
	//if(nLen == 0){
	//#ifdef _DEBUG
	//	ErrorHandlerEx(__LINE__, TEXT(__FILE__), TEXT("GetCategoryCount"));
	//#endif
	//	return 0;
	//}
	//else if(nLen == _countof(categoryList) - 1){
	//
	//}
	//
	//
	//
	//
	//if(categoryList[0] == TEXT('0')){
	//	return 0;
	//}
	//
	//INT i = 0;
	//while(!(categoryList[i] == '\0' || i > CATEGORY_LIST_LEN)){
	//
	//	// If character is found in string then increment count variable
	//	if(categoryList[i] == CATEGORY_NAME_DELIM){
	//		nCategoryCount++;
	//	}
	//	i++;
	//}
	//if(categoryList == TEXT("")){
	//	return 0;
	//}
	//nCategoryCount++;
	return nCategoryCount;
}

//
// @brief Adds new category to tab control, and saves it to ini file
// @param lpszNewCategoryName - New category name
// @return Returns number of categories loaded, or 0 if failed
//
INT AddCategory(LPCTSTR lpszNewCategoryName){

	if(nCategoryCount == 0){
		_tcscpy_s(arrszCategoryList[0], _countof(arrszCategoryList[0]), lpszNewCategoryName);
		nCategoryCount++;
	}
	else{
		_tcscpy_s(arrszCategoryList[nCategoryCount], _countof(arrszCategoryList[0]), lpszNewCategoryName);
		nCategoryCount++;
	}

	SaveCategoryList();

	if(hwndTab == NULL){
		if(CreateTabControl(hwndMain) == NULL){
			return 0;
		}
		if(CreateListView(hwndMain) == NULL){
			return 0;
		}
		//ResizeTabControl(hwndTab, hwndMain);
		//ResizeListView(hwndListView);
		return 1;
	}

	//insert the new tab
	TCITEM tie;
	UINT tabIndex = TabCtrl_GetItemCount(hwndTab);
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;
	tie.pszText = (LPTSTR)lpszNewCategoryName;
	TabCtrl_InsertItem(hwndTab, tabIndex, &tie);
	TabCtrl_SetCurSel(hwndTab, tabIndex);

	ResizeTabControl(hwndTab, hwndMain);
	ResizeListView(hwndMainListView);

	UpdateListView(hwndMainListView, -1);

	return 1;
}

//
// @brief Removes category from tab control, and saves result to ini file
// @param nCategoryIndex - index of category to delete
// @return Returns 1, or 0 if failed
//
INT RemoveCategory(UINT nCategoryIndex){

	if(nCategoryCount == 0){
		MessageBox(hwndMain, LoadLocalString(IDS_NO_CATEGORY), szError, MB_ICONERROR | MB_OK);
		return 0;
	}

	LPTSTR lpszMessage = (LPTSTR)calloc(50 + sizeof(arrszCategoryList[nCategoryIndex]), sizeof(TCHAR));
	if(lpszMessage == NULL){
		return 0;
	}
	_stprintf_s(lpszMessage, 50 + sizeof(arrszCategoryList[nCategoryIndex]), LoadLocalString(IDS_REMOVE_CATEGORY_PROMPT), arrszCategoryList[nCategoryIndex]);

	if(MessageBox(hwndTab, lpszMessage, LoadLocalString(IDS_REMOVE_CATEGORY), MB_ICONEXCLAMATION | MB_YESNO) == IDYES){

		nCategoryCount--;
		int i = nCategoryIndex;
		while(i < nCategoryCount){
			_tcscpy_s(arrszCategoryList[i], _countof(arrszCategoryList[i]), arrszCategoryList[i + 1]);
			i++;
		}
		SaveCategoryList();
		if(nCategoryCount <= 0){
			
			DestroyWindow(hwndTab);
			hwndTab = NULL;
			DestroyWindow(hwndMainListView);
			hwndMainListView = NULL;
		}
		else{
			SendMessage(hwndTab, TCM_DELETEITEM, (WPARAM)nCategoryIndex, (LPARAM)NULL);

			ResizeTabControl(hwndTab, hwndMain);
			ResizeListView(hwndMainListView);
			if(TabCtrl_GetCurSel(hwndTab) == -1){
				UpdateListView(hwndMainListView, -1);
			}
			InvalidateRect(hwndMain, NULL, TRUE);
		}
	}
	free(lpszMessage);
	RebuildIconCache();

	return 1;
}

//
// @brief Moves category in tab control in specified direction
// @param nCategoryIndex - index of category to move
// @param bLeft - indicates direction
// @return Returns 1, or 0 if failed
//
INT MoveCategory(INT nCategoryIndex, BOOL bLeft){

	INT nCurrentSel;
	LPTSTR lpszNameTemp = (LPTSTR)calloc(sizeof(arrszCategoryList[nCategoryIndex]), sizeof(TCHAR));
	if(lpszNameTemp == NULL){
		return 0;
	}

	_tcscpy_s(lpszNameTemp, sizeof(arrszCategoryList[nCategoryIndex]), arrszCategoryList[nCategoryIndex]);
	if(bLeft){
		if(nCategoryIndex < 1){
			free(lpszNameTemp);
			return 0;
		}
		_tcscpy_s(arrszCategoryList[nCategoryIndex], _countof(arrszCategoryList[nCategoryIndex]), arrszCategoryList[nCategoryIndex - 1]);
		_tcscpy_s(arrszCategoryList[nCategoryIndex - 1], _countof(arrszCategoryList[nCategoryIndex]), lpszNameTemp);
	}
	else{
		if(nCategoryIndex >= GetCategoryCount() - 1){
			free(lpszNameTemp);
			return 0;
		}
		_tcscpy_s(arrszCategoryList[nCategoryIndex], _countof(arrszCategoryList[nCategoryIndex]), arrszCategoryList[nCategoryIndex + 1]);
		_tcscpy_s(arrszCategoryList[nCategoryIndex + 1], _countof(arrszCategoryList[nCategoryIndex]), lpszNameTemp);
	}

	SaveCategoryList();

	nCurrentSel = TabCtrl_GetCurSel(hwndTab);

	//TODO: here should be used better method
	DestroyWindow(hwndTab);
	CreateTabControl(hwndMain);

	if(nCategoryIndex == nCurrentSel){
		TabCtrl_SetCurSel(hwndTab, bLeft ? nCurrentSel - 1 : nCurrentSel + 1);
	}
	else{
		TabCtrl_SetCurSel(hwndTab, nCurrentSel);
	}
	
	free(lpszNameTemp);
	RebuildIconCache();

	return 1;
}

//
// @brief Renames category specified by nCategoryIndex
// @param nCategoryIndex - index of category to rename
// @param lpszNewName - new name of the category
// @return Returns 1, or 0 if failed
//
INT RenameCategory(INT nCategoryIndex, LPTSTR lpszNewName){

	LAUNCHERENTRY tempBtnStruct;

	//TODO: rename section directly
	INT cCategoryCount = GetCategorySectionCount(nCategoryIndex);
	for(int i = 0; i < cCategoryCount; i++){
		GetSettingButtonStruct(arrszCategoryList[nCategoryIndex], i, &tempBtnStruct, FALSE);
		SetSettingButtonStruct(lpszNewName, i, &tempBtnStruct);
		//delete old button
		SetSettingButtonStruct(arrszCategoryList[nCategoryIndex], i, NULL);
	}

	_tcscpy_s(arrszCategoryList[nCategoryIndex], _countof(arrszCategoryList[nCategoryIndex]), lpszNewName);
	SaveCategoryList();

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT;
	tcItem.pszText = lpszNewName;
	TabCtrl_SetItem(hwndTab, nCategoryIndex, &tcItem);

	UpdateListView(hwndMainListView, -1);
	return 1;
}

//
//@brief Loads category list to arrszCategroryList[] from ini file
//@return Returns number of categories loaded, or 0 if failed
//
INT LoadCategoryList(){

	LPTSTR lpszCategoryList = (LPTSTR)malloc(sizeof(arrszCategoryList));
	if(lpszCategoryList == NULL){
		return 0;
	}
	ZeroMemory(lpszCategoryList, sizeof(arrszCategoryList));

	if(!GetSettingString(TEXT("categories"), TEXT("Categories"), lpszCategoryList, sizeof(arrszCategoryList), TEXT(""))){
		nCategoryCount = 0;
		free(lpszCategoryList);
		return 0;
	}
	TCHAR delim[] = {CATEGORY_NAME_DELIM_STRING};
	LPTSTR next_token = NULL;
	LPTSTR token = _tcstok_s(lpszCategoryList, delim, &next_token);
	INT i = 0;
	while(token){
		_tcscpy_s(arrszCategoryList[i], CATEGORY_NAME_LEN, token);
		token = _tcstok_s(NULL, delim, &next_token);
		i++;
	}

	nCategoryCount = i;

	free(lpszCategoryList);
	return i;
}

//
//@brief Saves category list from arrszCategroryList[] to ini file
//@return Returns 1, or 0 if failed
//
INT SaveCategoryList(){

	if(nCategoryCount == 0){
		if(!DeleteSettingKey(TEXT("categories"), TEXT("Categories"))){
			return 0;
		}
		return 1;
	}

	LPTSTR lpszCategoryList = (LPTSTR)malloc(sizeof(arrszCategoryList));
	if(lpszCategoryList == NULL){
		return 0;
	}
	ZeroMemory(lpszCategoryList, sizeof(arrszCategoryList));

	for(int i = 0; i < nCategoryCount; i++){
		if(i != 0){
			_tcscat_s(lpszCategoryList, CATEGORY_LIST_LEN, CATEGORY_NAME_DELIM_STRING);
		}
		_tcscat_s(lpszCategoryList, CATEGORY_LIST_LEN, arrszCategoryList[i]);
	}

	if(!SetSettingString(TEXT("categories"), TEXT("Categories"), lpszCategoryList)){
		free(lpszCategoryList);
		return 0;
	}
	free(lpszCategoryList);
	return 1;
}