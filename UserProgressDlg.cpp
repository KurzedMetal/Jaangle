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
#include "UserProgressDlg.h"
#include "PrgAPI.h"


// CUserProgressDlg dialog

IMPLEMENT_DYNAMIC(CUserProgressDlg, CDialog)

CUserProgressDlg::CUserProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUserProgressDlg::IDD, pParent),
	m_icon(NULL),
	m_progressValue(0.0),
	m_pParent(pParent),
	m_hWaitCursor(0),
	m_hOldCursor(0)
{
}

CUserProgressDlg::~CUserProgressDlg()
{
	if (m_hWnd != 0)
		DestroyWindow();
	if (m_hOldCursor != NULL)
		::SetCursor(m_hOldCursor);
	if (m_hWaitCursor != NULL)
		DeleteObject(m_hWaitCursor);

}

void CUserProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(CUserProgressDlg, CDialog)
END_MESSAGE_MAP()


void CUserProgressDlg::Show()
{
	if (m_hWnd == 0)
	{
		m_hWaitCursor = LoadCursor(NULL, IDC_WAIT); 
		Create(CUserProgressDlg::IDD, m_pParent);
		m_hOldCursor = ::SetCursor(m_hWaitCursor);
	}
	if (IsWindowVisible() == FALSE)
		ShowWindow(SW_SHOW);
	UpdateWindow();
}





BOOL CUserProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PrgAPI* pAPI = PRGAPI();

	SetWindowText(pAPI->GetString(IDS_WORKING));

	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.SetIconSize(32, 32);
	m_header.SetDraggable(FALSE);

	m_header.SetTitleText(m_title);
	m_header.SetIconHandle(m_icon);
	m_header.SetDescText(m_message);

	m_header.Init(this);
	m_header.MoveCtrls(this);

	m_progress.SetRange(0, 1000);
	SetProgress(m_progressValue);
	SetIcon(m_icon);
	SetTitle(m_title);//Default

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUserProgressDlg::SetTitle(LPCTSTR title)
{
	if (title == NULL || title == _T(""))
		title = CTeenSpiritApp::sAppTitle;
	if (m_title != title)
	{
		m_title = title;
		if (m_header.m_hWnd != NULL)
		{
			m_header.SetTitleText(title);
			m_header.Invalidate();
		}
	}
}
void CUserProgressDlg::SetMessage(LPCTSTR message)
{
	ASSERT(message != NULL);
	if (m_message != message)
	{
		m_message = message;
		if (m_header.m_hWnd != NULL)
		{
			m_header.SetDescText(message);
			m_header.Invalidate();
		}
	}

}
void CUserProgressDlg::SetProgress(FLOAT value)
{
	ASSERT(value >= 0.0f && value <= 1.0f);
	if (m_progressValue != value)
	{
		m_progressValue = value;
		if (m_progress.m_hWnd != NULL)
		{
			m_progress.SetPos(INT(value * 1000));
			m_progress.Invalidate();
		}
	}
}
void CUserProgressDlg::SetIcon(HICON icon)
{
	if (icon == NULL)
		icon = PRGAPI()->GetIcon(ICO_Main);
	if (m_icon != icon)
	{
		m_icon = icon;
		if (m_header.m_hWnd != NULL)
		{
			m_header.SetIconHandle(icon);
			m_header.Invalidate();
		}
	}
}

