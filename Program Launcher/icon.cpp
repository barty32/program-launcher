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
// Copyright �2021, barty12
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

#pragma warning(push)
#pragma warning(disable : 4200)

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

#pragma warning(pop)

typedef PACK(struct _ICONHEADER{
	WORD          idReserved;
	WORD          idType;
	WORD          idCount;
}) ICONHEADER, * LPICONHEADER;

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
HICON CLauncherItem::loadIcon(int nIconSize){
	
	bool bUseIconCaching = ptrParent->ptrParent->options.UseIconCaching;
	
	HICON hExtractedIcon = NULL;
	HRESULT result;

	wstring wsIniPath(szIniPath);
	wstring wsCachePath(RemoveFileSpecFromPath(wsIniPath));
	wsCachePath.append(L"\\" ICON_CACHE_DIRECTORY L"\\" + to_wstring(ptrParent->parentIndex) + L',' + to_wstring(parentIndex) + L".ico");

	if(bUseIconCaching){
		if(PathFileExistsW(wsPathIcon.c_str())){
			result = SHDefExtractIconW(wsCachePath.c_str(), iIconIndex, 0, &hExtractedIcon, nullptr, (UINT)nIconSize);
			if(result == S_OK){
				return hExtractedIcon;
			}
		}
		else{
			return LoadIconW(hInst, MAKEINTRESOURCEW(IDI_DEFAULT));
		}
	}

	result = SHDefExtractIconW(wsPathIcon.c_str(), iIconIndex, 0, &hExtractedIcon, NULL, (UINT)nIconSize);
	if(result == S_OK){
		LPWSTR lpExtension = PathFindExtensionW(wsPathIcon.c_str());
		if(lpExtension[0] != 0){
			if((wcscmp(lpExtension, L".exe") || wcscmp(lpExtension, L".dll")) && bUseIconCaching){
				fRebuildIconCache = TRUE;
			}
		}
		return hExtractedIcon;
	}
	return LoadIconW(hInst, MAKEINTRESOURCEW(IDI_DEFAULT));
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


//
// @brief Extracts icon from file specified by category and button index and saves it to IconCache folder as .ico file
// @param nCategoryIndex -
// @param nButtonIndex -
// @return 1 if success, 0 if fail
//
INT SaveIconFromResource(LPCWSTR lpszResourcePath, WORD iIconIndex, LPCWSTR lpszResultFileName){

	LPWSTR lpExtension = PathFindExtensionW(lpszResourcePath);
	if(lpExtension[0] == 0){
		return 0;
	}
	else if(wcscmp(lpExtension, L".exe") || wcscmp(lpExtension, L".dll")){
		wstring wspath(lpszResourcePath);
		wstring wsExpPath = ExpandEnvStrings(wspath);

		HMODULE hExecutable = LoadLibraryExW(wsExpPath.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(!hExecutable){
			return 0;
		}
		try{
			LPWSTR lpszResultResId = 0;
			ENUMRESNAMES enumResNames ={
				iIconIndex,
				lpszResultResId
			};
			EnumResourceNamesW(hExecutable, RT_GROUP_ICON, EnumResNameProc, (LPARAM)&enumResNames);
			if(enumResNames.lpszResultIdName == NULL){
				throw std::runtime_error("enum_0_resources_found");
			}
			HRSRC hRes = FindResourceW(hExecutable, enumResNames.lpszResultIdName, RT_GROUP_ICON);
			if(hRes == NULL){
				throw std::runtime_error("resource_not_found");
			}
			HGLOBAL hData = LoadResource(hExecutable, hRes);
			if(hData == NULL){
				throw std::runtime_error("resource_load_error");
			}
			LPGRPICONDIR iconGroupInfo = (LPGRPICONDIR)LockResource(hData);
			if(iconGroupInfo == NULL){
				throw std::runtime_error("resource_lock_error");
			}

			std::ofstream resultStream(lpszResultFileName, std::ios::out | std::ios::binary | std::ios::trunc);

			ICONHEADER iconHeader;
			iconHeader.idReserved = 0;
			iconHeader.idType = 1;
			iconHeader.idCount = iconGroupInfo->idCount;

			resultStream.write((char*)&iconHeader, sizeof(iconHeader));

			DWORD dwCurrentDataOffset = sizeof(ICONDIR) + iconGroupInfo->idCount * sizeof(ICONDIRENTRY);

			for(int i = 0; i < iconGroupInfo->idCount; i++){
				HRSRC hResL = FindResourceW(hExecutable, MAKEINTRESOURCEW(iconGroupInfo->idEntries[i].nID), RT_ICON);
				if(hResL == NULL){
					throw std::runtime_error("resource_not_found");
				}
				HGLOBAL hDataL = LoadResource(hExecutable, hResL);
				if(hDataL == NULL){
					throw std::runtime_error("resource_load_error");
				}
				LPVOID lpIcon = LockResource(hDataL);
				if(lpIcon == NULL){
					throw std::runtime_error("resource_lock_error");
				}

				ICONDIRENTRY iconInfo;
				iconInfo.bWidth = iconGroupInfo->idEntries[i].bWidth;
				iconInfo.bHeight = iconGroupInfo->idEntries[i].bHeight;
				iconInfo.bColorCount = iconGroupInfo->idEntries[i].bColorCount;
				iconInfo.bReserved = iconGroupInfo->idEntries[i].bReserved;
				iconInfo.wPlanes = iconGroupInfo->idEntries[i].wPlanes;
				iconInfo.wBitCount = iconGroupInfo->idEntries[i].wBitCount;
				iconInfo.dwBytesInRes = iconGroupInfo->idEntries[i].dwBytesInRes;
				iconInfo.dwImageOffset = dwCurrentDataOffset;
				resultStream.write((char*)&iconInfo, sizeof(iconInfo));

				int pos = resultStream.tellp();
				resultStream.seekp(dwCurrentDataOffset);
				resultStream.write((char*)lpIcon, iconGroupInfo->idEntries[i].dwBytesInRes);
				dwCurrentDataOffset = resultStream.tellp();
				resultStream.seekp(pos);
			}
			FreeLibrary(hExecutable);
		}
		catch(const std::exception&){
			FreeLibrary(hExecutable);
			return 0;
		}
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

	//if(GetSettingInt(L"general", L"UseIconCaching", DEFAULT_USEICONCACHING)){

	//	//first empty icon cache folder
	//	WCHAR szCacheFileName[MAX_PATH];
	//	wcscpy_s(szCacheFileName, _countof(szCacheFileName), szIniPath);
	//	PathRemoveFileSpecW(szCacheFileName);
	//	PathAppendW(szCacheFileName, ICON_CACHE_DIRECTORY);
	//	DeleteAllFiles(szCacheFileName);

	//	INT nCategoryCount = GetCategoryCount();
	//	for(int i = 0; i < nCategoryCount; i++){
	//		INT nCategorySectionCount = GetCategorySectionCount(i);
	//		for(int j = 0; j < nCategorySectionCount; j++){
	//			SaveIconFromResource(i, j);
	//		}
	//	}
	//}
	//fRebuildIconCache = FALSE;

	return 1;
}

