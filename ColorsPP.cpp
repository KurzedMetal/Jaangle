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
#include "ColorsPP.h"
#include "PrgAPI.h"

// CMediaPlayerPP dialog

IMPLEMENT_DYNAMIC(CColorsPP, CPropertyPage)

CColorsPP::CColorsPP()
	: CPropertyPage(CColorsPP::IDD)
{

}

CColorsPP::~CColorsPP()
{
}

void CColorsPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CColorsPP, CPropertyPage)
	ON_BN_CLICKED(IDC_APPLY, &CColorsPP::OnBnClickedApply)
END_MESSAGE_MAP()

void CColorsPP::RefreshStrings()
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		//SetDlgItemText(IDC_GR_STARTUP, pLang->GetString(IDS_STARTUP));
		//SetDlgItemText(IDC_CHK_AUTOPLAY, pLang->GetString(IDS_AUTOPLAY));
		//SetDlgItemText(IDC_CHK_REMTRACK, pLang->GetString(IDS_REMEMBERLASTTRACK));
		//SetDlgItemText(IDC_CHK_REMTIME, pLang->GetString(IDS_ALSOTHETIME));
		//SetDlgItemText(IDC_GR_GENERAL, pLang->GetString(IDS_GENERAL));
		//SetDlgItemText(IDC_ST_PLAYCONTROL, pLang->GetString(IDS_PLAYCONTROL));
		//SetDlgItemText(IDC_CHK_SHOWVISUALIZATIONS, pLang->GetString(IDS_SHOWVISUALIZATIONS));
		//SetDlgItemText(IDC_GR_GLOBALHOTKEYS, pLang->GetString(IDS_GLOBALHOTKEYS));
		//SetDlgItemText(IDC_ST_PLAY, pLang->GetString(IDS_PLAY));
		//SetDlgItemText(IDC_ST_PREVIOUS, pLang->GetString(IDS_PREVIOUS));
		//SetDlgItemText(IDC_ST_NEXT, pLang->GetString(IDS_NEXT));
		//SetDlgItemText(IDC_ST_STOP, pLang->GetString(IDS_STOP));
		//SetDlgItemText(IDC_CHK_USEMULTIMEDIAHOTKEYS, pLang->GetString(IDS_USEMULTIMEDIAHOTKEYS));

		////From LoadMPCtrlOptions
		//m_PlayControl.ResetContent();
		//m_PlayControl.AddString(pLang->GetString(IDS_NORMAL));
		//m_PlayControl.AddString(pLang->GetString(IDS_SHUFFLE));
		//m_PlayControl.AddString(pLang->GetString(IDS_LOOP));
		//m_PlayControl.AddString(pLang->GetString(IDS_AUTOCONTINUE));
	}
}
BOOL CColorsPP::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	RefreshStrings();
	LoadOptions();
	EnumChildWindows(m_hWnd, SetFontToChilds, (LPARAM) PRGAPI()->GetFont(FONT_Dialogs)->m_hObject);

	return TRUE;  // return TRUE unless you set the focus to a control
}
void CColorsPP::OnChange()
{
	if (IsWindowVisible())
	{
		m_bNeedsUpdating = TRUE;
		SetModified(TRUE);
	}
}

void CColorsPP::OnOK()
{
	if (m_bNeedsUpdating)
	{
		m_bSomethingChanged = TRUE;
		SaveOptions();
	}
	CPropertyPage::OnOK();
}

void CColorsPP::LoadOptions()
{
	PrgAPI* pAPI = PRGAPI();

}

void CColorsPP::SaveOptions()
{
}

void CColorsPP::OnBnClickedApply()
{
	PRGAPI()->RefreshColorControls();
}
