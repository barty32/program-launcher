//
//  ____                                        _                           _                               
// |  _ \ _ __ ___   __ _ _ __ __ _ _ __ ___   | |    __ _ _   _ _ __   ___| |__   ___ _ __ ___ _ __  _ __  
// | |_) | '__/ _ \ / _` | '__/ _` | '_ ` _ \  | |   / _` | | | | '_ \ / __| '_ \ / _ \ '__/ __| '_ \| '_ \ 
// |  __/| | | (_) | (_| | | | (_| | | | | | | | |__| (_| | |_| | | | | (__| | | |  __/ |_| (__| |_) | |_) |
// |_|   |_|  \___/ \__, |_|  \__,_|_| |_| |_| |_____\__,_|\__,_|_| |_|\___|_| |_|\___|_(_)\___| .__/| .__/ 
//                  |___/                                                                      |_|   |_|   
//
//  This file contains functions related to main window (WinMain, init functions and window procedure)
//
// 
// #include "LICENSE"
//

#include "pch.h"
#define DECLARE_GLOBAL_VARIABLES
#include "Program Launcher.h"

// The one and only CProgramLauncher object
CProgramLauncher theApp;

// Startup sequence:
// 1. Get ini file location
// 2. Parse ini file
// 3. Load options
// 4. Load categories
// 5. Create and show main window
// 6. Load elements

CProgramLauncher::CProgramLauncher() noexcept{
	hInst = GetModuleHandleW(null);
	Launcher = this;

	LPWSTR resultPath = null;

	// Locate .ini file

	//First try with MAX_PATH characters
	DWORD pathLen = MAX_PATH;
	//auto path = make_unique<WCHAR[]>(pathLen);
	LPWSTR path = new WCHAR[pathLen];
	LPWSTR expPath = null;
	::GetModuleFileNameW(null, path, pathLen);

	if(GetLastError() == ERROR_INSUFFICIENT_BUFFER){
		pathLen = 32768;
		delete[] path;// .release();
		path = new WCHAR[pathLen];//make_unique<WCHAR[]>(pathLen);
		::GetModuleFileNameW(null, path, pathLen);
	}

	PathRemoveFileSpecW(path);
	PathAppendW(path, TEXT(INI_FILE_NAME));

//CHECK_EXE_FOLDER:
	if(PathFileExistsW(path)){
		if(IsFileWritable(path)){
			//success
			goto SUCCESS_EXE;
		}
	}

//CHECK_APPDATA:

	//auto expPath = make_unique<WCHAR[]>(pathLen);
	expPath = new WCHAR[32768];
	ExpandEnvironmentStringsW(L"%AppData%\\Program Launcher\\" TEXT(INI_FILE_NAME), expPath, 32768);
	//PathAppendW(expPath, TEXT(INI_FILE_NAME));

	if(PathFileExistsW(expPath)){
		if(IsFileWritable(expPath)){
			//success
			goto SUCCESS_APPDATA;
		}
	}
	//create file in exe folder

	FILE* file_handle;
	if(!(_wfopen_s(&file_handle, path, L"w,ccs=UTF-8") || !file_handle)){
		//success
		fwrite(L";hello world", sizeof(WCHAR), 12, file_handle);
		fclose(file_handle);
		delete[] expPath;
		goto SUCCESS_EXE;
	}
	
	if(!(_wfopen_s(&file_handle, expPath, L"w,ccs=UTF-8") || !file_handle)){
		//success
		fwrite(L";hello world in appdata", sizeof(WCHAR), 23, file_handle);
		fclose(file_handle);
		goto SUCCESS_APPDATA;
	}

	AfxMessageBox(IDS_INI_FAIL);
	ini = null;
	return;

SUCCESS_EXE:
	
	ini = new IniParser(path);
	PathRemoveFileSpecW(path);
	SetCurrentDirectoryW(path);
	delete[] path;
	return;

SUCCESS_APPDATA:

	ini = new IniParser(expPath);
	PathRemoveFileSpecW(expPath);
	SetCurrentDirectoryW(expPath);
	delete[] path;
	delete[] expPath;
	return;
}

CProgramLauncher::~CProgramLauncher(){
	delete this->ini;
}



BOOL CProgramLauncher::InitInstance(){

	// Initialize common controls.
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_TAB_CLASSES | ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES | ICC_UPDOWN_CLASS | ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);

	CWinApp::InitInstance();

	ini->Init();

	//load options
	options.ShowAppNames = ini->ReadInt(L"general", L"ShowAppNames", DEFAULT_SHOWAPPNAMES);
	options.CloseAfterLaunch = ini->ReadInt(L"general", L"CloseAfterLaunch", DEFAULT_CLOSEAFTERLAUNCH);
	options.WindowWidth = ini->ReadInt(L"general", L"WindowWidth", DEFAULT_WINDOW_WIDTH);
	options.WindowHeight = ini->ReadInt(L"general", L"WindowHeight", DEFAULT_WINDOW_HEIGHT);
	options.UseIconCaching = ini->ReadInt(L"general", L"UseIconCaching", DEFAULT_USEICONCACHING);

	//appereance
	options.IconSize = ini->ReadInt(L"appereance", L"IconSize", DEFAULT_SHOWAPPNAMES);
	options.IconSpacingHorizontal = ini->ReadInt(L"appereance", L"IconSpacingHorizontal", DEFAULT_HORZ_SPACING);
	options.IconSpacingVertical = ini->ReadInt(L"appereance", L"IconSpacingVertical", DEFAULT_VERT_SPACING);

	//load categories
	wstring categories(ini->ReadString(L"categories", L"Categories"));
	vector<wstring> tokens;
	wistringstream stream(categories);
	wstring temp;
	while(getline(stream, temp, L';')){
		tokens.push_back(temp);
	}

	for(size_t i = 0; i < tokens.size(); i++){
		if(!(tokens[i].empty() || tokens[i] == L"general" || tokens[i] == L"appereance" || tokens[i] == L"categories")){
			vCategories.push_back(make_shared<CCategory>(this, i, tokens[i]));
		}
	}

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CLauncherWnd* pFrame = new CLauncherWnd;
	if(!pFrame) return false;
	m_pMainWnd = pFrame;
	CMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDC_PROGRAMLAUNCHER, WS_OVERLAPPEDWINDOW, nullptr, nullptr);

	if(pFrame->CreateTabControl()){
		pFrame->CreateListView();
	}

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	//load elements
	for(auto &cat : vCategories){
		//cat->LoadElements();
		::CreateThread(nullptr, 0, LoadElements, &cat, 0, 0);
	}
	//this->UpdateListView();
	return 1;
}

int CProgramLauncher::ExitInstance(){

	this->Save();

	return CWinApp::ExitInstance();
}

void CProgramLauncher::Save(){
	//save options
	ini->WriteInt(L"general", L"ShowAppNames", options.ShowAppNames);
	ini->WriteInt(L"general", L"CloseAfterLaunch", options.CloseAfterLaunch);
	ini->WriteInt(L"general", L"WindowWidth", options.WindowWidth);
	ini->WriteInt(L"general", L"WindowHeight", options.WindowHeight);
	ini->WriteInt(L"general", L"UseIconCaching", options.UseIconCaching);

	//appereance
	ini->WriteInt(L"appereance", L"IconSize", options.IconSize);
	ini->WriteInt(L"appereance", L"IconSpacingHorizontal", options.IconSpacingHorizontal);
	ini->WriteInt(L"appereance", L"IconSpacingVertical", options.IconSpacingVertical);

	//save categories
	wstring str;
	for(auto &cat : vCategories){
		str.append(cat->wsCategoryName);
		str.append(L";");
	}
	if(!str.empty()){
		str.pop_back();
		ini->WriteString(L"categories", L"Categories", str.c_str());
	}
	else{
		ini->DeleteKey(L"categories", L"Categories");
	}

	//save elements
	for(auto &cat : vCategories){
		for(size_t i = 0; i < cat->vItems.size(); i++){
			auto &item = cat->vItems[i];
			SaveElementProperties(cat->wsCategoryName.c_str(), i, item->props);
		}
	}

	if(bRebuildIconCache){
		//::CreateThread(null, 0, RebuildIconCache, null, 0, 0);
		RebuildIconCache();
	}

	ini->Flush();
}


BEGIN_MESSAGE_MAP(CProgramLauncher, CWinApp)
	ON_COMMAND(IDM_ABOUT, &CProgramLauncher::OnAppAbout)
END_MESSAGE_MAP()


CAboutDlg::CAboutDlg() noexcept : CDialog(IDD_ABOUTBOX){}

//void CAboutDlg::DoDataExchange(CDataExchange* pDX){
//	CDialog::DoDataExchange(pDX);
//}

BOOL CAboutDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	//center the dialog
	CenterWindow();

	CString szExePath;
	CString verData;
	DWORD  verHandle = 0;
	UINT   size = 0;
	LPCWSTR lpVersion = null;

	GetModuleFileNameW(hInst, szExePath.GetBuffer(MAX_PATH), MAX_PATH);
	szExePath.ReleaseBuffer();

	DWORD  verSize = GetFileVersionInfoSizeW(szExePath, &verHandle);
	if(verSize == 0) return true;

	if(GetFileVersionInfoW(szExePath, 0, verSize, verData.GetBuffer(verSize))){
		verData.ReleaseBuffer();
		if(VerQueryValueW(verData, L"\\StringFileInfo\\000904b0\\ProductVersion", (LPVOID*)&lpVersion, &size)){
			verData.Format(L"v%s", lpVersion);
		}
	}
	GetDlgItem(IDC_ABOUT_VERSION)->SetWindowTextW(verData);
	return true;
}




// App command to run the dialog
void CProgramLauncher::OnAppAbout(){
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}






//void TabControl::OnPaint(){
//	CTabCtrl::OnPaint();
//
//	CPaintDC dc(this); // device context for painting
//					   // TODO: Add your message handler code here
//					   // Do not call CTabCtrl::OnPaint() for painting messages
//	if(this->GetItemCount() == 0){
//		CMainWnd->CList.ShowWindow(SW_HIDE);
//		RECT clientRect;
//		GetClientRect(&clientRect);
//		//dc.TextOutW(clientRect.right / 2 - 200, clientRect.bottom / 2 - 5, LoadLocalString(IDS_FIRST_STARTUP_TIP), 55);
//		dc.DrawTextW(CString(GetString(IDS_FIRST_STARTUP_TIP).c_str()), &clientRect, DT_CENTER | DT_VCENTER);
//	}
//}




