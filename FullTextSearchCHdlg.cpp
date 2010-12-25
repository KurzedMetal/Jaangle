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
#include "FullTextSearchCHdlg.h"
#include "PrgApi.h"


// CSimpleSearchCHdlg dialog

IMPLEMENT_DYNAMIC(CFullTextSearchCHdlg, CPropertyPage)

CFullTextSearchCHdlg::CFullTextSearchCHdlg()
	: CSearchPropertyPages(CFullTextSearchCHdlg::IDD),
	m_tto(TT_Lyrics)
{

}

CFullTextSearchCHdlg::~CFullTextSearchCHdlg()
{
}

void CFullTextSearchCHdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFullTextSearchCHdlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_SEARCH, &CFullTextSearchCHdlg::OnChange)
END_MESSAGE_MAP()


void CFullTextSearchCHdlg::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		SetDlgItemText(IDC_INFO, pLang->GetString(IDS_FULLTEXTTEXT));
		SetDlgItemText(IDC_ITGROUP, pLang->GetString(IDS_INFORMATIONTYPE));
		SetDlgItemText(IDC_RDLYRICS, pLang->GetString(IDS_LYRICS));
		SetDlgItemText(IDC_RDCOMMENTS, pLang->GetString(IDS_COMMENTS));
		SetDlgItemText(IDC_RDPERSONAL, pLang->GetString(IDS_PERSONAL));
		SetDlgItemText(IDC_SEARCH, pLang->GetString(IDS_SEARCH));
	}
}


BOOL CFullTextSearchCHdlg::OnInitDialog()
{
	CSearchPropertyPages::OnInitDialog();
	//ApplyTranslation();
	SetTextTypeOption(m_tto);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFullTextSearchCHdlg::GetSearchText(LPTSTR bf, UINT len)
{
	if (m_hWnd)
		GetDlgItemText(IDC_TEXT, bf, len);
	else
		bf[0] = 0;
}
void CFullTextSearchCHdlg::SetSearchText(LPCTSTR bf)
{
	if (m_hWnd)
		SetDlgItemText(IDC_TEXT, bf);
}
CFullTextSearchCHdlg::TEXT_TYPES CFullTextSearchCHdlg::GetTextTypeOption()
{
	if (m_hWnd)
		return (CFullTextSearchCHdlg::TEXT_TYPES) 
			(GetCheckedRadioButton(IDC_RDLYRICS, IDC_RDPERSONAL) - IDC_RDLYRICS + 1);
	else
		return m_tto;
}
void CFullTextSearchCHdlg::SetTextTypeOption(TEXT_TYPES tto)
{
	if (m_hWnd)
		CheckRadioButton(IDC_RDLYRICS, IDC_RDPERSONAL, (int) tto - 1 + IDC_RDLYRICS);
	else
		m_tto = tto;
}

