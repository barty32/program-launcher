// CBtnEditDlg.cpp : implementation file
//
// #include "LICENSE"
//

#include "pch.h"
#include "Program Launcher.h"


// CBtnEditDlg dialog

IMPLEMENT_DYNAMIC(CBtnEditDlg, CDialog)

CBtnEditDlg::CBtnEditDlg(ElementProps& props, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_BTNSETTINGDLG, pParent), m_props(props){

}

CBtnEditDlg::~CBtnEditDlg()
{
	DestroyIcon(m_hPreview);
}

void CBtnEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNDLGCATSELECT, m_categorySelect);
	DDX_Text(pDX, IDC_BTNDLGPATHEDIT, m_csPath);
	DDX_Text(pDX, IDC_BTNDLGPATH64EDIT, m_csPath64);
	DDX_Text(pDX, IDC_BTNDLGNAMEEDIT, m_csName);
	DDX_Text(pDX, IDC_BTNDLGICONEDIT, m_csIconPath);
	DDX_Check(pDX, IDC_BTNDLGABSCHECK, m_bAbsPaths);
	DDX_Check(pDX, IDC_BTNDLGADMINCHECK, m_bAdmin);
	DDX_Check(pDX, IDC_BTNDLG64CHECK, m_64bitPath);
}


BEGIN_MESSAGE_MAP(CBtnEditDlg, CDialog)
	ON_BN_CLICKED(IDC_BTNDLG64CHECK, &CBtnEditDlg::On64BitCheck)
	ON_BN_CLICKED(IDC_BTNDLGPATHBROWSE, &CBtnEditDlg::OnPathBrowse)
	ON_BN_CLICKED(IDC_BTNDLGPATH64BROWSE, &CBtnEditDlg::OnPath64Browse)
	ON_BN_CLICKED(IDC_BTNDLGICONPICK, &CBtnEditDlg::OnIconPick)
	ON_EN_CHANGE(IDC_BTNDLGPATHEDIT, &CBtnEditDlg::OnPathChange)
	ON_EN_CHANGE(IDC_BTNDLGICONEDIT, &CBtnEditDlg::OnIconPathChange)
END_MESSAGE_MAP()


// CBtnEditDlg message handlers


BOOL CBtnEditDlg::OnInitDialog(){
	CDialog::OnInitDialog();

	//center the dialog
	this->CenterWindow();

	for(auto& cat : Launcher->vCategories){
		m_categorySelect.AddString(cat->wsCategoryName.c_str());
	}
	m_categorySelect.SetCurSel(Launcher->GetCurrentCategoryIndex());

	if(bNewButton){
		SetWindowTextW(GetString(IDS_NEW_BUTTON).c_str());
		this->FillData();
	}
	else{
		SetWindowTextW(GetString(IDS_EDIT_BUTTON).c_str());
		//load existing data
		//disable category select combobox
		m_categorySelect.EnableWindow(false);
		m_csPath = m_props.wsPath.c_str();
		if(!m_props.wsPath64.empty()){
			m_64bitPath = true;
			m_csPath64 = m_props.wsPath64.c_str();
			GetDlgItem(IDC_BTNDLGPATH64EDIT)->EnableWindow();
			GetDlgItem(IDC_BTNDLGPATH64BROWSE)->EnableWindow();
		}
		m_csIconPath = (m_props.wsPathIcon + L',' + to_wstring(m_props.iIconIndex)).c_str();
		m_csName = m_props.wsName.c_str();
		m_bAbsPaths = m_props.bAbsolute;
		m_bAdmin = m_props.bAdmin;
		UpdateData(false);
		OnIconPathChange();
	}

	return true;
}


void CBtnEditDlg::On64BitCheck(){
	UpdateData();
	GetDlgItem(IDC_BTNDLGPATH64EDIT)->EnableWindow(m_64bitPath);
	GetDlgItem(IDC_BTNDLGPATH64BROWSE)->EnableWindow(m_64bitPath);
}


void CBtnEditDlg::OnPathBrowse(){
	CFileDialog ofn(
		true,
		L"exe",
		null,
		OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NONETWORKBUTTON,
		L"Executables (*.exe)|*.exe|All Files (*.*)|*.*||",
		this
	);

OFN_RETRY:
	if(ofn.DoModal() == IDOK){
		m_csPath = ofn.GetPathName();

		if(!this->FillData()){
			goto OFN_RETRY;
		}
	}
}


bool CBtnEditDlg::FillData(){
	//check for compatibility
	DWORD dwExeType = 0;
	if(GetBinaryTypeW(m_csPath, &dwExeType)){
		switch(dwExeType){
			//case SCS_32BIT_BINARY:
			//	break;
			case SCS_DOS_BINARY:
				if(MessageBoxW(GetString(IDS_DOS_BINARY).c_str(), TEXT("MS-DOS Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
					return false;
				}
				break;
			case SCS_WOW_BINARY:
				if(MessageBoxW(GetString(IDS_WOW_BINARY).c_str(), TEXT("16-bit Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
					return false;
				}
				break;
			case SCS_64BIT_BINARY:
				if(MessageBoxW(GetString(IDS_64BIT_BINARY).c_str(), TEXT("64-bit Application"), MB_ICONEXCLAMATION | MB_YESNO) == IDNO){
					return false;
				}
				break;
		}
	}

	//fill name edit box if it's empty
	if(m_csName.IsEmpty()){
		DWORD dwLen = GetFileVersionInfoSizeW(m_csPath, null);
		if(dwLen){
			LPVOID lpBlock = HeapAlloc(GetProcessHeap(), 0, dwLen);
			if(lpBlock){
				WORD* langInfo;
				UINT cbLang;
				WCHAR tszVerStrName[128];
				LPVOID lpt;
				UINT cbBufSize = 0;
				GetFileVersionInfoW(m_csPath, 0, dwLen, lpBlock);
				//Get the Product Name
				//First, to get string information, we need to get language information.
				VerQueryValueW(lpBlock, L"\\VarFileInfo\\Translation", (LPVOID*)&langInfo, &cbLang);
				//Prepare the label -- default lang is bytes 0 & 1 of langInfo
				_snwprintf_s(tszVerStrName, 128, _TRUNCATE, L"\\StringFileInfo\\% 04x% 04x\\% s", langInfo[0], langInfo[1], L"ProductName");
				//Get the string from the resource data
				if(VerQueryValueW(lpBlock, tszVerStrName, &lpt, &cbBufSize)){
					m_csName = (LPCWSTR)lpt;
				}
				HeapFree(GetProcessHeap(), 0, lpBlock);
			}
		}
	}
	//tick the absolute paths checkbox 
	WCHAR szDriveLetter[2];
	if(m_csPath.GetLength() >= 2){
		// Get current drive letter
		if(GetModuleFileNameW(null, szDriveLetter, 2)){
			m_bAbsPaths = m_csPath[0] != szDriveLetter[0];
		}
	}
	UpdateData(false);
	OnPathChange();
	return true;
}


void CBtnEditDlg::OnPath64Browse(){

	CFileDialog ofn(
		true, 
		L"exe", 
		null, 
		OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		L"Executables (*.exe)|*.exe|All Files (*.*)|*.*||",
		this
	);

	if(ofn.DoModal() == IDOK){
		m_csPath64 = ofn.GetPathName();
		UpdateData(false);
	}
}


void CBtnEditDlg::OnIconPick(){
	UpdateData();
	INT nIconIndex = GetIconIndexFromPath(m_csIconPath.GetBuffer());
	m_csIconPath.ReleaseBuffer();
	if(PickIconDlg(this->GetSafeHwnd(), m_csIconPath.GetBuffer(MAX_PATH), MAX_PATH, &nIconIndex)){
		m_csIconPath.ReleaseBuffer();
		m_csIconPath += (L',' + to_wstring(nIconIndex)).c_str();
		UpdateData(false);
		OnIconPathChange();
	}
}


void CBtnEditDlg::OnPathChange(){
	UpdateData();
	if(PathFileExistsW(m_csPath)){
		m_csIconPath = m_csPath + L",0";
		UpdateData(false);
		OnIconPathChange();
	}
}


void CBtnEditDlg::OnIconPathChange(){
	//display icon preview if the icon path is changed
	UpdateData();
	INT nIconIndex = GetIconIndexFromPath(m_csIconPath.GetBuffer());
	m_csIconPath.ReleaseBuffer();

	DestroyIcon(m_hPreview);
	m_hPreview = ExtractIconW(hInst, m_csIconPath, nIconIndex);
	Static_SetIcon(GetDlgItem(IDC_BTNDLGICONPREV)->GetSafeHwnd(), m_hPreview);
}


void CBtnEditDlg::OnOK(){
	try{
		UpdateData();
		if(m_csName.IsEmpty()){
			throw IDS_ENTER_VALID_NAME;
		}
		if(m_csPath.IsEmpty()){
			throw IDS_ENTER_VALID_PATH;
		}
		if(m_csIconPath.IsEmpty()){
			throw IDS_ENTER_VALID_PATH;
		}
		m_props.wsPath = m_csPath;
		if(m_64bitPath){
			m_props.wsPath64 = m_csPath64;
		}
		else{
			m_props.wsPath64.clear();
		}
		m_props.wsName = m_csName;
		m_props.iIconIndex = GetIconIndexFromPath(m_csIconPath.GetBuffer());
		m_csIconPath.ReleaseBuffer();
		m_props.wsPathIcon = m_csIconPath;
		m_props.bAbsolute = m_bAbsPaths;
		m_props.bAdmin = m_bAdmin;

		uCategory = m_categorySelect.GetCurSel();

		EndDialog(IDOK);
		CDialog::OnOK();
	} catch(int code){
		MessageBoxW(GetString(code).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION | MB_OK);
	} catch(...){
		MessageBoxW(GetString(IDS_ERROR).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION | MB_OK);
	}
}
