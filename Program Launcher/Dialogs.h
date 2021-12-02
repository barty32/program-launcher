#pragma once

// CUserInputDlg dialog

class CUserInputDlg : public CDialog{
	DECLARE_DYNAMIC(CUserInputDlg)

	public:
	CUserInputDlg(wstring& pName, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CUserInputDlg();

	wstring& m_wsName;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_USERINPUTDLG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	public:
	virtual BOOL OnInitDialog();
	afx_msg void OnUserInput();
	virtual void OnOK();
};



// CPreferencesDlg dialog

class CPreferencesDlg : public CDialog{
	DECLARE_DYNAMIC(CPreferencesDlg)

	public:
	CPreferencesDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPreferencesDlg();

	void SyncSlider(CSpinButtonCtrl& spin, CSliderCtrl& slider, int maxVal);
	void ApplyOptions(CProgramLauncher::_options& options, DWORD dwFilter);
	CProgramLauncher::_options oldOptions;
	CProgramLauncher::_options& currentOptions;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_PREFDLG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnReset();
	afx_msg void OnAppNamesClicked();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnIconSizeEditUpdate();
	afx_msg void OnHSpacingEditUpdate();
	afx_msg void OnVSpacingEditUpdate();
	CSliderCtrl m_IconSizeSlider;
	CSliderCtrl m_hSpacingSlider;
	CSliderCtrl m_vSpacingSlider;
	CSpinButtonCtrl m_IconSizeSpin;
	CSpinButtonCtrl m_hSpacingSpin;
	CSpinButtonCtrl m_vSpacingSpin;
};

// CBtnEditDlg dialog

class CBtnEditDlg : public CDialog{
	DECLARE_DYNAMIC(CBtnEditDlg)

	public:
	CBtnEditDlg(ElementProps& props, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CBtnEditDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_BTNSETTINGDLG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	public:
	virtual BOOL OnInitDialog();
	afx_msg void On64BitCheck();
	afx_msg void OnPathBrowse();
	afx_msg void OnPath64Browse();
	afx_msg void OnIconPick();
	afx_msg void OnPathChange();
	afx_msg void OnIconPathChange();
	virtual void OnOK();
	CComboBox m_categorySelect;
	bool bNewButton = true;
	UINT uCategory = 0;
	ElementProps& m_props;

	CString m_csPath;
	CString m_csPath64;
	CString m_csName;
	CString m_csIconPath;
	BOOL m_bAbsPaths = false;
	BOOL m_bAdmin = false;
	BOOL m_64bitPath = false;
};


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog{
	public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_ABOUTBOX };
#endif

	protected:
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
	public:
	virtual BOOL OnInitDialog();
};