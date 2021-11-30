// CPreferencesDlg.cpp : implementation file
//

#include "pch.h"
#include "Program Launcher.h"


// CPreferencesDlg dialog

IMPLEMENT_DYNAMIC(CPreferencesDlg, CDialogEx)

CPreferencesDlg::CPreferencesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PREFDLG, pParent),
	currentOptions(Launcher->options){}

CPreferencesDlg::~CPreferencesDlg(){}

void CPreferencesDlg::DoDataExchange(CDataExchange* pDX){
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_PREFDLG_APPNAMES_DONTSHOW, reinterpret_cast<int&>(currentOptions.ShowAppNames));
	DDX_Check(pDX, IDC_PREFDLG_CLOSEAFTERLAUNCH, reinterpret_cast<BOOL&>(currentOptions.CloseAfterLaunch));
	DDX_Check(pDX, IDC_PREFDLG_ICONCACHING, reinterpret_cast<BOOL&>(currentOptions.UseIconCaching));
	DDX_Text(pDX, IDC_PREFDLG_ICONSIZEEDIT, currentOptions.IconSize);
	DDX_Text(pDX, IDC_PREFDLG_HICONSPACINGEDIT, currentOptions.IconSpacingHorizontal);
	DDX_Text(pDX, IDC_PREFDLG_VICONSPACINGEDIT, currentOptions.IconSpacingVertical);
	DDX_Slider(pDX, IDC_PREFDLG_ICONSIZESLIDER, currentOptions.IconSize);
	DDX_Slider(pDX, IDC_PREFDLG_HICONSPACINGSLIDER, currentOptions.IconSpacingHorizontal);
	DDX_Slider(pDX, IDC_PREFDLG_VICONSPACINGSLIDER, currentOptions.IconSpacingVertical);
	DDX_Control(pDX, IDC_PREFDLG_ICONSIZESLIDER, m_IconSizeSlider);
	DDX_Control(pDX, IDC_PREFDLG_HICONSPACINGSLIDER, m_hSpacingSlider);
	DDX_Control(pDX, IDC_PREFDLG_VICONSPACINGSLIDER, m_vSpacingSlider);
	DDX_Control(pDX, IDC_PREFDLG_ICONSIZE_SPIN, m_IconSizeSpin);
	DDX_Control(pDX, IDC_PREFDLG_HICONSPACING_SPIN, m_hSpacingSpin);
	DDX_Control(pDX, IDC_PREFDLG_VICONSPACING_SPIN, m_vSpacingSpin);
}





BEGIN_MESSAGE_MAP(CPreferencesDlg, CDialogEx)
	ON_BN_CLICKED(IDC_PREFDLG_RESET, &CPreferencesDlg::OnReset)
	ON_BN_CLICKED(IDC_PREFDLG_APPNAMES_DONTSHOW, &CPreferencesDlg::OnAppNamesClicked)
	ON_BN_CLICKED(IDC_PREFDLG_APPNAMES_INTOOLTIP, &CPreferencesDlg::OnAppNamesClicked)
	ON_BN_CLICKED(IDC_PREFDLG_APPNAMES_UNDERICON, &CPreferencesDlg::OnAppNamesClicked)
	ON_EN_UPDATE(IDC_PREFDLG_ICONSIZEEDIT, &CPreferencesDlg::OnIconSizeEditUpdate)
	ON_EN_UPDATE(IDC_PREFDLG_HICONSPACINGEDIT, &CPreferencesDlg::OnHSpacingEditUpdate)
	ON_EN_UPDATE(IDC_PREFDLG_VICONSPACINGEDIT, &CPreferencesDlg::OnVSpacingEditUpdate)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CPreferencesDlg message handlers
BOOL CPreferencesDlg::OnInitDialog(){
	CDialogEx::OnInitDialog();

	//make copy of current options
	oldOptions = currentOptions;

	//center the dialog
	this->CenterWindow();

	//set ranges to spin controls
	m_IconSizeSpin.SetRange(MIN_ICON_SIZE, MAX_ICON_SIZE);
	m_hSpacingSpin.SetRange(MIN_ICON_SPACING, MAX_ICON_SPACING);
	m_vSpacingSpin.SetRange(MIN_ICON_SPACING, MAX_ICON_SPACING);

	//set ranges to slider cotrols
	m_IconSizeSlider.SetRange(MIN_ICON_SIZE, MAX_ICON_SIZE, true);
	m_hSpacingSlider.SetRange(MIN_ICON_SPACING, MAX_ICON_SPACING, true);
	m_vSpacingSlider.SetRange(MIN_ICON_SPACING, MAX_ICON_SPACING, true);

	//set marks to size slider
	const UINT marks[] = {16,32,48,64,128,256};
	for(auto &mark : marks){
		m_IconSizeSlider.SetTic(mark);
	}

	//push data to controls
	UpdateData(false);
	
	return true;
}

void CPreferencesDlg::OnOK(){
	this->UpdateData(true);

	ApplyOptions(currentOptions, OPT_ALL);

	CDialogEx::OnOK();
}


void CPreferencesDlg::OnCancel(){
	currentOptions = oldOptions;

	ApplyOptions(currentOptions, OPT_ALL);

	CDialogEx::OnCancel();
}


void CPreferencesDlg::OnReset(){
	//constructor of options initializes it to default values
	CProgramLauncher::_options defaultOptions;
	currentOptions = defaultOptions;
	this->UpdateData(false);
	this->ApplyOptions(currentOptions, OPT_ALL);
}


void CPreferencesDlg::OnAppNamesClicked(){
	this->UpdateData();
	this->ApplyOptions(currentOptions, OPT_APPNAMES);
}


void CPreferencesDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar){
	this->UpdateData(true);
	this->UpdateData(false);
	if(*(CSliderCtrl*)pScrollBar == m_hSpacingSlider || *(CSliderCtrl*)pScrollBar == m_vSpacingSlider){
		this->ApplyOptions(currentOptions, OPT_SPACING);
	}
	if(*(CSliderCtrl*)pScrollBar == m_IconSizeSlider && nSBCode == TB_ENDTRACK){
		this->ApplyOptions(currentOptions, OPT_SIZE);
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CPreferencesDlg::OnIconSizeEditUpdate(){
	if(m_IconSizeSpin){
		this->SyncSlider(m_IconSizeSpin, m_IconSizeSlider, MAX_ICON_SIZE);
		this->UpdateData(true);
		this->ApplyOptions(currentOptions, OPT_SIZE);
	}
}
void CPreferencesDlg::OnHSpacingEditUpdate(){
	if(m_hSpacingSpin){
		this->SyncSlider(m_hSpacingSpin, m_hSpacingSlider, MAX_ICON_SPACING);
		this->UpdateData(true);
		this->ApplyOptions(currentOptions, OPT_SPACING);
	}
}
void CPreferencesDlg::OnVSpacingEditUpdate(){
	if(m_vSpacingSpin){
		this->SyncSlider(m_vSpacingSpin, m_vSpacingSlider, MAX_ICON_SPACING);
		this->UpdateData(true);
		this->ApplyOptions(currentOptions, OPT_SPACING);
	}
}


void CPreferencesDlg::SyncSlider(CSpinButtonCtrl& spin, CSliderCtrl& slider, int maxVal){
	//Update slider according to edit box
	CString sNum;
	spin.GetBuddy()->GetWindowTextW(sNum);
	int size = wcstol(sNum, null, 10);
	if(size > maxVal){
		size = maxVal;
		spin.SetPos(size);
	}
	slider.SetPos(size);
}

void CPreferencesDlg::ApplyOptions(CProgramLauncher::_options& options, DWORD dwFilter){
	if(dwFilter & OPT_APPNAMES){
		DWORD dwStyle = CMainWnd->CList.GetExtendedStyle();
		switch(options.ShowAppNames){
			case 0:
				dwStyle &= ~LVS_EX_INFOTIP;
				dwStyle &= ~LVS_EX_LABELTIP;
				[[fallthrough]];
			case 1:
				dwStyle |= LVS_EX_HIDELABELS;
				break;
			default:
				dwStyle &= ~LVS_EX_HIDELABELS;
		}
		CMainWnd->CList.SetExtendedStyle(dwStyle);
	}
	if(dwFilter & OPT_SIZE){
		Launcher->GetCurrentCategory()->UpdateImageLists(options.IconSize);
		CMainWnd->UpdateListView();
	}
	if(dwFilter & OPT_SPACING){
		CMainWnd->CList.SetIconSpacing(options.IconSpacingHorizontal, options.IconSpacingVertical);
		CMainWnd->CList.Invalidate(0);
	}
	if(dwFilter & OPT_WINDOW){
		CMainWnd->SetWindowPos(null, 0, 0, options.WindowWidth, options.WindowHeight, SWP_NOMOVE | SWP_NOZORDER);
	}
}
