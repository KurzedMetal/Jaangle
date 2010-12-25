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
#ifndef _CTeenSpiritDlg_h_
#define _CTeenSpiritDlg_h_

#include "TSSkinnable.h"

#include "ToolbarWnd.h"

#include "PaneController.h"

#include "PlayerBarWnd.h"
#include "StatusBarWnd.h"

#include "SystemTray.h"
#include "NewVersionChecker.h"

#define WM_THREADED_NOTIFICATION WM_APP + 100
#define ID_TRAY_SHOWMAINWINDOW 300

// CTeenSpiritDlg dialog
class CTeenSpiritDlg : public CDialog, public TSSkinnable, public TSStateChangeListener
{
// Construction
public:
	CTeenSpiritDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CTeenSpiritDlg();	// standard constructor


// Dialog Data
	enum { IDD = IDD_TEENSPIRIT_DIALOG };


	void OnUpdateFlush(CMenu *pMenu);

	virtual void ApplySkin(ISkin& skin);


	virtual BOOL OnStateChanged(UINT stateMessage);
	virtual LPCTSTR GetStateListenerName()								{return _T("CTeenSpiritDlg");}


protected:// by CDialog 
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK()									{}
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnFinalShowWindow(WPARAM wParam, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	DECLARE_MESSAGE_MAP()

private:
	void ShowTrayMenu(CPoint& pt);
	void ShowTray();
	void HideTray();
	LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnCheckVersion(WPARAM wParam, LPARAM lParam);

	void LoadSettings();
	void SaveSettings();
	void PositionControls(int cx, int cy);

	void MaximizeWindow(RECT& rcNormal);

private:
	static UINT __cdecl NewVersionCheckThreadST(LPVOID pParam);

	CToolbarWnd m_toolBar;
	PaneController m_PaneController;
	CStatusBarWnd m_statusBar;
	CPlayerBarWnd m_playerBar;

	HICON m_hBigIcon;
	HICON m_hSmallIcon;
	
	CSystemTray m_TrayIcon;
	static LPCTSTR SettingsCategory;

	NewVersionChecker::CheckResult m_newVerCheckResult;
	UINT NewVersionCheck();
	INT m_lastUpdateActionsPerformed;
	UINT m_monitorPlayerTick;

	UINT m_paneUpLine;
	UINT m_paneDownLine;

	enum Timers
	{
		TMR_InitialDelay = 500,
		TMR_CheckNewVersion,
		TMR_AutoUpdateDB,
		TMR_HeartBeat,
		TMR_AppMonitor,
		TMR_PlayerMonitor,
		TMR_ShowTrayTooltipOnMouseMove,
		TMR_Last
	};
	CPoint m_ptTrayCursorPos;

};

#endif
