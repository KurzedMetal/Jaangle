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
#include "MessageDlg.h"
#include "PrgAPI.h"


IMPLEMENT_DYNAMIC(CMessageDlg, CDialog)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent),
	m_hIcon(NULL),
	m_bChecked(FALSE)
{

}

CMessageDlg::~CMessageDlg()
{
	if (m_hWnd) DestroyWindow();
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LINK, m_helpLink);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN1, &CMessageDlg::OnBnClicked1)
	ON_BN_CLICKED(IDC_BTN2, &CMessageDlg::OnBnClicked2)
	ON_BN_CLICKED(IDC_BTN3, &CMessageDlg::OnBnClicked3)
	ON_STN_CLICKED(IDC_LINK, &CMessageDlg::OnStnClickedLink)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CEmptyDlg message handlers

BOOL CMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	PrgAPI* pAPI = PRGAPI();
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.SetIconSize(48, 48);
	m_header.SetDraggable(TRUE);
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM)pAPI->GetFont(FONT_Dialogs)->m_hObject);
	if (m_hIcon == NULL)
		m_hIcon = pAPI->GetIcon(ICO_Main);
	if (m_text[TT_Title].IsEmpty())
		m_text[TT_Title] = CTeenSpiritApp::sAppTitle;



	SetWindowText(m_text[TT_Title]);
	m_header.SetIconHandle(m_hIcon);
	m_header.SetTitleText(m_text[TT_Title]);
	m_header.SetDescText(m_text[TT_SubTitle]);

	if (m_text[TT_ButtonRight].IsEmpty())
		GetDlgItem(IDC_BTN1)->ShowWindow(SW_HIDE);
	else
		SetDlgItemText(IDC_BTN1, m_text[TT_ButtonRight]);

	if (m_text[TT_ButtonMid].IsEmpty())
		GetDlgItem(IDC_BTN2)->ShowWindow(SW_HIDE);
	else
		SetDlgItemText(IDC_BTN2, m_text[TT_ButtonMid]);
	
	if (m_text[TT_ButtonLeft].IsEmpty())
		GetDlgItem(IDC_BTN3)->ShowWindow(SW_HIDE);
	else
		SetDlgItemText(IDC_BTN3, m_text[TT_ButtonLeft]);

	if (!m_helpText.IsEmpty() && !m_helpURL.IsEmpty())
	{
		m_helpLink.ShowWindow(SW_SHOW);
		m_helpLink.SetWindowText(m_helpText);
		m_helpLink.ApplyPredifinedButtonsStyle(FlexButton::PBS_HyperLink);
		m_helpLink.SetTextStyle(FlexButton::TS_SingleLine);
		SetDlgItemText(IDC_LINK, m_helpText);
	}
	if (!m_text[TT_CheckBox].IsEmpty())
	{
		CheckDlgButton(IDC_CHECKBOX, m_bChecked ? 1 : 0);
		GetDlgItem(IDC_CHECKBOX)->ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_CHECKBOX, m_text[TT_CheckBox]);
	}
	m_helpLink.SetFont(pAPI->GetFont(FONT_DialogsUnderline));
	m_helpLink.SetIcon(pAPI->GetIcon(ICO_Internet16), 16, 16);

	m_header.Init(this);
	m_header.MoveCtrls(this);

	//SetDlgItemText(IDC_OK, pAPI->GetString(IDS_OK));
	//SetDlgItemText(IDC_HELP_S, pAPI->GetString(IDS_HELP_S));



	//SetWindowText(pAPI->GetString(IDS_EMPTYCOLLECTION_TITLE));

	//m_header.SetIconHandle(pAPI->GetIcon(ICO_Main));
	//m_header.SetTitleText(pAPI->GetString(IDS_EMPTYCOLLECTION_TITLE));
	//m_header.SetDescText(pAPI->GetString(IDS_EMPTYCOLLECTION_DETAIL));
	//m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBold));
	//m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	//m_header.SetIconSize(48, 48);
	//m_header.Init(this);
	//m_header.MoveCtrls(this);
	//m_header.SetDraggable(TRUE);

	//SetDlgItemText(IDC_OK, pAPI->GetString(IDS_OK));
	//SetDlgItemText(IDC_HELP_S, pAPI->GetString(IDS_HELP_S));

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CMessageDlg::OnBnClicked1()
{
	EndDialog(RES_Right);
}

void CMessageDlg::OnBnClicked2()
{
	EndDialog(RES_Mid);
}

void CMessageDlg::OnBnClicked3()
{
	EndDialog(RES_Left);
}


void CMessageDlg::OnStnClickedLink()
{
	ShellExecute(NULL, _T("open"), m_helpURL, NULL, NULL, SW_SHOWNORMAL);
}


void CMessageDlg::OnDestroy()
{
	m_bChecked = (IsDlgButtonChecked(IDC_CHECKBOX) == 1);
	CDialog::OnDestroy();
}
