//
//  _ _     _           _                               
// | (_)___| |_  __   _(_) _____      _____ _ __  _ __  
// | | / __| __| \ \ / / |/ _ \ \ /\ / / __| '_ \| '_ \ 
// | | \__ \ |_   \ V /| |  __/\ V  V / (__| |_) | |_) |
// |_|_|___/\__|   \_/ |_|\___| \_/\_(_)___| .__/| .__/ 
//                                         |_|   |_|    
//

#include "pch.h"
#include "Program Launcher.h"

#ifndef LVS_EX_AUTOSIZECOLUMNS
#define LVS_EX_AUTOSIZECOLUMNS 0
#endif

#define COL_COUNT 7

//
// @brief Creates a list view, sized to fit the specified parent window's client area
// @param hwndParent - parent window (the application's main window). 
// @return True: success, false: fail
// 
bool CLauncherWnd::CreateListView(){

	DWORD dwStyle = WS_TABSTOP |
					WS_CHILD |
					WS_VISIBLE |
					LVS_AUTOARRANGE |
					LVS_SINGLESEL |
					LVS_ICON |
					LVS_EDITLABELS |
					LVS_SHOWSELALWAYS;

	if(!CList.CreateEx(WS_EX_CONTROLPARENT, dwStyle, RECT{0,0,0,0}, this, ID_LISTVIEW)){
		return false;
	}

	DWORD dwExStyle = CList.GetExtendedStyle();

	dwExStyle |= LVS_EX_INFOTIP |
				 LVS_EX_LABELTIP |
				 LVS_EX_DOUBLEBUFFER |
				 LVS_EX_FULLROWSELECT |
				 LVS_EX_HEADERDRAGDROP |
				 LVS_EX_AUTOSIZECOLUMNS;

	switch(Launcher->options.ShowAppNames){
		case 0:
			dwExStyle &= ~LVS_EX_INFOTIP;
			[[fallthrough]];
		case 1:
			dwExStyle |= LVS_EX_HIDELABELS;
			break;
	}
	dwExStyle &= ~LVS_EX_GRIDLINES;
	CList.SetExtendedStyle(dwExStyle);

	CList.SetIconSpacing(Launcher->options.IconSpacingHorizontal, Launcher->options.IconSpacingVertical);
	try{
		CList.SetImageList(&Launcher->GetCurrentCategory()->imSmall, LVSIL_SMALL);
		CList.SetImageList(&Launcher->GetCurrentCategory()->imLarge, LVSIL_NORMAL);
	} catch(...){}

	this->ResizeListView();

	//load list view mode
	CList.SetView(Launcher->ini->ReadInt(L"general", L"ListViewMode"));

	//load columns widths
	const int defaultColWidths[COL_COUNT] = {160,300,300,300,100,80,80};
	vector<int> colWidths = StrSplitToInt(Launcher->ini->ReadString(L"general", L"ColumnWidths"));
	if(colWidths.size() < COL_COUNT || colWidths[0] == 0){
		for(int i = colWidths.size(); i < COL_COUNT; i++){
			colWidths.push_back(defaultColWidths[i]);
		}
	}

	//insert columns
	for(int i = 0; i < COL_COUNT; i++){
		CList.InsertColumn(i, GetString(IDS_COL_NAME + i).c_str(), LVCFMT_LEFT, colWidths[i], i);
	}

	//load column order
	vector<int> colOrder = StrSplitToInt(Launcher->ini->ReadString(L"general", L"ColumnOrder"));
	if(colOrder.size() == COL_COUNT){
		CList.SetColumnOrderArray(COL_COUNT, colOrder.data());
	}

	this->UpdateListView();
	::SetWindowTheme(CList.GetSafeHwnd(), L"Explorer", null);
	return true;
}

//
// @brief Updates image lists for specified list view control
// @param hwndListView - handle to list view
// @param nIconSizeOfLargeImageList - size of the large image list, size of small image list is 16x16
// @return Returns always TRUE
// 
void CCategory::UpdateImageLists(int nIconSizeOfLargeImageList){
	
	ImageList_RemoveAll(imSmall.GetSafeHandle());
	ImageList_RemoveAll(imLarge.GetSafeHandle());

	if(nIconSizeOfLargeImageList >= 0){
		ImageList_SetIconSize(imLarge.GetSafeHandle(), nIconSizeOfLargeImageList, nIconSizeOfLargeImageList);
	}else{
		ImageList_GetIconSize(imLarge.GetSafeHandle(), &nIconSizeOfLargeImageList, &nIconSizeOfLargeImageList);
	}

	for(auto &item : vItems){
		HICON hSmall;
		imLarge.Add(item->LoadIcon(Launcher->options.IconSize, &hSmall));
		imSmall.Add(hSmall);//item->loadIcon(SMALL_ICON_SIZE)
	}

	//// Open the archive to store the image list in.
	//WCHAR test[260];
	//GetModuleFileNameW(null, test, 260);
	//PathRemoveFileSpecW(test);
	//int ret = SetCurrentDirectoryW(test);
	//GetCurrentDirectoryW(260, test);
	//CFile myFile(L"iconcache.db", CFile::modeCreate | CFile::modeWrite);
	//CArchive ar(&myFile, CArchive::store);

	//// Store the image list in the archive.
	//imLarge.Write(&ar);
}

//
// @brief Updates specified list view control
// @param hwndListView - handle to the list view
// @return Returns always TRUE
// 
void CLauncherWnd::UpdateListView(bool bUpdateImageList){

	//empty the list
	CList.DeleteAllItems();
	try{
		CList.SetImageList(&Launcher->GetCurrentCategory()->imLarge, LVSIL_NORMAL);
		CList.SetImageList(&Launcher->GetCurrentCategory()->imSmall, LVSIL_SMALL);

		if(bUpdateImageList){
			Launcher->GetCurrentCategory()->UpdateImageLists();
		}

		for(auto &item : Launcher->GetCurrentCategory()->vItems){
			item->InsertIntoListView();
		}
	}catch(...){}
}

//
// @brief Switches view of the specified list view control
// @param hwndListView - handle to the list view
// @param dwView - can be one of these values: LVS_ICON, LVS_SMALLICON, LVS_LIST, LVS_REPORT
// 
void CLauncherWnd::SwitchListView(DWORD dwView){
	CList.SetView(dwView);
}

//
// @brief Resizes the specified list view control to fit into its parent window
// @param hwndListView - handle to the list view
// 
void CLauncherWnd::ResizeListView(){
	RECT rcTab;
	CTab.GetClientRect(&rcTab);
	CTab.AdjustRect(false, &rcTab);
	CList.MoveWindow(&rcTab);
}


void CLauncherWnd::ResetListViewColumns(){

	const int colWidths[] = {160,300,300,300,100,80,80};
	int colOrder[COL_COUNT];
	for(int i = 0; i < COL_COUNT; i++){
		CList.SetColumnWidth(i, colWidths[i]);
		colOrder[i] = i;
	}
	CList.SetColumnOrderArray(COL_COUNT, colOrder);
	this->Invalidate(0);
}




bool CLauncherWnd::DoListViewContextMenu(INT pX, INT pY){
	POINT displayCoords;
	UINT nItemId = (UINT)Launcher->GetSelectedItemIndex();

	if(pX == -1 && pY == -1){//Context menu called by keyboard
		if(nItemId < static_cast<UINT>(CList.GetItemCount())){
			RECT rcItem;
			CList.GetItemRect(nItemId, &rcItem, LVIR_BOUNDS);
			displayCoords.x = rcItem.right - (rcItem.right - rcItem.left) / 2;
			displayCoords.y = rcItem.bottom - (rcItem.bottom - rcItem.top) / 2;
		}
		else{
			displayCoords.x = 1;
			displayCoords.y = 1;
		}
		CList.ClientToScreen(&displayCoords);
	}
	else{
		displayCoords.x = pX;
		displayCoords.y = pY;
	}

	CMenu menu;
	menu.LoadMenuW(IDR_BTNMENU);
	CMenu* submenu = menu.GetSubMenu(0);

	//disable useless menu items when clicked outside button
	if(nItemId >= static_cast<UINT>(CList.GetItemCount())){
		submenu->SetDefaultItem(IDM_BUTTON_ADDNEWBUTTON);
	} 
	else{
		submenu->SetDefaultItem(IDM_BUTTON_LAUNCH);
	}
	this->UpdateMenu(submenu);
	submenu->TrackPopupMenu(0, displayCoords.x, displayCoords.y, CMainWnd);
	return true;
}