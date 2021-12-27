// CLauncherWnd.cpp : implementation of the CMainWnd class
// 
// #include "LICENSE"
// 
//

#include "pch.h"
#include "Program Launcher.h"


// CLauncherWnd

IMPLEMENT_DYNAMIC(CLauncherWnd, CFrameWnd)

BEGIN_MESSAGE_MAP(CLauncherWnd, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_INITMENUPOPUP()
//	ON_COMMAND(IDM_ABOUT, &CLauncherWnd::OnAbout)
	ON_COMMAND(IDM_FILE_PREF, &CLauncherWnd::OnFilePref)
	ON_COMMAND(IDM_CATEGORY_ADDCATEGORY, &CLauncherWnd::OnAddCategory)
	ON_COMMAND(IDM_CATEGORY_REMOVECURRENTCATEGORY, &CLauncherWnd::OnRemoveCurrentCategory)
	ON_COMMAND(IDM_CATEGORY_RENAMECURRENTCATEGORY, &CLauncherWnd::OnRenameCurrentCategory)
	ON_COMMAND(IDM_EXIT, &CLauncherWnd::OnExit)
	ON_COMMAND(IDM_BUTTON_ADDNEWBUTTON, &CLauncherWnd::OnAddNewButton)
	ON_COMMAND(IDM_BUTTON_EDIT, &CLauncherWnd::OnButtonEdit)
	ON_COMMAND(IDM_BUTTON_LAUNCH, &CLauncherWnd::OnButtonLaunch)
	ON_COMMAND(IDM_BUTTON_REMOVE, &CLauncherWnd::OnButtonRemove)
	ON_COMMAND(IDM_POPUPCATEGORY_REMOVECATEGORY, &CLauncherWnd::OnRemoveCategory)
	ON_COMMAND(IDM_POPUPCATEGORY_RENAME, &CLauncherWnd::OnRenameCategory)
	ON_COMMAND(IDM_VIEW_DETAILS, &CLauncherWnd::OnViewDetails)
	ON_COMMAND(IDM_VIEW_LARGEICONS, &CLauncherWnd::OnViewLargeicons)
	ON_COMMAND(IDM_VIEW_LIST, &CLauncherWnd::OnViewList)
	ON_COMMAND(IDM_VIEW_SMALLICONS, &CLauncherWnd::OnViewSmallicons)
	ON_COMMAND(IDM_BUTTON_MOVELEFT, &CLauncherWnd::OnButtonMoveLeft)
	ON_COMMAND(IDM_BUTTON_MOVERIGHT, &CLauncherWnd::OnButtonMoveRight)
	ON_COMMAND(IDM_POPUPCATEGORY_MOVELEFT, &CLauncherWnd::OnCategoryMoveLeft)
	ON_COMMAND(IDM_POPUPCATEGORY_MOVERIGHT, &CLauncherWnd::OnCategoryMoveRight)
	ON_COMMAND(IDM_CATEGORY_MOVELEFT, &CLauncherWnd::OnCurCategoryMoveLeft)
	ON_COMMAND(IDM_CATEGORY_MOVERIGHT, &CLauncherWnd::OnCurCategoryMoveRight)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(IDM_VIEW_RESETCOLUMNS, &CLauncherWnd::OnResetColumns)
	ON_COMMAND(IDM_REFRESH, &CLauncherWnd::OnRefresh)
	ON_COMMAND(IDM_SAVE, &CLauncherWnd::OnSave)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

static const UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CLauncherWnd construction/destruction

CLauncherWnd::CLauncherWnd() noexcept{}

CLauncherWnd::~CLauncherWnd(){}

int CLauncherWnd::OnCreate(LPCREATESTRUCT lpCreateStruct){
	if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//SetIcon(LoadIconW(nullptr, MAKEINTRESOURCEW(IDC_PROGRAMLAUNCHER)), true);
	CenterWindow();

	m_statusBar.Create(this);
	m_statusBar.GetStatusBarCtrl().SetSimple();
	//if(!m_wndStatusBar.Create(this)){
	//	TRACE0("Failed to create status bar\n");
	//	return -1;      // fail to create
	//}
	//m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	return 0;
}

BOOL CLauncherWnd::PreCreateWindow(CREATESTRUCT& cs){
	if(!CFrameWnd::PreCreateWindow(cs)) return false;

	cs.dwExStyle = WS_EX_ACCEPTFILES;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.cx = Launcher->options.WindowWidth;
	cs.cy = Launcher->options.WindowHeight;
	return true;
}


// CLauncherWnd message handlers

void CLauncherWnd::OnSetFocus(CWnd* /*pOldWnd*/){
	// forward focus to the view window
	CList.SetFocus();
}

void CLauncherWnd::OnClose(){

	//save list view state
	DWORD dwStyle = CList.GetView();
	Launcher->ini->WriteInt(L"general", L"ListViewMode", dwStyle);

	//save column widths
	vector<int> colWidths;
	for(int i = 0; i < 7; i++){
		colWidths.push_back(CList.GetColumnWidth(i));
	}
	if(colWidths[0]){
		Launcher->ini->WriteString(L"general", L"ColumnWidths", StrJoinFromInt(colWidths).c_str());
	}

	//save column order
	vector<int> colOrder(7);
	CList.GetColumnOrderArray(&colOrder[0]);
	Launcher->ini->WriteString(L"general", L"ColumnOrder", StrJoinFromInt(colOrder).c_str());

	CFrameWnd::OnClose();
}

void CLauncherWnd::OnContextMenu(CWnd* pWnd, CPoint point){
	
	if(pWnd == &CList && Launcher->vCategories.size() > 0){
		this->DoListViewContextMenu(point.x, point.y);
		return;
	}

	if(pWnd == &CTab && point.x == -1 && point.y == -1){
		m_clickedTab = CTab.GetCurFocus();
		this->DoTabControlContextMenu(m_clickedTab, -1, -1);
		return;
	}

	TCHITTESTINFO hitinfo;
	hitinfo.pt = point;
	this->ScreenToClient(&hitinfo.pt);
	m_clickedTab = CTab.HitTest(&hitinfo);

	if(m_clickedTab >= 0){
		this->DoTabControlContextMenu(m_clickedTab, point.x, point.y);
		return;
	}
	CFrameWnd::OnContextMenu(pWnd, point);
}

#pragma warning(push)
#pragma warning(disable : 26454) //disable arithmetic overflow warning

BOOL CLauncherWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult){
	LPNMHDR pInfo = reinterpret_cast<LPNMHDR>(lParam);
	if(Launcher == nullptr) return 0;
	if(pInfo->hwndFrom == CTab.GetSafeHwnd()){//Notify from tab control
		if(pInfo->code == TCN_SELCHANGE){//Tab changed
			try{
				this->UpdateListView();
			}catch(...){}
			return 0;
		} 
		else if(pInfo->code == TCN_KEYDOWN){
			if(((LPNMTVKEYDOWN)lParam)->wVKey == VK_TAB){//Key was pressed
				CList.SetFocus();
				return 0;
			}
		}
		else if(pInfo->code == TCN_TABORDERCHANGED){
			try{
				Launcher->GetCategory(((LPNMTCORDERCHANGE)lParam)->nSrcTab)->Move(((LPNMTCORDERCHANGE)lParam)->nDstTab);
			}catch(...){}
			return 0;
		}
	} 
	else if(pInfo->hwndFrom == CList.GetSafeHwnd()){//Notify from list view
		if(pInfo->code == NM_DBLCLK){//Double clicked
			try{
				Launcher->GetCurrentCategory()->GetItem(((LPNMITEMACTIVATE)lParam)->iItem)->Launch();
			} catch(...){}
			return 0;
		} 
		else if(pInfo->code == NM_RETURN){//Enter was pressed
			try{
				Launcher->GetCurrentCategory()->GetSelectedItem()->Launch();
			} catch(...){}
			return 0;
		} 
		else if(pInfo->code == LVN_KEYDOWN){//Key was pressed
			switch(((LPNMLVKEYDOWN)lParam)->wVKey){
				case VK_DELETE:
				{
					int nBtnIndex = Launcher->GetFocusedItemIndex();
					if(nBtnIndex != -1){
						try{
							Launcher->GetCurrentCategory()->GetItem(nBtnIndex)->Remove();
						} catch(...){}
						return 0;
					}
				}
				break;
				case VK_TAB:
					CTab.SetFocus();
					break;
				case VK_F2:
					CList.EditLabel(CList.GetNextItem(-1, LVNI_SELECTED));
					break;
				case VK_F5:
					this->UpdateListView(true);
					Launcher->RebuildIconCache();
					break;
				case VK_ESCAPE:
					if(m_bDragging){
						this->StopDragging(null);
					}
					break;
			}
		}
		else if(pInfo->code == LVN_ENDLABELEDITW){
			if(((LPNMLVDISPINFOW)lParam)->item.pszText){
				try{
					Launcher->GetCurrentCategory()->GetSelectedItem()->wsName = ((LPNMLVDISPINFOW)lParam)->item.pszText;
					Launcher->GetCurrentCategory()->GetSelectedItem()->UpdateInListView();
					return true;
				} catch(...){
					return false;
				}
			}
		}
		else if(pInfo->code == LVN_BEGINDRAG){
			try{
				Launcher->GetCurrentCategory()->imLarge.BeginDrag(CList.GetNextItem(-1, LVNI_SELECTED), {-10,-10});
				POINT pt = ((LPNMLISTVIEW)lParam)->ptAction;
				CList.ClientToScreen(&pt);
				Launcher->GetCurrentCategory()->imLarge.DragEnter(null, pt);
				m_bDragging = true;
				SetCapture();
				SetFocus();
			} catch(...){}
		}
	}
	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}
#pragma warning(pop)

void CLauncherWnd::OnMouseMove(UINT nFlags, CPoint point){
	
	if(m_bDragging){
		CPoint pt = point;
		ClientToScreen(&pt);
		Launcher->GetCurrentCategory()->imLarge.DragMove(pt);
		LVINSERTMARK mark = {0};
		mark.cbSize = sizeof(LVINSERTMARK);
		CList.InsertMarkHitTest(&point, &mark);
		CList.SetInsertMark(&mark);
	}

	CFrameWnd::OnMouseMove(nFlags, point);
}

void CLauncherWnd::OnLButtonUp(UINT nFlags, CPoint point){
	if(m_bDragging){
		this->StopDragging(&point);
	}
	CFrameWnd::OnLButtonUp(nFlags, point);
}

void CLauncherWnd::StopDragging(CPoint* point){
	m_bDragging = false;

	Launcher->GetCurrentCategory()->imLarge.DragLeave(null);
	Launcher->GetCurrentCategory()->imLarge.EndDrag();

	LVINSERTMARK mark = {0};
	mark.cbSize = sizeof(LVINSERTMARK);
	if(point){
		CList.InsertMarkHitTest(point, &mark);
		if(mark.iItem != -1){
			Launcher->GetCurrentCategory()->GetSelectedItem()->Move(mark.iItem);
		}
	}
	//remove insert mark
	mark.iItem = -1;
	CList.SetInsertMark(&mark);

	ReleaseCapture();
}

void CLauncherWnd::OnDropFiles(HDROP hDropInfo){

	WCHAR szItem[MAX_PATH];
	for(int i = 0; DragQueryFileW(hDropInfo, i, szItem, _countof(szItem)); i++){
		if(i >= 1){
			SetForegroundWindow();
			MessageBoxW(GetString(IDS_MULTIPLE_FILES_DESC).c_str(), GetString(IDS_MULTIPLE_FILES).c_str(), MB_OK | MB_ICONINFORMATION);
			break;
		}
		SetForegroundWindow();
		if(Launcher->vCategories.size() == 0){
			MessageBoxW(GetString(IDS_NO_CATEGORY).c_str(), GetString(IDS_ERROR).c_str(), MB_ICONEXCLAMATION);
			continue;
		}
		shared_ptr<CLauncherItem> pNewItem = make_shared<CLauncherItem>(nullptr, 0, L"", L"");
		CBtnEditDlg dlg(pNewItem->props);
		dlg.bNewButton = true;
		dlg.m_csPath = szItem;
		if(dlg.DoModal() == IDOK){
			shared_ptr<CCategory> category = Launcher->vCategories.at(dlg.uCategory);
			category->vItems.push_back(pNewItem);
			pNewItem->ptrParent = category.get();
			pNewItem->parentIndex = category->vItems.size() - 1;
			category->imLarge.Add(pNewItem->LoadIcon(Launcher->options.IconSize));
			category->imSmall.Add(pNewItem->LoadIcon(SMALL_ICON_SIZE));
			pNewItem->InsertIntoListView();
		}
	}
	DragFinish(hDropInfo);

	//CFrameWnd::OnDropFiles(hDropInfo);
}


void CLauncherWnd::OnSize(UINT nType, int cx, int cy){
	CFrameWnd::OnSize(nType, cx, cy);
	this->ResizeTabControl();
	this->ResizeListView();
	CRect rcWnd;
	this->GetWindowRect(&rcWnd);
	Launcher->options.WindowWidth = rcWnd.Width();
	Launcher->options.WindowHeight = rcWnd.Height();
}


void CLauncherWnd::OnPaint(){
	CFrameWnd::OnPaint();
	//CPaintDC dc(&CList); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CFrameWnd::OnPaint() for painting messages
	//CDC* dc = CList.GetDC();
	//PAINTSTRUCT ps;
	//HDC hdc = BeginPaint(hWnd, &ps);
	//// TODO: Add any drawing code that uses hdc here...
	//if(hwndTab == NULL){
	//	RECT clientRect;
	//	GetClientRect(hwndMain, &clientRect);
	//	TextOut(hdc, clientRect.right / 2 - 200, clientRect.bottom / 2 - 5, LoadLocalString(IDS_FIRST_STARTUP_TIP), 55);
	//}

	//if(CTab.GetItemCount() == 0){
	//	RECT clientRect;
	//	GetClientRect(&clientRect);
	//	//dc.TextOutW(clientRect.right / 2 - 200, clientRect.bottom / 2 - 5, LoadLocalString(IDS_FIRST_STARTUP_TIP), 55);
	//	dc->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
	//	dc->DrawTextW(GetString(IDS_FIRST_STARTUP_TIP).c_str(), &clientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	//	//CBrush tst = CBrush(RGB(255, 0, 0));
	//	//dc->FillRect(&clientRect, &tst);
	//}
	
	//EndPaint(hWnd, &ps);
	//return DefWindowProc(hWnd, message, wParam, lParam);
	
}


void CLauncherWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu){
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	this->UpdateMenu(pPopupMenu);
}


BOOL CLauncherWnd::OnCommand(WPARAM wParam, LPARAM lParam){
	// Parse the menu selections:
	try{
		switch(LOWORD(wParam)){
			case IDM_FILE_PREF:
				break;
			case IDM_REFRESH:
				this->UpdateListView(true);
				Launcher->RebuildIconCache();
				break;
			case IDM_SAVE:
				Launcher->Save();
				break;
			case IDM_CATEGORY_ADDCATEGORY:
				Launcher->NewCategory();
				break;
			case IDM_CATEGORY_RENAMECURRENTCATEGORY:
				Launcher->GetCurrentCategory()->Rename();
				break;
			case IDM_CATEGORY_REMOVECURRENTCATEGORY:
				Launcher->GetCurrentCategory()->Remove();
				break;
			case IDM_POPUPCATEGORY_REMOVECATEGORY:
				Launcher->GetCategory(m_clickedTab)->Remove();
				break;
			case IDM_POPUPCATEGORY_RENAME:
				Launcher->GetCategory(m_clickedTab)->Rename();
				break;
			case IDM_POPUPCATEGORY_MOVELEFT:
				Launcher->GetCategory(m_clickedTab)->MoveTab(-1, true);
				break;
			case IDM_POPUPCATEGORY_MOVERIGHT:
				Launcher->GetCategory(m_clickedTab)->MoveTab(1, true);
				break;
			case IDM_CATEGORY_MOVELEFT:
				Launcher->GetCurrentCategory()->MoveTab(-1, true);
				break;
			case IDM_CATEGORY_MOVERIGHT:
				Launcher->GetCurrentCategory()->MoveTab(1, true);
				break;
			case IDM_BUTTON_ADDNEWBUTTON:
				Launcher->NewItem();
				break;
			case IDM_VIEW_LARGEICONS:
				this->SwitchListView(LVS_ICON);
				break;
			case IDM_VIEW_SMALLICONS:
				this->SwitchListView(LVS_SMALLICON);
				break;
			case IDM_VIEW_LIST:
				this->SwitchListView(LVS_LIST);
				break;
			case IDM_VIEW_DETAILS:
				this->SwitchListView(LVS_REPORT);
				break;
			case IDM_VIEW_RESETCOLUMNS:
				this->ResetListViewColumns();
				break;
			case IDM_EXIT:
				break;
			case IDM_BUTTON_LAUNCH:
				Launcher->GetCurrentCategory()->GetSelectedItem()->Launch();
				break;
			case IDM_BUTTON_EDIT:
				Launcher->GetCurrentCategory()->GetSelectedItem()->Edit();
				break;
			case IDM_BUTTON_REMOVE:
				Launcher->GetCurrentCategory()->GetSelectedItem()->Remove();
				break;
			case IDM_BUTTON_MOVELEFT:
				Launcher->GetCurrentCategory()->GetSelectedItem()->Move(-1, true);
				break;
			case IDM_BUTTON_MOVERIGHT:
				Launcher->GetCurrentCategory()->GetSelectedItem()->Move(1, true);
				break;
		}
	} catch(...){
		MessageBoxW(L"Error when executing command", L"Error", MB_ICONERROR);
	}
	return CFrameWnd::OnCommand(wParam, lParam);
}



void CLauncherWnd::OnFilePref(){
	CPreferencesDlg dlg;
	dlg.DoModal();
}
void CLauncherWnd::OnRefresh(){}
void CLauncherWnd::OnSave(){}
void CLauncherWnd::OnAddCategory(){
	//Launcher->NewCategory();
}
void CLauncherWnd::OnRemoveCurrentCategory(){
	//Launcher->GetCurrentCategory()->Remove();
}
void CLauncherWnd::OnRenameCurrentCategory(){
	//Launcher->GetCurrentCategory()->Rename();
}
void CLauncherWnd::OnCategoryMoveLeft(){
	//Launcher->GetCategory(m_clickedTab)->MoveTab(-1, true);
}
void CLauncherWnd::OnCategoryMoveRight(){
	//Launcher->GetCategory(m_clickedTab)->MoveTab(1, true);
}
void CLauncherWnd::OnCurCategoryMoveLeft(){
	//Launcher->GetCurrentCategory()->MoveTab(-1, true);
}
void CLauncherWnd::OnCurCategoryMoveRight(){
	//Launcher->GetCurrentCategory()->MoveTab(1, true);
}
void CLauncherWnd::OnExit(){
	Launcher->ExitInstance();
}
void CLauncherWnd::OnAddNewButton(){
	//Launcher->NewItem();
}
void CLauncherWnd::OnButtonEdit(){
	//Launcher->GetCurrentCategory()->GetSelectedItem()->Edit();
}
void CLauncherWnd::OnButtonLaunch(){
	//Launcher->GetCurrentCategory()->GetSelectedItem()->Launch();
}
void CLauncherWnd::OnButtonRemove(){
	//Launcher->GetCurrentCategory()->GetSelectedItem()->Remove();
}
void CLauncherWnd::OnButtonMoveLeft(){
	//Launcher->GetCurrentCategory()->GetSelectedItem()->Move(-1, true);
}
void CLauncherWnd::OnButtonMoveRight(){
	//Launcher->GetCurrentCategory()->GetSelectedItem()->Move(1, true);
}
void CLauncherWnd::OnRemoveCategory(){
	//Launcher->GetCategory(m_clickedTab)->Remove();
}
void CLauncherWnd::OnRenameCategory(){
	//Launcher->GetCategory(m_clickedTab)->Rename();
}
void CLauncherWnd::OnViewDetails(){
	//this->SwitchListView(LVS_REPORT);
}
void CLauncherWnd::OnViewLargeicons(){
	//this->SwitchListView(LVS_ICON);
}
void CLauncherWnd::OnViewList(){
	//this->SwitchListView(LVS_LIST);
}
void CLauncherWnd::OnViewSmallicons(){
	//this->SwitchListView(LVS_SMALLICON);
}
void CLauncherWnd::OnResetColumns(){
	// nothing
}
