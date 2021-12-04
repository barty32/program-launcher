
#include "pch.h"
#include "Program Launcher.h"


CLauncherItem::CLauncherItem(CCategory* ptrParent, const UINT parentIndex, const wstring& wsName, const wstring& wsPath, const wstring& wsPathIcon, const int iIconIndex, const wstring& wsPath64, const bool bAdmin, const bool bAbsolute)
	:
	ptrParent(ptrParent),
	parentIndex(parentIndex)
{
	props.wsName = wsName;
	props.wsPath = wsPath;
	props.wsPathIcon = wsPathIcon;
	props.iIconIndex = iIconIndex;
	props.wsPath64 = wsPath64;
	props.bAdmin = bAdmin;
	props.bAbsolute = bAbsolute;
}

CLauncherItem::CLauncherItem(CCategory* ptrParent, const UINT parentIndex, const ElementProps& props)
	:
	ptrParent(ptrParent),
	parentIndex(parentIndex),
	props(props)
{}


CLauncherItem::~CLauncherItem(){
	//Launcher->CList.DeleteItem(parentIndex);
	try{
		if(ptrParent && ptrParent->imLarge.m_hImageList && ptrParent->imSmall.m_hImageList){
			ptrParent->imLarge.Remove(parentIndex);
			ptrParent->imSmall.Remove(parentIndex);
		}
		DestroyIcon(hSmallIcon);
		DestroyIcon(hItemIcon);
	}catch(...){
		ErrorHandlerDebug();
	}
}


bool CLauncherItem::Edit(){
	CBtnEditDlg dlg(props);
	dlg.bNewButton = false;
	if(dlg.DoModal() == IDOK){
		HICON hSmall;
		ptrParent->imLarge.Replace(parentIndex, this->LoadIcon(MAKELPARAM(Launcher->options.IconSize, SMALL_ICON_SIZE), &hSmall));
		ptrParent->imSmall.Replace(parentIndex, hSmall);
		this->UpdateInListView();
		Launcher->bRebuildIconCache = true;
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
		Launcher->bRebuildIconCache = true;
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
	CMainWnd->CList.SetItemState(newPos, LVIS_SELECTED | LVIS_FOCUSED, 0xFF);
	Launcher->bRebuildIconCache = true;

	return true;
}


//
// @brief Launches program based on button index and category
// @param nBtnId - id of the button
// @param nCategoryIndex - id of category
// @return 1 if success, 0 if fail (Returns the handle to the new process)
// 
bool CLauncherItem::Launch() const{

	//expand enviromnent variables
	wstring wsExpPath = ExpandEnvStrings(wsPath);
	wstring wsExpPath64;
	CString directory;

	SHELLEXECUTEINFO shExInfo;
	shExInfo.cbSize = sizeof(shExInfo);
	shExInfo.fMask = 0;//SEE_MASK_NOCLOSEPROCESS;
	shExInfo.hwnd = CMainWnd->GetSafeHwnd();
	shExInfo.lpVerb = bAdmin ? L"runas" : nullptr;
	if(Is64BitWindows() && wsPath64.size()){
		wsExpPath64 = ExpandEnvStrings(wsPath64);
		shExInfo.lpFile = wsExpPath64.c_str();
		directory = wsExpPath64.c_str();
	}
	else{
		shExInfo.lpFile = wsExpPath.c_str();
		directory = wsExpPath.c_str();
	}
	PathRemoveFileSpecW(directory.GetBuffer());
	directory.ReleaseBuffer();
	shExInfo.lpDirectory = directory;
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

