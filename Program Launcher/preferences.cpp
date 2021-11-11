
#include "framework.h"
#include "Program Launcher.h"

INT_PTR CALLBACK PrefDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam){
	return 0;/*
	static DWORD dwCurrentIconSpacing;
	static INT nCurrentIconSize;
	static INT nCurrentAppNamesState;
	switch(Message){
		case WM_INITDIALOG:
		{
			//center the dialog
			CenterDialog(hwndMain, hDlg);

			//SetWindowContextHelpId(GetDlgItem(hDlg,IDC_PREFDLG_APPNAMES_DONTSHOW, DWORD);

			//set up spin controls
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZE_SPIN), UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZEEDIT), (LPARAM)0);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACING_SPIN), UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGEDIT), (LPARAM)0);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACING_SPIN), UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGEDIT), (LPARAM)0);

			//set ranges to spin controls
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZE_SPIN), UDM_SETRANGE, (WPARAM)0, (LPARAM)MAKELPARAM(MAX_ICON_SIZE, MIN_ICON_SIZE));
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACING_SPIN), UDM_SETRANGE, (WPARAM)0, (LPARAM)MAKELPARAM(MAX_ICON_SPACING, MIN_ICON_SPACING));
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACING_SPIN), UDM_SETRANGE, (WPARAM)0, (LPARAM)MAKELPARAM(MAX_ICON_SPACING, MIN_ICON_SPACING));
			
			TCHAR szSliderPosition[INT_LEN_IN_DIGITS];
			
			//set ranges to slider cotrols
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETRANGEMIN, (WPARAM)TRUE, (LPARAM)MIN_ICON_SIZE);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETRANGEMAX, (WPARAM)TRUE, (LPARAM)MAX_ICON_SIZE);

			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), TBM_SETRANGEMIN, (WPARAM)TRUE, (LPARAM)MIN_ICON_SPACING);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), TBM_SETRANGEMAX, (WPARAM)TRUE, (LPARAM)MAX_ICON_SPACING);

			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), TBM_SETRANGEMIN, (WPARAM)TRUE, (LPARAM)MIN_ICON_SPACING);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), TBM_SETRANGEMAX, (WPARAM)TRUE, (LPARAM)MAX_ICON_SPACING);

			//set marks to size slider
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETTIC, (WPARAM)0, (LPARAM)16);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETTIC, (WPARAM)0, (LPARAM)32);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETTIC, (WPARAM)0, (LPARAM)48);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETTIC, (WPARAM)0, (LPARAM)64);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETTIC, (WPARAM)0, (LPARAM)128);
			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETTIC, (WPARAM)0, (LPARAM)256);

			//set the logical positions of sliders to actual values
			dwCurrentIconSpacing = ListView_GetItemSpacing(hwndMainListView, FALSE);
			nCurrentIconSize = GetSettingInt(TEXT("appereance"), TEXT("IconSize"), DEFAULT_ICON_SIZE);
			nCurrentAppNamesState = GetSettingInt(TEXT("general"), TEXT("ShowAppNames"), DEFAULT_SHOWAPPNAMES);

			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nCurrentIconSize);
			//set the value also to buddy box
			_itot_s(nCurrentIconSize, szSliderPosition, INT_LEN_IN_DIGITS, 10);
			Edit_SetText(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZEEDIT), szSliderPosition);

			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)LOWORD(dwCurrentIconSpacing));
			//set the value also to buddy box
			_itot_s(LOWORD(dwCurrentIconSpacing), szSliderPosition, INT_LEN_IN_DIGITS, 10);
			Edit_SetText(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGEDIT), szSliderPosition);

			SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)HIWORD(dwCurrentIconSpacing));
			//set the value also to buddy box
			_itot_s(HIWORD(dwCurrentIconSpacing), szSliderPosition, INT_LEN_IN_DIGITS, 10);
			Edit_SetText(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGEDIT), szSliderPosition);

			//close after launched checkbox
			BOOL bCloseAfterLaunch = GetSettingInt(TEXT("general"), TEXT("CloseAfterLaunch"), DEFAULT_CLOSEAFTERLAUNCH);
			Button_SetCheck(GetDlgItem(hDlg, IDC_PREFDLG_CLOSEAFTERLAUNCH), bCloseAfterLaunch);

			//icon caching checkbox
			BOOL bUseIconCaching = GetSettingInt(TEXT("general"), TEXT("UseIconCaching"), DEFAULT_USEICONCACHING);
			Button_SetCheck(GetDlgItem(hDlg, IDC_PREFDLG_ICONCACHING), bUseIconCaching);

			//show app names radio buttons
			switch(nCurrentAppNamesState){
				case 0:
					CheckRadioButton(hDlg, IDC_PREFDLG_APPNAMES_DONTSHOW, IDC_PREFDLG_APPNAMES_INTOOLTIP, IDC_PREFDLG_APPNAMES_DONTSHOW);
					break;
				case 1:
					//CheckRadioButton(hDlg, IDC_PREFDLG_APPNAMES_DONTSHOW, IDC_PREFDLG_APPNAMES_INTOOLTIP, IDC_PREFDLG_APPNAMES_INTOOLTIP);
					break;
				default:
					CheckRadioButton(hDlg, IDC_PREFDLG_APPNAMES_DONTSHOW, IDC_PREFDLG_APPNAMES_INTOOLTIP, IDC_PREFDLG_APPNAMES_UNDERICON);
					break;
			}


			return (INT_PTR)TRUE;
		}
		case WM_HSCROLL:
		{
			if((HWND)lParam == GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER)){
				if(LOWORD(wParam) == SB_ENDSCROLL){
					INT nValue = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZEEDIT), SE_SLIDER, MAX_ICON_SIZE, NULL);
					UpdateImageLists(hwndMainListView, nValue);
					SwitchListView(hwndMainListView, LVS_SMALLICON);
					SwitchListView(hwndMainListView, LVS_ICON);
				}
				else{
					ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZEEDIT), SE_SLIDER, MAX_ICON_SIZE, NULL);
				}
			}
			else if((HWND)lParam == GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER)){
				INT nIconSpacing = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGEDIT), SE_SLIDER, MAX_ICON_SPACING, NULL);
				ListView_SetIconSpacing(hwndMainListView, nIconSpacing, HIWORD(ListView_GetItemSpacing(hwndMainListView, FALSE)));
			}
			else if((HWND)lParam == GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER)){
				INT nIconSpacing = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGEDIT), SE_SLIDER, MAX_ICON_SPACING, NULL);
				ListView_SetIconSpacing(hwndMainListView, LOWORD(ListView_GetItemSpacing(hwndMainListView, FALSE)), nIconSpacing);
			}
		}
		break;
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			int wmNotify = HIWORD(wParam);
			switch(wmId){
				case IDC_PREFDLG_ICONSIZEEDIT:
				{
					if(wmNotify == EN_CHANGE && bAllowEnChange){
						INT nValue = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZEEDIT), SE_EDIT, MAX_ICON_SIZE, NULL);
						UpdateImageLists(hwndMainListView, nValue);
						SwitchListView(hwndMainListView, LVS_SMALLICON);
						SwitchListView(hwndMainListView, LVS_ICON);
					}
					else if(wmNotify == EN_SETFOCUS){
						SendMessage(hDlg, DM_SETDEFID, (WPARAM)IDC_STATIC, (LPARAM)0);
					}
				}
				case IDC_PREFDLG_HICONSPACINGEDIT:
				{
					if(wmNotify == EN_CHANGE && bAllowEnChange){
						INT nIconSpacing = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGEDIT), SE_EDIT, MAX_ICON_SPACING, NULL);
						ListView_SetIconSpacing(hwndMainListView, nIconSpacing, HIWORD(ListView_GetItemSpacing(hwndMainListView, FALSE)));
					}
					if(wmNotify == EN_SETFOCUS){
						SendMessage(hDlg, DM_SETDEFID, (WPARAM)IDC_STATIC, (LPARAM)0);
					}
				}
				case IDC_PREFDLG_VICONSPACINGEDIT:
				{
					if(wmNotify == EN_CHANGE && bAllowEnChange){
						INT nIconSpacing = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGEDIT), SE_EDIT, MAX_ICON_SPACING, NULL);
						ListView_SetIconSpacing(hwndMainListView, LOWORD(ListView_GetItemSpacing(hwndMainListView, FALSE)), nIconSpacing);
					}
					if(wmNotify == EN_SETFOCUS){
						SendMessage(hDlg, DM_SETDEFID, (WPARAM)IDC_STATIC, (LPARAM)0);
					}
				}
				break;
				case IDC_PREFDLG_RESET:

					SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)DEFAULT_ICON_SIZE);
					SendMessage(hDlg, WM_HSCROLL, (WPARAM)0, (LPARAM)GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER));

					SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)DEFAULT_HORZ_SPACING);
					SendMessage(hDlg, WM_HSCROLL, (WPARAM)0, (LPARAM)GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER));

					SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)DEFAULT_VERT_SPACING);
					SendMessage(hDlg, WM_HSCROLL, (WPARAM)0, (LPARAM)GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER));

					Button_SetCheck(GetDlgItem(hDlg, IDC_PREFDLG_CLOSEAFTERLAUNCH), DEFAULT_CLOSEAFTERLAUNCH);
					CheckRadioButton(hDlg, IDC_PREFDLG_APPNAMES_DONTSHOW, IDC_PREFDLG_APPNAMES_INTOOLTIP, IDC_PREFDLG_APPNAMES_UNDERICON);
					
				break;
				case IDC_PREFDLG_APPNAMES_DONTSHOW:
					if(wmNotify == BN_CLICKED){
						SetSettingInt(TEXT("general"), TEXT("ShowAppNames"), 0);
						int nItems = ListView_GetItemCount(hwndMainListView);
						for(int i = 0; i < nItems; i++){
							ListView_SetItemText(hwndMainListView, i, 0, NULL);
						}
					}
				break;
				case IDC_PREFDLG_APPNAMES_INTOOLTIP:
					if(wmNotify == BN_CLICKED){
						SetSettingInt(TEXT("general"), TEXT("ShowAppNames"), 1);
						int nItems = ListView_GetItemCount(hwndMainListView);
						for(int i = 0; i < nItems; i++){
							ListView_SetItemText(hwndMainListView, i, 0, NULL);
						}
					}
				break;
				case IDC_PREFDLG_APPNAMES_UNDERICON:
					if(wmNotify == BN_CLICKED){
						SetSettingInt(TEXT("general"), TEXT("ShowAppNames"), 2);
						LAUNCHERENTRY buttonProps;
						int nItems = ListView_GetItemCount(hwndMainListView);
						int nCategory = TabCtrl_GetCurSel(hwndTab);
						for(int i = 0; i < nItems; i++){
							GetSettingButtonStruct(arrszCategoryList[nCategory], i, &buttonProps, TRUE);
							ListView_SetItemText(hwndMainListView, i, 0, buttonProps.szName);
						}
					}
				break;
				case IDOK:
				{
					//save the set spacings
					INT nIconSize = SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
					INT nHorzSpacing = SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), TBM_GETPOS, (WPARAM)0, (LPARAM)0);
					INT nVertSpacing = SendMessage(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), TBM_GETPOS, (WPARAM)0, (LPARAM)0);

					SetSettingInt(TEXT("appereance"), TEXT("IconSize"), nIconSize);
					SetSettingInt(TEXT("appereance"), TEXT("IconSpacingHorizontal"), nHorzSpacing);
					SetSettingInt(TEXT("appereance"), TEXT("IconSpacingVertical"), nVertSpacing);

					if(Button_GetCheck(GetDlgItem(hDlg, IDC_PREFDLG_APPNAMES_DONTSHOW))){
						SetSettingInt(TEXT("general"), TEXT("ShowAppNames"), 0);
					}
					else if(Button_GetCheck(GetDlgItem(hDlg, IDC_PREFDLG_APPNAMES_INTOOLTIP))){
						//SetSettingInt(TEXT("general"), TEXT("ShowAppNames"), 1);
					}
					else{
						SetSettingInt(TEXT("general"), TEXT("ShowAppNames"), 2);
					}

					//save the value of the checkboxes
					BOOL bCloseAfterLaunch = Button_GetCheck(GetDlgItem(hDlg, IDC_PREFDLG_CLOSEAFTERLAUNCH));
					SetSettingInt(TEXT("general"), TEXT("CloseAfterLaunch"), bCloseAfterLaunch);
					BOOL bUseIconCaching = Button_GetCheck(GetDlgItem(hDlg, IDC_PREFDLG_ICONCACHING));
					SetSettingInt(TEXT("general"), TEXT("UseIconCaching"), bUseIconCaching);

					//force list refresh
					SwitchListView(hwndMainListView, LVS_SMALLICON);
					SwitchListView(hwndMainListView, LVS_ICON);
					//destroy the dialog
					EndDialog(hDlg, IDOK);
				}
				break;
				case IDCANCEL:

					SetSettingInt(TEXT("general"), TEXT("ShowAppNames"), nCurrentAppNamesState);

					UpdateImageLists(hwndMainListView, nCurrentIconSize);
					//set icon spacing to previous values
					ListView_SetIconSpacing(hwndMainListView, LOWORD(dwCurrentIconSpacing), HIWORD(dwCurrentIconSpacing));
					//force list refresh
					SwitchListView(hwndMainListView, LVS_SMALLICON);
					SwitchListView(hwndMainListView, LVS_ICON);

					//destroy the dialog
					EndDialog(hDlg, IDCANCEL);
					break;
			}
		}
		break;
		case WM_NOTIFY:
			if(((LPNMUPDOWN)lParam)->hdr.code == UDN_DELTAPOS){
				if(((LPNMUPDOWN)lParam)->hdr.hwndFrom == GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZE_SPIN)){
					INT nValue = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZESLIDER), GetDlgItem(hDlg, IDC_PREFDLG_ICONSIZEEDIT), SE_SPIN, MAX_ICON_SIZE, (LPNMUPDOWN)lParam);
					UpdateImageLists(hwndMainListView, nValue);
					SwitchListView(hwndMainListView, LVS_SMALLICON);
					SwitchListView(hwndMainListView, LVS_ICON);
				}
				else if(((LPNMUPDOWN)lParam)->hdr.hwndFrom == GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACING_SPIN)){
					INT nIconSpacing = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGSLIDER), GetDlgItem(hDlg, IDC_PREFDLG_HICONSPACINGEDIT), SE_SPIN, MAX_ICON_SPACING, (LPNMUPDOWN)lParam);
					ListView_SetIconSpacing(hwndMainListView, nIconSpacing, HIWORD(ListView_GetItemSpacing(hwndMainListView, FALSE)));
				}
				else if(((LPNMUPDOWN)lParam)->hdr.hwndFrom == GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACING_SPIN)){
					INT nIconSpacing = ChangeSliderValue(GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGSLIDER), GetDlgItem(hDlg, IDC_PREFDLG_VICONSPACINGEDIT), SE_SPIN, MAX_ICON_SPACING, (LPNMUPDOWN)lParam);
					ListView_SetIconSpacing(hwndMainListView, LOWORD(ListView_GetItemSpacing(hwndMainListView, FALSE)), nIconSpacing);
				}
			}
			break;
		default:
			return FALSE;
	}
	return (INT_PTR)TRUE;*/
}


INT ChangeSliderValue(HWND hwndSlider, HWND hwndEditBox, INT GetValueFrom, INT nMaxValue, LPNMUPDOWN lpstSpinValue){

	INT nValue = 0;
	TCHAR szValue[INT_LEN_IN_DIGITS];
	bAllowEnChange = FALSE;

	switch(GetValueFrom){
		case SE_SLIDER:
			nValue = SendMessage(hwndSlider, TBM_GETPOS, (WPARAM)0, (LPARAM)0);
			break;
		case SE_EDIT:
			Edit_GetText(hwndEditBox, szValue, INT_LEN_IN_DIGITS);
			nValue = _ttoi(szValue);
			if(nValue > nMaxValue){
				nValue = nMaxValue;
				_itot_s(nValue, szValue, INT_LEN_IN_DIGITS, 10);
				Edit_SetText(hwndEditBox, szValue);
			}
			break;
		case SE_SPIN:
			nValue = lpstSpinValue->iPos + lpstSpinValue->iDelta;
			break;
	}

	if(GetValueFrom != SE_SLIDER){
		//set value to slider
		SendMessage(hwndSlider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)nValue);
	}

	if(GetValueFrom != SE_EDIT && GetValueFrom != SE_SPIN){
		//set value to edit box
		_itot_s(nValue, szValue, INT_LEN_IN_DIGITS, 10);
		Edit_SetText(hwndEditBox, szValue);
	}
	bAllowEnChange = TRUE;
	return nValue;
}