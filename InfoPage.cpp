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
#include "InfoPage.h"
#include "PrgAPI.h"

#define WM_APP_IMPORTDATAFROMFILE	(WM_APP + 0x100)
// CInfoPage dialog

IMPLEMENT_DYNAMIC(CInfoPage, CTSPropertyPage)

CInfoPage::CInfoPage(UINT nIDCaption)
	: CTSPropertyPage(CInfoPage::IDD, nIDCaption),
	m_bInitialized(FALSE),
	m_bFixedFont(FALSE),
	m_bReadOnly(FALSE)
{

}

CInfoPage::~CInfoPage()
{
}

void CInfoPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_SUBTITLE, m_Static);
	DDX_Control(pDX, IDC_INFO, m_edit);
	CTSPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoPage, CPropertyPage)
	ON_MESSAGE(WM_APP_IMPORTDATAFROMFILE, NotifyHyperlinkClicked)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


BOOL CInfoPage::OnInitDialog()
{
	CTSPropertyPage::OnInitDialog();
	//m_Static.SetBkMode(TRANSPARENT);

	XHTMLSTATIC_APP_COMMAND AppCommands[] = 
	{
		{ m_hWnd, WM_APP_IMPORTDATAFROMFILE, 1, _T("WM_APP_IMPORTDATAFROMFILE") }
	};


	//**** Gia kapoio logo DE exei to sosto Client Rext


	m_Static.SetAppCommands(AppCommands, 
		sizeof(AppCommands)/sizeof(AppCommands[0]));
	//m_Static.SetCallbackHyperlink(UNM_HYPERLINK_CLICKED);
	SetDlgItemText(IDC_INFO, info);
	TCHAR txt[500];
	PrgAPI* pLang = PRGAPI();
	if (!m_bReadOnly)
	{
		_sntprintf(txt, 500, _T("· <a href=\"app:WM_APP_IMPORTDATAFROMFILE\"><u><font color=\"#0000FF\">%s</font></u></a>.<br>· %s."), 
			pLang->GetString(IDS_IMPORTDATADROMFILE),
			pLang->GetString(IDS_WRITEORPASTETEXT));
		m_Static.SetWindowText(txt);
	}
	else
		m_Static.SetWindowText(_T(""));

	m_InfoAtStart = info;
	m_bInitialized = TRUE;
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)pLang->GetFont(FONT_Dialogs)->m_hObject);

	if (m_bFixedFont)
		GetDlgItem(IDC_INFO)->SendMessage(WM_SETFONT, 
			(WPARAM)pLang->GetFont(FONT_Fixed)->m_hObject, FALSE);
	if (m_bReadOnly)
		GetDlgItem(IDC_INFO)->SendMessage(EM_SETREADONLY, 1, 0);


	//m_edit.SetBkColor()

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CInfoPage::NotifyHyperlinkClicked(WPARAM wParam, LPARAM lParam)
{
	CFileDialog fd(TRUE, NULL, NULL, NULL, _T("Text Files (*.txt)|*.txt|All Files (*.*)|*.*|"), this);
	fd.m_ofn.Flags |= OFN_EXPLORER;
	if (fd.DoModal() == IDOK)
	{
		CFile f;
		if (f.Open(fd.GetPathName(), CFile::modeRead))
		{
			UINT len = (UINT)f.GetLength();
			if (len > 0)
			{
				CHAR* txt = new CHAR[len + 1];
				f.Read(txt, len);
				txt[len] = 0;
				SetDlgItemText(IDC_INFO, (LPCTSTR)CA2CT(txt));
				delete txt;
			}
		}
	}
	return 0;
}

void CInfoPage::OnDestroy()
{
	CTSPropertyPage::OnDestroy();
	GetDlgItemText(IDC_INFO, info);

}

BOOL CInfoPage::OnSetActive()
{
	const INT cStaticHeight = 30;
	const INT cMargins = 6;
	CRect rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.InflateRect(-cMargins,-cMargins,-cMargins,-cMargins);
	CRect rcItem(rc);
	rcItem.bottom -= (cMargins + cStaticHeight);
	GetDlgItem(IDC_INFO)->MoveWindow(rcItem);
	rcItem = rc;
	rcItem.top = rcItem.bottom - cStaticHeight;
	GetDlgItem(IDC_SUBTITLE)->MoveWindow(rcItem);

	return CTSPropertyPage::OnSetActive();
}
