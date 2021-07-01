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

#define _CRT_SECURE_NO_WARNINGS 1

#include "Program Launcher.h"


//---------------------------- Defines -------------------------------------
#define ASCIILINESZ         (1024)
#define INI_INVALID_KEY     ((LPTSTR)-1)

//---------------------------------------------------------------------------
//                        Private to this module
//---------------------------------------------------------------------------
// 
// This enum stores the status for each parsed line (internal use only).
// 
//typedef enum _line_status_ : int{
//	LINE_UNPROCESSED,
//	LINE_ERROR,
//	LINE_EMPTY,
//	LINE_COMMENT,
//	LINE_SECTION,
//	LINE_VALUE
//} line_status;



//
//  FUNCTION:	GetSettingString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPTSTR lpszBuffer, DWORD nSize, LPCTSTR lpszDefault)
//
//  PURPOSE: Reads string from INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: The return value is the number of characters copied to the buffer, not including the terminating null character.
//	If neither lpAppName nor lpKeyName is NULL and the supplied destination buffer is too small to hold the requested string, the string is truncatedand followed by a null character, and the return value is equal to nSize minus one.
//	If either lpAppName or lpKeyName is NULL and the supplied destination buffer is too small to hold all the strings, the last string is truncatedand followed by two null characters.In this case, the return value is equal to nSize minus two.
//
INT GetSettingString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPTSTR lpszBuffer, DWORD nSize, LPCTSTR lpszDefault){

	return (INT)GetPrivateProfileStringW(lpszSectionName, lpszKeyName, lpszDefault, lpszBuffer, nSize, szIniPath);
}

//
//  FUNCTION: SetSettingString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszString)
//
//  PURPOSE: Writes string to INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: 1 if success, 0 if fail
//
INT SetSettingString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszString){

	if(lpszString == NULL){
		return 0;
	}

	CreateIniFile();

	return WritePrivateProfileString(lpszSectionName, lpszKeyName, lpszString, szIniPath);
}

//
//  FUNCTION: GetSettingInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, INT nDefault)
//
//  PURPOSE: Reads number from INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: number from the specified key. If the key is not found, the return value is the specified default value.
//
UINT GetSettingInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, INT nDefault){

	return GetPrivateProfileInt(lpszSectionName, lpszKeyName, nDefault, szIniPath);
}

//
//  FUNCTION:  SetSettingInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, INT nNumber)
//
//  PURPOSE: Writes int to INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: 1 if success, 0 if fail
//
INT SetSettingInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, INT nNumber){

	TCHAR szNumber[INT_LEN_IN_DIGITS];

	_itot_s(nNumber, szNumber, INT_LEN_IN_DIGITS, 10);

	CreateIniFile();

	return WritePrivateProfileString(lpszSectionName, lpszKeyName, szNumber, szIniPath);
}

//
//  FUNCTION: GetSettingStruct(LPCTSTR Section, LPCTSTR Key, LPVOID lpStruct, UINT uSizeStruct)
//
//  PURPOSE: Reads LaunchButton struct from INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: 1 if success, 0 if fail
//
INT GetSettingButtonStruct(LPCTSTR lpszCategoryName, INT nButtonIndex, LPLAUNCHERENTRY lpStruct, BOOL bRequestAbsolute){

	TCHAR szKeyNameBuffer[MAX_PATH + 5];

	//path
	_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Path"));
	if(!GetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szPath, MAX_PATH, TEXT(""))){
		lpStruct->szPath[0] = 0;
	}

	//64-bit path
	_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Path64"));
	if(!GetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szPath64, MAX_PATH, TEXT(""))){
		lpStruct->szPath64[0] = 0;
	}

	//icon path
	_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("PathIcon"));
	if(!GetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szPathIcon, MAX_PATH, TEXT(""))){
		lpStruct->szPathIcon[0] = 0;
	}

	//icon index
	_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("IconIndex"));
	lpStruct->nIconIndex = GetSettingInt(lpszCategoryName, szKeyNameBuffer, 0);

	//name
	_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Name"));
	if(!GetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szName, MAX_PATH, TEXT(""))){
		lpStruct->szName[0] = 0;
	}

	//admin
	_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Admin"));
	lpStruct->bAdmin = GetSettingInt(lpszCategoryName, szKeyNameBuffer, 0);

	//absolute paths
	_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("AbsolutePaths"));
	lpStruct->bAbsolute = GetSettingInt(lpszCategoryName, szKeyNameBuffer, 0);

	if(!lpStruct->bAbsolute && bRequestAbsolute){
		AddLetterToPath(lpStruct->szPath, MAX_PATH);
		AddLetterToPath(lpStruct->szPath64, MAX_PATH);
		AddLetterToPath(lpStruct->szPathIcon, MAX_PATH);
	}

	return 1;
}

//
//  FUNCTION: SetSettingButtonStruct(LPCTSTR lpszCategoryName, INT nButtonIndex, LaunchButton* lpStruct, UINT uSizeStruct)
//
//  PURPOSE: Writes LaunchButton struct to INI_FILE_NAME (ProgramLauncher.ini)
// 
//  RETURN: 1 if success, 0 if fail
//
INT SetSettingButtonStruct(LPCTSTR lpszCategoryName, INT nButtonIndex, LPLAUNCHERENTRY lpStruct){

	TCHAR szKeyNameBuffer[MAX_PATH + 5];

	if(lpStruct == NULL){
		//path
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Path"));
		DeleteSettingKey(lpszCategoryName, szKeyNameBuffer);

		//64-bit path
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Path64"));
		DeleteSettingKey(lpszCategoryName, szKeyNameBuffer);

		//icon path
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("PathIcon"));
		DeleteSettingKey(lpszCategoryName, szKeyNameBuffer);

		//icon index
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("IconIndex"));
		DeleteSettingKey(lpszCategoryName, szKeyNameBuffer);

		//name
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Name"));
		DeleteSettingKey(lpszCategoryName, szKeyNameBuffer);

		//admin
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Admin"));
		DeleteSettingKey(lpszCategoryName, szKeyNameBuffer);

		//absolute paths
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("AbsolutePaths"));
		DeleteSettingKey(lpszCategoryName, szKeyNameBuffer);
		return 1;
	}
	else{
		if(!lpStruct->bAbsolute){
			RemoveLetterFromPath(lpStruct->szPath);
			RemoveLetterFromPath(lpStruct->szPath64);
			RemoveLetterFromPath(lpStruct->szPathIcon);
		}
		//path
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Path"));
		SetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szPath);

		//64-bit path
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Path64"));
		SetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szPath64);

		//icon path
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("PathIcon"));
		SetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szPathIcon);

		//icon index
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("IconIndex"));
		SetSettingInt(lpszCategoryName, szKeyNameBuffer, lpStruct->nIconIndex);

		//name
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Name"));
		SetSettingString(lpszCategoryName, szKeyNameBuffer, lpStruct->szName);

		//admin
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("Admin"));
		SetSettingInt(lpszCategoryName, szKeyNameBuffer, lpStruct->bAdmin);

		//absolute paths
		_stprintf_s(szKeyNameBuffer, MAX_PATH + 5, TEXT("%i.%s"), nButtonIndex, TEXT("AbsolutePaths"));
		SetSettingInt(lpszCategoryName, szKeyNameBuffer, lpStruct->bAbsolute);
	}
	return 1;
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

/*
//-------------------------------------------------------------------------
//
//  @brief    Remove blanks at the beginning and the end of a string.
//  @param    str  String to parse and alter.
//  @return   unsigned New size of the string without terminating NULL character
//
//--------------------------------------------------------------------------
static UINT strstrip(LPTSTR s){
	LPTSTR last = NULL;
	LPTSTR dest = s;

	if(s == NULL) return 0;

	last = s + _tcslen(s);
	while(isspace((int)*s) && *s) s++;
	while(last > s){
		if(!isspace((int)*(last - 1)))
			break;
		last--;
	}
	*last = (TCHAR)0;

	//memmove(dest, s, last - s + 1);
	memmove_s(dest, last - s + 1, s, last - s + 1);
	return last - s;
}


//-------------------------------------------------------------------------
//
//  @brief    Load a single line from an INI file
//  @param    input_line  Input line, may be concatenated multi-line input
//  @param    section     Output space to store section
//  @param    key         Output space to store key
//  @param    value       Output space to store value
//  @return   line_status value
//
//--------------------------------------------------------------------------
static line_status ParseIniLine(LPCTSTR input_line, LPTSTR section, LPTSTR key, LPTSTR value, INT nLineSize){

	LPTSTR line = _tcsdup(input_line);
	INT len = strstrip(line);
	line_status lineStatus = LINE_UNPROCESSED;

	if(len > nLineSize){
		return (line_status)-len;
	}
	else if(len < 1){
		// Empty line 
		lineStatus = LINE_EMPTY;
	}
	else if(line[0] == '#' || line[0] == ';'){
		// Comment line 
		lineStatus = LINE_COMMENT;
	}
	else if(line[0] == '[' && line[len - 1] == ']'){
		// Section name
		_stscanf_s(line, TEXT("[%[^]]"), section, MAX_SECTION_NAME_LEN);
		strstrip(section);
		//strlwc(section, section, len);
		lineStatus = LINE_SECTION;
	}
	else if(_stscanf_s(line, "%[^=] = \"%[^\"]\"", key, MAX_KEY_NAME_LEN, value, nLineSize) == 2
		|| _stscanf_s(line, "%[^=] = '%[^\']'", key, MAX_KEY_NAME_LEN, value, nLineSize) == 2){
		// Usual key=value with quotes, with or without comments
		strstrip(key);
		//strlwc(key, key, len);
		// Don't strip spaces from values surrounded with quotes
		lineStatus = LINE_VALUE;
	}
	else if(_stscanf_s(line, "%[^=] = %[^;#]", key, MAX_KEY_NAME_LEN, value, nLineSize) == 2){
		// Usual key=value without quotes, with or without comments 
		strstrip(key);
		//strlwc(key, key, len);
		strstrip(value);
		// 
		// sscanf cannot handle '' or "" as empty values
		// this is done here
		// 
		if(!strcmp(value, "\"\"") || (!strcmp(value, "''"))){
			value[0] = 0;
		}
		lineStatus = LINE_VALUE;
	}
	else if(_stscanf_s(line, "%[^=] = %[;#]", key, MAX_KEY_NAME_LEN, value, nLineSize) == 2
		|| _stscanf_s(line, "%[^=] %[=]", key, MAX_KEY_NAME_LEN, value, nLineSize) == 2){
		// 
		// Special cases:
		// key=
		// key=;
		// key=#
		// 
		strstrip(key);
		//strlwc(key, key, len);
		value[0] = 0;
		lineStatus = LINE_VALUE;
	}
	else{
		// Generate syntax error //
		lineStatus = LINE_ERROR;
	}

	free(line);
	return lineStatus;
	//return (line_status)-10;
}

//-------------------------------------------------------------------------
//
//  @brief    Parse an ini file and return an allocated dictionary object
//  @param    ininame Name of the ini file to read.
//  @return   Pointer to newly allocated dictionary
//
//  This is the parser for ini files. This function is called, providing
//  the name of the file to be read. It returns a dictionary object that
//  should not be accessed directly, but through accessor functions
//  instead.
//
//  The returned dictionary must be freed using iniparser_freedict().
//
//--------------------------------------------------------------------------
INT GetIniString(LPCTSTR lpSectionName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpReturnedString, DWORD nSize, LPCTSTR lpIniName){
	FILE* fileStream;

	TCHAR szLine[ASCIILINESZ + 1];
	TCHAR szSection[ASCIILINESZ + 1];
	TCHAR szKey[ASCIILINESZ + 1];
	//TCHAR szTmp[(ASCIILINESZ * 2) + 2];
	TCHAR szVal[ASCIILINESZ + 1];

	LPTSTR lpszLine = (LPTSTR)calloc(nSize, sizeof(TCHAR));
	LPTSTR lpszSection = (LPTSTR)calloc(MAX_SECTION_NAME_LEN, sizeof(TCHAR));
	LPTSTR lpszKey = (LPTSTR)calloc(MAX_KEY_NAME_LEN, sizeof(TCHAR));
	LPTSTR lpszVal = (LPTSTR)calloc(nSize, sizeof(TCHAR));

	int  last = 0;
	int  len;
	int  lineno = 0;
	int  errs = 0;
	//int  mem_err = 0;

	//dictionary* dict;

	if((fileStream = _tfopen(lpIniName, TEXT("r"))) == NULL){
		//iniparser_error_callback("iniparser: cannot open %s\n", ininame);
		return NULL;
	}

	//dict = dictionary_new(0);
	//if(!dict){
	//    fclose(in);
	//    return NULL;
	//}

	memset(szLine, 0, ASCIILINESZ);
	memset(szSection, 0, ASCIILINESZ);
	memset(szKey, 0, ASCIILINESZ);
	memset(szVal, 0, ASCIILINESZ);
	last = 0;

	while(_fgetts(szLine + last, ASCIILINESZ - last, fileStream) != NULL){
		lineno++;
		len = (int)_tcslen(szLine) - 1;
		if(len <= 0){
			continue;
		}
		// Safety check against buffer overflows
		if(szLine[len] != '\n' && !feof(fileStream)){
			//iniparser_error_callback( "iniparser: input line too long in %s (%d)\n",ininame, lineno);
			//dictionary_del(dict);
			fclose(fileStream);
			return NULL;
		}
		// Get rid of \n and spaces at end of line
		while((len >= 0) &&
			((szLine[len] == '\n') || (isspace(szLine[len])))){
			szLine[len] = 0;
			len--;
		}
		if(len < 0){ // Line was entirely \n and/or spaces
			len = 0;
		}
		// Detect multi-line 
		if(szLine[len] == '\\'){
			// Multi-line value
			last = len;
			continue;
		}
		else{
			last = 0;
		}

		line_status ParseResult = ParseIniLine(szLine, szSection, szKey, szVal, nSize);
		if(ParseResult < 0){
			//too small buffer
			return ParseResult;
		}

		switch(ParseResult){
			case LINE_EMPTY:
				break;
			case LINE_COMMENT:
				break;

			case LINE_SECTION:
				//mem_err = dictionary_set(dict, section, NULL);
				break;

			case LINE_VALUE:
				//sprintf(tmp, "%s:%s", section, key);
				//mem_err = dictionary_set(dict, tmp, val);
				if(!_tcscmp(szSection, lpSectionName) && !_tcscmp(szKey, lpKeyName)){
					INT nResultLen = _tcslen(szVal);
					if(nResultLen >= nSize){
						fclose(fileStream);
						return -nResultLen;
					}
					_tcscpy_s(lpReturnedString, nSize, szVal);
					fclose(fileStream);
					return 1;
				}
				break;

			case LINE_ERROR:
				//iniparser_error_callback("iniparser: syntax error in %s (%d):\n-> %s\n", ininame,lineno,line);
				errs++;
				break;
			default:
				break;
		}
		memset(szLine, 0, ASCIILINESZ);
		last = 0;
		//if(mem_err < 0){
			//iniparser_error_callback("iniparser: memory allocation failure\n");
		//    break;
		//}
	}
	if(errs){
	   // dictionary_del(dict);
		//dict = NULL;
	}
	//key not found
	_tcscpy_s(lpReturnedString, nSize, lpDefault);

	fclose(fileStream);
	return 1;
}
*/