// CommonFunctions.c
//
//
// #include "LICENSE"
//
//
#include "pch.h"
#include "Program Launcher.h"




bool GetElementProperties(const wstring& categoryName, UINT iIndex, ElementProps& lpProps){
	wstring sIndex = to_wstring(iIndex);
	lpProps.wsPath = Launcher->ini->ReadString(categoryName, sIndex + L".Path", L"$err");
	if(lpProps.wsPath == L"$err")
		return false;

	lpProps.wsName =     Launcher->ini->ReadString(categoryName, sIndex + L".Name");
	lpProps.wsPathIcon = Launcher->ini->ReadString(categoryName, sIndex + L".PathIcon");
	lpProps.wsPath64 =   Launcher->ini->ReadString(categoryName, sIndex + L".Path64");
	lpProps.iIconIndex = Launcher->ini->ReadInt(categoryName, sIndex + L".IconIndex");
	lpProps.bAdmin =     Launcher->ini->ReadInt(categoryName, sIndex + L".Admin");
	lpProps.bAbsolute =  Launcher->ini->ReadInt(categoryName, sIndex + L".AbsolutePaths");

	if(lpProps.wsPathIcon == L"$path"){
		lpProps.wsPathIcon = lpProps.wsPath;
	}

	//For compatibility with old versions
	if(lpProps.wsPath64 == L"0"){
		lpProps.wsPath64 = L"";
	}

	if(!lpProps.bAbsolute){
		AddLetterToPath(lpProps.wsPath);
		AddLetterToPath(lpProps.wsPath64);
		AddLetterToPath(lpProps.wsPathIcon);
	}
	return true;
}

bool SaveElementProperties(const wstring& categoryName, UINT iIndex, const ElementProps& lpProps){
	wstring sIndex = to_wstring(iIndex);
	ElementProps props = lpProps;
	if(!lpProps.bAbsolute){
		RemoveLetterFromPath(props.wsPath);
		RemoveLetterFromPath(props.wsPath64);
		RemoveLetterFromPath(props.wsPathIcon);
	}

	if(props.wsPath == props.wsPathIcon){
		props.wsPathIcon = L"$path";
	}

	Launcher->ini->WriteString(categoryName, sIndex + L".Name", props.wsName);
	Launcher->ini->WriteString(categoryName, sIndex + L".Path", props.wsPath);
	Launcher->ini->WriteString(categoryName, sIndex + L".Path64", props.wsPath64);
	Launcher->ini->WriteString(categoryName, sIndex + L".PathIcon", props.wsPathIcon);
	Launcher->ini->WriteInt(categoryName, sIndex + L".IconIndex", props.iIconIndex);
	Launcher->ini->WriteInt(categoryName, sIndex + L".Admin", props.bAdmin);
	Launcher->ini->WriteInt(categoryName, sIndex + L".AbsolutePaths", props.bAbsolute);
	return true;
}



//
// @brief Creates a tab control, sized to fit the specified parent window's client area, and adds some tabs. 
// @return 1 ok, 0 fail
// 
bool CLauncherWnd::CreateTabControl(){

	DWORD dwStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP/*| TCS_BOTTOM*/ | TCS_MULTILINE;

	if(!CTab.CreateEx(WS_EX_CONTROLPARENT, dwStyle, RECT{0,0,0,0}, this, ID_TABCONTROL)){
		return false;
	}
	
	ResizeTabControl();

	CTab.SendMessageW(WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), false);

	for(size_t i = 0; i < Launcher->vCategories.size(); i++){
		CTab.InsertItem(i, Launcher->vCategories[i]->wsCategoryName.c_str());
	}
	return true;
}

//
// @brief Fit the specified tab control into parent window's client area
// @param hwndTabControl - handle to tab control
// @param hwndParent - handle to parent window (the application's main window). 
// 
void CLauncherWnd::ResizeTabControl(){
	CRect rc;
	this->GetClientRect(&rc);
	CRect rcStatus;
	m_statusBar.GetWindowRect(&rcStatus);
	rc.DeflateRect(0, 0, 0, rcStatus.Height());
	CTab.MoveWindow(&rc);
}




wstring ExpandEnvStrings(const wstring &source){
	//vector<WCHAR> vTemp(source.begin(), source.end());
	//vTemp.resize(MAX_PATH, 0);
	//ExpandEnvironmentStringsW(source.data(), &vTemp[0], vTemp.size());
	//return wstring(&vTemp[0]);
	wstring result;
	result.resize(6);
	DWORD size = ExpandEnvironmentStringsW(source.c_str(), result.data(), result.capacity());
	if(size > result.capacity()){
		result.resize(size);
		ExpandEnvironmentStringsW(source.c_str(), result.data(), result.capacity());
	}
	result.resize(size - 1);
	return result;
}

bool CProgramLauncher::NewItem(){
	if(vCategories.size() == 0){
		ErrorMsg(IDS_NO_CATEGORY, IDS_ERROR);
		return false;
	}
	shared_ptr<CLauncherItem> pNewItem = make_shared<CLauncherItem>(nullptr, 0, L"", L"");
	CBtnEditDlg dlg(pNewItem->props);
	dlg.bNewButton = true;
	if(dlg.DoModal() == IDOK){
		shared_ptr<CCategory> category = vCategories.at(dlg.uCategory);
		category->vItems.push_back(pNewItem);
		pNewItem->ptrParent = category.get();
		pNewItem->parentIndex = category->vItems.size() - 1;
		category->imLarge.Add(pNewItem->LoadIcon(Launcher->options.IconSize));
		category->imSmall.Add(pNewItem->LoadIcon(SMALL_ICON_SIZE));
		pNewItem->InsertIntoListView();
		Launcher->bRebuildIconCache = true;
		return true;
	}
	return false;
}

bool CProgramLauncher::NewCategory(){
	wstring wsNewName;
	CUserInputDlg dlg(wsNewName);
	if(dlg.DoModal() == 1){
		shared_ptr<CCategory> newCategory = make_shared<CCategory>(this, vCategories.size(), wsNewName);
		vCategories.push_back(newCategory);
		CMainWnd->CTab.InsertItem(CMainWnd->CTab.GetItemCount(), newCategory->wsCategoryName.c_str());
		CMainWnd->CTab.SetCurSel(newCategory->parentIndex);
		CMainWnd->ResizeListView();
		CMainWnd->CList.DeleteAllItems();
		if(ini->GetKeyCount(wsNewName) > 0){
			::CreateThread(nullptr, 0, LoadElements, &vCategories[newCategory->parentIndex], 0, nullptr);
		}
		return true;
	}
	return false;
}


void CProgramLauncher::ReindexCategories(UINT iStart){
	for(size_t i = iStart; i < vCategories.size(); ++i){
		vCategories[i]->parentIndex = i;
	}
}


CCategory* CProgramLauncher::GetCategory(UINT index){
	return vCategories.at(index).get();
}
int CProgramLauncher::GetCurrentCategoryIndex(){
	return CMainWnd->CTab.GetCurSel();
}
int CProgramLauncher::GetSelectedItemIndex(){
	return CMainWnd->CList.GetNextItem(-1, LVNI_SELECTED);
}
int CProgramLauncher::GetFocusedItemIndex(){
	return CMainWnd->CList.GetNextItem(-1, LVNI_FOCUSED);
}
CCategory* CProgramLauncher::GetCurrentCategory(){
	return vCategories.at(GetCurrentCategoryIndex()).get();
}





wstring GetString(UINT uID){
	LPWSTR strRes = nullptr;
	size_t cch = LoadStringW(hInst, uID, reinterpret_cast<LPWSTR>(&strRes), 0);
	return wstring(strRes, cch);
}


DWORD WINAPI LoadElements(LPVOID lParam){
	auto &cat = *(shared_ptr<CCategory>*)lParam;
	cat->LoadElements();
	//if(cat->parentIndex == Launcher->GetCurrentCategoryIndex()){
	//	cat->ptrParent->UpdateListView();
	//}
	return 0;
}

//
// @brief Separates (removes) icon index from specified string
// @param pszIconPath - Path to remove index from
// @return separated icon index
//
INT GetIconIndexFromPath(LPWSTR pszIconPath){

	LPWSTR lpCommaPosition;
	int nLen = wcslen(pszIconPath);
	int iIconIndex = 0;

	for(int i = nLen; i >= 0; i--){
		if(pszIconPath[i] == L','){
			lpCommaPosition = pszIconPath + i;
			iIconIndex = _wtoi(lpCommaPosition + 1);
			*lpCommaPosition = 0;
			break;
		}
	}
	return iIconIndex;
}

int ErrorMsg(const wstring& msg, const wstring& title, UINT type){
	return MessageBoxW(CMainWnd->GetSafeHwnd(), msg.c_str(), title.c_str(), type);
}

int ErrorMsg(UINT msgID, UINT titleID, UINT type){
	return MessageBoxW(CMainWnd->GetSafeHwnd(), GetString(msgID).c_str(), GetString(titleID).c_str(), type);
}



void ErrorHandlerEx(LONG wLine, LPCWSTR lpszFile, LPCWSTR lpszFunctionName){

	LPVOID  lpvMessage;
	DWORD   dwError;
	LPWSTR  lpszBuffer;
	DWORD   dwLastError = GetLastError();

	// Allow FormatMessage() to look up the error code returned by GetLastError
	dwError = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPWSTR)&lpvMessage,
		0,
		NULL
	);

	// Check to see if an error occurred calling FormatMessage()
	if(dwError == 0){
	FMT_MESSAGE_ERROR:;
		WCHAR szBuffer[100 + 2 * 11] ={0};
		_snwprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, L"An error occurred calling FormatMessage().\nError Code: %d\nFormatMessage failure error code: %d", dwLastError, GetLastError());
		MessageBoxW(CMainWnd->GetSafeHwnd(), szBuffer, L"Error", MB_ICONERROR);
		return;
	}

	// Display the error information along with the place the error happened.
	//_sntprintf_s(szBuffer, _countof(szBuffer), _TRUNCATE, TEXT("Generic, Line=%d, File=%s"), /*wLine*/0, /*lpszFile*/"0");
#ifdef _DEBUG
	//display more info if debug mode is active
	lpszBuffer = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (wcslen((LPCWSTR)lpvMessage) + 401) * sizeof(WCHAR));
	if(lpszBuffer == NULL) goto FMT_MESSAGE_ERROR;

	_snwprintf_s(lpszBuffer, (wcslen((LPCWSTR)lpvMessage) + 401), _TRUNCATE, L"%s\nError Code: %d\nFile: %s\nFunction: %s\nLine: %i", (LPWSTR)lpvMessage, GetLastError(), lpszFile, lpszFunctionName, wLine);
	MessageBoxW(CMainWnd->GetSafeHwnd(), lpszBuffer, L"Error", MB_ICONERROR | MB_OK);
	HeapFree(GetProcessHeap(), 0, lpszBuffer);
#else
	MessageBoxW(CMainWnd->GetSafeHwnd(), (LPWSTR)lpvMessage, L"Error", MB_ICONERROR | MB_OK);
#endif
	LocalFree(lpvMessage);
}


//
// @brief Updates popup menu before it shows
// @param hwndListView - handle to list view from which get the current view
// @param hMenu - handle to menu
// 
void CLauncherWnd::UpdateMenu(CMenu* menu){
	UINT uID = IDM_VIEW_LARGEICONS;

	if(Launcher->vCategories.size() == 0){
		menu->EnableMenuItem(IDM_CATEGORY_REMOVECURRENTCATEGORY, MF_GRAYED);
		menu->EnableMenuItem(IDM_CATEGORY_RENAMECURRENTCATEGORY, MF_GRAYED);
		menu->EnableMenuItem(IDM_CATEGORY_MOVELEFT, MF_GRAYED);
		menu->EnableMenuItem(IDM_CATEGORY_MOVERIGHT, MF_GRAYED);
		menu->EnableMenuItem(IDM_POPUPCATEGORY_REMOVECATEGORY, MF_GRAYED);
		menu->EnableMenuItem(IDM_POPUPCATEGORY_RENAME, MF_GRAYED);
		menu->EnableMenuItem(IDM_POPUPCATEGORY_MOVELEFT, MF_GRAYED);
		menu->EnableMenuItem(IDM_POPUPCATEGORY_MOVERIGHT, MF_GRAYED);
	}
	if((UINT)Launcher->GetSelectedItemIndex() >= static_cast<UINT>(CList.GetItemCount())){
		menu->EnableMenuItem(IDM_BUTTON_EDIT, MF_GRAYED);
		menu->EnableMenuItem(IDM_BUTTON_LAUNCH, MF_GRAYED);
		menu->EnableMenuItem(IDM_BUTTON_MOVELEFT, MF_GRAYED);
		menu->EnableMenuItem(IDM_BUTTON_MOVERIGHT, MF_GRAYED);
		menu->EnableMenuItem(IDM_BUTTON_REMOVE, MF_GRAYED);
	}

	//check the appropriate view menu item
	switch(CList.GetView()){
		case LVS_ICON:
			uID = IDM_VIEW_LARGEICONS;
			break;
		case LVS_SMALLICON:
			uID = IDM_VIEW_SMALLICONS;
			break;
		case LVS_LIST:
			uID = IDM_VIEW_LIST;
			break;
		case LVS_REPORT:
			uID = IDM_VIEW_DETAILS;
			break;
	}
	menu->CheckMenuRadioItem(IDM_VIEW_DETAILS, IDM_VIEW_SMALLICONS, uID, MF_BYCOMMAND | MF_CHECKED);
}



bool CLauncherWnd::DoTabControlContextMenu(UINT iItemId, INT pX, INT pY){
	POINT displayCoords;

	if(pX == -1 && pY == -1){
		RECT rcItem;
		CTab.GetItemRect(iItemId, &rcItem);

		displayCoords.x = rcItem.right - (rcItem.right - rcItem.left) / 2;
		displayCoords.y = rcItem.bottom - (rcItem.bottom - rcItem.top) / 2;
		CTab.ClientToScreen(&displayCoords);
	}
	else{
		displayCoords.x = pX;
		displayCoords.y = pY;
	}

	HMENU hMenu = LoadMenuW(hInst, MAKEINTRESOURCEW(IDR_CATMENU));
	hMenu = GetSubMenu(hMenu, 0);

	TrackPopupMenu(hMenu, 0, displayCoords.x, displayCoords.y, 0, CMainWnd->GetSafeHwnd(), NULL);
	return true;
}


wstring StrJoin(vector<wstring> arrStr, const wchar_t* lpDelim){
	if(arrStr.empty()) return L"";
	wstring ret;
	for(const auto &s : arrStr){
		ret += s + lpDelim;
	}
	ret = ret.substr(0, ret.size() - wcslen(lpDelim));
	return ret;
}

wstring StrJoinFromInt(vector<int> arrStr, const wchar_t* lpDelim){
	if(arrStr.empty()) return L"";
	wstring ret;
	for(const auto &s : arrStr){
		ret += to_wstring(s) + lpDelim;
	}
	ret = ret.substr(0, ret.size() - wcslen(lpDelim));
	return ret;
}


vector<wstring> StrSplit(const wstring& lpSource, const wchar_t lpDelim){
	vector<wstring> tokens;
	wistringstream stream(lpSource);
	wstring temp;
	while(getline(stream, temp, lpDelim)){
		tokens.push_back(temp);
	}
	return tokens;
}

vector<int> StrSplitToInt(const wstring& lpSource, const wchar_t lpDelim){
	vector<int> tokens;
	wistringstream stream(lpSource);
	wstring temp;
	while(getline(stream, temp, lpDelim)){
		tokens.push_back(wcstol(temp.c_str(), null, 10));
	}
	return tokens;
}


//
// @brief Adds drive letter from specified path
// @param &path - Path to add letter to
// @return Returns 1 if letter was added, 0 if it already exists
//
bool AddLetterToPath(wstring& path){

	WCHAR szDriveLetter[2];
	if(path.size() < 2) return false;

	// Get current drive letter
	if(!GetModuleFileNameW(null, szDriveLetter, 2)){
		ErrorHandlerDebug();
	}

	// Check if drive letter already exists
	wstring expandedPath = ExpandEnvStrings(path);
	if(expandedPath[0] == L'*' && expandedPath[1] == L':'){
		path[0] = szDriveLetter[0];
		return true;
	}
	return false;
}

//
// @brief Removes drive letter from specified path
// @param lpszPath - Path to remove letter from
// @return Returns 1 if letter was removed, 0 if not found
//
bool RemoveLetterFromPath(wstring& path){
	if(((path[0] > 0x40 && path[0] < 0x5B) || (path[0] > 0x60 && path[0] < 0x7B)) && path[1] == L':'){
		path[0] = L'*';
		return true;
	}
	return false;
}


bool Is64BitWindows(){
#if defined(_WIN64)
	return true;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
	// 32-bit programs run on both 32-bit and 64-bit Windows, so must sniff
	BOOL f64 = FALSE;
	return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
	return false; // Win64 does not support Win16
#endif
}

bool IsFileWritable(const fs::path& filePath){
	FILE* hFile;
	if(_wfopen_s(&hFile, filePath.c_str(), L"a") || !hFile){
		return false;
	}
	fclose(hFile);
	return true;
}

