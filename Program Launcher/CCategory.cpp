//
//            _                                                
//   ___ __ _| |_ ___  __ _  ___  _ __ _   _   ___ _ __  _ __  
//  / __/ _` | __/ _ \/ _` |/ _ \| '__| | | | / __| '_ \| '_ \ 
// | (_| (_| | ||  __/ (_| | (_) | |  | |_| || (__| |_) | |_) |
//  \___\__,_|\__\___|\__, |\___/|_|   \__, (_)___| .__/| .__/ 
//                    |___/            |___/      |_|   |_|    
//
// This file contains functions related to Program Launcher's categories (in tab control)
// 
// Functions included in this file:
// 
//  Copyright ©2021, barty12
// #include "LICENSE"
//

#include "pch.h"
#include "Program Launcher.h"


CCategory::CCategory(CProgramLauncher* ptrParent, const UINT parentIndex, const wstring& wsName)
	:
	ptrParent(ptrParent),
	parentIndex(parentIndex),
	wsCategoryName(wsName)
{
	imSmall.Create(SMALL_ICON_SIZE, SMALL_ICON_SIZE, ILC_COLOR32, 0, 0);
	imLarge.Create(Launcher->options.IconSize, Launcher->options.IconSize, ILC_COLOR32, 0, 0);
	//CFile file(L"iconcache.db", CFile::modeRead);
	//CArchive ar(&file, CArchive::load);
	//imLarge.Read(&ar);
}


int CCategory::LoadElements(){
	vItems.clear();
	ElementProps props;
	UINT j = 0;
	for(; GetElementProperties(wsCategoryName, j, props); ++j){
		vItems.push_back(make_shared<CLauncherItem>(this, j, props));
		imLarge.Add(vItems[j]->LoadIcon(Launcher->options.IconSize));
		imSmall.Add(vItems[j]->LoadIcon(SMALL_ICON_SIZE));
		if(parentIndex == Launcher->GetCurrentCategoryIndex()){
			vItems[j]->InsertIntoListView();
			CMainWnd->m_statusBar.GetStatusBarCtrl().SetText(wstring(to_wstring(j) + L" items, loading...").c_str(), 255, 0);
			//CMainWnd->m_statusBar.Invalidate();
		}
	}
	CMainWnd->m_statusBar.GetStatusBarCtrl().SetText(wstring(to_wstring(j) + L" items").c_str(), 255, 0);
	return j;
}

bool CCategory::Rename(){
	CUserInputDlg dlg(wsCategoryName);
	if(dlg.DoModal() == 1){
		TCITEMW tie = {0};
		tie.mask = TCIF_TEXT;
		tie.pszText = wsCategoryName.data();
		tie.cchTextMax = wsCategoryName.size();
		CMainWnd->CTab.SetItem(parentIndex, &tie);
		CMainWnd->Invalidate(false);
		return true;
	}
	return false;
}

bool CCategory::Remove(bool bAsk, bool bKeepItems){
	if(!bAsk || CMainWnd->MessageBoxW(std::format(GetString(IDS_REMOVE_CATEGORY_PROMPT), wsCategoryName).c_str(), GetString(IDS_REMOVE_CATEGORY).c_str(), MB_YESNO | MB_ICONEXCLAMATION) == IDYES){
		UINT index = parentIndex;
		Launcher->vCategories.erase(Launcher->vCategories.begin() + index);
		Launcher->ReindexCategories(index);
		CMainWnd->CTab.DeleteItem(index);
		if(Launcher->vCategories.size() == 0){
			CMainWnd->UpdateListView();
			return true;
		}
		CMainWnd->CTab.SetCurSel(CMainWnd->CTab.GetItemCount() - 1);
		CMainWnd->UpdateListView();
		Launcher->bRebuildIconCache = true;
		return true;
	}
	return false;
}

bool CCategory::Move(UINT newPos, bool bRelative){
	if(bRelative){
		newPos += parentIndex;
	}

	shared_ptr<CCategory> temp = ptrParent->vCategories[parentIndex];
	if(parentIndex == newPos){
		return true;
	} 
	else if(newPos >= ptrParent->vCategories.size()){
		return false;
	} 
	else if(parentIndex < newPos){
		for(UINT i = parentIndex; i < newPos; ++i){
			ptrParent->vCategories[i] = ptrParent->vCategories[i + 1];
		}
	} 
	else{
		for(UINT i = parentIndex; i > newPos; --i){
			ptrParent->vCategories[i] = ptrParent->vCategories[i - 1];
		}
	}
	ptrParent->vCategories[newPos] = temp;
	ptrParent->ReindexCategories();

	CMainWnd->UpdateListView(true);
	Launcher->bRebuildIconCache = true;

	return true;
}

bool CCategory::MoveTab(UINT newPos, bool bRelative){
	if(bRelative){
		newPos += parentIndex;
	}

	CMainWnd->CTab.ReorderTab(parentIndex, newPos);

	return true;
}

void CCategory::ReindexItems(UINT iStart){
	for(size_t i = iStart; i < vItems.size(); ++i){
		vItems[i]->parentIndex = i;
	}
}

CLauncherItem* CCategory::GetItem(UINT index){
	return vItems.at(index).get();
}
CLauncherItem* CCategory::GetSelectedItem(){
	return vItems.at(Launcher->GetSelectedItemIndex()).get();
}




// CUserInputDlg dialog

IMPLEMENT_DYNAMIC(CUserInputDlg, CDialog)

CUserInputDlg::CUserInputDlg(wstring& pName, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_USERINPUTDLG, pParent),
	m_wsName(pName){

}

CUserInputDlg::~CUserInputDlg(){}

void CUserInputDlg::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CUserInputDlg, CDialog)
	ON_COMMAND(IDC_USERINPUTDLGEDIT, &CUserInputDlg::OnUserInput)
END_MESSAGE_MAP()


// CUserInputDlg message handlers


BOOL CUserInputDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	//center the dialog
	this->CenterWindow();

	//set maximum number of characters to CATEGORY_NAME_LEN
	this->SendDlgItemMessageW(IDC_USERINPUTDLGEDIT, EM_SETLIMITTEXT, CATEGORY_NAME_LEN);

	if(!m_wsName.empty()){
		this->SetWindowTextW(GetString(IDS_RENAME_CATEGORY).c_str());
		this->GetDlgItem(IDC_USERINPUTDLGEDIT)->SetWindowTextW(m_wsName.c_str());
	}

	return true;
}


void CUserInputDlg::OnUserInput(){
	// Set the default push button to "OK" when the user enters text.
	//if(HIWORD(wParam) == EN_CHANGE){
		this->SendMessageW(DM_SETDEFID, IDOK);
	//}
}


void CUserInputDlg::OnOK(){
	//CDialog::OnOK();
	try{
		CString name;
		this->GetDlgItem(IDC_USERINPUTDLGEDIT)->GetWindowTextW(name);

		if(name.GetLength() >= CATEGORY_NAME_LEN){
			throw IDS_NAME_TOO_LONG;
		}
		if(name.GetLength() <= 0){
			throw IDS_ENTER_VALID_NAME;
		}
		if(name.Find(CATEGORY_NAME_DELIM) != -1 || name == L"general" || name == L"appereance" || name == L"categories"){
			throw IDS_NAME_INVALID_CHARACTERS;
		}
		m_wsName = name;

		this->EndDialog(true);
		return;
	} catch(int code){
		ErrorMsg(code);
		return;
	} catch(...){
		ErrorMsg(IDS_ERROR);
		ErrorHandlerDebug();
		return;
	}
}
