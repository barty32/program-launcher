
#include "pch.h"
#include "Program Launcher.h"


#ifndef COLOR_HOTLIGHT
#define COLOR_HOTLIGHT   26
#endif

#define INDICATOR_WIDTH  2
#define INDICATOR_COLOR  COLOR_HOTLIGHT
#define METHOD           DSTINVERT

BEGIN_MESSAGE_MAP(TabControl, CTabCtrl)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CAPTURECHANGED()
//    ON_WM_PAINT()
ON_WM_KEYDOWN()
END_MESSAGE_MAP()


TabControl::TabControl(){}

TabControl::~TabControl(){
    if(m_pSpinCtrl){
        m_pSpinCtrl->Detach();
        delete m_pSpinCtrl;
    }
}


void TabControl::OnLButtonDown(UINT nFlags, CPoint point){
    //int test = GetSystemMetrics(SM_CYDRAG);
    if(DragDetect(point)){
        m_bDragging = true;

        m_bHotTracking = true;
        ModifyStyle(TCS_HOTTRACK, 0);

        // Find and remember the source tab (the one we're going to move/drag 'n drop)
        TCHITTESTINFO hitinfo;
        hitinfo.pt = point;
        m_nSrcTab = HitTest(&hitinfo);

        m_nDstTab = m_nSrcTab;

        // Reset insert indicator
        m_InsertPosRect.SetRect(0, 0, 0, 0);

        DrawIndicator(point);

        SetCapture();
    } else{
        CTabCtrl::OnLButtonDown(nFlags, point);
    }
    // Note: We're not calling the base classes CTabCtrl::OnLButtonDown 
    //       everytime, because we want to be able to drag a tab without
    //       actually select it first (so that it gets the focus).
}

//
//
// @brief Handler that is called when the left mouse button is released.
//        Is used to stop the drag 'n drop process, releases the mouse 
//        capture and reorders the tabs accordingly to insertion (drop) 
//        position.
void TabControl::OnLButtonUp(UINT nFlags, CPoint point){

    CTabCtrl::OnLButtonUp(nFlags, point);

    if(m_bDragging){
      // We're going to drop something now...

      // Stop the dragging process and release the mouse capture
      // This will eventually call our OnCaptureChanged which stops the draggin
        if(GetCapture() == this)
            ReleaseCapture();

        // Modify the tab control style so that Hot Tracking is re-enabled
        if(m_bHotTracking){
            m_bHotTracking = false;
            ModifyStyle(0, TCS_HOTTRACK);
        }

        // Reorder the tabs
        ReorderTab(m_nSrcTab, m_nDstTab);

    }
}

//
// 'TabControl::OnMouseMove'
//
// @mfunc Handler that is called when the mouse is moved. This is used
//        to, when in a drag 'n drop process, to:
//
//        1) Draw the drop indicator of where the tab can be inserted.
//        2) Possible scroll the tab so more tabs is viewed.
//
void TabControl::OnMouseMove(UINT nFlags, CPoint point){
    CTabCtrl::OnMouseMove(nFlags, point);

    // This code added to do extra check - shouldn't be strictly necessary!
    if(!(nFlags & MK_LBUTTON))
        m_bDragging = false;

    if(m_bDragging){
        // Draw the indicator
        DrawIndicator(point);

        // Get the up-down (spin) control that is associated with the tab control
        // and which contains scroll position information.
        if(!m_pSpinCtrl){
            CWnd * pWnd = FindWindowEx(GetSafeHwnd(), 0, _T("msctls_updown32"), 0);
            if(pWnd){
              // DevNote: It may be somewhat of an overkill to use the MFC version
              //          of the CSpinButtonCtrl since were actually only using it
              //          for retrieving the current scroll position (GetPos). A simple
              //          HWND could have been enough.
                m_pSpinCtrl = new CSpinButtonCtrl;
                m_pSpinCtrl->Attach(pWnd->GetSafeHwnd());
            }

      //  
      //      CWnd *pWnd = GetWindow(GW_CHILD);
      //      while( pWnd )
      //      {
      //        // This code it some extra check for whether we actually have found 
      //        // a up-down (spin) control, by comparing for correct class name.
      //        TCHAR achClassName[80];
      //        GetClassName(pWnd->GetSafeHwnd(),achClassName,sizeof(achClassName)/sizeof(TCHAR));
      //        
      //        if( !strcmp(achClassName,EBC_TEXT("msctls_updown32")) )
      //        {
      //          // DevNote: It may be somewhat of an overkill to use the MFC version
      //          //          of the CSpinButtonCtrl since were actually only using it
      //          //          for retrieving the current scroll position (GetPos). A simple
      //          //          HWND could have been enough.
      //          
      //          m_pSpinCtrl = new CSpinButtonCtrl;
      //          m_pSpinCtrl->Attach(pWnd->GetSafeHwnd());
      //          pWnd=0; // This will stop while-loop
      //        }
      //        else
      //        {
      //          pWnd = pWnd->GetWindow(GW_HWNDNEXT);
      //        }
      //      }
        }

        CRect rect;

        GetClientRect(&rect);

        // Examine whether we should scroll left...
        if(point.x < rect.left && m_pSpinCtrl){
            int nPos = LOWORD(m_pSpinCtrl->GetPos());
            if(nPos > 0){
                InvalidateRect(&m_InsertPosRect, false);
                ZeroMemory(&m_InsertPosRect, sizeof(m_InsertPosRect));

                SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPos - 1), 0);
            }
        }

        // Examine whether we should scroll right...
        if(point.x > rect.right && m_pSpinCtrl && m_pSpinCtrl->IsWindowVisible()){
            InvalidateRect(&m_InsertPosRect, false);
            ZeroMemory(&m_InsertPosRect, sizeof(m_InsertPosRect));

            int nPos = LOWORD(m_pSpinCtrl->GetPos());
            SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, nPos + 1), 0);
        }

    }

}

//
// 'TabControl::OnCaptureChanged'
//
// @mfunc Handler that is called when the WM_CAPTURECHANGED message is received. It notifies
//        us that we do not longer capture the mouse. Therefore we must stop or drag 'n drop
//        process. Clean up code etc.
//
void TabControl::OnCaptureChanged(CWnd*){
    if(m_bDragging){
        m_bDragging = false;

        // Remove the indicator by invalidate the rectangle (forces repaint)
        InvalidateRect(&m_InsertPosRect); //,false);

        // If a drag image is in play this probably should be cleaned up here.
        // ...
    }
}


void TabControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){

    if(m_bDragging && nChar == VK_ESCAPE){
        m_bDragging = false;
    }

    CTabCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

//
// 'TabControl::DrawIndicator'
//
// @mfunc Utility member function to draw the (drop) indicator of where the 
//        tab will be inserted.
// @param Specifies a position (e.g. the mouse pointer position) which
//       will be used to determine whether the indicator should be
//       painted to the left or right of the indicator.
bool TabControl::DrawIndicator(CPoint point){

    //m_bRedirect = !m_bRedirect;

    TCHITTESTINFO hitinfo;
    hitinfo.pt = point;

    CRect rect;
    // Adjust position to top of tab control (allow the mouse the actually
    // be outside the top of tab control and still be able to find the right
    // tab index
    if(GetItemRect(0, &rect)){
        hitinfo.pt.y = rect.top;
    }

    // If the position is inside the rectangle where tabs are visible we
    // can safely draw the insert indicator...
    unsigned int nTab = HitTest(&hitinfo);

    if(hitinfo.flags != TCHT_NOWHERE) //&& nTab != m_nSrcTab )
    {
        m_nDstTab = nTab;
    } else{
        if(m_nDstTab == GetItemCount())
            m_nDstTab--;
    }

    GetItemRect(m_nDstTab, &rect);

    //m_bRedirect = !m_bRedirect;

    CRect newInsertPosRect(rect.left - 1, rect.top, rect.left - 1 + INDICATOR_WIDTH, rect.bottom);

    // Determine whether the indicator should be painted at the right of
    // the tab - in which case we update the indicator position and the 
    // destination tab ...
    if(point.x >= rect.right - rect.Width() / 2) //&& point.x < rect.right )
    {
        newInsertPosRect.MoveToX(rect.right - 1);
        //m_nDstTab++; // = nTab+1;
    }


    if(newInsertPosRect != m_InsertPosRect){
      // Remove the current indicator by invalidate the rectangle (forces repaint)
        InvalidateRect(&m_InsertPosRect); //,false);

        // Update to new insert indicator position...
        m_InsertPosRect = newInsertPosRect;
    }

    // Create a simple device context in which we initialize the pen and brush
    // that we will use for drawing the new indicator...
    CClientDC dc(this);

    CBrush brush(GetSysColor(INDICATOR_COLOR));
    CPen   pen(PS_SOLID, 1, GetSysColor(INDICATOR_COLOR));

    dc.SelectObject(&brush);
    dc.SelectObject(&pen);

    // Draw the insert indicator
    dc.Rectangle(m_InsertPosRect);

    return true; // success
}

//
// 'TabControl::ReorderTab'
//
// @mfunc Reorders the tab by moving the source tab to the position of the
//        destination tab.
//
// @devnote Currently the tab is fully reconstructed by first removing all
//          tabs and then rebuilded. We could be optimized by only moving
//          the necessary tabs.
//
bool TabControl::ReorderTab(UINT nSrcTab, UINT nDstTab){
    if(nSrcTab == nDstTab)
        return true; // Return success (we didn't need to do anything
    if(nSrcTab >= (UINT)this->GetItemCount() || nDstTab >= (UINT)this->GetItemCount()){
        return false;
    }

    bool bOK = true;

    // Remember the current selected tab
    UINT nSelectedTab = this->GetCurSel();

    // Get information from the tab to move (to be deleted)
    TCHAR sBuffer[50];
    TCITEM item;

    item.mask = TCIF_IMAGE | TCIF_PARAM | TCIF_TEXT; //| TCIF_STATE;
    item.pszText = sBuffer;
    item.cchTextMax = sizeof(sBuffer) / sizeof(TCHAR);

    bOK = GetItem(nSrcTab, &item);
    ASSERT(bOK);

    bOK = DeleteItem(nSrcTab);
    ASSERT(bOK);

    // Insert it at new location
    bOK = InsertItem(nDstTab, &item);
    //assert(bOK);

    // Setup new selected tab
    if(nSelectedTab == nSrcTab)
        SetCurSel(nDstTab);
    else{
        if(nSelectedTab > nSrcTab && nSelectedTab < nDstTab)
            SetCurSel(nSelectedTab - 1);

        if(nSelectedTab < nSrcTab && nSelectedTab > nDstTab)
            SetCurSel(nSelectedTab + 1);
    }

    // Force update of tab control
    // Necessary to do so that notified clients ('users') - by selection change call
    // below - can draw the tab contents in correct tab.
    this->UpdateWindow();

    NMHDR nmh;
    nmh.hwndFrom = GetSafeHwnd();
    nmh.idFrom = GetDlgCtrlID();
    nmh.code = TCN_SELCHANGE;

    // Send parent TCN_SELCHANGE
    this->GetParent()->SendMessageW(WM_NOTIFY, nmh.idFrom, (LPARAM)&nmh);

    NMTCORDERCHANGE norder;
    norder.hdr.hwndFrom = nmh.hwndFrom;
    norder.hdr.idFrom = nmh.idFrom;
    norder.hdr.code = TCN_TABORDERCHANGED;
    norder.nSrcTab = m_nSrcTab;
    norder.nDstTab = m_nDstTab;
    this->GetParent()->SendMessageW(WM_NOTIFY, nmh.idFrom, (LPARAM)&norder);

    return bOK;
}