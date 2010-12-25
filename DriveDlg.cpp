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
#include "DriveDlg.h"
#include "PrgAPI.h"
//#include "Language.h"

// CDriveDlg dialog

IMPLEMENT_DYNAMIC(CDriveDlg, CDialog)

CDriveDlg::CDriveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDriveDlg::IDD, pParent)
{
	
}

CDriveDlg::~CDriveDlg()
{
}

void CDriveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDriveDlg, CDialog)
END_MESSAGE_MAP()


// CDriveDlg message handlers

BOOL CDriveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	PrgAPI* pAPI = PRGAPI();
	m_Header.SetIconHandle(pAPI->GetIcon(ICO_Media16));
	m_Header.SetTitleText(pAPI->GetString(IDS_SELECTDRIVE));
	m_Header.SetDescText(pAPI->GetString(IDS_SELECTDRIVEDET));
	m_Header.SetTitleFont(pAPI->GetFont(FONT_DialogsBigBold));
	m_Header.SetDescFont(pAPI->GetFont(FONT_Dialogs));
	m_Header.Init(this);
	m_Header.MoveCtrls(this);
	SetWindowText(pAPI->GetString(IDS_SELECTDRIVE));

	return TRUE;  // return TRUE unless you set the focus to a control
}
