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

#include "pch.h"
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
	LPWSTR lpszResultIdName;
}ENUMRESNAMES, * LPENUMRESNAMES;


//
//  FUNCTION: LoadIconForButton(INT nBtnId, INT nCategoryIndex, INT nIconSize)
//
//  PURPOSE: Loads icon specified by button and category index from icon cache, and if not found extracts it from .exe
// 
//  RETURN: HICON of the loaded icon, returns NULL if error
//
HICON CLauncherItem::LoadIcon(UINT nIconSize, HICON* phSmall){
	
	HICON hExtractedIcon = null;
	HRESULT result;
	phSmall = &hSmallIcon;
	
	//wstring wsIniPath(/*szIniPath*/L"");
	wstring wsCachePath = ICON_CACHE_DIRECTORY L"\\" + to_wstring(ptrParent->parentIndex) + L',' + to_wstring(parentIndex) + L".ico";// (RemoveFileSpecFromPath(wsIniPath));
	//wsCachePath.append(L"\\" ICON_CACHE_DIRECTORY L"\\" + to_wstring(ptrParent->parentIndex) + L',' + to_wstring(parentIndex) + L".ico");

	DestroyIcon(hItemIcon);
	DestroyIcon(hSmallIcon);
	
	if(Launcher->options.UseIconCaching){
		if(PathFileExistsW(wsPathIcon.c_str())){
			
			result = SHDefExtractIconW(wsCachePath.c_str(), iIconIndex, 0, &hItemIcon, &hSmallIcon, nIconSize);
			if(result == S_OK){
				return hItemIcon;
			}
		}
		else{
			return LoadIconW(hInst, MAKEINTRESOURCEW(IDI_DEFAULT));
		}
	}

	result = SHDefExtractIconW(wsPathIcon.c_str(), iIconIndex, 0, &hItemIcon, &hSmallIcon, nIconSize);
	if(result == S_OK){
		LPWSTR lpExtension = PathFindExtensionW(wsPathIcon.c_str());
		if(Launcher->options.UseIconCaching && lpExtension[0] != 0 && (wcscmp(lpExtension, L".exe") || wcscmp(lpExtension, L".dll"))){
			Launcher->bRebuildIconCache = true;
		}
		return hItemIcon;
	}
	//*phSmall = LoadIconW(hInst, MAKEINTRESOURCEW(IDI_DEFAULT));
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
bool SaveIconFromResource(const fs::path& lpszResourcePath, WORD iIconIndex, const fs::path& lpszResultFileName){

	LPWSTR lpExtension = PathFindExtensionW(lpszResourcePath.c_str());
	if(lpExtension[0] == 0){
		return false;
	}
	else if(wcscmp(lpExtension, L".exe") || wcscmp(lpExtension, L".dll")){
		wstring wspath(lpszResourcePath);
		wstring wsExpPath = ExpandEnvStrings(wspath);

		HMODULE hExecutable = LoadLibraryExW(wsExpPath.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(!hExecutable){
			return false;
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

			fs::create_directories(fs::path(lpszResultFileName).remove_filename());
			std::ofstream resultStream(lpszResultFileName, std::ios::out | std::ios::binary | std::ios::trunc);
			if(!resultStream.is_open()){
				throw std::runtime_error("file_not_opened");
			}
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

				std::streamoff pos = resultStream.tellp();
				resultStream.seekp(dwCurrentDataOffset);
				resultStream.write((char*)lpIcon, iconGroupInfo->idEntries[i].dwBytesInRes);
				dwCurrentDataOffset = static_cast<DWORD>(resultStream.tellp());
				resultStream.seekp(pos);
			}
			resultStream.close();
			FreeLibrary(hExecutable);
		}
		catch(const std::exception&){
			FreeLibrary(hExecutable);
			return false;
		}
	}
	return true;
}




//static void DeleteAllFiles(LPCWSTR folderPath){
//	WCHAR fileFound[MAX_PATH];
//	WIN32_FIND_DATAW info;
//	HANDLE hp;
//	ZeroMemory(&info, sizeof(info));
//	_snwprintf_s(fileFound, MAX_PATH, _TRUNCATE, L"%s\\*.*", folderPath);
//	hp = FindFirstFileW(fileFound, &info);
//	do{
//		_snwprintf_s(fileFound, MAX_PATH, _TRUNCATE, L"%s\\%s", folderPath, info.cFileName);
//		DeleteFileW(fileFound);
//
//	} while(FindNextFileW(hp, &info));
//	FindClose(hp);
//}

//
// @brief Completely rebuilds icon cache
// @return 1 if success, 0 if fail
//
void CProgramLauncher::RebuildIconCache(){

	if(options.UseIconCaching){
		//first empty icon cache folder
		fs::remove_all(ICON_CACHE_DIRECTORY);
		int i = 0;
		for(auto &cat : vCategories){
			int j = 0;
			for(auto &item : cat->vItems){
				SaveIconFromResource(item->wsPathIcon.c_str(), item->iIconIndex, wstring(ICON_CACHE_DIRECTORY L"\\" + to_wstring(cat->parentIndex) + L',' + to_wstring(item->parentIndex) + L".ico").c_str());
				j++;
			}
			i++;
		}
		bRebuildIconCache = false;
	}
}

