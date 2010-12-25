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
#include "DetailedSearchCHdlg.h"
#include "PrgApi.h"


// CPPDetailedSearch dialog

IMPLEMENT_DYNAMIC(CDetailedSearchCHdlg, CPropertyPage)

CDetailedSearchCHdlg::CDetailedSearchCHdlg()
	: CSearchPropertyPages(CDetailedSearchCHdlg::IDD)
{

}

CDetailedSearchCHdlg::~CDetailedSearchCHdlg()
{
}

void CDetailedSearchCHdlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CDetailedSearchCHdlg, CSearchPropertyPages)
	ON_BN_CLICKED(IDC_SEARCH, &CDetailedSearchCHdlg::OnChange)
END_MESSAGE_MAP()

void CDetailedSearchCHdlg::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		SetDlgItemText(IDC_ST_ARTIST, pLang->GetString(IDS_ARTIST));
		SetDlgItemText(IDC_ST_ALBUM, pLang->GetString(IDS_ALBUM));
		SetDlgItemText(IDC_ST_TITLE, pLang->GetString(IDS_TITLE));
		SetDlgItemText(IDC_ST_LOCATION, pLang->GetString(IDS_LOCATION));
		SetDlgItemText(IDC_SEARCH, pLang->GetString(IDS_SEARCH));
	}
}

BOOL CDetailedSearchCHdlg::OnInitDialog()
{
	CSearchPropertyPages::OnInitDialog();
	//ApplyTranslation();
	return TRUE;  // return TRUE unless you set the focus to a control
}

LPCTSTR CDetailedSearchCHdlg::GetArtist()
{
	if (m_hWnd)
		GetDlgItemText(IDC_EDT_ARTIST, m_Artist, CDetailedSearchCHdlgLEN);
	else
		m_Artist[0] = 0;
	return m_Artist;
}
LPCTSTR CDetailedSearchCHdlg::GetAlbum()
{
	if (m_hWnd)
		GetDlgItemText(IDC_EDT_ALBUM, m_Album, CDetailedSearchCHdlgLEN);
	else
		m_Album[0] = 0;
	return m_Album;
}
LPCTSTR CDetailedSearchCHdlg::GetTitle()
{
	if (m_hWnd)
		GetDlgItemText(IDC_EDT_TITLE, m_Title, CDetailedSearchCHdlgLEN);
	else
		m_Title[0] = 0;
	return m_Title;
}
LPCTSTR CDetailedSearchCHdlg::GetLocation()
{
	if (m_hWnd)
		GetDlgItemText(IDC_EDT_LOCATION, m_Location, CDetailedSearchCHdlgLEN);
	else
		m_Location[0] = 0;
	return m_Location;
}
