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
#include "StdAfx.h"
#include "resource.h"
#include "InputBox.h"

CInputBox::CInputBox(CWnd* pParent):
CDialog(IDD, pParent)
{

}
CInputBox::~CInputBox()
{
	if (m_hWnd) DestroyWindow();
}

void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CInputBox::SetTexts(LPCTSTR Title, LPCTSTR SubTitle, LPCTSTR DefaultString)
{
	m_Title = Title;
	m_SubTitle = SubTitle;
	m_InputString = DefaultString;
}
void CInputBox::SetButtonTexts(LPCTSTR OkCaption, LPCTSTR CancelCaption)
{
	m_OkCaption = OkCaption;
	m_CancelCaption = CancelCaption;
}

LPCTSTR CInputBox::GetInputString()
{
	return m_InputString;
}

BOOL CInputBox::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(m_Title);
	SetDlgItemText(IDC_SUBTITLE, m_SubTitle);
	SetDlgItemText(IDC_INPUTSTRING, m_InputString);
	if (!m_OkCaption.IsEmpty())
		SetDlgItemText(IDOK, m_OkCaption);
	if (!m_CancelCaption.IsEmpty())
		SetDlgItemText(IDCANCEL, m_CancelCaption);

	GetDlgItem(IDC_INPUTSTRING)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
BEGIN_MESSAGE_MAP(CInputBox, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CInputBox::OnDestroy()
{
	GetDlgItemText(IDC_INPUTSTRING, m_InputString);
	CDialog::OnDestroy();
}
