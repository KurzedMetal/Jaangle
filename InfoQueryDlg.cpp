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
#include "InfoQueryDlg.h"
//#include "Language.h"
#include "PrgAPI.h"


// CInfoQuery dialog

IMPLEMENT_DYNAMIC(CInfoQueryDlg, CDialog)

CInfoQueryDlg::CInfoQueryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoQueryDlg::IDD, pParent),
	m_bkColor(RGB(255,220,150)),
	m_title(NULL),
	m_subTitle(NULL),
	m_helpURL(NULL),
	m_bFixedFont(FALSE)
{
	m_EditBrush = CreateSolidBrush(m_bkColor);
}

CInfoQueryDlg::~CInfoQueryDlg()
{
	DeleteObject(m_EditBrush);
}

void CInfoQueryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoQueryDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CInfoQueryDlg::SetExtraInfo(LPCTSTR title, LPCTSTR subTitle, LPCTSTR helpURL)
{
	m_title = title;
	m_subTitle = subTitle;
	m_helpURL = helpURL;
}

void CInfoQueryDlg::SetMainText(const std::tstring& mainText)
{
	ASSERT(m_hWnd == NULL);
	m_mainText = mainText;
}

BOOL CInfoQueryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	PrgAPI* pAPI = PRGAPI();
	if (m_title != NULL)
		SetWindowText(m_title);
	SetDlgItemText(IDC_ACCEPT, pAPI->GetString(IDS_ACCEPT));
	SetDlgItemText(IDC_CANCEL, pAPI->GetString(IDS_CANCEL_S));
	SetDlgItemText(IDC_RETRY, pAPI->GetString(IDS_RETRY_S));
	SetDlgItemText(IDC_INFORMATION, m_mainText.c_str());
	SetDlgItemText(IDC_CHK_DONOTASKAGAIN, pAPI->GetString(IDS_DONOTASKAGAIN));


	m_header.SetIconHandle(pAPI->GetIcon(ICO_Main));
	m_header.SetTitleText(m_title);
	m_header.SetDescText(m_subTitle);
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.SetIconSize(48, 48);
	m_header.Init(this);
	m_header.MoveCtrls(this);

	if (m_bFixedFont)
		GetDlgItem(IDC_INFORMATION)->SetFont(pAPI->GetFont(FONT_Fixed));
	return TRUE;  // return TRUE unless you set the focus to a control
}





void CInfoQueryDlg::OnDestroy()
{
	HWND trgHwnd = ::GetDlgItem(m_hWnd, IDC_INFORMATION);
	LRESULT len = ::SendMessage(trgHwnd, WM_GETTEXTLENGTH, 0, 0);  
	m_mainText.resize(len + 1);
	::SendMessage(trgHwnd, WM_GETTEXT, len + 1, (LPARAM)m_mainText.c_str());
	CDialog::OnDestroy();
}

HBRUSH CInfoQueryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = NULL;//CPropertyPage::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_LISTBOX)
	{
		hbr = m_EditBrush;//(HBRUSH)GetStockObject(GRAY_BRUSH);
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(m_bkColor);
		pDC->SetTextColor(RGB(0,0,50));
		//if (!m_bLocationEnabled)
		//{
		//	if (::GetWindowLong(pWnd->m_hWnd, GWL_STYLE) & ES_READONLY)
		//	{
		//		pDC->SetTextColor(RGB(200,0,0));
		//		pDC->SetBkColor(RGB(255,200,200));
		//		hbr = (HBRUSH)GetStockObject(GRAY_BRUSH);
		//	}
		//}
	}
	else
		hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	return hbr;
}


BOOL CInfoQueryDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	TRACEST(_T("CInfoQueryDlg::OnCommand"), wParam);
	switch (wParam)
	{
	case IDC_ACCEPT:
		EndDialog(IsDlgButtonChecked(IDC_CHK_DONOTASKAGAIN) ? RES_AcceptAll: RES_Accept);
		break;
	case IDC_RETRY:
		EndDialog(RES_Retry);
		break;
	case IDC_CANCEL:
		EndDialog(IsDlgButtonChecked(IDC_CHK_DONOTASKAGAIN) ? RES_CancelAll: RES_Cancel);
	//case IDC_WEBHELP:
	//	ASSERT(m_helpURL != NULL);
	//	ShellExecute(m_hWnd, _T("open"), m_helpURL, 0,0,0);
	//	break;
	default:
		//ASSERT(0);
		break;
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CInfoQueryDlg::OnCancel()
{
	EndDialog(RES_Cancel);
}
