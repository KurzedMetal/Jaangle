
#include "stdafx.h"

#ifdef _MFC_VER
	#pragma message("    compiling for MFC")
	#define CPPString	CString	//MFC program
#else
	#pragma message("    compiling for Win32")
	#include <windows.h>
	#include <stdio.h>
	#include <crtdbg.h>
	#include <tchar.h>
	#include "StdString.h"
	#ifdef _UNICODE
	#define CPPString	CStdStringW	//non-MFC program UNICODE
	#else
	#define CPPString	CStdStringA	//non-MFC program ANSI
	#endif
#endif

#include "PPMessageBox.h"
#include "PPHtmlDrawer.h"

//SetWindowLongPtr is available on _WIN64 platform
#ifndef SetWindowLongPtr
#define SetWindowLongPtr SetWindowLong
#endif

#pragma warning(disable : 4127)		// conditional expression is constant

//////////////////////////////////////////////////////////////
//
//
#define PPMSGBOX_DO_NOT_SAVE_CHECKBOX

//////////////////////////////////////////////////////////////
//Uncomments a following line if you want store "DoNotAsk" value to INI file
//
//#define PPMSGBOX_USE_PROFILE_FILE

//The sizes
#define PPMSGBOX_SIZE_SEPX		0
#define PPMSGBOX_SIZE_MSGX		1
#define PPMSGBOX_SIZE_MSGY		2
#define PPMSGBOX_SIZE_BTNX		3
#define PPMSGBOX_SIZE_BTNY		4
#define PPMSGBOX_SIZE_CHKX		5
#define PPMSGBOX_SIZE_CHKY		6
#define PPMSGBOX_SIZE_CTRLX		7
#define PPMSGBOX_SIZE_CTRLY		8


#define PPMSGBOX_BUTTON_DISABLED	_T("%s = %d")
#define PPMSGBOX_INI_FILE			_T("PPMessageBox.ini")
#define PPMSGBOX_REGISTRY_KEY		_T("PPMessageBox")

#define UDM_HTMLDRAWER_REPAINT	   (WM_APP + 100)

class CPPMessageBoxTemplate
{
public:
	CPPMessageBoxTemplate(const PPMSGBOXPARAMS *pMsgBox); //Constructor of CPPMessageBoxTemplate
	virtual ~CPPMessageBoxTemplate();
	int Display();

protected:
#pragma pack(1)
	typedef struct _STRUCT_BUTTONS
	{
		_STRUCT_BUTTONS()
		{
			hWnd = NULL;
			nTypeBtn = 0;
		}
		int nTypeBtn;		//The Type of the Button
		HWND hWnd;		    //HWND of the button
	} STRUCT_BUTTONS;
#pragma pack()

	typedef std::vector<STRUCT_BUTTONS> vecButton;	//Buttons
	vecButton m_vecButtons;

	STRUCT_BUTTONS m_stCheckBtn;
	STRUCT_BUTTONS m_stCheckText;

	DWORD m_dwSizes [PPMSGBOX_SIZE_CTRLY + 1]; //The sizes

	HICON m_hIcon;

	PPMSGBOXPARAMS m_MsgParam;
	UINT m_nDefBtn; //A default button index (0 - to max counts of the buttons)
	HFONT m_hFont;

	DWORD m_dwHeaderHeight;
	DWORD m_dwMessageHeight;
	DWORD m_dwControlHeight;
	DWORD m_dwMoreInfoHeight;

	BOOL m_bIsCancelExist;
	BOOL m_bMoreInfoOpened;

	DWORD m_dwCountdownTimer;

	CPPString m_sDisabledCaption;
	CPPString m_sHeaderSepText;
	CPPString m_sControlSepText;
	CPPString m_sMoreInfoSepText;
	CPPString m_sText;
	CPPString m_sCaption;
	CPPString m_sHeader;
	CPPString m_sMoreInfoOpened; //Text for the opened "MoreInfo" button
	CPPString m_sMoreInfoClosed; //Text for the closed "MoreInfo" button
	CPPString m_sTimeoutText; //Text for the timeout button or the timeout caption

	CPPHtmlDrawer m_pHeader;
	CPPHtmlDrawer m_pDrawer;
	CPPHtmlDrawer m_pMoreInfo;
	HBITMAP m_hBitmapBk;

	SIZE PrepareMessageBox(HWND hwnd);
	void RemoveAllButtons();
	CPPString GetString(LPCTSTR lpszText);
	CPPString GetLocalizedButtonText(DWORD dwBtnID, CPPString sDefText = _T(""));

	void OnInitDialog(HWND hwnd);
	void OnTimer(HWND hwnd);

	//Draw methods
	void DrawBox(HWND hwnd, HDC hDC);
	void DrawSeparator(HDC hDC, LPCRECT lpRect, int nTypeSeparator, LPCTSTR lpszText, int nTextAlign);

	static BOOL CALLBACK MsgBoxDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

int PPMessageBox(HWND hwnd,
				LPCTSTR lpszMessage,
				LPCTSTR lpszCaption /*= NULL*/,
				DWORD dwStyle /*= MB_OK | MB_ICONEXCLAMATION*/,
				const PPMSGBOXPARAMS * pMsgBox /*= NULL*/)
{
	PPMSGBOXPARAMS MsgBox;
	if (NULL != pMsgBox)
		MsgBox = *pMsgBox;

	MsgBox.lpszCaption = lpszCaption;
	MsgBox.lpszText = lpszMessage;
	MsgBox.dwStyle = dwStyle;
	MsgBox.hParentWnd = hwnd;

	return PPMessageBoxIndirect(&MsgBox);
} //End of PPMessageBox

int PPMessageBoxIndirect(const PPMSGBOXPARAMS * pMsgBox)
{
	_ASSERT(pMsgBox);
	_ASSERT((NULL != pMsgBox->lpszText) && (0 != pMsgBox->lpszText [0]));
	// cannot have both disabled and timeout seconds
//	_ASSERT((pMsgBox->nDisabledSeconds == 0) || (pMsgBox->nTimeoutSeconds == 0));

	PPMSGBOXPARAMS MsgBox;
	MsgBox = *pMsgBox;
	
	if (MsgBox.hParentWnd == NULL)
	{
		MsgBox.hParentWnd = ::GetActiveWindow();
		if (MsgBox.hParentWnd != NULL)
			MsgBox.hParentWnd = ::GetLastActivePopup(MsgBox.hParentWnd);
	} //if
	
	TCHAR tchCaption[MAX_PATH];
	if ((NULL == MsgBox.lpszCaption) || (0 == MsgBox.lpszCaption [0]))
	{
		if (NULL != MsgBox.hParentWnd)
		{
			::SendMessage(MsgBox.hParentWnd, WM_GETTEXT, MAX_PATH, (LPARAM)tchCaption);
			MsgBox.lpszCaption = tchCaption;
		} //if
		if ((NULL == MsgBox.lpszCaption) || (0 == MsgBox.lpszCaption [0]))
			MsgBox.lpszCaption = AfxGetAppName();//Alex 21-8-06
	} //if

#ifdef PPMSGBOX_DO_NOT_SAVE_CHECKBOX
	//ENG: Checks a states of the "DonNotAskAgain" checkboxes
	if ((MsgBox.dwStyle & MB_CHECKBOX))
	{
		if ((NULL != MsgBox.lpszModuleName) && (0 != MsgBox.lpszModuleName [0]))
		{
			//ENG: Default value of PPMessageBox return code (checkbox wasn't checked)
			DWORD dwData = 0;

			//ENG: Get full path of the current module
			TCHAR szModule[_MAX_PATH];
			::GetModuleFileName(NULL, szModule, _MAX_PATH);

			TCHAR szPath[MAX_PATH];
			LPTSTR lpFilePart;
			::GetFullPathName(szModule, _MAX_PATH, szPath, &lpFilePart);

			//ENG: Creates a key value
			CPPString sDoNotAskKey;
			sDoNotAskKey.Format(_T("%s%d"), MsgBox.lpszModuleName, MsgBox.nLine);
#ifdef PPMSGBOX_USE_PROFILE_FILE
			//ENG: An info will be search in the INI file
			//RUS: Οξθρκ θντξπμΰφθθ ασδες οπξθηβεδεν β INI τΰιλε
			
			//ENG: Removes a module name from path
			//RUS: Σδΰλεμ θμ μξδσλ θη οσςθ
			if (NULL != lpFilePart)
				*(lpFilePart) = _T('\0');
			
			//ENG: Add an INI filename to path
			//RUS: Δξαΰβλεμ θμ INI τΰιλΰ κ οσςθ
			_tcscat(szPath, PPMSGBOX_INI_FILE);
			
			const nHexStringLength = 16;
			TCHAR szBuf[nHexStringLength];
			
			//ENG: Get a return code of PPMessageBox as HEX string
			//RUS: Οξλσχΰεμ κξδ νΰζΰςξι κλΰβθψθ PPMessageBox β τξπμΰςε HEX ρςπξκθ
			if (::GetPrivateProfileString(_T("DoNotAsk"),	// section name
				sDoNotAskKey,	// key name
				_T(""),			// default string
				szBuf,			// destination buffer
				nHexStringLength,// size of destination buffer
				szPath))		// initialization file name
			{
				//ENG: Convert from HEX string to DWORD value
				//RUS: Οπεξαπΰηξβΰνθε HEX ρςπξκθ β DWORD χθρλξ
				dwData = _tcstoul(szBuf, NULL, 16);
			} //if
#else
			//ENG: An info will be search in the system registry
			CPPString sSection;
			if ((NULL == MsgBox.lpszCompanyName) || (0 == MsgBox.lpszCompanyName [0]))
				sSection.Format(_T("Software\\%s\\%s"), lpFilePart, PPMSGBOX_REGISTRY_KEY);
			else
				sSection.Format(_T("Software\\%s\\%s\\%s"), MsgBox.lpszCompanyName, lpFilePart, PPMSGBOX_REGISTRY_KEY);

			//ENG: Read a value from registry
			HKEY hKey = NULL;
			LONG lRet = ::RegOpenKeyEx(HKEY_CURRENT_USER, sSection, 0, KEY_READ, &hKey);
			if (lRet == ERROR_SUCCESS)
			{
				DWORD dwType = 0;
				DWORD dwSize = sizeof(DWORD);
				lRet = ::RegQueryValueEx(hKey, sDoNotAskKey, 0, &dwType, (LPBYTE)&dwData, &dwSize);
				::RegCloseKey(hKey);
				if (lRet != ERROR_SUCCESS)
					dwData = 0;
			} //if
#endif //PPMSGBOX_USE_PROFILE_FILE
			//ENG: If "DoNotAsk" flag was checked
			if (dwData) 
				return (int)dwData;
		} //if
	} //if
#endif //PPMSGBOX_DO_NOT_SAVE_CHECKBOX

	if ((MsgBox.dwStyle & MB_NOSOUND) == 0)
		::MessageBeep(MsgBox.dwStyle & MB_ICONMASK);
	
	CPPMessageBoxTemplate Dlg(&MsgBox);
	
	int nResult = Dlg.Display();
	
	return nResult;
} //End of PPMessageBoxIndirect

CPPMessageBoxTemplate::CPPMessageBoxTemplate(const PPMSGBOXPARAMS *pMsgBox)
{
	m_MsgParam = *pMsgBox;

	if (NULL == m_MsgParam.hInstanceStrings)
		m_MsgParam.hInstanceStrings = ::GetModuleHandle(NULL);

	if (NULL == m_MsgParam.hInstanceIcons)
		m_MsgParam.hInstanceIcons = ::GetModuleHandle(NULL);

	m_hBitmapBk = NULL;

	m_bMoreInfoOpened = FALSE;
	m_dwMoreInfoHeight = 0;

	m_sDisabledCaption.Empty();
	m_sTimeoutText.Empty();
	m_sMoreInfoClosed.Empty();
	m_sMoreInfoOpened.Empty();
	m_sHeader.Empty();
	m_sCaption = GetString(m_MsgParam.lpszCaption);
	m_sText.Empty();

	m_vecButtons.clear();
//	m_mapBtnText.clear();

	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(ncm));
	ncm.cbSize = sizeof(ncm);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
	m_hFont = ::CreateFontIndirect(&ncm.lfMessageFont);

	//Load a default icon
	m_hIcon = NULL;
	switch (m_MsgParam.dwStyle & MB_ICONMASK)
	{
	case MB_ICONERROR:
		m_hIcon = ::LoadIcon(NULL, IDI_ERROR);
		break;
	case MB_ICONQUESTION:
		m_hIcon = ::LoadIcon(NULL, IDI_QUESTION);
		break;
	case MB_ICONEXCLAMATION:
		m_hIcon = ::LoadIcon(NULL, IDI_EXCLAMATION);
		break;
	case MB_ICONASTERISK:
		m_hIcon = ::LoadIcon(NULL, IDI_ASTERISK);
		break;
	case MB_USERICON:
		if (NULL != m_MsgParam.lpszIcon)
		{
			m_hIcon = (HICON)::LoadImage(m_MsgParam.hInstanceIcons, m_MsgParam.lpszIcon, 
				IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		} //if
		break;
	} //switch
	
	//Setup a sizes
	m_dwSizes [PPMSGBOX_SIZE_SEPX] = 5;
	m_dwSizes [PPMSGBOX_SIZE_MSGX] = 14;
	m_dwSizes [PPMSGBOX_SIZE_MSGY] = 14;
//	m_dwSizes [PPMSGBOX_SIZE_BTNX] = 90;
//	m_dwSizes [PPMSGBOX_SIZE_BTNY] = 23;
	m_dwSizes [PPMSGBOX_SIZE_CHKX] = 10;
	m_dwSizes [PPMSGBOX_SIZE_CHKY] = 13;
	m_dwSizes [PPMSGBOX_SIZE_CTRLX] = 11;
	m_dwSizes [PPMSGBOX_SIZE_CTRLY] = 11;

/*	//Customizes a button's text
	SetButtonText(IDOK, _T("OK"), IDS_OK);
	SetButtonText(IDCANCEL, _T("Cancel"), IDS_CANCEL);
	SetButtonText(IDABORT, _T("Abort"), IDS_ABORT);
	SetButtonText(IDRETRY, _T("Retry"), IDS_RETRY);
	SetButtonText(IDIGNORE, _T("Ignore"), IDS_IGNORE);
	SetButtonText(IDYES, _T("Yes"), IDS_YES);
	SetButtonText(IDNO, _T("No"), IDS_NO);
	SetButtonText(IDCLOSE, _T("Close"), 0);
	SetButtonText(IDHELP, _T("Help"), IDS_HELP);
	SetButtonText(IDTRYAGAIN, _T("Try Again"), IDS_TRYAGAIN);
	SetButtonText(IDCONTINUE, _T("Continue"), IDS_CONTINUE);
	SetButtonText(IDSKIP, _T("Skip"), IDS_SKIP);
	SetButtonText(IDSKIPALL, _T("Skip All"), IDS_SKIPALL);
	SetButtonText(IDIGNOREALL, _T("Ignore All"), IDS_IGNORE);
	SetButtonText(IDYESTOALL, _T("Yes to All"), IDS_YESTOALL);
	SetButtonText(IDNOTOALL, _T("No to All"), IDS_NOTOALL);
	if (m_MsgParam.sCheckBoxText.IsEmpty())
		SetButtonText(MB_CHECKBOX, _T("Do not show again"), IDS_DONOTSHOWAGAIN);
	else
		SetButtonText(MB_CHECKBOX, m_MsgParam.sCheckBoxText, IDS_DONOTSHOWAGAIN);*/
} //End of CPPMessageBoxTemplate

CPPMessageBoxTemplate::~CPPMessageBoxTemplate()
{
	if (NULL != m_hFont)
		::DeleteObject(m_hFont);

	if (NULL != m_hBitmapBk)
		::DeleteObject(m_hBitmapBk);

	if (NULL != m_hIcon)
		::DeleteObject(m_hIcon);

	RemoveAllButtons();
} //End of ~CPPMessageBoxTemplate

int CPPMessageBoxTemplate::Display()
{
	LPDLGTEMPLATE pdlg;

	HGLOBAL hgbl = GlobalAlloc(GMEM_ZEROINIT, sizeof(DLGTEMPLATE)*2);
	_ASSERT(hgbl);
	if (NULL == hgbl)
		return IDCANCEL;

    pdlg = (LPDLGTEMPLATE)GlobalLock(hgbl);
	_ASSERT(pdlg);
	if (NULL == hgbl)
	{
		GlobalFree(hgbl);
		return IDCANCEL;
	} //if

    pdlg->x = 0;
    pdlg->y = 0;
    pdlg->cx = 100;
    pdlg->cy = 100;
    pdlg->style = DS_MODALFRAME|WS_VISIBLE|WS_POPUP|WS_BORDER|WS_CAPTION|WS_SYSMENU|DS_CENTER;
    pdlg->dwExtendedStyle = 0;
    pdlg->cdit = 0;

	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	int nResult = ::DialogBoxIndirectParam(hInstance, pdlg, m_MsgParam.hParentWnd, MsgBoxDlgProc, (LPARAM)this);

	GlobalUnlock(pdlg);
    GlobalFree(hgbl);

	return nResult;
} //End of PPMessageBox


//subclassing a checkbox
//LRESULT CALLBACK NewCheckBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam); // prototype for the combo box subclass proc
//WNDPROC lpfnOrgCheckboxProc;

///////////////////////////////////////////////////////////////////////////////
// MsgBoxDlgProc
BOOL CALLBACK CPPMessageBoxTemplate::MsgBoxDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CPPMessageBoxTemplate * pDlg = (CPPMessageBoxTemplate*)::GetWindowLong(hwnd, GWL_USERDATA);

	switch (message)
	{
	case WM_INITDIALOG:
		{
			::SetWindowLong(hwnd, GWL_USERDATA, lParam);	// save it for the others
			pDlg = (CPPMessageBoxTemplate*)lParam;
			_ASSERTE(pDlg);
			pDlg->OnInitDialog(hwnd);
			if (NULL != pDlg->m_stCheckBtn.hWnd)
			{
				//Subclassing a checkbox
//				lpfnOrgCheckboxProc = (WNDPROC)::SetWindowLongPtr(
//					pDlg->m_stCheckBtn.hWnd,
//					GWL_WNDPROC, 
//					(LONG)NewCheckBoxProc);
			} //if
			return FALSE;
		} //End of case WM_INITDIALOG
	case WM_DESTROY:
		{
//			if (NULL != pDlg->m_stCheckBtn.hWnd)
//			{
//				//Unsubclassing a checkbox
//				::SetWindowLongPtr(pDlg->m_stCheckBtn.hWnd,
//					GWL_WNDPROC, (LONG)lpfnOrgCheckboxProc);
//			} //if
			pDlg->RemoveAllButtons();
			return FALSE;
		} //End of case WM_DESTROY
	case WM_PAINT:
		{
			PAINTSTRUCT ps; 
			BeginPaint(hwnd, &ps);
			pDlg->DrawBox(hwnd, ps.hdc);
			EndPaint(hwnd, &ps);
			return FALSE;
		} //End of case WM_PAINT
	case WM_COMMAND:
		{
			//ENG: Get a clicked button ID
			DWORD dwBtnID = LOWORD(wParam);
			if(dwBtnID)
			{
				//ENG: Disable VK_ESCAPE if "Cancel" button is not present in messagebox
				if ((IDCANCEL == dwBtnID) && !pDlg->m_bIsCancelExist)
					return FALSE;

				switch (dwBtnID)
				{
				case IDHELP:
					{
						TCHAR szBuf[_MAX_PATH];
						memset(&szBuf, 0, sizeof(szBuf));
						::GetModuleFileName(NULL, szBuf, _MAX_PATH);
						if (_tcslen(szBuf) > 0)
						{
							//searching a last dot char '.'
							TCHAR *cp = _tcsrchr(szBuf, _T('.'));
							if (cp)
							{
								_tcscpy(cp, _T(".hlp"));
								//Call a context help
								::WinHelp(hwnd, szBuf,
									(pDlg->m_MsgParam.dwContextHelpID == 0) ? HELP_PARTIALKEY : HELP_CONTEXT,
									pDlg->m_MsgParam.dwContextHelpID);
							} //if
						} //if
						return FALSE;
					}
				case IDMOREINFO_CLOSED:
				case IDMOREINFO_OPENED:
					{
						RECT rcWindow;
						::GetWindowRect(hwnd, &rcWindow);
						DWORD dwHeight = rcWindow.bottom - rcWindow.top;
						for (DWORD i = 0; i < pDlg->m_vecButtons.size(); ++i)
						{
							STRUCT_BUTTONS & sbTemp = pDlg->m_vecButtons [i];
							if (IDMOREINFO_CLOSED == sbTemp.nTypeBtn)
							{
								if (pDlg->m_bMoreInfoOpened)
								{
									pDlg->m_bMoreInfoOpened = FALSE;
									dwHeight -= pDlg->m_dwMoreInfoHeight;
									::SendMessage(sbTemp.hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)pDlg->m_sMoreInfoClosed);
								}
								else
								{
									pDlg->m_bMoreInfoOpened = TRUE;
									dwHeight += pDlg->m_dwMoreInfoHeight;
									::SendMessage(sbTemp.hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)pDlg->m_sMoreInfoOpened);
								} //if
								::MoveWindow(hwnd, rcWindow.left, rcWindow.top, rcWindow.right - rcWindow.left, dwHeight, TRUE);
							} //if
						} //for
						return FALSE;
					}
				case IDREPORT:
					{
						if ((pDlg->m_MsgParam.dwReportMsgID) && (NULL != pDlg->m_MsgParam.hParentWnd))
							::SendMessage(pDlg->m_MsgParam.hParentWnd, pDlg->m_MsgParam.dwReportMsgID, 0, 0);
						return FALSE;
					}
				default:
					{
						//ENG: Stop of timer
						::KillTimer(hwnd, 1);

						//ENG: Handles a checkbox state
						if (NULL != pDlg->m_stCheckBtn.hWnd)
						{
							//ENG: Toggles a checkbox state
							LRESULT nCheck = ::SendMessage(pDlg->m_stCheckBtn.hWnd, BM_GETCHECK, 0, 0);
							if (BST_CHECKED == nCheck)
							{
								dwBtnID |= pDlg->m_MsgParam.dwStyle & MB_CHECKBOXCHECKED;
#ifdef PPMSGBOX_DO_NOT_SAVE_CHECKBOX
								if ((NULL != pDlg->m_MsgParam.lpszModuleName) && (0 != pDlg->m_MsgParam.lpszModuleName [0]))
								{
									//ENG: Get full path of the current module
									TCHAR szModule[_MAX_PATH];
									::GetModuleFileName(NULL, szModule, _MAX_PATH);
									
									TCHAR szPath[_MAX_PATH];
									LPTSTR lpFilePart;
									::GetFullPathName(szModule, _MAX_PATH, szPath, &lpFilePart);
									
									//ENG: Creates a key value
									CPPString sDoNotAskKey;
									sDoNotAskKey.Format(_T("%s%d"), pDlg->m_MsgParam.lpszModuleName, pDlg->m_MsgParam.nLine);
									
#ifdef PPMSGBOX_USE_PROFILE_FILE
									//ENG: An info will be search in the INI file
									//RUS: Οξθρκ θντξπμΰφθθ ασδες οπξθηβεδεν β INI τΰιλε
									
									//ENG: Removes a module name from path
									//RUS: Σδΰλεμ θμ μξδσλ θη οσςθ
									if (NULL != lpFilePart)
										*(lpFilePart) = _T('\0');
									
									//ENG: Add an INI filename to path
									//RUS: Δξαΰβλεμ θμ INI τΰιλΰ κ οσςθ
									_tcscat(szPath, PPMSGBOX_INI_FILE);
									
									CPPString sData;
									sData.Format(_T("0x%08X"), dwBtnID);

									::WritePrivateProfileString(_T("DoNotAsk"),	// section name
															sDoNotAskKey,		// key name
															sData,				// string to add
															szPath);			// initialization file
#else
									//ENG: An info will be search in the system registry
									
									CPPString sSection;
									if ((NULL == pDlg->m_MsgParam.lpszCompanyName) || (0 == pDlg->m_MsgParam.lpszCompanyName [0]))
										sSection.Format(_T("Software\\%s\\%s"), lpFilePart, PPMSGBOX_REGISTRY_KEY);
									else
										sSection.Format(_T("Software\\%s\\%s\\%s"), pDlg->m_MsgParam.lpszCompanyName, lpFilePart, PPMSGBOX_REGISTRY_KEY);

									//ENG: Stores a checkbox state
									DWORD dwResult = 0;
									HKEY hKey = NULL;
									LONG lRet = ::RegCreateKeyEx(HKEY_CURRENT_USER, sSection, 0, NULL,
										REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwResult);
									if (lRet == ERROR_SUCCESS)
									{
										::RegSetValueEx(hKey, sDoNotAskKey, 0, REG_DWORD,
											(const BYTE *) &dwBtnID, sizeof(DWORD));
										::RegCloseKey(hKey);
									} //if
#endif //PPMSGBOX_USE_PROFILE_FILE
								} //if
#endif //PPMSGBOX_DO_NOT_SAVE_CHECKBOX
							} //if
						} //if

						//ENG: Close a message box and return code
						::EndDialog(hwnd, dwBtnID);
						return TRUE;
					}
				} //switch
			} //if
		} //End of case WM_COMMAND
		break;
	case WM_TIMER:		// used for timeout
		{
			if (wParam == 1)
			{
				pDlg->OnTimer(hwnd);
				return TRUE;
			} //if
			return FALSE;
		} //End of case WM_TIMER
		break;
	case WM_CTLCOLORSTATIC:
		{
			//ENG: For a transparent text of the checkbox
			::SetBkMode((HDC)wParam, TRANSPARENT);
			return (BOOL)::GetStockObject(NULL_BRUSH);
		} //End of case WM_CTLCOLORSTATIC
		break;
	case WM_SETCURSOR:
		{
			POINT point;
			::GetCursorPos(&point);
			::ScreenToClient(hwnd, &point);

			if (pDlg->m_dwHeaderHeight)
			{
				if (pDlg->m_pHeader.OnSetCursor(&point))
					return TRUE; //The cursor over the hyperlink
			} //if

			if (pDlg->m_dwMessageHeight)
			{
				if (pDlg->m_pDrawer.OnSetCursor(&point))
					return TRUE; //The cursor over the hyperlink
			} //if

			if (pDlg->m_dwMoreInfoHeight)
			{
				if (pDlg->m_pMoreInfo.OnSetCursor(&point))
					return TRUE; //The cursor over the hyperlink
			} //if
		} //End of case WM_SETCURSOR
		break;
	case WM_LBUTTONDOWN:
		{
			POINT point = {LOWORD(lParam), HIWORD(lParam)};

			if (pDlg->m_dwHeaderHeight)
				pDlg->m_pHeader.OnLButtonDown(&point);
			if (pDlg->m_dwMessageHeight)
				pDlg->m_pDrawer.OnLButtonDown(&point);
			if (pDlg->m_dwMoreInfoHeight)
				pDlg->m_pMoreInfo.OnLButtonDown(&point);
		} //End of case WM_LBUTTONDOWN
		break;
	case WM_ERASEBKGND:
		{
			return TRUE;
		} //End of case WM_ERASEBKGND
	case UDM_HTMLDRAWER_REPAINT:
		{
			pDlg->DrawBox(hwnd, NULL);
			//::InvalidateRect(hwnd, NULL, FALSE);
			return TRUE;
		} //End of case UDM_HTMLDRAWER_REPAINT

//	case WM_LBUTTONDOWN:
//		{
//			//ENG: If a checkbox window is exist
//			//RUS: Ερλθ checkbox ρσωερςβσες
//			if ((NULL != pDlg->m_stCheckText.hWnd) && (NULL != pDlg->m_stCheckBtn.hWnd))
//			{
//				//ENG: Get a child window
//				//RUS: Οξλσχΰεμ δξχεπνεε ξκνξ
//				POINT pt;
//				pt.x = LOWORD(lParam); 
//				pt.y = HIWORD(lParam);
//				HWND hWndClient = ::ChildWindowFromPoint(hwnd, pt);
//				if (pDlg->m_stCheckText.hWnd == hWndClient)
//				{
//					//ENG: Toggles a checkbox state
//					//RUS: Οεπεκλώχΰεμ ρξρςξνθε checkbox
//					LRESULT nCheck = ::SendMessage(pDlg->m_stCheckBtn.hWnd, BM_GETCHECK, 0, 0);
//					if (nCheck == BST_CHECKED)
//						::SendMessage(pDlg->m_stCheckBtn.hWnd, BM_SETCHECK, BST_UNCHECKED, 0);
//					else if (nCheck == BST_UNCHECKED)
//						::SendMessage(pDlg->m_stCheckBtn.hWnd, BM_SETCHECK, BST_CHECKED, 0);
//					return TRUE;
//				} //if
//			} //if
//		} //End of case WM_LBUTTONDOWN
//		break;
	} //switch

	return FALSE;
}
/*
BOOL m_bIsFocusedCheckBox(FALSE);

LRESULT CALLBACK NewCheckBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:
		{
			
			TRACE ("WM_ERASEBKGND\n");
			m_bIsFocusedCheckBox = FALSE;
		}
	case WM_SETFOCUS:
		break;
	case WM_KILLFOCUS:
//		::SendMessage(hwnd, WM_PAINT, 0, 0);
		break;
//	case WM_DRAWITEM:
//		{
//			LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
//
//			BOOL bIsPressed = (lpDIS->itemState & ODS_SELECTED);
//			BOOL bIsFocused  = (lpDIS->itemState & ODS_FOCUS);
//			
//			RECT rcClient = lpDIS->rcItem;
//			::SetBkMode(lpDIS->hDC, TRANSPARENT);
//
//			RECT rcBox;
//			rcBox.left = rcClient.left;
//			rcBox.top = rcClient.top;
//			rcBox.right = rcBox.left + 13;
//			rcBox.bottom = rcBox.top + 13;
//			::DrawFrameControl(lpDIS->hDC, &rcBox, DFC_BUTTON, DFCS_BUTTONCHECK);
//
//			return TRUE;
//		}
	case WM_PAINT:
		{
			TRACE ("WM_PAINT\n");
			RECT rect;
			::GetClientRect(hwnd, &rect);

			PAINTSTRUCT ps; 
			HDC hDC = BeginPaint(hwnd, &ps);

			UINT uState = ::SendMessage(hwnd, BM_GETSTATE, 0, 0);
			
			TCHAR szText [_MAX_PATH];
			int nCount = ::SendMessage(hwnd, WM_GETTEXT, _MAX_PATH, (WPARAM)szText);

			HFONT hFont = (HFONT)::SendMessage(hwnd, WM_GETFONT, 0, 0);
			HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
			
			int nOldBkMode = ::SetBkMode(hDC, TRANSPARENT);

			RECT rcCheck = rect;
			rcCheck.right = rcCheck.left + rcCheck.bottom - rcCheck.top;
			if (uState & BST_CHECKED)
				::DrawFrameControl(hDC, &rcCheck, DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_HOT);
			else
				::DrawFrameControl(hDC, &rcCheck, DFC_BUTTON, DFCS_BUTTONCHECK | DFCS_HOT);

			rect.left = rcCheck.right + 6;
			::DrawText(hDC, szText, nCount, &rect, DT_SINGLELINE|DT_LEFT|DT_VCENTER);
//			::TextOut(hDC, 19, 0, szText, nCount);

			rect.left -= 2;
			rect.top -= 1;
			rect.right += 2;
			rect.bottom += 1;

//			if ((uState & BST_FOCUS) && !m_bIsFocusedCheckBox)
//			{
//				TRACE ("WM_FOCUS\n");
//				m_bIsFocusedCheckBox = TRUE;
//				::DrawFocusRect(hDC, &rect);
//			}
//			else if (!(uState & BST_FOCUS) && m_bIsFocusedCheckBox)
//			{
//				TRACE ("WM_UNFOCUS\n");
//				m_bIsFocusedCheckBox = FALSE;
//				::DrawFocusRect(hDC, &rect);
//			}

//			::Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
			
			::SetBkMode(hDC, nOldBkMode);
			::SelectObject(hDC, hOldFont);
			EndPaint(hwnd, &ps);
			return TRUE;
		}
	}
	return CallWindowProc(lpfnOrgCheckboxProc, hwnd, message, wParam, lParam);

} //End of NewCheckBoxProc
*/
void CPPMessageBoxTemplate::OnTimer(HWND hwnd)
{
	//Gets info about default button
	STRUCT_BUTTONS & sb = m_vecButtons [m_nDefBtn];
	if (m_dwCountdownTimer > 0)
	{
		m_dwCountdownTimer--;
		if (m_dwCountdownTimer > 0) 
		{
			//Time was not complete
			CPPString str;
			if ((m_MsgParam.nDisabledSeconds > 0) && (m_MsgParam.bDisableAllCtrls))
			{
				str.Format(m_sDisabledCaption, m_sCaption, m_dwCountdownTimer);
				::SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)str);
				return;
			}
			else
			{
				str.Format(PPMSGBOX_BUTTON_DISABLED, m_sTimeoutText, m_dwCountdownTimer);
				::SendMessage(sb.hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)str);
				return;
			} //if
		} //if
	} //if

	//ENG: Stop timer
	::KillTimer(hwnd, 1);

	if (m_MsgParam.nDisabledSeconds > 0)
	{
		//ENG: Enable a system menu if Cancel button exist
		if (m_bIsCancelExist)
			::EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_ENABLED);

		if (m_MsgParam.bDisableAllCtrls)
		{
			//ENG: Set a caption text
			::SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)m_sCaption);
			
			//ENG: Enables all buttons except default button
			for (UINT i = 0; i < m_vecButtons.size(); ++i) 
			{
				if (i != m_nDefBtn)
				{
					STRUCT_BUTTONS & sbTemp = m_vecButtons [i];
					LONG nStyle = ::GetWindowLong(sbTemp.hWnd, GWL_STYLE);
					nStyle &= ~WS_DISABLED;
					::SetWindowLong(sbTemp.hWnd, GWL_STYLE, nStyle);
					::SendMessage(sbTemp.hWnd, WM_ENABLE, 1, 0);
				} //if
			} //for
		}
		else
		{
			//ENG: Set a real text on the default button only
			::SendMessage(sb.hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)m_sTimeoutText);
		} //if

		//ENG: Enable a default button and set it to default
		LONG nStyle = ::GetWindowLong(sb.hWnd, GWL_STYLE);
		nStyle &= ~WS_DISABLED;
		nStyle |= BS_DEFPUSHBUTTON;
		::SetWindowLong(sb.hWnd, GWL_STYLE, nStyle);
		::SendMessage(sb.hWnd, WM_ENABLE, 1, 0);
	}
	else if (m_MsgParam.nTimeoutSeconds > 0)
	{
		//ENG: Set a real text on the default button only
		::SendMessage(sb.hWnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)m_sTimeoutText);
		
		//ENG: Imitation to click on default button
		::SendMessage(hwnd, WM_COMMAND, sb.nTypeBtn | MB_TIMEOUT, 0);
	} //if
} //End of OnTimer

void CPPMessageBoxTemplate::OnInitDialog(HWND hwnd)
{
	//ENG: Setup a timer
	m_dwCountdownTimer = 0;
	if (m_MsgParam.nDisabledSeconds > 0)
		m_dwCountdownTimer = m_MsgParam.nDisabledSeconds;
	if (m_MsgParam.nTimeoutSeconds > 0)
		m_dwCountdownTimer = m_MsgParam.nTimeoutSeconds;

	m_pHeader.SetCallbackRepaint(hwnd, UDM_HTMLDRAWER_REPAINT);
	m_pDrawer.SetCallbackRepaint(hwnd, UDM_HTMLDRAWER_REPAINT);
	m_pMoreInfo.SetCallbackRepaint(hwnd, UDM_HTMLDRAWER_REPAINT);
	
	CPPString str;
	m_sText = GetString(m_MsgParam.lpszText);
	if (m_MsgParam.dwStyle & MB_RIGHT)
		str.Format(_T("<right>%s</right>"), m_sText);
	else
		str = m_sText;

	int nSysIconWidth = ::GetSystemMetrics(SM_CXICON);
	if (NULL != m_hIcon)
		m_sText.Format(_T("<table cellpadding=%d><tr><td width=%d><icon handle=0x%X></td><td>%s</td></tr></table>"), m_dwSizes [PPMSGBOX_SIZE_SEPX], nSysIconWidth, m_hIcon, str);
	else
		m_sText = str;//.Format(_T("<table><tr><td>%s</td></tr></table>"), str);

	SIZE sz = PrepareMessageBox(hwnd);

	//Set a message box caption
	BOOL bGrayedClose = m_bIsCancelExist ? FALSE : TRUE;
	str = m_sCaption;
	if (m_MsgParam.nDisabledSeconds > 0)
	{
		if (m_MsgParam.bDisableAllCtrls)
		{
			m_sDisabledCaption = GetLocalizedButtonText(MB_TIMEOUT);
			str.Format(m_sDisabledCaption, m_sCaption, m_dwCountdownTimer);
			bGrayedClose = TRUE;
		}
		else
		{
			STRUCT_BUTTONS & sb = m_vecButtons [m_nDefBtn];
			if (IDCANCEL == sb.nTypeBtn)
				bGrayedClose = TRUE;
		} //if
	} //if
	::SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)str);

	//Disable a menu
	if (bGrayedClose)
		::EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_GRAYED);


	//Centering of the dialog
	RECT rcParent;
	if (NULL != GetParent(hwnd))
		::GetWindowRect(::GetParent(hwnd), &rcParent);
	else
	{
			rcParent.left    = 0;
			rcParent.top     = 0;
			rcParent.right   = ::GetSystemMetrics(SM_CXSCREEN);
			rcParent.bottom  = ::GetSystemMetrics(SM_CYSCREEN);
	} //if
	::MoveWindow(hwnd, rcParent.left + (rcParent.right - rcParent.left - sz.cx) / 2, 
						rcParent.top + (rcParent.bottom - rcParent.top - sz.cy) / 2, 
						sz.cx, sz.cy, TRUE);

	//ENG: Gets a size of the client area of the messagebox
	RECT rcClient;
	::GetClientRect(hwnd, &rcClient);

	//ENG: Calculates a full width of all buttons
	int nWidth = m_vecButtons.size() * (m_dwSizes [PPMSGBOX_SIZE_BTNX] + m_dwSizes [PPMSGBOX_SIZE_CTRLX]) - m_dwSizes [PPMSGBOX_SIZE_CTRLX];

	RECT rcBtn = rcClient;

	rcBtn.bottom -= m_dwSizes [PPMSGBOX_SIZE_CTRLY];
	if ((m_MsgParam.dwStyle & MB_CHECKBOX) && (m_MsgParam.dwStyle & MB_CHECKBOXUNDERBUTTONS))
		rcBtn.bottom -= m_dwSizes [PPMSGBOX_SIZE_CTRLY] + m_dwSizes [PPMSGBOX_SIZE_CHKY];
	rcBtn.top = rcBtn.bottom - m_dwSizes [PPMSGBOX_SIZE_BTNY];
	switch (m_MsgParam.nControlsAlign) 
	{
	case PPMSGBOX_ALIGN_LEFT:
		rcBtn.left += m_dwSizes [PPMSGBOX_SIZE_CTRLX];
		break;
	case PPMSGBOX_ALIGN_RIGHT:
		rcBtn.left = rcBtn.right - m_dwSizes [PPMSGBOX_SIZE_CTRLX] - nWidth;
		break;
	case PPMSGBOX_ALIGN_CENTER:
	default:
		rcBtn.left += (rcBtn.right - rcBtn.left - nWidth) / 2;
		break;
	} //switch

	//Create buttons
	for (UINT i = 0; i < m_vecButtons.size(); ++i) 
	{
		//ENG: Get a right edge of the button
		//RUS: Τξπμθπσεμ κξξπδθνΰςσ οπΰβξι γπΰνθφϋ κνξοκθ
		rcBtn.right = rcBtn.left + m_dwSizes [PPMSGBOX_SIZE_BTNX];

		STRUCT_BUTTONS & sb = m_vecButtons [i];
	
		//ENG: Create a button
		//RUS: Ρξηδΰεμ κνξοκσ
		::MoveWindow(sb.hWnd, rcBtn.left, rcBtn.top, m_dwSizes [PPMSGBOX_SIZE_BTNX], m_dwSizes [PPMSGBOX_SIZE_BTNY], TRUE);

		//ENG: Jump to the left edge of the next button
		rcBtn.left += m_dwSizes [PPMSGBOX_SIZE_BTNX] + m_dwSizes [PPMSGBOX_SIZE_CTRLX];
	} //for

	//ENG: Create a checkbox
	if (m_stCheckBtn.nTypeBtn)
	{
		//ENG: Gets a text of the checkbox
//		CPPString sCheckText = GetButtonText(m_stCheckBtn.nTypeBtn);
		
		//Creates a checkbox control
		rcBtn = rcClient;
		rcBtn.bottom -= m_dwSizes [PPMSGBOX_SIZE_CTRLY];
		if (!(m_MsgParam.dwStyle & MB_CHECKBOXUNDERBUTTONS))
			rcBtn.bottom -= m_dwSizes [PPMSGBOX_SIZE_CTRLY] + m_dwSizes [PPMSGBOX_SIZE_BTNY];
		rcBtn.top = rcBtn.bottom - m_dwSizes [PPMSGBOX_SIZE_CHKY];
		rcBtn.left += m_dwSizes [PPMSGBOX_SIZE_CHKX];

		::MoveWindow(m_stCheckBtn.hWnd, rcBtn.left, rcBtn.top, m_dwSizes [PPMSGBOX_SIZE_CHKY], // + m_dwSizes [PPMSGBOX_SIZE_CHKY] / 2 + sz.cx, 
			m_dwSizes [PPMSGBOX_SIZE_CHKY], TRUE);

		//ENG: To the transparency output of the text of the checkbox
		rcBtn.left += m_dwSizes [PPMSGBOX_SIZE_CHKY] + m_dwSizes [PPMSGBOX_SIZE_CHKY] / 2;
		RECT rcCheck;
		::GetWindowRect(m_stCheckText.hWnd, &rcCheck);
		::MoveWindow(m_stCheckText.hWnd, rcBtn.left, rcBtn.top, rcCheck.right - rcCheck.left, 
			rcCheck.bottom - rcCheck.top, TRUE);
	} //if

	//ENG: The message box is created with the WS_EX_TOPMOST window style
	if (m_MsgParam.dwStyle & MB_TOPMOST)
		::SetWindowPos(hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

	//ENG: The message box becomes the foreground window
	if (m_MsgParam.dwStyle & MB_SETFOREGROUND)
		::SetForegroundWindow(hwnd);

	//ENG: The countdown timer is enabled
	if (m_dwCountdownTimer > 0)
		::SetTimer(hwnd, 1, 1000, NULL);
} //End of OnInitDialog

CPPString CPPMessageBoxTemplate::GetString(LPCTSTR lpszText)
{
	if ((NULL == lpszText) || (0 == lpszText [0]))
		return _T("");

	CPPString sText = _T("");
	if (0 == HIWORD(lpszText)) 
	{
		//lpszText is a MAKEINTRESOURCE(ID) of the text from the resources
		if ((NULL != m_MsgParam.hInstanceStrings) && !(m_MsgParam.dwStyle & MB_NORESOURCE))
		{
			TCHAR tchText [_MAX_PATH];
			int nCount = ::LoadString(m_MsgParam.hInstanceStrings, (UINT)lpszText, tchText, _MAX_PATH);
			if (nCount)
				sText = (CPPString)tchText;
		} //if
	}
	else
	{
		//lpszText is a pointer to the button text
		sText = (CPPString)lpszText;
	} //if
	return sText;
} //End of GetString

CPPString CPPMessageBoxTemplate::GetLocalizedButtonText(DWORD dwBtnID, CPPString sDefText /*=_T("")*/)
{
	DWORD dwResID = 0;
	CPPString sText = _T("");
	switch (dwBtnID) 
	{
	case IDOK:
		dwResID = IDS_OK;
		sText = _T("OK");
		break;
	case IDCANCEL:
		dwResID = IDS_CANCEL;
		sText = _T("Cancel");
		break;
	case IDIGNORE:
		dwResID = IDS_IGNORE;
		sText = _T("Ignore");
		break;
	case IDRETRY:
		dwResID = IDS_RETRY;
		sText = _T("Retry");
		break;
	case IDABORT:
		dwResID = IDS_ABORT;
		sText = _T("Abort");
		break;
	case IDHELP:
		dwResID = IDS_HELP;
		sText = _T("Help");
		break;
	case IDYES:
		dwResID = IDS_YES;
		sText = _T("Yes");
		break;
	case IDNO:
		dwResID = IDS_NO;
		sText = _T("No");
		break;
	case IDCONTINUE:
		dwResID = IDS_CONTINUE;
		sText = _T("Continue");
		break;
	case IDYESTOALL:
		dwResID = IDS_YESTOALL;
		sText = _T("Yes to all");
		break;
	case IDNOTOALL:
		dwResID = IDS_NOTOALL;
		sText = _T("No to all");
		break;
	case IDTRYAGAIN:
		dwResID = IDS_TRYAGAIN;
		sText = _T("Try again");
		break;
	case IDREPORT:
		dwResID = IDS_REPORT;
		sText = _T("Report");
		break;
	case IDIGNOREALL:
		dwResID = IDS_IGNOREALL;
		sText = _T("Ignore all");
		break;
	case IDSKIP:
		dwResID = IDS_SKIP;
		sText = _T("Skip");
		break;
	case IDSKIPALL:
		dwResID = IDS_SKIPALL;
		sText = _T("Skip all");
		break;
	case MB_CHECKBOX:
		dwResID = IDS_DONOTSHOWAGAIN;
		sText = _T("Do not show again");
		break;
	case IDMOREINFO_OPENED:
		dwResID = IDS_MOREINFO_OPENED;
		sText = _T("<< More");
		break;
	case IDMOREINFO_CLOSED:
		dwResID = IDS_MOREINFO_CLOSED;
		sText = _T("More >>");
		break;
	case MB_TIMEOUT:
		dwResID = IDS_GLOBALDISABLE;
		sText = _T("%s (%d seconds remaining)");
		break;
	} //switch

	if (sDefText.IsEmpty())
		sDefText = sText;
	sText.Empty();

	//Try to get a localized button text from the specified resources or from the pointer
	//to real text
	if (NULL != m_MsgParam.pLocalBtnText)
	{
		typedef mapLocalBtnText::iterator iterLocalBtnText;
		iterLocalBtnText iterMap = m_MsgParam.pLocalBtnText->find(dwBtnID);
		
		if (iterMap != m_MsgParam.pLocalBtnText->end())
		{
			LPCTSTR lpszText = iterMap->second;
			sText = GetString(lpszText);
/*			if (0 == HIWORD(lpszText)) 
			{
				//lpszText is a MAKEINTRESOURCE() of the buton text from the resources
				if ((NULL != m_MsgParam.hInstanceStrings) && !(m_MsgParam.dwStyle & MB_NORESOURCE))
				{
					TCHAR tchText [_MAX_PATH];
					int nCount = ::LoadString(m_MsgParam.hInstanceStrings, (UINT)lpszText, tchText, _MAX_PATH);
					if (nCount)
						sText = (CPPString)tchText;
				} //if
			}
			else
			{
				//lpszText is a pointer to the button text
				sText = (CPPString)lpszText;
			} //if
*/		} //if
	} //if

	//Try to get a button text from resources
	if (sText.IsEmpty() && (NULL != m_MsgParam.hInstanceStrings) && !(m_MsgParam.dwStyle & MB_NORESOURCE) && dwResID)
	{
		TCHAR tchText [_MAX_PATH];
		int nCount = ::LoadString(m_MsgParam.hInstanceStrings, dwResID, tchText, _MAX_PATH);
		if (nCount)
			sText = (CPPString)tchText;
	} //if

	if (sText.IsEmpty())
		sText = sDefText;

	return sText;
} //End of GetLocalizedButtonText

void CPPMessageBoxTemplate::RemoveAllButtons()
{
	//ENG: Removes an existing control buttons
	for (UINT i = 0; i < m_vecButtons.size(); ++i)
	{
		STRUCT_BUTTONS & sb = m_vecButtons [i];
		if (NULL != sb.hWnd)
		{
			::DestroyWindow(sb.hWnd);
			sb.hWnd = NULL;
		}
	} //for
	m_vecButtons.clear();

	//ENG: Removes a checkbox
	if (NULL != m_stCheckBtn.hWnd)
	{
		::DestroyWindow(m_stCheckBtn.hWnd);
		m_stCheckBtn.hWnd = NULL;
	} //if

	//ENG: Removes a text of the checkbox
	if (NULL != m_stCheckText.hWnd)
	{
		::DestroyWindow(m_stCheckText.hWnd);
		m_stCheckText.hWnd = NULL;
	} //if
}

SIZE CPPMessageBoxTemplate::PrepareMessageBox(HWND hwnd)
{
	RemoveAllButtons();

	//ENG: Gets a device context
	HDC hDC = ::GetDC(NULL);
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

	SIZE sz = {0, 0};
	SIZE szTemp = {0, 0};

	//---------------- Header area -----------------------

	m_dwHeaderHeight = 0;
	if (m_MsgParam.nHeaderHeight)
	{
		//ENG: Stores a minimal height of the header area
		//RUS: Ρξυπΰνεμ μθνθμΰλόνσώ βϋρξςσ ξαλΰρςθ ηΰγξλξβκΰ
		m_dwHeaderHeight = m_MsgParam.nHeaderHeight;
		szTemp.cx = 0;
		m_sHeader = GetString(m_MsgParam.lpszHeaderText);
		if (!m_sHeader.IsEmpty())
		{
			//ENG: If header text is available then prepare him to output
			//RUS: Ερλθ ςεκρς ηΰγξλξβκΰ ρσωερςβσες, ςξ οξδγξςΰβλθβΰεμ εγξ κ βϋβξδσ
			m_pHeader.PrepareOutput(hDC, m_sHeader, &szTemp);
			szTemp.cx += m_dwSizes[PPMSGBOX_SIZE_MSGX] * 2;
			
			//ENG: Updates a height of the header area
			//RUS: Ξανξβλεμ βϋρξςσ ξαλΰρςθ ηΰγξλξβκΰ
			m_dwHeaderHeight = max(m_dwHeaderHeight, szTemp.cy + m_dwSizes[PPMSGBOX_SIZE_MSGX] * 2);
		} //if
		sz.cx = max(sz.cx, szTemp.cx);

		//ENG: Header separator will be drawn
		//RUS: Πΰηδελθςελό ηΰγξλξβκΰ ασδες βϋβεδεν
		m_sHeaderSepText = GetString(m_MsgParam.pHeaderBk.lpszSepText);
		if (m_MsgParam.pHeaderBk.nSepType != PPMSGBOX_SEP_NONE)
		{
			::GetTextExtentPoint32(hDC, m_sHeaderSepText, m_sHeaderSepText.GetLength(), &szTemp);
			sz.cx = max(sz.cx, (int)(szTemp.cx + 2 * m_dwSizes [PPMSGBOX_SIZE_SEPX]));
		} //if
	} //if
	sz.cy += m_dwHeaderHeight;

	//--------------- Message area -----------------------

	m_dwMessageHeight = 0;
	if (!m_sText.IsEmpty())
	{
		//ENG: If message text is available then prepare him to output
		//RUS: Ερλθ ςεκρς ρξξαωενθ ρσωερςβσες, ςξ οξδγξςΰβλθβΰεμ εγξ κ βϋβξδσ
		m_pDrawer.PrepareOutput(hDC, m_sText, &szTemp);
		
		//ENG: Updates a height of the message area
		//RUS: Ξανξβλεμ βϋρξςσ ξαλΰρςθ ρξξαωενθ
		m_dwMessageHeight = szTemp.cy + 2 * m_dwSizes [PPMSGBOX_SIZE_MSGY];
		sz.cx = max(sz.cx, (int)(szTemp.cx + 2 * m_dwSizes [PPMSGBOX_SIZE_MSGX]));
	} //if
	sz.cy += m_dwMessageHeight;

	//--------------- Moreinfo area -----------------------
	
	m_dwMoreInfoHeight = 0;
	CPPString sMoreInfo = GetString(m_MsgParam.lpszMoreInfo);
	if (!sMoreInfo.IsEmpty())
	{
		//ENG: If message text is available then prepare him to output
		//RUS: Ερλθ ςεκρς ρξξαωενθ ρσωερςβσες, ςξ οξδγξςΰβλθβΰεμ εγξ κ βϋβξδσ
		m_pMoreInfo.PrepareOutput(hDC, sMoreInfo, &szTemp);
		
		//ENG: Updates a height of the message area
		//RUS: Ξανξβλεμ βϋρξςσ ξαλΰρςθ ρξξαωενθ
		m_dwMoreInfoHeight = szTemp.cy + 2 * m_dwSizes [PPMSGBOX_SIZE_MSGY];
		sz.cx = max(sz.cx, (int)(szTemp.cx + 2 * m_dwSizes [PPMSGBOX_SIZE_MSGX]));
	} //if
	
	//---------------- Control area ----------------------

	m_dwControlHeight = m_dwSizes [PPMSGBOX_SIZE_CTRLY];
	if ((m_MsgParam.pControlBk.nEffectBk >= 0) || (m_MsgParam.pControlBk.nSepType != PPMSGBOX_SEP_NONE))
	{
		m_dwControlHeight += m_dwSizes [PPMSGBOX_SIZE_CTRLY];
		
		//ENG: Comtrol separator will be drawn
		//RUS: Πΰηδελθςελό ξαλΰρςθ σοπΰβλενθ ασδες βϋβεδεν
		m_sControlSepText = GetString(m_MsgParam.pControlBk.lpszSepText);
		if (m_MsgParam.pControlBk.nSepType != PPMSGBOX_SEP_NONE)
		{
			::GetTextExtentPoint32(hDC, m_sControlSepText, m_sControlSepText.GetLength(), &szTemp);
			sz.cx = max(sz.cx, (int)(szTemp.cx + 2 * m_dwSizes [PPMSGBOX_SIZE_SEPX]));
		} //if
	} //if

	//ENG: Get a checkbox type
	//RUS: Οξλσχΰεμ ςθο checkbox
	m_stCheckBtn.hWnd = NULL;
	m_stCheckBtn.nTypeBtn = (m_MsgParam.dwStyle & MB_CHECKBOX);
	m_stCheckText = m_stCheckBtn;
	if (m_stCheckBtn.nTypeBtn)
	{
		//ENG: Checkbox will drawn
		//RUS: Τλΰζξκ οπθρσςρβσες β δθΰλξγε
		m_dwControlHeight += m_dwSizes [PPMSGBOX_SIZE_CTRLY] + m_dwSizes [PPMSGBOX_SIZE_CHKY];

		//ENG: Gets the sizes of the text of the checkbox
		//RUS: Οξλσχΰεμ πΰημεπϋ ςεκρςΰ τλΰζκΰ
		CPPString sCheckBoxText = GetString(m_MsgParam.lpszCheckBoxText);
		if (sCheckBoxText.IsEmpty()) 
			sCheckBoxText = _T("Do not show again");
		CPPString str = GetLocalizedButtonText(MB_CHECKBOX, sCheckBoxText);
		::GetTextExtentPoint32(hDC, str, str.GetLength(), &szTemp);

		//ENG: Check a max width of the dialog
		//RUS: Οπξβεπεμ μΰκρθμΰλόνσώ ψθπθνσ δθΰλξγΰ
		sz.cx = max(sz.cx, (int)(szTemp.cx + 2 * m_dwSizes [PPMSGBOX_SIZE_CHKX] + 20));

		m_stCheckBtn.hWnd = ::CreateWindow(_T("BUTTON"), NULL /*sCheckText*/, 
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_AUTOCHECKBOX, 
			0, 0, m_dwSizes [PPMSGBOX_SIZE_CHKY], // + m_dwSizes [PPMSGBOX_SIZE_CHKY] / 2 + sz.cx, 
			m_dwSizes [PPMSGBOX_SIZE_CHKY], hwnd, NULL, NULL, NULL);
		if (m_MsgParam.dwStyle & MB_CHECKBOXCHECKED)
			::SendMessage(m_stCheckBtn.hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, FALSE);

		//ENG: To the transparency output of the text of the checkbox
		//RUS: εκρς δλ checkbox βϋβξδθμ ξςδελόνξ δλ οπξηπΰχνξγξ βϋβξδΰ
		m_stCheckText.hWnd = ::CreateWindow(_T("STATIC"), str, 
			WS_VISIBLE | WS_CHILD | SS_LEFT,
			0, 0, szTemp.cx, m_dwSizes [PPMSGBOX_SIZE_CHKY],
			hwnd, NULL, NULL, NULL);
		::SendMessage(m_stCheckText.hWnd, WM_SETFONT, (WPARAM)m_hFont, FALSE);
	} //if

	//ENG: Creates a list of the buttons 
	//RUS: Ρξηδΰεμ ροθρξκ κνξοξκ
	STRUCT_BUTTONS sb;
	sb.hWnd = NULL;

	CPPString sCustomButtons = GetString(m_MsgParam.lpszCustomButtons);
	if (((m_MsgParam.dwStyle & MB_TYPEMASK) == MB_OK) && sCustomButtons.IsEmpty())
		m_bIsCancelExist = TRUE;
	else
		m_bIsCancelExist = FALSE;
	
	//ENG: Try to load a custom buttons
	//RUS: Οπξασεμ ηΰγπσηθςό οξλόηξβΰςελόρκθε κνξοκθ
	if (!sCustomButtons.IsEmpty())
	{
		//ENG: Will be use the custom buttons
		//RUS: Ασδσς θροξλόηξβΰνϋ οξλόηξβΰςελόρκθε κνξοκθ
		sCustomButtons.Remove(_T('\r'));
		DWORD dwButton = IDCUSTOM1;
		sCustomButtons += _T("\n");
		int nLength = sCustomButtons.GetLength();
		for (int i = 0; (i < nLength) && (dwButton <= IDCUSTOM4); ++i)
		{
			int nIndex = sCustomButtons.Find(_T('\n'), i);
			if (i != nIndex)
			{
				//ENG: A custom button was found
				//RUS: Οξλόηξβΰςελόρκΰ κνξοκΰ νΰιδενΰ
//				SetButtonText(dwButton, m_MsgParam.sCustomButtons.Mid(i, nIndex - i));
				sb.nTypeBtn = dwButton;
				m_vecButtons.push_back(sb);
				dwButton ++;
				i = nIndex;
			} //if
		} //for
	} //if

	if (0 == m_vecButtons.size())
	{
		//ENG: Custom buttons not used
		//RUS: Οξλόηξβΰςελόρκθε κνξοκθ νε θροξλόησώςρ
		switch (m_MsgParam.dwStyle & MB_TYPEMASK) 
		{
		case MB_OK:
			sb.nTypeBtn = IDOK;
			m_vecButtons.push_back(sb);
			break;
		case MB_OKCANCEL:
			sb.nTypeBtn = IDOK;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDCANCEL;
			m_vecButtons.push_back(sb);
			m_bIsCancelExist = TRUE;
			break;
		case MB_RETRYCANCEL:
			sb.nTypeBtn = IDRETRY;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDCANCEL;
			m_vecButtons.push_back(sb);
			m_bIsCancelExist = TRUE;
			break;
		case MB_YESNO:
			sb.nTypeBtn = IDYES;
			m_vecButtons.push_back(sb);
			if (m_MsgParam.dwStyle & MB_YESTOALL)
			{
				sb.nTypeBtn = IDYESTOALL;
				m_vecButtons.push_back(sb);
			} //if
			sb.nTypeBtn = IDNO;
			m_vecButtons.push_back(sb);
			if (m_MsgParam.dwStyle & MB_NOTOALL)
			{
				sb.nTypeBtn = IDNOTOALL;
				m_vecButtons.push_back(sb);
			} //if
			break;
		case MB_CONTINUEABORT:
			sb.nTypeBtn = IDCONTINUE;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDABORT;
			m_vecButtons.push_back(sb);
			break;
		case MB_YESNOCANCEL:
			sb.nTypeBtn = IDYES;
			m_vecButtons.push_back(sb);
			if (m_MsgParam.dwStyle & MB_YESTOALL)
			{
				sb.nTypeBtn = IDYESTOALL;
				m_vecButtons.push_back(sb);
			} //if
			sb.nTypeBtn = IDNO;
			m_vecButtons.push_back(sb);
			if (m_MsgParam.dwStyle & MB_NOTOALL)
			{
				sb.nTypeBtn = IDNOTOALL;
				m_vecButtons.push_back(sb);
			} //if
			sb.nTypeBtn = IDCANCEL;
			m_vecButtons.push_back(sb);
			m_bIsCancelExist = TRUE;
			break;
		case MB_ABORTRETRYIGNORE:
			sb.nTypeBtn = IDABORT;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDRETRY;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDIGNORE;
			m_vecButtons.push_back(sb);
			break;
		case MB_CANCELTRYCONTINUE:
			sb.nTypeBtn = IDCANCEL;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDRETRY;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDCONTINUE;
			m_vecButtons.push_back(sb);
			m_bIsCancelExist = TRUE;
			break;
		case MB_SKIPSKIPALLCANCEL:
			sb.nTypeBtn = IDSKIP;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDSKIPALL;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDCANCEL;
			m_vecButtons.push_back(sb);
			m_bIsCancelExist = TRUE;
			break;
		case MB_IGNOREIGNOREALLCANCEL:
			sb.nTypeBtn = IDIGNORE;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDIGNOREALL;
			m_vecButtons.push_back(sb);
			sb.nTypeBtn = IDCANCEL;
			m_vecButtons.push_back(sb);
			m_bIsCancelExist = TRUE;
			break;
		} //switch
	} //if

	//ENG: Adds a Help button to the message box.
	//RUS: Δξαΰβλεμ κνξοκσ Help
	if (m_MsgParam.dwStyle & MB_HELP) 
	{
		sb.nTypeBtn = IDHELP;
		m_vecButtons.push_back(sb);
	} //if

	//ENG: Adds a Report button to the message box.
	//RUS: Δξαΰβλεμ κνξοκσ Report
	if (m_MsgParam.dwReportMsgID && (NULL != m_MsgParam.hParentWnd)) 
	{
		sb.nTypeBtn = IDREPORT;
		m_vecButtons.push_back(sb);
	} //if

	//ENG: Adds a More Info button to message box.
	//RUS: Δξαΰβλεμ κνξοκσ "Δξοξλνθςελόνξ"
	m_sMoreInfoSepText = GetString(m_MsgParam.pMoreInfoBk.lpszSepText);
	m_sMoreInfoClosed = m_sMoreInfoOpened = _T("");
	if (!sMoreInfo.IsEmpty()) 
	{
		sb.nTypeBtn = IDMOREINFO_CLOSED;
		m_vecButtons.push_back(sb);

		//Load a "MoreInfo" button text
		m_sMoreInfoOpened = GetLocalizedButtonText(IDMOREINFO_OPENED);
		m_sMoreInfoClosed = GetLocalizedButtonText(IDMOREINFO_CLOSED);
	} //if

	//ENG: Remembers a default button
	//RUS: Ηΰοξμθνΰεμ κνξοκσ οξ σμξλχΰνθώ
	m_nDefBtn = (m_MsgParam.dwStyle >> 8) & 0x0f;
	if (m_nDefBtn >= m_vecButtons.size())
		m_nDefBtn = 0;

	//ENG:
	m_dwSizes [PPMSGBOX_SIZE_BTNY] = 0;
	m_dwSizes [PPMSGBOX_SIZE_BTNX] = 0;
	int nCustomIndex = 0;
	for (UINT i = 0; i < m_vecButtons.size(); ++i)
	{
		STRUCT_BUTTONS & sbTemp = m_vecButtons [i];

		CPPString sBtnText; 
		if (IDMOREINFO_CLOSED == sbTemp.nTypeBtn)
		{
			::GetTextExtentPoint32(hDC, m_sMoreInfoOpened, m_sMoreInfoOpened.GetLength(), &szTemp);
			SIZE szTemp2;
			::GetTextExtentPoint32(hDC, m_sMoreInfoClosed, m_sMoreInfoClosed.GetLength(), &szTemp2);
			szTemp.cx = max (szTemp.cx, szTemp2.cx);
			szTemp.cy = max (szTemp.cy, szTemp2.cy);
			sBtnText = m_sMoreInfoClosed;
		}
		else 
		{
			if ((sbTemp.nTypeBtn >= IDCUSTOM1) && (sbTemp.nTypeBtn <= IDCUSTOM4))
			{
				int nIndex = sCustomButtons.Find(_T('\n'), nCustomIndex);
				sBtnText = sCustomButtons.Mid(nCustomIndex, nIndex - nCustomIndex);
				nCustomIndex = nIndex + 1;
			}
			else
				sBtnText = GetLocalizedButtonText(sbTemp.nTypeBtn);
			::GetTextExtentPoint32(hDC, sBtnText, sBtnText.GetLength(), &szTemp);
		} //if
		szTemp.cx = ::MulDiv(szTemp.cx, 80, 45);//40);
		szTemp.cy = ::MulDiv(szTemp.cy, 23, 13);
		m_dwSizes [PPMSGBOX_SIZE_BTNY] = max ((DWORD)szTemp.cy, m_dwSizes [PPMSGBOX_SIZE_BTNY]);
		m_dwSizes [PPMSGBOX_SIZE_BTNX] = max ((DWORD)szTemp.cx, m_dwSizes [PPMSGBOX_SIZE_BTNX]);

		//ENG: Common styles for all buttons
		//RUS: Ξαωθε ρςθλθ δλ βρευ κνξοξκ
		DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP;

		if (sbTemp.nTypeBtn == IDHELP)
		{
			//RUS: Ροεφθΰλόνξ ξαπΰαΰςϋβΰεμ κνξοκσ Help, ςΰκ κΰκ ξνΰ νε δξλζνΰ νθκξγδΰ ηΰοπεωΰςόρ
			if (i != m_nDefBtn)
				dwStyle |= BS_PUSHBUTTON;
			else
			{
				dwStyle |= BS_DEFPUSHBUTTON;
				m_sTimeoutText = sBtnText;
				if ((m_MsgParam.nTimeoutSeconds > 0) || ((m_MsgParam.nDisabledSeconds > 0) && !m_MsgParam.bDisableAllCtrls))
					sBtnText.Format(PPMSGBOX_BUTTON_DISABLED, m_sTimeoutText, m_dwCountdownTimer);
			} //if
		}
		else
		{
			if ((m_MsgParam.nDisabledSeconds > 0) && m_MsgParam.bDisableAllCtrls)
			{
				dwStyle |= WS_DISABLED | BS_PUSHBUTTON;
			}
			else
			{
				if (i != m_nDefBtn)
				{
					dwStyle |= BS_PUSHBUTTON;
				}
				else
				{
					m_sTimeoutText = sBtnText;
					if (m_MsgParam.nDisabledSeconds > 0)
						dwStyle |= BS_PUSHBUTTON;
					else
						dwStyle |= BS_DEFPUSHBUTTON;
					
					if (m_dwCountdownTimer > 0)
					{
						sBtnText.Format(PPMSGBOX_BUTTON_DISABLED, m_sTimeoutText, m_dwCountdownTimer);
						if (m_MsgParam.nDisabledSeconds > 0)
							dwStyle |= WS_DISABLED;
					} //if
				} //if
			} //if
		} //if

		//ENG: Create a button
		//RUS: Ρξηδΰεμ κνξοκσ
		sbTemp.hWnd = ::CreateWindow(_T("BUTTON"), sBtnText, dwStyle, 0, 0, 0, 0, hwnd, (HMENU)sbTemp.nTypeBtn, NULL, NULL);
		::SendMessage(sbTemp.hWnd, WM_SETFONT, (WPARAM)m_hFont, FALSE);

		//ENG: Set a focus for default button
		//RUS: Δλ κνξοκθ οξ σμξλχΰνθώ, οεπεδΰεμ ει τξκσρ
		if (i == m_nDefBtn)
			::SetFocus(sbTemp.hWnd);
	} //if

	if (m_dwSizes [PPMSGBOX_SIZE_BTNX] < 80)
		m_dwSizes [PPMSGBOX_SIZE_BTNX] = 80;

	if (m_dwSizes [PPMSGBOX_SIZE_BTNY] < 23)
		m_dwSizes [PPMSGBOX_SIZE_BTNY] = 23;
	
	//ENG: Calculates a width of the all buttons area
	//RUS: Οξδρχθςϋβΰεμ ψθπθνσ ξαλΰρςθ βρευ κνξοξκ
	szTemp.cx = m_dwSizes [PPMSGBOX_SIZE_CTRLX] + m_vecButtons.size() * (m_dwSizes [PPMSGBOX_SIZE_CTRLX] + m_dwSizes [PPMSGBOX_SIZE_BTNX]);
	sz.cx = max(sz.cx, szTemp.cx);

	m_dwControlHeight += m_dwSizes [PPMSGBOX_SIZE_BTNY];
	
	//ENG: Add a height of the control area
	//RUS: Δξαΰβλεμ βϋρξςσ ξαλΰρςθ κξνςπξλξβ
	sz.cy += m_dwControlHeight;

	//----------------------------------------------------
	//ENG: Add the edges and the caption
	//RUS: Δξαΰβλεμ αξπδώπ θ ηΰγξλξβξκ δθΰλξγΰ 
	sz.cx += 2 * ::GetSystemMetrics(SM_CXFIXEDFRAME);
	sz.cy += ::GetSystemMetrics(SM_CYCAPTION) + 2 * ::GetSystemMetrics(SM_CYFIXEDFRAME);

	//ENG: Release a device context
	//RUS: Ξρβξαξζδΰεμ κξνςεκρς σρςπξρςβΰ
	::SelectObject(hDC, hOldFont);
	if (NULL != hDC)
		::ReleaseDC(NULL, hDC);

	return sz;
} //End of GetClientSize

void CPPMessageBoxTemplate::DrawBox(HWND hwnd, HDC hDC)
{
	BOOL bAutoReleaseDC = FALSE;
	if (NULL == hDC)
	{
		hDC = ::GetDC(hwnd);
		bAutoReleaseDC = TRUE;
	} //if

	RECT rcClient;
	::GetClientRect(hwnd, &rcClient);

	RECT rcFull = rcClient;
	if (!m_bMoreInfoOpened)
		rcFull.bottom += m_dwMoreInfoHeight;

	int nWidth = rcFull.right - rcFull.left;
	int nHeight = rcFull.bottom - rcFull.top;

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HDC hSrcDC = ::CreateCompatibleDC(hDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
	
	HBITMAP hOldSrcBitmap = NULL;
	HFONT hOldFont = NULL;

	if (NULL == m_hBitmapBk)
	{
		hOldFont = (HFONT)::SelectObject(hSrcDC, m_hFont);
		m_hBitmapBk = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
		hOldSrcBitmap = (HBITMAP)::SelectObject(hSrcDC, m_hBitmapBk);

		RECT rect = rcClient;
		
		//Fill a message box window
		if (m_MsgParam.pMsgBoxBk.nEffectBk >= 0)
			m_pDrawer.GetDrawManager()->FillEffect(hSrcDC, m_MsgParam.pMsgBoxBk.nEffectBk, &rcFull, m_MsgParam.pMsgBoxBk.crStartBk, m_MsgParam.pMsgBoxBk.crMidBk, m_MsgParam.pMsgBoxBk.crEndBk);
		else
		{
			HBRUSH hBrush = ::CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
			::FillRect(hSrcDC, &rcFull, hBrush);
			::DeleteObject(hBrush);
		} //if

		//Draw a header area
		if (0 != m_dwHeaderHeight) 
		{
			//Fill a header background
			rcClient.bottom = rcClient.top + m_dwHeaderHeight;
			if (m_MsgParam.pHeaderBk.nEffectBk >= 0)
				m_pDrawer.GetDrawManager()->FillEffect(hSrcDC, m_MsgParam.pHeaderBk.nEffectBk, &rcClient, m_MsgParam.pHeaderBk.crStartBk, m_MsgParam.pHeaderBk.crMidBk, m_MsgParam.pHeaderBk.crEndBk);
			//Draw a Separator
			rcClient.top = rcClient.bottom;
			if (m_MsgParam.pHeaderBk.nSepType != PPMSGBOX_SEP_NONE)
				DrawSeparator(hSrcDC, &rcClient, m_MsgParam.pHeaderBk.nSepType, m_sHeaderSepText, m_MsgParam.pHeaderBk.nSepAlign);
		} //if

		rcClient.top += m_dwMessageHeight;

		//Draw a control area
		if (m_MsgParam.pControlBk.nEffectBk >= 0)
		{
			rcClient.bottom = rcClient.top + m_dwControlHeight;
			m_pDrawer.GetDrawManager()->FillEffect(hSrcDC, m_MsgParam.pControlBk.nEffectBk, &rcClient, m_MsgParam.pControlBk.crStartBk, m_MsgParam.pControlBk.crMidBk, m_MsgParam.pControlBk.crEndBk);
		} //if
		if (m_MsgParam.pControlBk.nSepType != PPMSGBOX_SEP_NONE)
			DrawSeparator(hSrcDC, &rcClient, m_MsgParam.pControlBk.nSepType, m_sControlSepText, m_MsgParam.pControlBk.nSepAlign);

		//Draw a MoreInfo area background
		if (m_dwMoreInfoHeight)
		{
			rcClient.top += m_dwControlHeight;
			//Draw a control's area
			if (m_MsgParam.pMoreInfoBk.nEffectBk >= 0)
			{
				rcClient.bottom = rcFull.bottom;
				m_pDrawer.GetDrawManager()->FillEffect(hSrcDC, m_MsgParam.pMoreInfoBk.nEffectBk, &rcClient, m_MsgParam.pMoreInfoBk.crStartBk, m_MsgParam.pMoreInfoBk.crMidBk, m_MsgParam.pMoreInfoBk.crEndBk);
			} //if
			if (m_MsgParam.pMoreInfoBk.nSepType != PPMSGBOX_SEP_NONE)
				DrawSeparator(hSrcDC, &rcClient, m_MsgParam.pMoreInfoBk.nSepType, m_sMoreInfoSepText, m_MsgParam.pMoreInfoBk.nSepAlign);
		} //if

		::SelectObject(hSrcDC, hOldFont);
		rcClient = rect;
	}
	else
	{
		hOldSrcBitmap = (HBITMAP)::SelectObject(hSrcDC, m_hBitmapBk);
	} //if

	hOldFont = (HFONT)::SelectObject(hMemDC, m_hFont);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	::BitBlt(hMemDC, 0, 0, nWidth, nHeight, hSrcDC, 0, 0, SRCCOPY);

	//Draw a header's area
	if (0 != m_dwHeaderHeight) 
	{
		//Fill a header background
		rcClient.bottom = rcClient.top + m_dwHeaderHeight;
		//Draw a header text
		if (!m_sHeader.IsEmpty())
		{
			RECT rcHeader;
			rcHeader.left = rcClient.left + m_dwSizes[PPMSGBOX_SIZE_MSGX];
			rcHeader.right = rcClient.right - m_dwSizes[PPMSGBOX_SIZE_MSGX];
			rcHeader.top = rcClient.top + m_dwSizes[PPMSGBOX_SIZE_MSGY];
			rcHeader.bottom = rcClient.bottom - m_dwSizes[PPMSGBOX_SIZE_MSGY];
			m_pHeader.DrawPreparedOutput(hMemDC, &rcHeader);
		} //if
		//Draw a Separator
		rcClient.top = rcClient.bottom;
	} //if

	//Draw a message's area
	if (!m_sText.IsEmpty()) 
	{
		//Margins from the top
		RECT rcMsg = rcClient;
		rcMsg.bottom = rcClient.top + m_dwMessageHeight - m_dwSizes [PPMSGBOX_SIZE_MSGY];
		rcMsg.top += m_dwSizes [PPMSGBOX_SIZE_MSGY];
		rcMsg.left += m_dwSizes [PPMSGBOX_SIZE_MSGX];
		rcMsg.right -= m_dwSizes [PPMSGBOX_SIZE_MSGX];
		m_pDrawer.DrawPreparedOutput(hMemDC, &rcMsg);
	} //if

	//Draw a moreinfo's area
	if (m_bMoreInfoOpened && m_dwMoreInfoHeight)
	{
		//Margins from the top
		RECT rcMoreInfo = rcFull;
		rcMoreInfo.top += rcMoreInfo.bottom - m_dwMoreInfoHeight + m_dwSizes [PPMSGBOX_SIZE_MSGY];
		rcMoreInfo.left += m_dwSizes [PPMSGBOX_SIZE_MSGX];
		rcMoreInfo.right -= m_dwSizes [PPMSGBOX_SIZE_MSGX];
		m_pMoreInfo.DrawPreparedOutput(hMemDC, &rcMoreInfo);
	} //if
	::SelectObject(hMemDC, hOldFont);

	if (!bAutoReleaseDC)
		::BitBlt(hDC, 0, 0, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);
	else
	{
		::BitBlt(hDC, 0, 0, nWidth, m_dwHeaderHeight + m_dwMessageHeight, hMemDC, 0, 0, SRCCOPY);
		if (m_bMoreInfoOpened)
			::BitBlt(hDC, 0, rcFull.bottom - m_dwMoreInfoHeight, nWidth, m_dwMoreInfoHeight, hMemDC, 0, nHeight - m_dwMoreInfoHeight, SRCCOPY);
	}

	::SelectObject(hMemDC, hOldBitmap);
	::SelectObject(hSrcDC, hOldSrcBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::DeleteDC(hSrcDC);

	if (bAutoReleaseDC)
		::ReleaseDC(hwnd, hDC);
} //End of DrawBox

void CPPMessageBoxTemplate::DrawSeparator(HDC hDC, LPCRECT lpRect, int nTypeSeparator, LPCTSTR lpszText, int nTextAlign)
{
	RECT rect = {lpRect->left + m_dwSizes [PPMSGBOX_SIZE_SEPX], lpRect->top, lpRect->right - m_dwSizes [PPMSGBOX_SIZE_SEPX], lpRect->bottom};
	RECT rcTemp = rect;
	if ((NULL != lpszText) && (0 != lpszText [0]))
	{
		CPPString sText = lpszText;
		SIZE sz = {0, 0};
		::GetTextExtentPoint32(hDC, lpszText, sText.GetLength(), &sz);
		rcTemp.top = rect.top - sz.cy / 2;
		rcTemp.bottom = rcTemp.top + sz.cy;
		switch (nTextAlign)
		{
		case PPMSGBOX_ALIGN_LEFT:
			::DrawState(hDC, NULL, NULL, (LPARAM)lpszText, 0, rcTemp.left, rcTemp.top, 0, 0, DST_TEXT | DSS_DISABLED);
			rcTemp.left += sz.cx + m_dwSizes [PPMSGBOX_SIZE_SEPX];
			break;
		case PPMSGBOX_ALIGN_RIGHT:
			::DrawState(hDC, NULL, NULL, (LPARAM)lpszText, 0, rcTemp.right - sz.cx, rcTemp.top, 0, 0, DST_TEXT | DSS_DISABLED);
			rcTemp.right -= sz.cx + m_dwSizes [PPMSGBOX_SIZE_SEPX];
			break;
		case PPMSGBOX_ALIGN_CENTER:
			rcTemp.right = rcTemp.left + (rcTemp.right - rcTemp.left - sz.cx) / 2;
			::DrawState(hDC, NULL, NULL, (LPARAM)lpszText, 0, rcTemp.right, rcTemp.top, 0, 0, DST_TEXT | DSS_DISABLED);
			rcTemp.right -= m_dwSizes [PPMSGBOX_SIZE_SEPX];
			if (rcTemp.left < rcTemp.right)
			{
				rcTemp.top = rcTemp.bottom = rect.top;
				switch (nTypeSeparator)
				{
				case PPMSGBOX_SEP_ETCHED:
					::DrawEdge(hDC, &rcTemp, EDGE_ETCHED, BF_TOP);
					break;
				case PPMSGBOX_SEP_BLACK:
					m_pDrawer.GetDrawManager()->DrawLine(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.top, RGB (0, 0, 0));
					break;
				case PPMSGBOX_SEP_WHITE:
					m_pDrawer.GetDrawManager()->DrawLine(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.top, RGB (255, 255, 255));
					break;
				} //switch
			} //if
			rcTemp.left = rcTemp.right + 2 * m_dwSizes [PPMSGBOX_SIZE_SEPX] + sz.cx;
			rcTemp.right = rect.right;

			break;
		} //switch
		rcTemp.top = rcTemp.bottom = rect.top;
	} //if
	
	if (rcTemp.left < rcTemp.right)
	{
		switch (nTypeSeparator)
		{
		case PPMSGBOX_SEP_ETCHED:
			::DrawEdge(hDC, &rcTemp, EDGE_ETCHED, BF_TOP);
			break;
		case PPMSGBOX_SEP_BLACK:
			m_pDrawer.GetDrawManager()->DrawLine(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.top, RGB (0, 0, 0));
			break;
		case PPMSGBOX_SEP_WHITE:
			m_pDrawer.GetDrawManager()->DrawLine(hDC, rcTemp.left, rcTemp.top, rcTemp.right, rcTemp.top, RGB (255, 255, 255));
			break;
		} //switch
	} //if
} //End of DrawSeparator
