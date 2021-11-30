//CommonFunctions.c
//
//
//
//
//
#include "pch.h"
#include "Program Launcher.h"





//UINT GetElementCount(LPCWSTR lpszCategory){
//	//UINT i = 0;
//	//while(i < MAX_CATEGORY_SIZE){
//	//	//ReadIniValue throws when key is not found
//	//	try{
//	//		Launcher->ini->ReadValue(lpszCategory, wstring(to_wstring(i) + L".Path").c_str());
//	//	}
//	//	catch(const std::runtime_error&){
//	//		break;
//	//	}
//	//	i++;
//	//}
//	//return i;
//	return Launcher->ini->GetKeyCount(lpszCategory);
//}

bool GetElementProperties(const wstring& categoryName, UINT iIndex, ElementProps& lpProps){
	wstring sIndex = to_wstring(iIndex);
	lpProps.wsName = Launcher->ini->ReadString(categoryName, wstring(sIndex + L".Name").c_str());
	lpProps.wsPath = Launcher->ini->ReadString(categoryName, wstring(sIndex + L".Path").c_str());
	lpProps.wsPathIcon = Launcher->ini->ReadString(categoryName, wstring(sIndex + L".PathIcon").c_str());
	lpProps.iIconIndex = Launcher->ini->ReadInt(categoryName, wstring(sIndex + L".IconIndex").c_str());
	lpProps.wsPath64 = Launcher->ini->ReadString(categoryName, wstring(sIndex + L".Path64").c_str());
	lpProps.bAdmin = Launcher->ini->ReadInt(categoryName, wstring(sIndex + L".Admin").c_str());
	lpProps.bAbsolute = Launcher->ini->ReadInt(categoryName, wstring(sIndex + L".AbsolutePaths").c_str());

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

bool SaveElementProperties(LPCWSTR categoryName, UINT iIndex, const ElementProps& lpProps){
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

	Launcher->ini->WriteString(categoryName, wstring(sIndex + L".Name").c_str(), props.wsName.c_str());
	Launcher->ini->WriteString(categoryName, wstring(sIndex + L".Path").c_str(), props.wsPath.c_str());
	Launcher->ini->WriteString(categoryName, wstring(sIndex + L".Path64").c_str(), props.wsPath64.c_str());
	Launcher->ini->WriteString(categoryName, wstring(sIndex + L".PathIcon").c_str(), props.wsPathIcon.c_str());
	Launcher->ini->WriteInt(categoryName, wstring(sIndex + L".IconIndex").c_str(), props.iIconIndex);
	Launcher->ini->WriteInt(categoryName, wstring(sIndex + L".Admin").c_str(), props.bAdmin);
	Launcher->ini->WriteInt(categoryName, wstring(sIndex + L".AbsolutePaths").c_str(), props.bAbsolute);
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




wstring ExpandEnvStrings(wstring &source){
	vector<WCHAR> vTemp(source.begin(), source.end());
	vTemp.resize(MAX_PATH, 0);
	ExpandEnvironmentStringsW(source.data(), &vTemp[0], vTemp.size());
	return wstring(&vTemp[0]);
}

wstring RemoveFileSpecFromPath(wstring& path){
	vector<WCHAR> vTemp(path.begin(), path.end());
	PathRemoveFileSpecW(&vTemp[0]);
	return wstring(&vTemp[0]);
	//return wstring{strRes, static_cast<size_t>(cch)};
}

bool CProgramLauncher::NewItem(){
	if(vCategories.size() == 0){
		CMainWnd->MessageBoxW(GetString(IDS_NO_CATEGORY).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION);
		return false;
	}
	shared_ptr<CLauncherItem> pNewItem = make_shared<CLauncherItem>(nullptr, -1, L"", L"");
	if(DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_BTNSETTINGDLG), CMainWnd->GetSafeHwnd(), BtnEditDlgProc, (LPARAM)pNewItem.get())){
		shared_ptr<CCategory> category = vCategories.at(pNewItem->parentIndex);
		category->vItems.push_back(pNewItem);
		pNewItem->ptrParent = category.get();
		pNewItem->parentIndex = category->vItems.size() - 1;
		category->imLarge.Add(pNewItem->LoadIcon(Launcher->options.IconSize));
		category->imSmall.Add(pNewItem->LoadIcon(SMALL_ICON_SIZE));
		pNewItem->InsertIntoListView();
		return true;
	}
	return false;
}

bool CProgramLauncher::NewCategory(){
	wstring wsNewName;
	CUserInputDlg dlg(&wsNewName);
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
	int cch = LoadStringW(hInst, uID, reinterpret_cast<LPWSTR>(&strRes), 0);
	return wstring{strRes, static_cast<size_t>(cch)};
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
	BOOL bFoundDelim = FALSE;

	if(path.size() < 2) return false;

	// Get current drive letter
	if(!GetModuleFileNameW(null, szDriveLetter, 2)){
		ErrorHandlerDebug();
	}

	// Check if drive letter already exists
	wstring expandedPath = ExpandEnvStrings(path);
	if(expandedPath[0] == L'*' && expandedPath[1] == L':'){
		//path.insert(path.begin(), szDriveLetter[0]);
		//path.insert(path.begin(), L':');
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
	//size_t pos = path.find(L':');
	//if(pos != path.npos){
	//	path.erase(path.begin(), path.begin() + ++pos);
	//	return true;
	//}
	//return false;
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


bool IsFileWritable(const wchar_t* file_path){
	FILE* file_handle;
	if(_wfopen_s(&file_handle, file_path, L"a") || !file_handle){
		return false;
	}
	fclose(file_handle);
	return true;
}

//
// @brief Centers hwndDialog in hwndParent
// @param hwndParent - Handle to parent window
// @param hwndDialog - Handle to dialog to be centered
//
void CenterDialog(HWND hwndParent, HWND hwndDialog){
	//center the dialog
	RECT rectWindow, rectParent;
	GetWindowRect(hwndDialog, &rectWindow);
	GetWindowRect(hwndParent, &rectParent);

	int nWidth = rectWindow.right - rectWindow.left;
	int nHeight = rectWindow.bottom - rectWindow.top;

	int nX = ((rectParent.right - rectParent.left) - nWidth) / 2 + rectParent.left;
	int nY = ((rectParent.bottom - rectParent.top) - nHeight) / 2 + rectParent.top;

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// make sure that the dialog box never moves outside of the screen
	if(nX < 0) nX = 0;
	if(nY < 0) nY = 0;
	if(nX + nWidth > nScreenWidth) nX = nScreenWidth - nWidth;
	if(nY + nHeight > nScreenHeight) nY = nScreenHeight - nHeight;

	MoveWindow(hwndDialog, nX, nY, nWidth, nHeight, FALSE);
}

