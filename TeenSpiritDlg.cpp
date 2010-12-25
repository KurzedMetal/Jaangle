//	/*
// 	*
// 	* Copyright (C) 2003-2010 Alexandros Economou
//	*
//	* This file is part of Jaangle (http://www.jaangle.com)
// 	*
// 	* This Program is free software; you can redistribute it and/or modify
// 	* it under the terms of the GNU General Public License as published by
// 	* the Free Software Foundation; either version 2, or (at your option)
// 	* any later version.
// 	*
// 	* This Program is distributed in the hope that it will be useful,
// 	* but WITHOUT ANY WARRANTY; without even the implied warranty of
// 	* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// 	* GNU General Public License for more details.
// 	*
// 	* You should have received a copy of the GNU General Public License
// 	* along with GNU Make; see the file COPYING. If not, write to
// 	* the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
// 	* http://www.gnu.org/copyleft/gpl.html
// 	*
//	*/ 

#include "stdafx.h"
#include "TeenSpirit.h"
#include "TeenSpiritDlg.h"
#include "AppSettings/AppSettings.h"
#include "ToolbarWnd.h"
#include "ActionManager.h"
#include "MiniPlayerDlg.h"
#include "MessageDlg.h"
#include "PrgAPI.h"
#include "CollectionManager.h"
#include "MediaPlayer.h"
#include "TSCommands.h"
#include "MenuManager.h"
#include "InfoCtrl.h"
#include "SQLManager.h"
//#include "HotKeyManager.h"
#include "Trackers/ITracker.h"
#include "TSOnlineServices.h"
#include "SystemTimeFunctions.h"
#include "ShortcutManager.h"
#include "ApplicationMonitor.h"
#include "AppMonitor.h"
#include "TranslationManager.h"
#include "TrayToolTipDlg.h"
#include "QuickSearchDlg.h"
#include "SearchDlg.h"

#define UM_PLAYERCOMMAND WM_APP + 111
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDM_SHOWMINIPLAYER 0x700 & 0xFFF0






//#define DELAY_TIMER			500
#define DELAY_TIMER_DELAY	50

//#define CHECKNEWVERSION_TIMER			510
#define CHECKNEWVERSION_TIMER_DELAY		60000

//#define AUTOUPDATEDB_TIMER	501
//#define HEARTBEAT_TIMER			502
#define HEARTBEAT_TIMER_DELAY	500
//#define COLLECTIONREFRESH_TIMER			503
#define COLLECTIONREFRESH_TIMER_DELAY	15000
//#define APPMONITOR_TIMER			550
#define APPMONITOR_TIMER_DELAY		1000
//#define PLAYERMONITOR_TIMER			600
#define PLAYERMONITOR_TIMER_DELAY	200



//#define SEARCH_DELAY_TIMER 501
//#define SEARCH_DELAY_TIMER_DELAY 200
#define	WM_ICON_NOTIFY			WM_APP+10


#define IDC_TSDLGSTATUS 201

#define MSG_UPDATECHECKER 200
LPCTSTR CTeenSpiritDlg::SettingsCategory = _T("MAINDLG");

// CTeenSpiritDlg dialog


CTeenSpiritDlg::CTeenSpiritDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeenSpiritDlg::IDD, pParent),
	m_lastUpdateActionsPerformed(0),
	m_monitorPlayerTick(0),
	m_paneUpLine(0),
	m_paneDownLine(0)
{
	m_hBigIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_LOADTRANSPARENT);//LR_LOADTRANSPARENT LR_VGACOLOR
	m_hSmallIcon = (HICON)::LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_LOADTRANSPARENT);//LR_LOADTRANSPARENT LR_VGACOLOR
//	PRGAPI()->RegisterControl(this);

}

CTeenSpiritDlg::~CTeenSpiritDlg()
{
	DestroyIcon(m_hBigIcon);
	DestroyIcon(m_hSmallIcon);
	if (m_hWnd) DestroyWindow();
}

void CTeenSpiritDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LISTVIEW, m_ListCtrl);
	//DDX_Control(pDX, IDC_ENHLISTCTRL, m_EnhTrackListCtrl);
	//DDX_Control(pDX, IDC_PLAYLIST, m_PlayList);
}

BEGIN_MESSAGE_MAP(CTeenSpiritDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_WM_GETMINMAXINFO()
	ON_MESSAGE(WM_ICON_NOTIFY, OnTrayNotify)
	ON_MESSAGE(MSG_UPDATECHECKER, OnCheckVersion)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_COPYDATA()
	ON_WM_ENDSESSION()
END_MESSAGE_MAP()


enum PlayerCommands
{
	PC_Play,
	PC_Pause,
	PC_Stop,
	PC_Next,
	PC_Previous,
	PC_GetPlayState,	//returns PlayStateEnum
	PC_GetMediaLength,	//returns Media Length in sec
	PC_GetMediaPos,		//returns Media Position in sec
	PC_GetHiddenInfoWindow,
	PC_GetMute,			//0 means Mute is disabled
	PC_SetMute,			//if lParam == 0 --> Disable Mute
	PC_GetVolume,		//returns 0-100 volume
	PC_SetVolume,		//lParam is value 0-100
	PC_GetPlayListCount,//Returns The tracks in playlist
	PC_GetCurrentPlaylistPosition//Returns the position starting from 0
	//PC_ShowPlayer		//if lParam == 0 --> Hide else Show
};

void CTeenSpiritDlg::ApplySkin(ISkin& _skin)
{
	CRect rc;
	GetClientRect(&rc);
	PositionControls(rc.Width(), rc.Height());
}



BOOL CTeenSpiritDlg::OnInitDialog()
{
	TRACEST(_T("CTeenSpiritDlg::OnInitDialog"));
	CDialog::OnInitDialog();

	PrgAPI* pAPI = PRGAPI();
	ShortcutManager* pSM = pAPI->GetShortcutManager();
	pSM->Init(m_hWnd);

	AppSettings* appSettings = PRGAPI()->GetAppSettings();
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->AppendMenu(MF_STRING, IDM_SHOWMINIPLAYER, pAPI->GetString(IDS_SHOWMINIPLAYER));
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, pAPI->GetString(IDS_ABOUT));
	}
	TCHAR title[100];
	FileVersion fv;
	GetFileVersion(fv);
	_sntprintf(title, 100, _T("%s %d.%de"), CTeenSpiritApp::sAppTitle, fv.Major, fv.Minor);
#ifdef _DEBUG
	_sntprintf(&title[_tcslen(title)], 100, _T(" [DEBUG: %d]"), fv.Build);
#endif
	SetWindowText(title);

	BCMenu::SetMenuDrawMode(1);

	SetIcon(m_hBigIcon, TRUE);			// Set big icon
	SetIcon(m_hSmallIcon, FALSE);		// Set small icon
		

	m_toolBar.Create(WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, 772);
	m_playerBar.SetMediaPlayer(pAPI->GetMediaPlayer());
	m_playerBar.Create(WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, 773);
	m_statusBar.Create(WS_VISIBLE | WS_CHILDWINDOW, CRect(0,0,0,0), this, 774);


	pAPI->SetNotificationTarget(&m_statusBar);

	m_PaneController.Init(this);

	//HotKeyManager* pHKM = pAPI->GetHotKeyManager();
	//pHKM->Init();//m_hWnd);

	SetTimer(TMR_InitialDelay, DELAY_TIMER_DELAY, NULL);
	if (pAPI->GetOption(OPT_GEN_CheckForUpdates))
		SetTimer(TMR_CheckNewVersion, CHECKNEWVERSION_TIMER_DELAY, NULL);
	SetTimer(TMR_HeartBeat, HEARTBEAT_TIMER_DELAY, NULL);
	SetTimer(TMR_AppMonitor, APPMONITOR_TIMER_DELAY, NULL);
	//SetTimer(TMR_PlayerMonitor, PLAYERMONITOR_TIMER_DELAY, NULL);
	LoadSettings();

	if (pAPI->GetOption(OPT_GEN_AutoUpdateDB) != 0)
		SetTimer(TMR_AutoUpdateDB, pAPI->GetOption(OPT_GEN_AutoUpdateDBDelay) * 1000, NULL);

	return FALSE;
}

void CTeenSpiritDlg::MaximizeWindow(RECT& rcNormal)
{
	ModifyStyle(WS_CAPTION | WS_THICKFRAME, WS_MAXIMIZE, 0);// SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	wp.flags = 0;
	wp.ptMaxPosition.x = wp.ptMinPosition.x = wp.ptMaxPosition.y = wp.ptMinPosition.y = 0;
	wp.rcNormalPosition = rcNormal;
	wp.showCmd = SW_SHOWMAXIMIZED;
	SetWindowPlacement(&wp);
	HMONITOR mon = ::MonitorFromRect(&wp.rcNormalPosition, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi;
	mi.cbSize = sizeof( mi );
	::GetMonitorInfo( mon, &mi );
	SetWindowPos(NULL, 
		mi.rcWork.left, 
		mi.rcWork.top, 
		mi.rcWork.right - mi.rcWork.left, 
		mi.rcWork.bottom - mi.rcWork.top, 
		SWP_NOZORDER|SWP_SHOWWINDOW);
}

void CTeenSpiritDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	PrgAPI* pAPI = PRGAPI();
	int nnID = nID & 0xFFF0;
	//static LONG s_normalStyle = 0;
	switch (nnID)
	{
	case IDM_SHOWMINIPLAYER:
		pAPI->GetMiniPlayerDlg(TRUE)->Show();
		break;
	case IDM_ABOUTBOX:
		pAPI->GetActionManager()->ShowAboutDlg(this);
		break;
	case SC_MINIMIZE:
		CDialog::OnSysCommand(nID, lParam);
		if (pAPI->GetOption(OPT_GEN_MinimizeToTray))
		{
			ShowTray();
			ShowWindow(SW_HIDE);
		}
		if (pAPI->GetOption(OPT_MINIPLR_Automatic) == 1)//Automatic
			pAPI->GetMiniPlayerDlg(TRUE)->Show();
		break;
	case SC_MAXIMIZE:
		{
			CRect rc;
			GetWindowRect(&rc);
			MaximizeWindow(rc);
			if (pAPI->GetOption(OPT_MINIPLR_Automatic) == 1)//Automatic
			{
				CMiniPlayerDlg* pMPDlg = pAPI->GetMiniPlayerDlg(FALSE);
				if (pMPDlg != NULL)
					pMPDlg->Hide();
			}
		}
		break;
	case SC_RESTORE:
		{
			DWORD style = GetWindowLong(GetSafeHwnd(), GWL_STYLE);
			if ((style & WS_VISIBLE) == WS_VISIBLE)
			{
				ModifyStyle(0, WS_OVERLAPPEDWINDOW, 0);
				CDialog::OnSysCommand(nID, lParam);
			}
			else
			{
				HideTray();
				WINDOWPLACEMENT wp;
				GetWindowPlacement(&wp);
				if ((wp.flags & WPF_RESTORETOMAXIMIZED) == WPF_RESTORETOMAXIMIZED)
					MaximizeWindow(wp.rcNormalPosition);
				else
					ShowWindow(SW_RESTORE);
				if (pAPI->GetOption(OPT_MINIPLR_Automatic) == 1)//Automatic
				{
					CMiniPlayerDlg* pMPDlg = pAPI->GetMiniPlayerDlg(FALSE);
					if (pMPDlg != NULL)
						pMPDlg->Hide();
				}
			}
			SetForegroundWindow();
			UpdateWindow();
			Invalidate(TRUE);
			if (pAPI->GetOption(OPT_MINIPLR_Automatic) == 1)//Automatic
			{
				CMiniPlayerDlg* pMPDlg = pAPI->GetMiniPlayerDlg(FALSE);
				if (pMPDlg != NULL)
					pMPDlg->Hide();
			}

		}
		break;
	default:
		//if (nID == MENU_ToggleMiniPlayer)
		//	pAPI->GetActionManager()->ShowMiniPlayer(!pAPI->GetActionManager()->IsMiniPlayerVisible());
		//else
			CDialog::OnSysCommand(nID, lParam);
	}
}

//void CTeenSpiritDlg::MinimizeWindow()
//{
//	if (PRGAPI()->GetOption(OPT_GEN_MinimizeToTray))
//	{
//		ShowTray();
//		ShowWindow(SW_HIDE);
//	}
//	PRGAPI()->GetActionManager()->UpdateMiniPlayerOptions();
//}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTeenSpiritDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hSmallIcon);
	}
	else
	{
		CPaintDC dc(this);
		CPen* pOldPen = (CPen*) dc.SelectStockObject(DC_PEN);
		CRect rc;
		GetClientRect(&rc);

		TSSkin& skin =  PRGAPI()->GetSkinManager()->GetSkin();
		COLORREF col1 = skin.GetColor(CSEC_Global, CVAL_BorderLine);

		dc.SetDCPenColor(col1);

		dc.MoveTo(0, m_paneUpLine);
		dc.LineTo(rc.right, m_paneUpLine);

		dc.MoveTo(0, m_paneDownLine );
		dc.LineTo(rc.right, m_paneDownLine);

		dc.SelectObject(pOldPen);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTeenSpiritDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hSmallIcon);
}

void CTeenSpiritDlg::OnDestroy()
{
	TRACEST(_T("CTeenSpiritDlg::OnDestroy"));
	SaveSettings();

	PrgAPI* pAPI = PRGAPI();
	//pAPI->UnRegisterReceiver(m_playerBar);
	//pAPI->UnRegisterReceiver(*this);
	pAPI->SetNotificationTarget(NULL);//Disable Notifications
	m_TrayIcon.RemoveIcon();
	pAPI->DestroyMiniPlayerDlg();
	pAPI->DestroyQuickSearchDlg();
	pAPI->DestroyTrayToolTipDlg();
	pAPI->DestroyAdvancedSearchDlg();
	//pAPI->GetSkinManager()->UnRegisterSkinnable(m_ListCtrl);
	CDialog::OnDestroy();
}

void CTeenSpiritDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (m_toolBar.m_hWnd!=NULL)
		PositionControls(cx, cy);
}

void CTeenSpiritDlg::PositionControls(int cx, int cy)
{
	TSSkin& skin = PRGAPI()->GetSkinManager()->GetSkin();
	UINT cToolBarHeight = skin.GetIntParameter(CSEC_Toolbar, IPARAM_Height) + skin.GetRelativeFontSize();
	UINT cStatusBarHeight = skin.GetIntParameter(CSEC_StatusBar, IPARAM_Height) + skin.GetRelativeFontSize();
	UINT cPlayerBarHeight = skin.GetIntParameter(CSEC_PlayerBar, IPARAM_Height) + skin.GetRelativeFontSize();

	m_toolBar.MoveWindow(0,0,cx, cToolBarHeight, TRUE);
	m_paneUpLine = cToolBarHeight;
	m_paneDownLine = cy - cStatusBarHeight - cPlayerBarHeight - 1;
	m_playerBar.MoveWindow(0,cy - cStatusBarHeight - cPlayerBarHeight, cx, cPlayerBarHeight, TRUE);
	m_statusBar.MoveWindow(0,cy - cStatusBarHeight, cx, cStatusBarHeight, TRUE);
	m_PaneController.SetPosition(0, m_paneUpLine + 1, cx, m_paneDownLine - m_paneUpLine - 1);//It waS FALSE
	Invalidate(FALSE);
}

BOOL CTeenSpiritDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CTeenSpiritDlg::OnCommand"), wParam);
	PrgAPI* pAPI = PRGAPI();

	switch (wParam)
	{
	case CToolbarWnd::CMD_About:
		return pAPI->GetActionManager()->ShowAboutDlg(this);
	case CToolbarWnd::CMD_Collections:
		return pAPI->GetActionManager()->ShowCollectionsDlg(this);
	case CToolbarWnd::CMD_Search:
		return pAPI->GetAdvancedSearchDlg()->ShowWindow(SW_SHOW);
	case CToolbarWnd::CMD_History:
		return pAPI->GetActionManager()->ShowHistoryDlg(this);
	case CToolbarWnd::CMD_Options:
		return pAPI->GetActionManager()->ShowOptionsDlg(this);
	case CToolbarWnd::CMD_Games:
		return pAPI->GetActionManager()->ShowGamesDlg(this);

	case CToolbarWnd::CMD_Min:
		return SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	case CToolbarWnd::CMD_Restore:
		return SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
	case CToolbarWnd::CMD_Close:
		return SendMessage(WM_SYSCOMMAND, SC_CLOSE, 0);
		break;

	default:
		{
			if (wParam > MENU_First && wParam < MENU_Last)
			{
				MenuManager* pMM = pAPI->GetMenuManager();
				MenuCommandsEnum cmd = (MenuCommandsEnum) wParam;
				if (pMM->HandleGeneralCommands(cmd) || pMM->HandlePlayerCommands(*pAPI->GetMediaPlayer(), cmd, 0))
					return TRUE;
			}
		}
		return CDialog::OnCommand(wParam, lParam);
	}
	return TRUE;
}


void CTeenSpiritDlg::OnTimer(UINT nIDEvent)
{
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	switch (nIDEvent)
	{
	case TMR_InitialDelay:
		SetTimer(TMR_PlayerMonitor, PLAYERMONITOR_TIMER_DELAY, NULL);
		pAPI->GetTrayToolTipDlg();
		KillTimer(nIDEvent);
		{
			TracksFilter tf;
			if (pSM->GetTrackCount(tf) == 0)
			{
				TCHAR url[1000];
				pAPI->GetSiteURL(url, 1000, PrgAPI::SA_Help, _T("emptycollection"));
				CMessageDlg dlg(this);
				dlg.SetText(CMessageDlg::TT_Title, pAPI->GetString(IDS_EMPTYCOLLECTION_TITLE));
				dlg.SetText(CMessageDlg::TT_SubTitle, pAPI->GetString(IDS_EMPTYCOLLECTION_DETAIL));
				dlg.SetHelpLink(pAPI->GetString(IDS_HELP_S), url);
				dlg.SetText(CMessageDlg::TT_ButtonRight, pAPI->GetString(IDS_OK));
				switch (dlg.DoModal())
				{
				case CMessageDlg::RES_Close:
					pAPI->GoToSiteURL(PrgAPI::SA_Help, _T("emptycollection"));
					break;
				case CMessageDlg::RES_Right:
					pAPI->GetActionManager()->ShowAddNewCollectionDlg(this);
					UpdateWindow();
					break;
				}
			}
			if (pAPI->GetOption(OPT_GEN_UseNativeOnlineServices) == -1)
			{
				CMessageDlg dlg(this);
				
				dlg.SetText(CMessageDlg::TT_Title, pAPI->GetString(IDS_OPTIONS));
				dlg.SetText(CMessageDlg::TT_SubTitle, pAPI->GetString(IDS_CONFIRMATION));
				TCHAR bf[1000];
				_sntprintf(bf, 1000, _T("%s (beta)"), pAPI->GetString(IDS_TSONLINESERVICES));
				dlg.SetText(CMessageDlg::TT_CheckBox, bf);
				pAPI->GetSiteURL(bf, 1000, PrgAPI::SA_Navigate, _T("onlineservices-about"));
				dlg.SetHelpLink(pAPI->GetString(IDS_HELP_S), bf);
				dlg.SetCheck(TRUE);
				dlg.SetText(CMessageDlg::TT_ButtonRight, pAPI->GetString(IDS_OK));
				if (dlg.DoModal() != CMessageDlg::RES_Close)
					pAPI->SetOption(OPT_GEN_UseNativeOnlineServices, dlg.IsChecked() ? 1 : 0);
			}
			MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
			m_playerBar.SetMediaPlayer(pPlayer);
			if (pAPI->GetOption(OPT_MINIPLR_ShowAtStartup) != 0)
				pAPI->GetMiniPlayerDlg(TRUE)->Show();
		}
		break;
	case TMR_CheckNewVersion:
		KillTimer(nIDEvent);
		if (pAPI->GetOption(OPT_GEN_CheckForUpdates))
		{
			PrgAPI* pAPI = PRGAPI();
			SYSTEMTIME st;
			GetLocalTime(&st);
			DOUBLE thisDay;
			SystemTimeToVariantTime(&st, &thisDay);
			if (INT(thisDay) - pAPI->GetOption(OPT_GEN_CheckLastCheck) >= pAPI->GetOption(OPT_GEN_DaysBetweenCheck))
			{
				pAPI->SetOption(OPT_GEN_CheckLastCheck, INT(thisDay));
				AfxBeginThread(&NewVersionCheckThreadST, (void*)this ,0 ,0 ,0);
			}
		}
		break;
	case TMR_AutoUpdateDB:
		KillTimer(nIDEvent);
		if (pAPI->GetOption(OPT_GEN_AutoUpdateDB) != 0)
			pAPI->GetCollectionManager()->RefreshLocalCollections(TRUE, FALSE, TRUE);
		break;
	//case COLLECTIONREFRESH_TIMER:
	//	{
	//		PrgAPI* pAPI = PRGAPI();
	//		CollectionManager* pCM = pAPI->GetCollectionManager();
	//		if (pCM->GetState() == CollectionManager::S_Working)
	//		{
	//			const CollectionUpdateJob::Progress prog = pCM->GetProgress();
	//			INT curVal = prog.curPos;
	//			if (curVal - m_lastUpdateActionsPerformed > 100)
	//			{
	//				m_lastUpdateActionsPerformed = curVal;
	//			}
	//		}
	//		else
	//			KillTimer(nIDEvent);
	//	}
	//	break;
	case TMR_HeartBeat://Used by BaseDirector for Send/Post Messages
		pAPI->GetStateManager()->HeartBeat();
		break;
	case TMR_AppMonitor://Used for History Tracking, Download Status Update etc
		pAPI->Monitor();
		break;
	case TMR_PlayerMonitor:
		{
			MediaPlayer::Changes changes = pAPI->GetMediaPlayer()->GetChanges();
			if (changes.tickPlayList >= m_monitorPlayerTick)
				pAPI->GetStateManager()->SendMessage(SM_PlayListChanged);
			if (changes.tickMediaChanged >= m_monitorPlayerTick)
				pAPI->GetStateManager()->SendMessage(SM_MediaChanged);
			m_monitorPlayerTick = GetTickCount();
		}
		break;
	case TMR_ShowTrayTooltipOnMouseMove:
		{
			KillTimer(nIDEvent);
			CPoint ptCurPos;
			GetCursorPos(&ptCurPos);
			if (ptCurPos == m_ptTrayCursorPos)
			{
				pAPI->GetTrayToolTipDlg()->OnMouseOverTray(ptCurPos);
			}
		}
		break;
	default:
		CDialog::OnTimer(nIDEvent);
		break;
	}

}


void CTeenSpiritDlg::LoadSettings()
{
	AppSettings* params = PRGAPI()->GetAppSettings();

	WINDOWPLACEMENT pl;
	GetWindowPlacement(&pl);
	int showCmd;
	params->Read(SettingsCategory, _T("CMDSHOW"), showCmd, SW_SHOWNORMAL);
	params->Read(SettingsCategory, _T("LEFT"), (int&) pl.rcNormalPosition.left, 40);
	params->Read(SettingsCategory, _T("TOP"), (int&) pl.rcNormalPosition.top, 25);
	params->Read(SettingsCategory, _T("RIGHT"), (int&) pl.rcNormalPosition.right, 900);
	params->Read(SettingsCategory, _T("BOTTOM"), (int&) pl.rcNormalPosition.bottom, 650);
	//pl.showCmd = SW_SHOWMINIMIZED;
	pl.showCmd = showCmd;
	ModifyStyle(WS_VISIBLE, 0);
	if (showCmd == SW_SHOWMINIMIZED)
	{
		ModifyStyle(WS_VISIBLE, 0);
		SetWindowPlacement(&pl);
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	}
	else if (showCmd == SW_SHOWMAXIMIZED)
	{
		SetWindowPlacement(&pl);
		ModifyStyle(WS_CAPTION | WS_THICKFRAME, 0);
		MaximizeWindow(pl.rcNormalPosition);

	}
	else
		SetWindowPlacement(&pl);
}

void CTeenSpiritDlg::SaveSettings()
{
	PrgAPI* pAPI = PRGAPI();
	AppSettings* params = pAPI->GetAppSettings();
	WINDOWPLACEMENT pl;
	GetWindowPlacement(&pl);
	params->Write(SettingsCategory, _T("CMDSHOW"), (int&) pl.showCmd);
	params->Write(SettingsCategory, _T("LEFT"), (int&) pl.rcNormalPosition.left);
	params->Write(SettingsCategory, _T("TOP"), (int&) pl.rcNormalPosition.top);
	params->Write(SettingsCategory, _T("RIGHT"), (int&) pl.rcNormalPosition.right);
	params->Write(SettingsCategory, _T("BOTTOM"), (int&) pl.rcNormalPosition.bottom);
	m_PaneController.SaveSettings();
	BOOL bIsPlayerVisible = FALSE;
	CMiniPlayerDlg* pMPDlg = pAPI->GetMiniPlayerDlg(FALSE);
	if (pMPDlg != NULL)
		bIsPlayerVisible = pMPDlg->IsWindowVisible();
	pAPI->SetOption(OPT_MINIPLR_ShowAtStartup, bIsPlayerVisible);

}

BOOL CTeenSpiritDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 'F')
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				QuickSearchDlg* pQS = PRGAPI()->GetQuickSearchDlg();
				pQS->SetSearchText(_T(""));
				pQS->ShowRelative(this, QuickSearchDlg::RP_Centered);
				return TRUE;
			}
		}

	}
	switch (pMsg->message)
	{
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:
		//TRACE(_T("CTeenSpiritDlg::PreTranslateMessage. KeyDown: %d\r\n"), pMsg->wParam);
		switch(pMsg->wParam)
		{
		case VK_ESCAPE:
		case VK_RETURN:
			return FALSE;
		}
		break;

	}
	return __super::PreTranslateMessage(pMsg);
}

void CTeenSpiritDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize = CPoint(630,430);
	__super::OnGetMinMaxInfo(lpMMI);
}


void CTeenSpiritDlg::ShowTrayMenu(CPoint& pt)
{

	PrgAPI* pAPI = PRGAPI();
	MenuManager* pMM = pAPI->GetMenuManager();
	ITSMenu& trayMenu = pMM->GetMenu(MenuManager::MNU_Tray);
	pMM->PrepareMenu(MenuManager::MNU_Tray);
	pMM->PreparePlayerCommands(*pAPI->GetMediaPlayer());
	//=== This handling is done because otherwise the menu will not disappear when the user clicks outside
	SetForegroundWindow();
	trayMenu.TrackPopupMenu(pt.x, pt.y, GetSafeHwnd());
	PostMessage(WM_NULL, 0, 0);
}

void CTeenSpiritDlg::ShowTray()
{
	if (m_TrayIcon.m_hWnd == 0)
	{//Create it
		if (!m_TrayIcon.Create(
			this,                           // Let icon deal with its own messages
			WM_ICON_NOTIFY,                 // Icon notify message to use
			CTeenSpiritApp::sAppTitle,		// tooltip
			m_hSmallIcon,
			IDR_MAINFRAME,                 // ID of tray icon
			FALSE,
			NULL, 
			NULL,//CTeenSpiritApp::sAppTitle,
			NIIF_NONE,
			0,
			FALSE))
		{
			return;
		}
	}
	else
		m_TrayIcon.ShowIcon();
	//ShowWindow(FALSE);
}

void CTeenSpiritDlg::HideTray()
{
	if (m_TrayIcon.m_hWnd)
		m_TrayIcon.HideIcon();
}
LRESULT CTeenSpiritDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam) 
{ 
	if (wParam != IDR_MAINFRAME)
		return 0;
	switch (lParam ) 
	{ 
	case WM_MOUSEMOVE:
		{
			CPoint p;
			GetCursorPos(&p);
			if (m_ptTrayCursorPos != p)
			{
				m_ptTrayCursorPos = p;
				KillTimer(TMR_ShowTrayTooltipOnMouseMove);
				SetTimer(TMR_ShowTrayTooltipOnMouseMove, 100, NULL);
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONDOWN:
		PostMessage(WM_SYSCOMMAND, SC_RESTORE);
		break;
	case WM_CONTEXTMENU:
		break;
	case WM_RBUTTONUP:
		{
			CPoint pt;	
			GetCursorPos(&pt);
			ShowTrayMenu(pt);
		}
		break;
	} 
	return 0; 
} 

LRESULT CTeenSpiritDlg::OnCheckVersion(WPARAM wParam, LPARAM lParam) 
{
	if (m_newVerCheckResult.error == NewVersionChecker::ERR_OK)
	{
		PrgAPI* pAPI = PRGAPI();
		FileVersion curVer;
		GetFileVersion(curVer);
		if (m_newVerCheckResult.stableVersion > curVer)
		{
			CMessageDlg dlg(this);
			dlg.SetText(CMessageDlg::TT_SubTitle, pAPI->GetString(IDS_UPDATEAVAILABLE));
			dlg.SetText(CMessageDlg::TT_ButtonRight, pAPI->GetString(IDS_NO_S));
			dlg.SetText(CMessageDlg::TT_ButtonMid, pAPI->GetString(IDS_YES_S));
			switch (dlg.DoModal())
			{
			case CMessageDlg::RES_Mid:
				ShellExecute(m_hWnd, _T("open"), m_newVerCheckResult.infoURL.c_str(), 0,0,0);
				break;
			}
		}
		else
			pAPI->NotifyUser(IDS_UPDATENOTAVAILABLE,0,SEV_InfoL1);

	}
	return 0;
}

BOOL CTeenSpiritDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;//__super::OnEraseBkgnd(pDC);
}

LRESULT CTeenSpiritDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_GETTEXT && wParam > GTB_First && wParam < GTB_Last && lParam != 0)//External commands interface
	{
		INT ret = 0;
		LPCTSTR str = NULL;
		LPTSTR externalBuffer = (LPTSTR) lParam;
		externalBuffer[0] = 0;
		MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
		INT curTrack = pPlayer->GetPlayListPos();
		if (curTrack >= 0)
		{
			MediaPlayListItem mpli;
			if (pPlayer->GetPlayListItem(mpli, curTrack))
			{
				switch (wParam)
				{
				case GTB_Artist:
					str = mpli.artist;
					break;
				case GTB_Title:
					str = mpli.title;
					break;
				default:
					ASSERT(0);
					break;
				}
			}
		}
		if (str == NULL)
			return 0;
		_tcsncpy(externalBuffer, str, wParam);
		return _tcslen(str);
	}
	else if (message == WM_COMMAND)//External commands interface
	{
		MediaPlayer* pPlayer = PRGAPI()->GetMediaPlayer();
		if (pPlayer != NULL)
		{
			BOOL bHandled = TRUE;
			switch (wParam)
			{
			case EXTERNALCOMMAND_Play:			//Returns 0 on success
				pPlayer->Start();
				break;
			case EXTERNALCOMMAND_Pause:			//Returns 0 on success
				pPlayer->Pause();
				break;
			case EXTERNALCOMMAND_Stop:			//Returns 0 on success
				pPlayer->Stop();
				break;
			case EXTERNALCOMMAND_Next:			//Returns 0 on success
				pPlayer->Next();
				break;
			case GTB_Artist:
			case EXTERNALCOMMAND_Previous:		//Returns 0 on success
				pPlayer->Previous();
				break;
			case EXTERNALCOMMAND_ShowHidePlayer:	//lparam=1 to show. 0 to hide. Returns 0 on success
				if (IsWindowVisible() && lParam == 0)
					PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
				else if (!IsWindowVisible() && lParam == 1)
					PostMessage(WM_SYSCOMMAND, SC_RESTORE);
				break;
			case EXTERNALCOMMAND_GetVolume:		//Return vol 0-100. Returns 0-100 on success
				return pPlayer->GetVolume();
			case EXTERNALCOMMAND_SetVolume:		//lParam is the new value(0-100) retunrns 0 on success
				pPlayer->SetVolume(lParam);
				PRGAPI()->GetStateManager()->SendMessage(SM_PlayerVolumeChanged);
				break;
			case EXTERNALCOMMAND_GetMute:		//Returns 1 on mute ON. 0 on Mute off
				return pPlayer->GetMute();
			case EXTERNALCOMMAND_SetMute:		//lparam=1 for mute ON. 0  Returns 0 on success
				pPlayer->SetMute(lParam != 0);
				PRGAPI()->GetStateManager()->PostMessage(SM_PlayerSettingsChanged);
				break;
			case EXTERNALCOMMAND_GetMediaLength:	//returns the number of seconds
				return INT(pPlayer->GetMediaLength());
			case EXTERNALCOMMAND_GetMediaPos:	//returns the number of seconds
				return INT(pPlayer->GetMediaPos());
			case EXTERNALCOMMAND_SetMediaPos:		//lparam=1 for mute ON. 0  Returns 0 on success
				pPlayer->SetMediaPos(lParam);
				break;
			default:
				bHandled = FALSE;
			}
			if (bHandled)
				return 0;
		}
	}
	else if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
	{
		TRACE(_T("CTeenSpiritDlg::WindowProc. KeyDown: %d-%d\r\n"), wParam, lParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


BOOL CTeenSpiritDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct->dwData == COMMANDLINE_ARG)
	{
		static BOOL bEnqueue;
		if (pCopyDataStruct->cbData != 0)
		{
			LPCTSTR path = (LPCTSTR)pCopyDataStruct->lpData;
			if (_tcsicmp(path, _T("-q")) == 0)
				bEnqueue = TRUE;
			if (path[0] != '-' && path[0] !='\\')
			{
				PrgAPI* pAPI = PRGAPI();
				MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
				MediaPlayListItem mpli;
				if (bEnqueue)
					pPlayer->Enqueue(path);
				else
					pPlayer->Play(path);
				bEnqueue = TRUE;
			}
		}
		else
			bEnqueue = FALSE;

	}
	return __super::OnCopyData(pWnd, pCopyDataStruct);
}


UINT CTeenSpiritDlg::NewVersionCheckThreadST(LPVOID pParam)
{
	ASSERT(pParam != NULL);
	DWORD ret = 0;
	if (pParam != NULL)
		ret = ((CTeenSpiritDlg*)pParam)->NewVersionCheck();
	return ret;
}

UINT CTeenSpiritDlg::NewVersionCheck()
{
	PrgAPI* pAPI = PRGAPI();
	std::string sysID;
	if (pAPI->GetURLSystemID(sysID))
	{
		m_newVerCheckResult = NewVersionChecker::CheckForNewVersion(pAPI->GetInternetHandle(), sUpdateURL, (LPCTSTR)CA2CT(sysID.c_str()));
		PostMessage(MSG_UPDATECHECKER);
	}
	return 0;
}

void CTeenSpiritDlg::OnEndSession(BOOL bEnding)
{
	if (bEnding)
	{
		TRACEST(_T("CTeenSpiritDlg::OnEndSession"));
		PrgAPI* pAPI = PRGAPI();
		pAPI->GetAppMonitor()->StopMonitoring();
		pAPI->GetMediaPlayer()->Pause();
		SaveSettings();
		pAPI->SaveSettings();
	}
	else
		TRACE(_T("@D CTeenSpiritDlg::OnEndSession (FALSE)\r\n"));
	__super::OnEndSession(bEnding);
}

BOOL CTeenSpiritDlg::OnStateChanged(UINT stateMessage)
{
	if (stateMessage == SM_MediaChanged)
	{
		MediaPlayListItem mpli;
		PrgAPI* pAPI = PRGAPI();
		MediaPlayer* pMP = pAPI->GetMediaPlayer();
		INT nPos = pMP->GetPlayListPos();
		if (nPos != -1 && pMP->GetPlayListItem(mpli, nPos))
		{
			if (mpli.artist == NULL) mpli.artist = _T("");
			if (mpli.title == NULL) mpli.title = mpli.url;
			//=== Show Tray ToolTip
			//pAPI->GetTrayToolTipDlg()->OnMediaChanged(pAPI->GetOption(OPT_GEN_ShowToolTipOnMediaChanged));

			//====      NOTIFIERS
			TrackerTrack track;
			track.artist = mpli.artist;
			track.title = mpli.title;
			track.rating = mpli.rating;
			track.length = (UINT)pMP->GetMediaLength();
			pAPI->GetTracker()->SubmitTrack(ITracker::ACT_Started, track, GetCurrentUnixTimeStamp());
		}
		return TRUE;
	}

	//	case SM_MediaChanged:
	//		{
	//			MediaPlayer* pPlayer = pAPI->GetMediaPlayer();
	//			INT curMediaIdx = pPlayer->GetPlayListPos();
	//			if (curMediaIdx >= 0)
	//			{
	//				MediaPlayListItem mpli;
	//				if (pPlayer->GetPlayListItem(mpli, curMediaIdx))
	//				{
	//					//====      NOTIFIERS
	//					if (mpli.artist == NULL) mpli.artist = _T("");
	//					if (mpli.title == NULL) mpli.title = mpli.url;
	//					TrackerTrack track;
	//					track.artist = mpli.artist;
	//					track.title = mpli.title;
	//					track.rating = mpli.rating;
	//					track.length = (UINT)pPlayer->GetMediaLength();
	//					pAPI->GetTracker()->SubmitTrack(ITracker::ACT_Started, track, GetCurrentUnixTimeStamp());
	//					//====      Info Control
	//					if (pAPI->GetOption(OPT_VIEWER_Auto))// m_ViewerPane.GetAutoViewerMode())
	//					{
	//						TrackTypesEnum tt = GetTrackType(mpli.url);
	//						if (tt < TTYPE_FIRSTVIDEO)
	//						{
	//							m_ViewerPane.SetMode(CViewerPane::MOD_NormalInfo);
	//						}
	//						else if (tt < TTYPE_FIRSTVIRTUAL)
	//						{
	//							m_ViewerPane.SetMode(CViewerPane::MOD_Visualizations);
	//						}
	//						else
	//						{
	//							m_ViewerPane.SetMode(CViewerPane::MOD_Browser);
	//						}
	//					}
	//					if (m_ViewerPane.GetMode() == CViewerPane::MOD_NormalInfo && pAPI->GetOption(OPT_VIEWER_SyncWithPlayer))
	//					{
	//						MediaPlayListItem mpli;
	//						if (pPlayer->GetPlayListItem(mpli, curMediaIdx))
	//						{
	//							FullTrackRecordSP rec;
	//							if (pAPI->GetSQLManager()->GetFullTrackRecordByLocation(rec, mpli.url))
	//							{
	//								CInfoCtrl* pIC = m_ViewerPane.GetInfoControl();
	//								if (pIC != NULL)
	//								{
	//									pIC->SetActiveItem(rec);
	//									pIC->SetStyle(IT_Track);
	//									pIC->Refresh(100);
	//								}
	//							}
	//						}
	//					}
	//				}
	//	
	//			}
	//
	//		}
	//
	//		break;
	return FALSE;
}

