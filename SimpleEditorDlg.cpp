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
#include "SimpleEditorDlg.h"
#include "PrgAPI.h"


// CSimpleEditorDlg dialog

IMPLEMENT_DYNAMIC(CSimpleEditorDlg, CDialog)

CSimpleEditorDlg::CSimpleEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpleEditorDlg::IDD, pParent)
{

}

CSimpleEditorDlg::~CSimpleEditorDlg()
{
}

void CSimpleEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSimpleEditorDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SAVE, &CSimpleEditorDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// CSimpleEditorDlg message handlers

BOOL CSimpleEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PrgAPI* pAPI = PRGAPI();
	m_header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_header.SetIconSize(48, 48);
	m_header.SetDraggable(TRUE);

	SetWindowText(m_title);
	m_header.SetTitleText(m_title);
	m_header.SetDescText(m_subTitle);
	SetDlgItemText(IDC_EDIT, m_text);

	m_header.Init(this);
	m_header.MoveCtrls(this);


	CenterWindow();


	return TRUE;  
}

void CSimpleEditorDlg::OnDestroy()
{
	GetDlgItemText(IDC_EDIT, m_text);
	CDialog::OnDestroy();
}

void CSimpleEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
}

void CSimpleEditorDlg::OnBnClickedSave()
{
	OnOK();
}
