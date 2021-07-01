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
HWND CreateListView(HWND hwndParent){
	DWORD       dwStyle;
	HIMAGELIST  himlSmall;
	HIMAGELIST  himlLarge;

	dwStyle = WS_TABSTOP |
		WS_CHILD |
		//WS_BORDER |
		WS_VISIBLE |
		LVS_AUTOARRANGE |
		LVS_SINGLESEL |
		LVS_ICON |
		LVS_EDITLABELS;

	hwndMainListView = CreateWindowEx(WS_EX_CONTROLPARENT,	// ex style
		WC_LISTVIEW,								// class name - defined in commctrl.h
		TEXT(""),									// dummy text
		dwStyle,									// style
		0,											// x position
		0,											// y position
		0,											// width
		0,											// height
		hwndParent,									// parent
		(HMENU)ID_LISTVIEW,							// ID
		hInst,										// instance
		NULL);										// no extra data

	if(!hwndMainListView){
		return NULL;
	}

	ListView_SetExtendedListViewStyleEx(hwndMainListView, 0, LVS_EX_INFOTIP | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_AUTOSIZECOLUMNS);

	ResizeListView(hwndMainListView);

	//load spacing settings from ini
	INT nHorzSpacing = GetSettingInt(TEXT("appereance"), TEXT("IconSpacingHorizontal"), DEFAULT_HORZ_SPACING);
	INT nVertSpacing = GetSettingInt(TEXT("appereance"), TEXT("IconSpacingVertical"), DEFAULT_VERT_SPACING);
	ListView_SetIconSpacing(hwndMainListView, nHorzSpacing, nVertSpacing);

	//load icon size from ini
	INT nIconSize = GetSettingInt(TEXT("appereance"), TEXT("IconSize"), DEFAULT_ICON_SIZE);

	//create the image lists
	himlSmall = ImageList_Create(SMALL_ICON_SIZE, SMALL_ICON_SIZE, ILC_COLOR32, 1, 0);
	himlLarge = ImageList_Create(nIconSize, nIconSize, ILC_COLOR32, 1, 0);

	if(himlSmall){
		ListView_SetImageList(hwndMainListView, himlSmall, LVSIL_SMALL);
	}
	if(himlLarge){
		ListView_SetImageList(hwndMainListView, himlLarge, LVSIL_NORMAL);
	}
	
	

	LVCOLUMN   lvColumn;
	//TCHAR       szString[7][20] = {TEXT("Name"), TEXT("Path"), TEXT("64-bit Path"), TEXT("Path to icon"), TEXT("Icon index"), TEXT("Run as Admin"), TEXT("Uses absolute paths")};
	INT			nColWidths[7] = {160,300,300,300,100,80,80};

	//initialize the columns
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	
	for(int i = 0; i < 7; i++){
		//lvColumn.pszText = szString[i];
		lvColumn.pszText = LoadLocalString(IDS_COL_NAME + i);
		lvColumn.iSubItem = i;
		lvColumn.cx = nColWidths[i];
		ListView_InsertColumn(hwndMainListView, i, &lvColumn);
	}

	UpdateListView(hwndMainListView, nIconSize);

	SetWindowTheme(hwndMainListView, L"Explorer", NULL);

	return hwndMainListView;
}

//
// @brief Updates image lists for specified list view control
// @param hwndListView - handle to list view
// @param nIconSizeOfLargeImageList - size of the large image list, size of small image list is 16x16
// @return Returns always TRUE
// 
BOOL UpdateImageLists(HWND hwndListView, INT nIconSizeOfLargeImageList){
	HIMAGELIST himlSmall;
	HIMAGELIST himlLarge;
	HICON hIcon;
	INT cCategorySectionCount;
	INT nCurrentCategory;
	
	himlSmall = ListView_GetImageList(hwndListView, LVSIL_SMALL);
	himlLarge = ListView_GetImageList(hwndListView, LVSIL_NORMAL);
	nCurrentCategory = TabCtrl_GetCurSel(hwndTab);
	cCategorySectionCount = GetCategorySectionCount(nCurrentCategory);
	
	ImageList_RemoveAll(himlSmall);

	//set up the small image list
	for(int i = 0; i < cCategorySectionCount; i++){
		hIcon = LoadIconForButton(i, nCurrentCategory, SMALL_ICON_SIZE);
		if(!hIcon){
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DEFAULT));
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
	for(int j = 0; j < cCategorySectionCount; j++){
		hIcon = LoadIconForButton(j, nCurrentCategory, nIconSizeOfLargeImageList);
		if(!hIcon){
			hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_DEFAULT));
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
BOOL UpdateListView(HWND hwndListView, INT nIconSize){

	//empty the list
	ListView_DeleteAllItems(hwndListView);

	UpdateImageLists(hwndListView, nIconSize);

	LVITEM listItem;
	ZeroMemory(&listItem, sizeof(LVITEM));
	LAUNCHERENTRY ButtonProps;
	INT cCategorySectionCount = GetCategorySectionCount(TabCtrl_GetCurSel(hwndTab));

	switch(GetSettingInt(TEXT("general"), TEXT("ShowAppNames"), DEFAULT_SHOWAPPNAMES)){
		case 0:
			listItem.mask = LVIF_IMAGE;
			break;
		case 1:
			listItem.mask = LVIF_IMAGE;
			break;
		default:
			listItem.mask = LVIF_IMAGE | LVIF_TEXT;
	}

	for(int j = 0; j < cCategorySectionCount; j++){
		GetSettingButtonStruct(arrszCategoryList[TabCtrl_GetCurSel(hwndTab)], j, &ButtonProps, TRUE);
		listItem.pszText = ButtonProps.szName;
		listItem.iItem = j;
		listItem.iImage = j;
		ListView_InsertItem(hwndListView, &listItem);

		for(int k = 1; k < 7; k++){
			//Add Subitems to columns
			WCHAR szBuf[INT_STR_LEN];
			LPWSTR lpText;
			switch(k){
				case 1:// Path
					lpText = ButtonProps.szPath;
					break;
				case 2:// 64-bit Path
					if(ButtonProps.szPath64[0] == L'0'){
						lpText = L"";
					}
					else{
						lpText = ButtonProps.szPath64;
					}
					break;
				case 3:// Path to icon
					lpText = ButtonProps.szPathIcon;
					break;
				case 4:// Icon index
					_itow_s(ButtonProps.nIconIndex, szBuf, _countof(szBuf), 10);
					lpText = szBuf;
					break;
				case 5:// Run as admin
					if(ButtonProps.bAdmin){
						lpText = LoadLocalString(IDS_YES);
					}
					else{
						lpText = LoadLocalString(IDS_NO);
					}
					break;
				case 6:// Absolute paths
					if(ButtonProps.bAbsolute){
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
			ListView_SetItemText(hwndListView, j, k, lpText);
		}
	}

	return TRUE;
}

//
// @brief Switches view of the specified list view control
// @param hwndListView - handle to the list view
// @param dwView - can be one of these values: LVS_ICON, LVS_SMALLICON, LVS_LIST, LVS_REPORT
// 
void SwitchListView(HWND hwndListView, DWORD dwView){
	DWORD dwStyle = GetWindowLong(hwndListView, GWL_STYLE);

	SetWindowLong(hwndListView, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	ResizeListView(hwndListView);
}

//
// @brief Resizes the specified list view control to fit into its parent window
// @param hwndListView - handle to the list view
// 
void ResizeListView(HWND hwndListView){
	RECT rcTabClientArea;
	GetClientRect(hwndTab, &rcTabClientArea);
	TabCtrl_AdjustRect(hwndTab, FALSE, &rcTabClientArea);

	MoveWindow(hwndListView,
		rcTabClientArea.left,
		rcTabClientArea.top,
		rcTabClientArea.right - rcTabClientArea.left,
		rcTabClientArea.bottom - rcTabClientArea.top,
		TRUE);
}




INT DoListViewContextMenu(HWND hwndListView, INT pX, INT pY){
	int nItemId;
	POINT displayCoords;
	nItemId = SendMessageW(hwndListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);

	if(pX == -1 && pY == -1){
		RECT rcItem;
		ListView_GetItemRect(hwndListView, nItemId, &rcItem, LVIR_BOUNDS);

		displayCoords.x = rcItem.right - (rcItem.right - rcItem.left) / 2;
		displayCoords.y = rcItem.bottom - (rcItem.bottom - rcItem.top) / 2;
		ClientToScreen(hwndListView, &displayCoords);
	}
	else{
		displayCoords.x = pX;
		displayCoords.y = pY;
	}

	HMENU hMenu = LoadMenuW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDR_BTNMENU));
	hMenu = GetSubMenu(hMenu, 0);

	SetMenuDefaultItem(hMenu, ID_BUTTON_LAUNCH, FALSE);

	//disable useless menu items when clicked outside button
	if(nItemId > ListView_GetItemCount(hwndListView) || nItemId < 0){
		EnableMenuItem(hMenu, ID_BUTTON_EDIT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, ID_BUTTON_LAUNCH, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, ID_BUTTON_MOVELEFT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, ID_BUTTON_MOVERIGHT, MF_BYCOMMAND | MF_GRAYED);
		EnableMenuItem(hMenu, ID_BUTTON_REMOVE, MF_BYCOMMAND | MF_GRAYED);
		SetMenuDefaultItem(hMenu, ID_BUTTON_ADDNEWBUTTON, FALSE);
	}
	

	UpdateMenu(hwndListView, hMenu);
	TrackPopupMenu(hMenu, 0, displayCoords.x, displayCoords.y, 0, hwndMain, NULL);
	return TRUE;
}