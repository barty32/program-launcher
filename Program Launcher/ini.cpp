//
//  _       _                   
// (_)_ __ (_)  ___ _ __  _ __  
// | | '_ \| | / __| '_ \| '_ \ 
// | | | | | || (__| |_) | |_) |
// |_|_| |_|_(_)___| .__/| .__/ 
//                 |_|   |_|    
//
// This file contains functions to read and write to ini file
// 
// Functions included in this file:
// 
//		INT GetSettingString(LPCTSTR Section, LPCTSTR Key, LPTSTR pszBuffer, DWORD nSize, LPCTSTR lpDefault);
//		
//		INT SetSettingString(LPCTSTR Section, LPCTSTR Key, LPCTSTR lpString);
//		
//		UINT GetSettingInt(LPCTSTR Section, LPCTSTR Key, INT nDefault);
//		
//		INT SetSettingInt(LPCTSTR Section, LPCTSTR Key, INT nNumber);
//		
//		INT GetSettingButtonStruct(LPCTSTR lpszCategoryName, INT nButtonIndex, LaunchButton* lpStruct, UINT uSizeStruct, BOOL bRequestAbsolute);
//		
//		INT SetSettingButtonStruct(LPCTSTR lpszCategoryName, INT nButtonIndex, LaunchButton* lpStruct, UINT uSizeStruct);
//		
//		INT DeleteSettingKey(LPCTSTR lpszSection, LPCTSTR lpszKeyName);
//		
//		INT CreateIniFile();
// 
//  Copyright ©2021, barty12
//

#include "Program Launcher.h"


static wstring trim(const wstring& s){
	auto start = s.begin();
	while(start != s.end() && std::isspace(*start)){
		start++;
	}
	auto end = s.end();
	do{
		end--;
	} while(std::distance(start, end) > 0 && std::isspace(*end));

	return wstring(start, end + 1);
}

int CProgramLauncher::Init(){

	//load options
	options.ShowAppNames =     ReadIniInt(L"general", L"ShowAppNames", DEFAULT_SHOWAPPNAMES);
	options.CloseAfterLaunch = ReadIniInt(L"general", L"CloseAfterLaunch", DEFAULT_CLOSEAFTERLAUNCH);
	options.WindowWidth =      ReadIniInt(L"general", L"WindowWidth", DEFAULT_WINDOW_WIDTH);
	options.WindowHeight =     ReadIniInt(L"general", L"WindowHeight", DEFAULT_WINDOW_HEIGHT);
	options.UseIconCaching =   ReadIniInt(L"general", L"UseIconCaching", DEFAULT_USEICONCACHING);

	//appereance
	options.IconSize =              ReadIniInt(L"appereance", L"IconSize", DEFAULT_SHOWAPPNAMES);
	options.IconSpacingHorizontal = ReadIniInt(L"appereance", L"IconSpacingHorizontal", DEFAULT_HORZ_SPACING);
	options.IconSpacingVertical =   ReadIniInt(L"appereance", L"IconSpacingVertical", DEFAULT_VERT_SPACING);

	//load categories
	wstring categories(ReadIniString(L"categories", L"Categories"));
	vector<wstring> tokens;
	wistringstream stream(categories);
	wstring temp;
	while(getline(stream, temp, L';')){
		tokens.push_back(temp);
	}

	for(size_t i = 0; i < tokens.size(); i++){
		if(!(tokens[i].empty() || tokens[i] == L"general" || tokens[i] == L"appereance" || tokens[i] == L"categories")){
			vCategories.push_back(make_shared<CCategory>(this, i, tokens[i]));
			vCategories.at(i)->imSmall.Create(SMALL_ICON_SIZE, SMALL_ICON_SIZE, ILC_COLOR32, 1, 0);
			vCategories.at(i)->imLarge.Create(options.IconSize, options.IconSize, ILC_COLOR32, 1, 0);
		}
	}

	//create main window
	if(!CMainWnd.CreateEx(
		/*WS_EX_CONTROLPARENT | */WS_EX_ACCEPTFILES,
		GetString(IDC_PROGRAMLAUNCHER).c_str(),
		GetString(IDS_APP_TITLE).c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		options.WindowWidth,
		options.WindowHeight,
		nullptr,
		nullptr
	)){
		MessageBoxW(NULL, GetString(IDS_WND_CREATION_FAILED).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	if(CreateTabControl()){
		CreateListView();
	}

	//show window
	CMainWnd.ShowWindow(SW_SHOW);
	CMainWnd.UpdateWindow();

	//load elements
	for(auto cat : vCategories){
		LPCWSTR cName = cat->wsCategoryName.c_str();
		int count = GetElementCount(cName);
		for(int j = 0; j < count; j++){
			cat->vItems.push_back(make_shared<CLauncherItem>(
				cat.get(),
				j,
				ReadIniString(cName, wstring(to_wstring(j) + L".Name").c_str()),
				ReadIniString(cName, wstring(to_wstring(j) + L".Path").c_str()),
				ReadIniString(cName, wstring(to_wstring(j) + L".PathIcon").c_str()),
				ReadIniInt(cName, wstring(to_wstring(j) + L".IconIndex").c_str()),
				ReadIniString(cName, wstring(to_wstring(j) + L".Path64").c_str()),
				ReadIniInt(cName, wstring(to_wstring(j) + L".Admin").c_str()),
				ReadIniInt(cName, wstring(to_wstring(j) + L".AbsolutePaths").c_str())
			));
			cat->imLarge.Add(cat->vItems[j]->loadIcon(options.IconSize));
			cat->imSmall.Add(cat->vItems[j]->loadIcon(SMALL_ICON_SIZE));
		}
	}
	UpdateListView();
	return 1;
}




wstring ReadIniValue(LPCWSTR lpszSectionName, LPCWSTR lpszKeyName){
	
	wstring line;
	wifstream iniFile(szIniPath);
	bool bSectionFound = false;
	if(lpszSectionName == nullptr || lpszKeyName == nullptr){
		throw std::invalid_argument("ReadIniValue, invalid argument");
	}
	if(iniFile.is_open()){
		wstring section;
		while(getline(iniFile, line)){
			line = trim(line);
			if(line[0] == L';' || line[0] == L'#' || line.size() == 0){
				// Comment, do nothing
				continue;
			}
			if(line[0] == L'[' && line[line.length() - 1] == L']'){
				section = wstring(line.begin() + 1, line.end() - 1);
				if(section == lpszSectionName){
					bSectionFound = true;
				}
				if(bSectionFound && section != lpszSectionName){
					throw std::runtime_error("ini_key_not_found");
				}
				continue;
			}
			else if(section == lpszSectionName && line.find(L'=') != line.npos){
				int eq = line.find(L'=');
				if(wstring(line.begin(), line.begin() + eq) == lpszKeyName){
					wstring value(line.begin() + eq + 1, line.end());
					iniFile.close();
					return value;
				}
				continue;
			}
			else{
				//error, invalid line
			}
		}
		iniFile.close();
	}
	else{
		ErrorHandler();
		//throw xx;
	}
	throw std::runtime_error("ini_key_not_found");
}

wstring ReadIniString(LPCWSTR lpszSectionName, LPCWSTR lpszKeyName, LPCWSTR lpszDefault){
	try{
		return ReadIniValue(lpszSectionName, lpszKeyName);
	}
	catch(...){
		return lpszDefault;
	}
}

int ReadIniInt(LPCWSTR lpszSectionName, LPCWSTR lpszKeyName, int iDefault){
	try{
		return stoi(ReadIniValue(lpszSectionName, lpszKeyName));
	}
	catch(...){
		return iDefault;
	}
}

int WriteIniString(LPCWSTR lpszSectionName, LPCWSTR lpszKeyName, LPCWSTR lpszValue){

	wofstream outFile(L"temp.ini");
	wifstream readFile(szIniPath);
	wstring curLine;
	wstring section;
	bool bSectionFound = false;
	bool bValueFound = false;
	if(lpszSectionName == nullptr || lpszKeyName == nullptr){
		throw std::invalid_argument("WriteIniString, invalid argument");
	}
	while(getline(readFile, curLine)){
		wstring line = curLine;
		line = trim(line);

		if(line[0] == L';' || line[0] == L'#' || line.size() == 0){
			// Comment, do nothing
			goto WRITE_LINE;
		}
		if(line[0] == L'[' && line[line.length() - 1] == L']'){
			section = wstring(line.begin() + 1, line.end() - 1);
			if(section == lpszSectionName){
				bSectionFound = true;
			}
			if(bSectionFound && !bValueFound && section != lpszSectionName){
				bValueFound = true;
				outFile << lpszKeyName << L'=' << lpszValue << endl;
			}
			goto WRITE_LINE;
		}
		else if(section == lpszSectionName && line.find(L'=') != line.npos){
			int eq = line.find(L'=');
			if(wstring(line.begin(), line.begin() + eq) == lpszKeyName){
				bValueFound = true;
				if(lpszValue != nullptr){
					outFile << lpszKeyName << L'=' << lpszValue << endl;
				}
				continue;
			}
			else{
				goto WRITE_LINE;
			}
		}
		else{
			//error, invalid line
			goto WRITE_LINE;
		}
	WRITE_LINE:
		outFile << curLine << endl;
		continue;
	}
	if(!bSectionFound){
		outFile << L'[' << lpszSectionName << L']' << endl;
		outFile << lpszKeyName << L'=' << lpszValue << endl;
	}
	readFile.close();
	outFile.close();
	_wremove(szIniPath);
	if(_wrename(L"temp.ini", szIniPath)){
		return 0;
	}
	return 1;
}

int WriteIniInt(LPCWSTR lpszSectionName, LPCWSTR lpszKeyName, int iValue){
	wstring strVal = to_wstring(iValue);
	return WriteIniString(lpszSectionName, lpszKeyName, strVal.c_str());
}

int DeleteIniKey(LPCWSTR lpszSectionName, LPCWSTR lpszKeyName){
	return WriteIniString(lpszSectionName, lpszKeyName, nullptr);
}




//
//  FUNCTION: DeleteSettingKey(LPCTSTR lpszSection, LPCTSTR lpszKeyName)
//
//  PURPOSE: Deletes specified key from INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: 1 if success, 0 if fail
//
INT DeleteSettingKey(LPCTSTR lpszSection, LPCTSTR lpszKeyName){

	return WritePrivateProfileString(lpszSection, lpszKeyName, NULL, szIniPath);
}


//
//  FUNCTION: CreateIniFile()
//
//  PURPOSE: Creates new ini file with name: INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: 1 if success, 0 if fail
//
INT CreateIniFile(){

	if(!PathFileExistsW(szIniPath)){

		//CreateFileW(szIniPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0, NULL);

		//WriteFile()

		

		if(!WritePrivateProfileStringW(L"general", L"Test", L"0", szIniPath)){
			if(GetLastError() == ERROR_ACCESS_DENIED){
				ExpandEnvironmentStringsW(L"%AppData%\\Program Launcher", szIniPath, _countof(szIniPath));
				if(!CreateDirectoryW(szIniPath, NULL)){
					ErrorHandler();
				}
				PathAppendW(szIniPath, TEXT(INI_FILE_NAME));
				if(!WritePrivateProfileStringW(L"general", L"Test", L"0", szIniPath)){
					return 0;
				}
			}
		}
		WritePrivateProfileStringW(L"general", L"Test", NULL, szIniPath);

		//general
		WritePrivateProfileString(TEXT("general"), TEXT("ShowAppNames"), TEXT(TOSTRING(DEFAULT_SHOWAPPNAMES)), szIniPath);
		WritePrivateProfileString(TEXT("general"), TEXT("CloseAfterLaunch"), TEXT(TOSTRING(DEFAULT_CLOSEAFTERLAUNCH)), szIniPath);
		WritePrivateProfileString(TEXT("general"), TEXT("UseIconCaching"), TEXT(TOSTRING(DEFAULT_USEICONCACHING)), szIniPath);
		//WritePrivateProfileString(TEXT("general"), TEXT("LaunchWhenConnected"), TEXT("0"), iniPath);
		WritePrivateProfileString(TEXT("general"), TEXT("Language"), TEXT("English"), szIniPath);
		WritePrivateProfileString(TEXT("general"), TEXT("WindowWidth"), TEXT(TOSTRING(DEFAULT_WINDOW_WIDTH)), szIniPath);
		WritePrivateProfileString(TEXT("general"), TEXT("WindowHeight"), TEXT(TOSTRING(DEFAULT_WINDOW_HEIGHT)), szIniPath);
		
		//appereance
		WritePrivateProfileString(TEXT("appereance"), TEXT("IconSize"), TEXT(TOSTRING(DEFAULT_ICON_SIZE)), szIniPath);
		WritePrivateProfileString(TEXT("appereance"), TEXT("IconSpacingHorizontal"), TEXT(TOSTRING(DEFAULT_HORZ_SPACING)), szIniPath);
		WritePrivateProfileString(TEXT("appereance"), TEXT("IconSpacingVertical"), TEXT(TOSTRING(DEFAULT_VERT_SPACING)), szIniPath);

	}
	return 1;
}

