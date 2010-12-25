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
#include "AboutDlg.h"
#include "NewVersionChecker.h"
#include "AppMonitor.h"
#include "ActionManager.h"
//#include "UrlDownloader.h"
#include "PrgAPI.h"

enum WM_Messages
{
	WM_APP_CHECKFORUPDATES = WM_APP + 0x100,
	WM_APP_WEBSITE,
	WM_APP_FORUM,
	WM_APP_DONATE,
	WM_APP_LAST
};





IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{

}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION, m_VersionInfo);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_MESSAGE(WM_APP_CHECKFORUPDATES, CheckForUpdates)
	ON_MESSAGE(WM_APP_WEBSITE, GoToWebsite)
	ON_MESSAGE(WM_APP_FORUM, GoToForum)
	ON_MESSAGE(WM_APP_DONATE, GoToDonate)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);
	
	CString msg = _T("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY.\r\n\r\n")
		_T("---Code fragments---\r\n\r\nCodeProject, CodeGuru etc. by\r\n")
		_T("Alexander Atamas\r\nArmen Hakobyan\r\nBrent Corkum\r\nCarlos Antollini\r\nChris Maunder\r\n")
		_T("Dany Cantin\r\nDavide Calabro\r\nFoss\r\nHans Dietrich\r\nJim Alsup\r\nKeith Rule\r\n")
		_T("Ravi Bhavnani\r\nS.Chan\r\nScott Thomas Haug\r\nSven Wiegand\r\nThijmen Klok\r\nYovav Gad\r\n")
		_T("\r\n---Special thanks---\r\n\r\n")
		_T("XC-SE\r\nArnulf\r\nMiskairal\r\nLeif\r\nAll the people in the forum\r\n\r\n");

	AppMonitor* pAM = PRGAPI()->GetAppMonitor();
	const AppMonitor::State& st = pAM->GetState();
	CString stats;
	stats.Format(
		_T("---Statistics---\r\n\r\n")
		_T("AppUniqueID: 0x%08X\r\n")
		_T("Version Runs: %u\r\n")
		_T("Total Runs: %u\r\n")
		_T("Version Crashes: %u\r\n")
		_T("Total Crashes: %u\r\n")
		_T("Intallation Date: %02d-%02d-%d\r\n")
		_T("Hours Run: %d\r\n")
		_T("IsAfterCrash: %s\r\n\r\n\r\n"),
		st.AppUniqueID,
		st.Runs,
		st.TotalRuns,
		st.Crashes,
		st.TotalCrashes,
		st.InstallationDate.wDay, st.InstallationDate.wMonth, st.InstallationDate.wYear,
		INT(pAM->GetRunningTimeInMinutes() / 60),
		pAM->IsAfterCrash() ? _T("TRUE") : _T("FALSE")
		);
	msg += stats;
	msg += _T("© Alex Economou 2005-2009");


	SetDlgItemText(IDC_MESSAGE, msg);
	//m_VersionInfo.SetAppCommands()
	FileVersion fv;
	if (!GetFileVersion(fv))
		return TRUE;
	TCHAR bf[1000];
	_sntprintf(bf, 1000, _T("<b>%s v%d.%d build %d.%d</b>")
		_T("<br> - <font color=\"#0000ff\"><u><a href=\"app:WM_APP_CHECKFORUPDATES\">Check for updates</a></u></font>.")
		_T("<br> - Visit <font color=\"#0000ff\"><u><a href=\"app:WM_APP_WEBSITE\">Jaangle Website</a></u></font>.")
		_T("<br> - Get help in the <font color=\"#0000ff\"><u><a href=\"app:WM_APP_FORUM\">Forum</a></u></font>.")
		_T("<br> - <font color=\"#0000ff\"><u><a href=\"app:WM_APP_DONATE\">Donate</a></u></font> to support this free project.")
		,
		CTeenSpiritApp::sAppTitle,
		fv.Major, fv.Minor, fv.Build, fv.SubBuild);
	SetDlgItemText(IDC_VERSION, bf);
	m_VersionInfo.SetFont(PRGAPI()->GetFont(FONT_Dialogs));

	XHTMLSTATIC_APP_COMMAND AppCommands[] = 
	{
		{ m_hWnd, WM_APP_CHECKFORUPDATES, 1, _T("WM_APP_CHECKFORUPDATES") },
		{ m_hWnd, WM_APP_WEBSITE, 1, _T("WM_APP_WEBSITE") },
		{ m_hWnd, WM_APP_FORUM, 1, _T("WM_APP_FORUM") },
		{ m_hWnd, WM_APP_DONATE, 2, _T("WM_APP_DONATE") }
	};

	m_VersionInfo.SetAppCommands(AppCommands, 
		sizeof(AppCommands)/sizeof(AppCommands[0]));


	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CAboutDlg::CheckForUpdates(WPARAM wParam, LPARAM lParam)
{
	PRGAPI()->GetActionManager()->CheckForUpdates(this);
	return 0;
}

LRESULT CAboutDlg::GoToWebsite(WPARAM wParam, LPARAM lParam)
{
	PRGAPI()->GoToSiteURL(PrgAPI::SA_Navigate, _T("home"));
	return 0;
}
LRESULT CAboutDlg::GoToForum(WPARAM wParam, LPARAM lParam)
{
	PRGAPI()->GoToSiteURL(PrgAPI::SA_Navigate, _T("forum"));
	return 0;
}
LRESULT CAboutDlg::GoToDonate(WPARAM wParam, LPARAM lParam)
{
	PRGAPI()->GoToSiteURL(PrgAPI::SA_Navigate, _T("donate"));
	return 0;
}

