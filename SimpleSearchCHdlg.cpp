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
#include "SimpleSearchCHdlg.h"
#include "SearchDlg.h"
#include "PrgApi.h"


// CSimpleSearchCHdlg dialog

IMPLEMENT_DYNAMIC(CSimpleSearchCHdlg, CPropertyPage)

CSimpleSearchCHdlg::CSimpleSearchCHdlg()
	: CSearchPropertyPages(CSimpleSearchCHdlg::IDD),
	m_fto(FTO_All)
{

}

CSimpleSearchCHdlg::~CSimpleSearchCHdlg()
{
}

void CSimpleSearchCHdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSimpleSearchCHdlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_SEARCH, &CSimpleSearchCHdlg::OnChange)
END_MESSAGE_MAP()

void CSimpleSearchCHdlg::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		SetDlgItemText(IDC_INFO, pLang->GetString(IDS_SEARCH));
		SetDlgItemText(IDC_RDALL, pLang->GetString(IDS_ALL));
		SetDlgItemText(IDC_RDAUDIO, pLang->GetString(IDS_AUDIO));
		SetDlgItemText(IDC_RDVIDEO, pLang->GetString(IDS_VIDEO));
		SetDlgItemText(IDC_FTGROUP, pLang->GetString(IDS_FILETYPEFILTER));
		SetDlgItemText(IDC_SEARCH, pLang->GetString(IDS_SEARCH));
	}
}


BOOL CSimpleSearchCHdlg::OnInitDialog()
{
	CSearchPropertyPages::OnInitDialog();

	SetFileTypeOption(m_fto);
	return TRUE;  // return TRUE unless you set the focus to a control
}

//void CSimpleSearchCHdlg::ShowAllCheckChanged()
//{
//	GetDlgItem(IDC_EDT_SIMPLE)->EnableWindow(!IsDlgButtonChecked(IDC_SHOWALLTRACKS));
//	//CSearchPropertyPages::OnChange();
//}

void CSimpleSearchCHdlg::GetSimpleSearchText(LPTSTR bf, UINT len)
{
	if (m_hWnd)
	{
		if (IsDlgButtonChecked(IDC_SHOWALLTRACKS))
			_tcsncpy(bf, sShowAllTracksCode, len);
		else
			GetDlgItemText(IDC_EDT_SIMPLE, bf, len);
	}
	else
		bf[0] = 0;
}
void CSimpleSearchCHdlg::SetSimpleSearchText(LPCTSTR bf)
{
	if (m_hWnd)
		SetDlgItemText(IDC_EDT_SIMPLE, bf);
}
CSimpleSearchCHdlg::FILETYPE_OPTIONS CSimpleSearchCHdlg::GetFileTypeOption()
{
	if (m_hWnd)
		return (CSimpleSearchCHdlg::FILETYPE_OPTIONS) 
			(GetCheckedRadioButton(IDC_RDALL, IDC_RDVIDEO) - IDC_RDALL + 1);
	else
		return m_fto;
}
void CSimpleSearchCHdlg::SetFileTypeOption(FILETYPE_OPTIONS fto)
{
	if (m_hWnd)
		CheckRadioButton(IDC_RDALL, IDC_RDVIDEO, (int) fto - 1 + IDC_RDALL);
	else
		m_fto = fto;
}
