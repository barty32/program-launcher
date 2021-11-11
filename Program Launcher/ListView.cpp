//
//  _ _     _           _                               
// | (_)___| |_  __   _(_) _____      _____ _ __  _ __  
// | | / __| __| \ \ / / |/ _ \ \ /\ / / __| '_ \| '_ \ 
// | | \__ \ |_   \ V /| |  __/\ V  V / (__| |_) | |_) |
// |_|_|___/\__|   \_/ |_|\___| \_/\_(_)___| .__/| .__/ 
//                                         |_|   |_|    
//

#include "Program Launcher.h"

#ifndef LVS_EX_AUTOSIZECOLUMNS
#define LVS_EX_AUTOSIZECOLUMNS 0
#endif

//
// @brief Creates a list view, sized to fit the specified parent window's client area
// @param hwndParent - parent window (the application's main window). 
// @return Returns the handle to the list view 
// 
int CProgramLauncher::CreateListView(){
	//HIMAGELIST  himlSmall;
	//HIMAGELIST  himlLarge;

	DWORD dwStyle = WS_TABSTOP |
					WS_CHILD |
					WS_VISIBLE |
					LVS_AUTOARRANGE |
					LVS_SINGLESEL |
					LVS_ICON |
					LVS_EDITLABELS;

	//hwndMainListView = CreateWindowExW(
	//	WS_EX_CONTROLPARENT,// ex style
	//	WC_LISTVIEWW,		// class name - defined in commctrl.h
	//	L"",			// dummy text
	//	dwStyle,			// style
	//	0,					// x position
	//	0,					// y position
	//	0,					// width
	//	0,					// height
	//	CMainWnd.GetSafeHwnd(),// parent
	//	(HMENU)ID_LISTVIEW,	// ID
	//	hInst,				// instance
	//	nullptr);				// no extra data

	//if(!hwndMainListView){
	//	return 0;
	//}

	if(!CList.CreateEx(
		WS_EX_CONTROLPARENT,// ex style
		nullptr,		// class name - defined in commctrl.h
		L"",				// dummy text
		dwStyle,			// style
		RECT{0,0,0,0},
		&CMainWnd,			// parent
		ID_LISTVIEW			// ID
	)){
		return 0;
	}
	hwndMainListView = CList.m_hWnd;

	CList.GetListCtrl().SetExtendedStyle(LVS_EX_INFOTIP | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_AUTOSIZECOLUMNS);
	//ListView_SetExtendedListViewStyleEx(hwndMainListView, 0, LVS_EX_INFOTIP | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_AUTOSIZECOLUMNS);

	ResizeListView();

	//load spacing settings from ini
	CList.GetListCtrl().SetIconSpacing(options.IconSpacingHorizontal, options.IconSpacingVertical);
	//ListView_SetIconSpacing(hwndMainListView, options.IconSpacingHorizontal, options.IconSpacingVertical);

	//create the image lists
	//himlSmall = ImageList_Create(SMALL_ICON_SIZE, SMALL_ICON_SIZE, ILC_COLOR32, 1, 0);
	//himlLarge = ImageList_Create(options.IconSize, options.IconSize, ILC_COLOR32, 1, 0);

	/*if(himlSmall){
		ListView_SetImageList(hwndMainListView, himlSmall, LVSIL_SMALL);
	}
	if(himlLarge){
		ListView_SetImageList(hwndMainListView, himlLarge, LVSIL_NORMAL);
	}*/

	CList.GetListCtrl().SetImageList(&GetCurrentCategory()->imSmall, LVSIL_SMALL);
	CList.GetListCtrl().SetImageList(&GetCurrentCategory()->imLarge, LVSIL_NORMAL);

	//LVCOLUMNW  lvColumn;
	INT nColWidths[7] = {160,300,300,300,100,80,80};

	//initialize the columns
	//lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	//lvColumn.fmt = LVCFMT_LEFT;
	
	for(int i = 0; i < 7; i++){
		//wstring tmp(GetString(IDS_COL_NAME + i));
		//lvColumn.pszText = (LPWSTR)tmp.c_str();
		//lvColumn.iSubItem = i;
		//lvColumn.cx = nColWidths[i];
		//ListView_InsertColumn(hwndMainListView, i, &lvColumn);
		CList.GetListCtrl().InsertColumn(i, GetString(IDS_COL_NAME).c_str(), LVCFMT_LEFT, nColWidths[i], i);
	}

	

	UpdateListView();

	SetWindowTheme(hwndMainListView, L"Explorer", NULL);

	return 1;
}

//
// @brief Updates image lists for specified list view control
// @param hwndListView - handle to list view
// @param nIconSizeOfLargeImageList - size of the large image list, size of small image list is 16x16
// @return Returns always TRUE
// 
BOOL CProgramLauncher::UpdateImageLists(INT nIconSizeOfLargeImageList){
	HICON hIcon;
	
	HIMAGELIST himlSmall = ListView_GetImageList(hwndMainListView, LVSIL_SMALL);
	HIMAGELIST himlLarge = ListView_GetImageList(hwndMainListView, LVSIL_NORMAL);
	CCategory* currentCategory = GetCurrentCategory();
	INT cCategorySectionCount = currentCategory->vItems.size();
	
	ImageList_RemoveAll(himlSmall);

	//set up the small image list
	for(int i = 0; i < cCategorySectionCount; i++){
		hIcon = currentCategory->GetItem(i)->loadIcon(SMALL_ICON_SIZE);
		if(!hIcon){
			hIcon = LoadIconW(hInst, MAKEINTRESOURCEW(IDI_DEFAULT));
		}
		ImageList_AddIcon(himlSmall, hIcon);
	}
	
	ImageList_RemoveAll(himlLarge);
	if(nIconSizeOfLargeImageList >= 0){
		ImageList_SetIconSize(himlLarge, nIconSizeOfLargeImageList, nIconSizeOfLargeImageList);
	}
	else{
		ImageList_GetIconSize(himlLarge, &nIconSizeOfLargeImageList, &nIconSizeOfLargeImageList);
	}

	//set up the large image list
	for(int i = 0; i < cCategorySectionCount; i++){
		hIcon = currentCategory->GetItem(i)->loadIcon(nIconSizeOfLargeImageList);
		if(!hIcon){
			hIcon = LoadIconW(hInst, MAKEINTRESOURCE(IDI_DEFAULT));
		}
		ImageList_AddIcon(himlLarge, hIcon);
	}

	return TRUE;
}

//
// @brief Updates specified list view control
// @param hwndListView - handle to the list view
// @return Returns always TRUE
// 
BOOL CProgramLauncher::UpdateListView(){

	//empty the list
	ListView_DeleteAllItems(hwndMainListView);

	//UpdateImageLists(options.IconSize);

	//LVITEMW listItem = {0};
	CCategory* currentCategory = GetCurrentCategory();
	INT cCategorySectionCount = currentCategory->vItems.size();

	/*switch(options.ShowAppNames){
		case 0:
			listItem.mask = LVIF_IMAGE;
			break;
		case 1:
			listItem.mask = LVIF_IMAGE;
			break;
		default:
			listItem.mask = LVIF_IMAGE | LVIF_TEXT;
	}*/
	
	for(int j = 0; j < cCategorySectionCount; j++){
		currentCategory->GetItem(j)->InsertIntoListView();
		//shared_ptr<CLauncherItem> currentItem = currentCategory->vItems[j];
		//listItem.pszText = (LPWSTR)currentItem->wsName.c_str();
		//listItem.iItem = j;
		//listItem.iImage = j;
		//ListView_InsertItem(hwndMainListView, &listItem);

		//for(int k = 1; k < 7; k++){
		//	//Add Subitems to columns
		//	wstring wsText;
		//	switch(k){
		//		case 1:// Path
		//			wsText = currentItem->wsPath;
		//			break;
		//		case 2:// 64-bit Path
		//			if(!currentItem->wsPath64.empty()){
		//				wsText = currentItem->wsPath64;
		//			}
		//			break;
		//		case 3:// Path to icon
		//			wsText = currentItem->wsPathIcon;
		//			break;
		//		case 4:// Icon index
		//			wsText = to_wstring(currentItem->iIconIndex);
		//			break;
		//		case 5:// Run as admin
		//			wsText = GetString(currentItem->bAdmin ? IDS_YES : IDS_NO);
		//			break;
		//		case 6:// Absolute paths
		//			wsText = GetString(currentItem->bAbsolute ? IDS_YES : IDS_NO);
		//			break;
		//		default:
		//			wsText = GetString(IDS_ERROR);
		//			break;
		//	}
		//	ListView_SetItemText(hwndMainListView, j, k, (LPWSTR)wsText.c_str());
		//}
	}
	return TRUE;
}

//
// @brief Switches view of the specified list view control
// @param hwndListView - handle to the list view
// @param dwView - can be one of these values: LVS_ICON, LVS_SMALLICON, LVS_LIST, LVS_REPORT
// 
void CProgramLauncher::SwitchListView(DWORD dwView){
	DWORD dwStyle = GetWindowLongW(hwndMainListView, GWL_STYLE);

	SetWindowLongW(hwndMainListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	ResizeListView();
}

//
// @brief Resizes the specified list view control to fit into its parent window
// @param hwndListView - handle to the list view
// 
void CProgramLauncher::ResizeListView(){
	RECT rcTab;
	CTab.GetClientRect(&rcTab);
	CTab.AdjustRect(FALSE, &rcTab);

	MoveWindow(hwndMainListView, rcTab.left, rcTab.top, rcTab.right - rcTab.left, rcTab.bottom - rcTab.top,TRUE);
}




INT CProgramLauncher::DoListViewContextMenu(INT pX, INT pY){
	POINT displayCoords;
	int nItemId = SendMessageW(hwndMainListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);

	if(pX == -1 && pY == -1){
		RECT rcItem;
		ListView_GetItemRect(hwndMainListView, nItemId, &rcItem, LVIR_BOUNDS);

		displayCoords.x = rcItem.right - (rcItem.right - rcItem.left) / 2;
		displayCoords.y = rcItem.bottom - (rcItem.bottom - rcItem.top) / 2;
		ClientToScreen(hwndMainListView, &displayCoords);
	}
	else{
		displayCoords.x = pX;
		displayCoords.y = pY;
	}

	HMENU hMenu = LoadMenuW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDR_BTNMENU));
	hMenu = GetSubMenu(hMenu, 0);

	SetMenuDefaultItem(hMenu, IDM_BUTTON_LAUNCH, FALSE);

	//disable useless menu items when clicked outside button
	if(nItemId > ListView_GetItemCount(hwndMainListView) || nItemId < 0){
		EnableMenuItem(hMenu, IDM_BUTTON_EDIT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_BUTTON_LAUNCH, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_BUTTON_MOVELEFT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_BUTTON_MOVERIGHT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_BUTTON_REMOVE, MF_BYCOMMAND | MF_GRAYED);
		SetMenuDefaultItem(hMenu, IDM_BUTTON_ADDNEWBUTTON, FALSE);
	}
	

	UpdateMenu(hwndMainListView, hMenu);
	TrackPopupMenu(hMenu, 0, displayCoords.x, displayCoords.y, 0, CMainWnd.GetSafeHwnd(), NULL);
	return TRUE;
}