//
//  _                                   
// (_) ___ ___  _ __    ___ _ __  _ __  
// | |/ __/ _ \| '_ \  / __| '_ \| '_ \ 
// | | (_| (_) | | | || (__| |_) | |_) |
// |_|\___\___/|_| |_(_)___| .__/| .__/ 
//                         |_|   |_|    
//
// This file contains functions related to icons - loading, extracting and saving
// 
//  Functions included in this file:
// 
//		HICON LoadIconForButton(INT nBtnId, INT nCategoryIndex, INT nIconSize);
// 
//		INT SaveIconFromResource(INT nCategoryIndex, INT nButtonIndex);
// 
//		INT RebuildIconCache();
// 
// Copyright ©2021, barty12
//

#include "framework.h"
#include "Program Launcher.h"


typedef PACK(struct _ICONDIRENTRY{
	BYTE  bWidth;				// Width, in pixels, of the image
	BYTE  bHeight;				// Height, in pixels, of the image
	BYTE  bColorCount;			// Number of colors in image (0 if >=8bpp)
	BYTE  bReserved;			// Reserved
	WORD  wPlanes;				// Color Planes
	WORD  wBitCount;			// Bits per pixel
	DWORD dwBytesInRes;			// how many bytes in this resource?
	DWORD dwImageOffset;		// relative offset to image data
}) ICONDIRENTRY, * LPICONDIRENTRY;

typedef PACK(struct _GRPICONDIRENTRY{
	BYTE   bWidth;               // Width, in pixels, of the image
	BYTE   bHeight;              // Height, in pixels, of the image
	BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE   bReserved;            // Reserved
	WORD   wPlanes;              // Color Planes
	WORD   wBitCount;            // Bits per pixel
	DWORD  dwBytesInRes;         // how many bytes in this resource?
	WORD   nID;                  // the ID
}) GRPICONDIRENTRY, * LPGRPICONDIRENTRY;

typedef PACK(struct _ICONDIR{
	WORD          idReserved;
	WORD          idType;
	WORD          idCount;
	ICONDIRENTRY  idEntries[];
}) ICONDIR, * LPICONDIR;

typedef PACK(struct _GRPICONDIR{
	WORD          idReserved;
	WORD          idType;
	WORD          idCount;
	GRPICONDIRENTRY  idEntries[];
}) GRPICONDIR, * LPGRPICONDIR;

typedef struct _ENUMRESNAMES{
	WORD wResId;
	LPTSTR lpszResultIdName;
}ENUMRESNAMES, * LPENUMRESNAMES;


//
//  FUNCTION: LoadIconForButton(INT nBtnId, INT nCategoryIndex, INT nIconSize)
//
//  PURPOSE: Loads icon specified by button and category index from icon cache, and if not found extracts it from .exe
// 
//  RETURN: HICON of the loaded icon, returns NULL if error
//
HICON LoadIconForButton(INT nBtnId, INT nCategoryIndex, INT nIconSize){
	
	LAUNCHERENTRY buttonProps;
	GetSettingButtonStruct(arrszCategoryList[nCategoryIndex], nBtnId, &buttonProps, TRUE);
	
	BOOL bUseIconCaching = GetSettingInt(TEXT("general"), TEXT("UseIconCaching"), DEFAULT_USEICONCACHING);
	
	HICON hExtractedIcon = NULL;
	HRESULT result;
	WCHAR szIconFileName[2 * INT_STR_LEN + 5];
	WCHAR szIconCachePath[MAX_PATH];

	_snwprintf_s(szIconFileName, _countof(szIconFileName), _TRUNCATE, L"%i,%i.ico", nCategoryIndex, nBtnId);
	
	wcscpy_s(szIconCachePath, _countof(szIconCachePath), szIniPath);
	PathRemoveFileSpecW(szIconCachePath);

	PathAppendW(szIconCachePath, ICON_CACHE_DIRECTORY);
	PathAppendW(szIconCachePath, szIconFileName);

	//hExtractedIcon = (HICON)LoadImage(NULL, szIconCachePath, IMAGE_ICON, nIconSize, nIconSize, LR_LOADFROMFILE);
	//if(hExtractedIcon == NULL){
	//if(LoadIconWithScaleDown(NULL, szIconCachePath, nIconSize, nIconSize, &hExtractedIcon) != S_OK){
	// 	   //hExtractedIcon = LoadImageW(NULL, szIconCachePath, IMAGE_ICON, nIconSize, nIconSize, LR_LOADFROMFILE);
	// result == S_FALSE || result == E_FAIL

	if(bUseIconCaching){
		if(PathFileExistsW(buttonProps.szPathIcon)){
			result = SHDefExtractIcon(szIconCachePath, buttonProps.nIconIndex, 0, &hExtractedIcon, NULL, (UINT)nIconSize);
			if(result == S_OK){
				return hExtractedIcon;
			}
		}
		else{
			return LoadIconW(hInst, MAKEINTRESOURCEW(IDI_DEFAULT));
		}
	}

	result = SHDefExtractIcon(buttonProps.szPathIcon, buttonProps.nIconIndex, 0, &hExtractedIcon, NULL, (UINT)nIconSize);
	if(result == S_OK){
		LPWSTR lpExtension = PathFindExtensionW(buttonProps.szPathIcon);
		if(lpExtension[0] != 0){
			if((wcscmp(lpExtension, L".exe") || wcscmp(lpExtension, L".dll")) && bUseIconCaching){
				fRebuildIconCache = TRUE;
			}
		}
		return hExtractedIcon;
	}
	return LoadIconW(hInst, MAKEINTRESOURCEW(IDI_DEFAULT));
	//return ExtractIcon(hInst, buttonProps.szPathIcon, buttonProps.nIconIndex);
}


//
// @brief Used internally in SaveIconFromResource()
//
BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpType, LPTSTR lpName, LPARAM lParam){
	static WORD i = 0;
	if(i == ((LPENUMRESNAMES)lParam)->wResId){
		((LPENUMRESNAMES)lParam)->lpszResultIdName = lpName;
		i = 0;
		return FALSE;
	}
	i++;
	return TRUE;
}


/*
//
// @brief Extracts icon from file specified by category and button index and saves it to IconCache folder as .ico file
// @param nCategoryIndex - 
// @param nButtonIndex - 
// @return 1 if success, 0 if fail
//
INT SaveIconFromResource(INT nCategoryIndex, INT nButtonIndex){

	LAUNCHERENTRY buttonProps;
	if(!GetSettingButtonStruct(arrszCategoryList[nCategoryIndex], nButtonIndex, &buttonProps, TRUE)){
		ErrorHandlerDebug();
		return 0;
	}

	LPTSTR lpExtension = PathFindExtension(buttonProps.szPathIcon);
	if(lpExtension[0] == 0){
		return 0;
	}
	else if(lpExtension[0] == TEXT('.') && lpExtension[1] == TEXT('e') && lpExtension[2] == TEXT('x') && lpExtension[3] == TEXT('e')){


		//HMODULE hExecutable = LoadLibrary(buttonProps.szPathIcon);
		LPTSTR lpszIconPathResult = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
		if(lpszIconPathResult == NULL) return 0;

		ExpandEnvironmentStrings(buttonProps.szPathIcon, lpszIconPathResult, MAX_PATH);
		HMODULE hExecutable = LoadLibraryEx(lpszIconPathResult, NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(hExecutable == NULL){
#ifdef _DEBUG
			TCHAR szErrorMessage[255];
			_stprintf_s(szErrorMessage, 255, TEXT("SaveIconFromResource, Path: %s"), buttonProps.szPathIcon);
			//ErrorHandlerEx(__LINE__, TEXT(__FILE__), szErrorMessage);
#endif
			return 0;
		}

		LPTSTR lpszResultResId = 0;
		ENUMRESNAMES enumResNames = {
			(WORD)buttonProps.nIconIndex,
			lpszResultResId
		};
		EnumResourceNames(hExecutable, RT_GROUP_ICON, EnumResNameProc, (LPARAM)&enumResNames);
		//if(lpszResultResId == 0){
	//#ifdef _DEBUG
		//	ErrorHandlerEx(__LINE__, TEXT(__FILE__), TEXT("SaveIconFromResource"));
	//#endif
		//	return 0;
		//}
		if(enumResNames.lpszResultIdName == NULL){
			FreeLibrary(hExecutable);
			return 0;
		}

		HRSRC hRes = FindResource(hExecutable, enumResNames.lpszResultIdName, RT_GROUP_ICON);
		if(hRes == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}

		HGLOBAL hData = LoadResource(hExecutable, hRes);
		if(hData == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}

		LPVOID lpIconGroup = LockResource(hData);
		if(lpIconGroup == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}

		//LPWORD lpReserved = (LPWORD)lpIcon;
		//LPWORD lpType = (LPWORD)lpIcon + 1;
		//LPWORD lpCount = (LPWORD)lpIcon + 2;

		//const int cEntriesCount = *((LPWORD)lpIconGroup + 2);

		//GRPICONDIR iconGroupInfo = {
		//	iconGroupInfo.idReserved = 0,// *(LPWORD)lpIcon,
		//	iconGroupInfo.idType = 0,// *((LPWORD)lpIcon + 1),
		//	iconGroupInfo.idCount = 0,// *((LPWORD)lpIcon + 2),
		//	iconGroupInfo.idEntries[cEntriesCount]
		//};

		//LPGRPICONDIR iconGroupInfo = (LPGRPICONDIR)malloc(sizeof(*iconGroupInfo) + cEntriesCount * 14);

		//int size = sizeof(*iconGroupInfo);

		//*iconGroupInfo = *((LPGRPICONDIR)lpIcon);

		//memcpy_s(iconGroupInfo, 6 + cEntriesCount * 14, lpIcon, 6 + cEntriesCount * 14);

		LPGRPICONDIR iconGroupInfo = (LPGRPICONDIR)lpIconGroup;



		LPICONDIR lpResultIcon = (LPICONDIR)malloc(sizeof(ICONDIR) + sizeof(ICONDIRENTRY));
		if(lpResultIcon == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}

		lpResultIcon->idReserved = 0;
		lpResultIcon->idType = 1;
		lpResultIcon->idCount = iconGroupInfo->idCount;


		DWORD dwCurrentDataSize = sizeof(ICONDIR) + iconGroupInfo->idCount * sizeof(ICONDIRENTRY);
		DWORD dwCurrentDataOffset = sizeof(ICONDIR) + iconGroupInfo->idCount * sizeof(ICONDIRENTRY);


		for(int i = 0; i < iconGroupInfo->idCount; i++){


			HRSRC hResL = FindResource(hExecutable, MAKEINTRESOURCE(iconGroupInfo->idEntries[i].nID), RT_ICON);
			if(hResL == NULL){
				ErrorHandlerDebug();
				FreeLibrary(hExecutable);
				free(lpResultIcon);
				return 0;
			}

			HGLOBAL hDataL = LoadResource(hExecutable, hResL);
			if(hDataL == NULL){
				ErrorHandlerDebug();
				FreeLibrary(hExecutable);
				free(lpResultIcon);
				return 0;
			}

			LPVOID lpIcon = LockResource(hDataL);
			if(lpIcon == NULL){
				ErrorHandlerDebug();
				FreeLibrary(hExecutable);
				free(lpResultIcon);
				return 0;
			}

			dwCurrentDataSize += iconGroupInfo->idEntries[i].dwBytesInRes;


			LPICONDIR lpNewPointer = (LPICONDIR)realloc(lpResultIcon, dwCurrentDataSize);


			if(lpNewPointer != NULL){
				lpResultIcon = lpNewPointer;
			}
			else{
				FreeLibrary(hExecutable);
				free(lpResultIcon);
				return 0;
			}

			lpResultIcon->idEntries[i].bWidth = iconGroupInfo->idEntries[i].bWidth;
			lpResultIcon->idEntries[i].bHeight = iconGroupInfo->idEntries[i].bHeight;
			lpResultIcon->idEntries[i].bColorCount = iconGroupInfo->idEntries[i].bColorCount;
			lpResultIcon->idEntries[i].bReserved = iconGroupInfo->idEntries[i].bReserved;
			lpResultIcon->idEntries[i].wPlanes = iconGroupInfo->idEntries[i].wPlanes;
			lpResultIcon->idEntries[i].wBitCount = iconGroupInfo->idEntries[i].wBitCount;
			lpResultIcon->idEntries[i].dwBytesInRes = iconGroupInfo->idEntries[i].dwBytesInRes;

			//dwCurrentOffset += sizeof(ICONDIRENTRY);

			lpResultIcon->idEntries[i].dwImageOffset = dwCurrentDataOffset;

			LPBYTE lpIconPosition = (LPBYTE)lpResultIcon + dwCurrentDataOffset;// sizeof(lpResultIcon);

			memcpy_s(lpIconPosition, iconGroupInfo->idEntries[i].dwBytesInRes, lpIcon, iconGroupInfo->idEntries[i].dwBytesInRes);

			dwCurrentDataOffset += iconGroupInfo->idEntries[i].dwBytesInRes;

		}

		FILE* pFile;

		TCHAR szCacheFolderPath[MAX_PATH];
		GetModuleFileName(NULL, szCacheFolderPath, MAX_PATH);
		PathRemoveFileSpec(szCacheFolderPath);

		TCHAR szNewDirectoryName[MAX_PATH];
		_stprintf_s(szNewDirectoryName, MAX_PATH, TEXT("%s\\%s"), szCacheFolderPath, ICON_CACHE_DIRECTORY);
		CreateDirectory(szNewDirectoryName, NULL);

		TCHAR szCacheFileName[MAX_PATH];
		_stprintf_s(szCacheFileName, MAX_PATH, TEXT("%s\\%s\\%i,%i.ico"), szCacheFolderPath, ICON_CACHE_DIRECTORY,nCategoryIndex, nButtonIndex);


		if(_tfopen_s(&pFile, szCacheFileName, TEXT("wb"))){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			free(lpResultIcon);
			return 0;
		}


		fwrite(lpResultIcon, dwCurrentDataSize, 1, pFile);

		fclose(pFile);



		//INT structOffset;

		//GRPICONDIRENTRY iconGroupInfoEntry = *((LPGRPICONDIRENTRY)lpIcon);

		//for(int i = 0; i < iconGroupInfo.idCount; i++){
		//
		//	GRPICONDIRENTRY iconGroupInfoEntry;
		//
		//	int test = sizeof(iconGroupInfoEntry);
		//	structOffset = 6 + (i * 14);
		//
		//	iconGroupInfoEntry.bWidth = *((LPBYTE)lpIcon + structOffset);
		//	iconGroupInfoEntry.bHeight = *((LPBYTE)lpIcon + structOffset + 1);
		//	iconGroupInfoEntry.bColorCount = *((LPBYTE)lpIcon + structOffset + 2);
		//	iconGroupInfoEntry.bReserved = *((LPBYTE)lpIcon + structOffset + 3);
		//	iconGroupInfoEntry.wPlanes = *((LPWORD)lpIcon + (structOffset / sizeof(WORD)) + (4 / sizeof(WORD)));
		//	iconGroupInfoEntry.wBitCount = *((LPWORD)lpIcon + (structOffset / sizeof(WORD)) + (6 / sizeof(WORD)));
		//	iconGroupInfoEntry.dwBytesInRes = *((LPDWORD)lpIcon + (structOffset / sizeof(DWORD)) + (8 / sizeof(DWORD)));
		//	iconGroupInfoEntry.nID = *((LPWORD)lpIcon + (structOffset / sizeof(WORD)) + (12 / sizeof(WORD)));
		//
		//
		//	iconGroupInfo.idEntries[i] = iconGroupInfoEntry;
		//}



		//INT test = sizeof(iconGroupInfo);

		//iconGroupInfo.idReserved = *(LPWORD)lpIcon;
		//iconGroupInfo.idType = *(LPWORD)lpIcon;
		//iconGroupInfo.idCount = *(LPWORD)lpIcon;
		FreeLibrary(hExecutable);
		free(lpResultIcon);
		return 1;
	}
	return 0;
}
*/

//
// @brief Extracts icon from file specified by category and button index and saves it to IconCache folder as .ico file
// @param nCategoryIndex -
// @param nButtonIndex -
// @return 1 if success, 0 if fail
//
INT SaveIconFromResource(INT nCategoryIndex, INT nButtonIndex){

	LAUNCHERENTRY buttonProps;
	if(!GetSettingButtonStruct(arrszCategoryList[nCategoryIndex], nButtonIndex, &buttonProps, TRUE)){
		ErrorHandlerDebug();
		return 0;
	}

	LPWSTR lpExtension = PathFindExtensionW(buttonProps.szPathIcon);
	if(lpExtension[0] == 0){
		return 0;
	}
	else if(wcscmp(lpExtension, L".exe") || wcscmp(lpExtension, L".dll")){

		//get expanded string length by passing a zero buffer size
		int cchExpandedPathSize = ExpandEnvironmentStringsW(buttonProps.szPathIcon, NULL, 0);
		LPWSTR lpszExpandedPath = (LPWSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cchExpandedPathSize * sizeof(WCHAR));
		if(lpszExpandedPath == NULL) return 0;
		ExpandEnvironmentStringsW(buttonProps.szPathIcon, lpszExpandedPath, cchExpandedPathSize);

		HMODULE hExecutable = LoadLibraryExW(lpszExpandedPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
		HeapFree(GetProcessHeap(), 0, lpszExpandedPath);

		if(hExecutable == NULL){
			return 0;
		}


		LPWSTR lpszResultResId = 0;
		ENUMRESNAMES enumResNames = {
			(WORD)buttonProps.nIconIndex,
			lpszResultResId
		};
		EnumResourceNamesW(hExecutable, RT_GROUP_ICON, EnumResNameProc, (LPARAM)&enumResNames);
		if(enumResNames.lpszResultIdName == NULL){
			FreeLibrary(hExecutable);
			return 0;
		}



		HRSRC hRes = FindResourceW(hExecutable, enumResNames.lpszResultIdName, RT_GROUP_ICON);
		if(hRes == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}

		HGLOBAL hData = LoadResource(hExecutable, hRes);
		if(hData == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}

		LPGRPICONDIR iconGroupInfo = LockResource(hData);
		if(iconGroupInfo == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}


		LPICONDIR lpResultIcon = (LPICONDIR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ICONDIR) + sizeof(ICONDIRENTRY));
		if(lpResultIcon == NULL){
			ErrorHandlerDebug();
			FreeLibrary(hExecutable);
			return 0;
		}

		lpResultIcon->idReserved = 0;
		lpResultIcon->idType = 1;
		lpResultIcon->idCount = iconGroupInfo->idCount;


		DWORD dwCurrentDataSize = sizeof(ICONDIR) + iconGroupInfo->idCount * sizeof(ICONDIRENTRY);
		DWORD dwCurrentDataOffset = sizeof(ICONDIR) + iconGroupInfo->idCount * sizeof(ICONDIRENTRY);


		for(int i = 0; i < iconGroupInfo->idCount; i++){

			HRSRC hResL = FindResourceW(hExecutable, MAKEINTRESOURCEW(iconGroupInfo->idEntries[i].nID), RT_ICON);
			if(hResL == NULL){
				ErrorHandlerDebug();
				HeapFree(GetProcessHeap(), 0, lpResultIcon);
				FreeLibrary(hExecutable);
				return 0;
			}

			HGLOBAL hDataL = LoadResource(hExecutable, hResL);
			if(hDataL == NULL){
				ErrorHandlerDebug();
				HeapFree(GetProcessHeap(), 0, lpResultIcon);
				FreeLibrary(hExecutable);
				return 0;
			}

			LPVOID lpIcon = LockResource(hDataL);
			if(lpIcon == NULL){
				ErrorHandlerDebug();
				HeapFree(GetProcessHeap(), 0, lpResultIcon);
				FreeLibrary(hExecutable);
				return 0;
			}

			dwCurrentDataSize += iconGroupInfo->idEntries[i].dwBytesInRes;

			LPICONDIR lpNewPointer = (LPICONDIR)HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lpResultIcon, dwCurrentDataSize);


			if(lpNewPointer != NULL){
				lpResultIcon = lpNewPointer;
			}
			else{
				HeapFree(GetProcessHeap(), 0, lpResultIcon);
				FreeLibrary(hExecutable);
				return 0;
			}

			lpResultIcon->idEntries[i].bWidth = iconGroupInfo->idEntries[i].bWidth;
			lpResultIcon->idEntries[i].bHeight = iconGroupInfo->idEntries[i].bHeight;
			lpResultIcon->idEntries[i].bColorCount = iconGroupInfo->idEntries[i].bColorCount;
			lpResultIcon->idEntries[i].bReserved = iconGroupInfo->idEntries[i].bReserved;
			lpResultIcon->idEntries[i].wPlanes = iconGroupInfo->idEntries[i].wPlanes;
			lpResultIcon->idEntries[i].wBitCount = iconGroupInfo->idEntries[i].wBitCount;
			lpResultIcon->idEntries[i].dwBytesInRes = iconGroupInfo->idEntries[i].dwBytesInRes;
			lpResultIcon->idEntries[i].dwImageOffset = dwCurrentDataOffset;

			LPBYTE lpIconPosition = (LPBYTE)lpResultIcon + dwCurrentDataOffset;

			CopyMemory(lpIconPosition, lpIcon, iconGroupInfo->idEntries[i].dwBytesInRes);

			dwCurrentDataOffset += iconGroupInfo->idEntries[i].dwBytesInRes;

		}


		WCHAR szCacheFileName[MAX_PATH];
		WCHAR szIconName[MAX_PATH];

		wcscpy_s(szCacheFileName, _countof(szCacheFileName), szIniPath);
		PathRemoveFileSpecW(szCacheFileName);
		PathAppendW(szCacheFileName, ICON_CACHE_DIRECTORY);
		CreateDirectoryW(szCacheFileName, NULL);
		_snwprintf_s(szIconName, _countof(szIconName), _TRUNCATE, L"%i,%i.ico", nCategoryIndex, nButtonIndex);
		PathAppendW(szCacheFileName, szIconName);

		HANDLE hIconFile = CreateFileW(szCacheFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if(hIconFile == INVALID_HANDLE_VALUE){
			ErrorHandlerDebug();
			HeapFree(GetProcessHeap(), 0, lpResultIcon);
			FreeLibrary(hExecutable);
			return 0;
		}

		DWORD dwBytesWritten = 0;
		WriteFile(hIconFile, lpResultIcon, dwCurrentDataSize, &dwBytesWritten, NULL);
		CloseHandle(hIconFile);

		HeapFree(GetProcessHeap(), 0, lpResultIcon);
		FreeLibrary(hExecutable);
	}
	return 1;
}




static void DeleteAllFiles(LPWSTR folderPath){
	WCHAR fileFound[MAX_PATH];
	WIN32_FIND_DATAW info;
	HANDLE hp;
	ZeroMemory(&info, sizeof(info));
	_snwprintf_s(fileFound, MAX_PATH, _TRUNCATE, L"%s\\*.*", folderPath);
	hp = FindFirstFileW(fileFound, &info);
	do{
		_snwprintf_s(fileFound, MAX_PATH, _TRUNCATE, L"%s\\%s", folderPath, info.cFileName);
		DeleteFileW(fileFound);

	} while(FindNextFileW(hp, &info));
	FindClose(hp);
}

//
// @brief Completely rebuilds icon cache
// @return 1 if success, 0 if fail
//
INT RebuildIconCache(){

	if(GetSettingInt(L"general", L"UseIconCaching", DEFAULT_USEICONCACHING)){

		//first empty icon cache folder
		WCHAR szCacheFileName[MAX_PATH];
		wcscpy_s(szCacheFileName, _countof(szCacheFileName), szIniPath);
		PathRemoveFileSpecW(szCacheFileName);
		PathAppendW(szCacheFileName, ICON_CACHE_DIRECTORY);
		DeleteAllFiles(szCacheFileName);

		INT nCategoryCount = GetCategoryCount();
		for(int i = 0; i < nCategoryCount; i++){
			INT nCategorySectionCount = GetCategorySectionCount(i);
			for(int j = 0; j < nCategorySectionCount; j++){
				SaveIconFromResource(i, j);
			}
		}
	}
	fRebuildIconCache = FALSE;

	return 1;
}

