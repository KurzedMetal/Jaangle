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
#include "LatestAdditionsPP.h"
#include "PrgApi.h"


// CLatestAdditionsPP dialog

IMPLEMENT_DYNAMIC(CLatestAdditionsPP, CPropertyPage)

CLatestAdditionsPP::CLatestAdditionsPP()
	: CSearchPropertyPages(CLatestAdditionsPP::IDD)
{
	m_st.wYear = 0;
}

CLatestAdditionsPP::~CLatestAdditionsPP()
{
}

void CLatestAdditionsPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATEPICKER, m_Date);
}


BEGIN_MESSAGE_MAP(CLatestAdditionsPP, CPropertyPage)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATEPICKER, &CLatestAdditionsPP::OnDtnDatetimechangeDatepicker)
END_MESSAGE_MAP()

void CLatestAdditionsPP::ApplyTranslation(ITranslation& translation)
{
	if(m_hWnd)
	{
		PrgAPI* pLang = PRGAPI();
		SetDlgItemText(IDC_INFO, pLang->GetString(IDS_LATESTADDITIONS));
		SetDlgItemText(IDC_SINCE, pLang->GetString(IDS_SINCE));
	}
}

// CLatestAdditionsPP message handlers
BOOL CLatestAdditionsPP::OnInitDialog()
{
	CSearchPropertyPages::OnInitDialog();
	//m_Info.SetWindowText(_T("Returns the <b>latest</b> files that have been added to you collection."));
	if (m_st.wYear > 1500)
		m_Date.SetTime(&m_st);
	//ApplyTranslation();
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CLatestAdditionsPP::OnDtnDatetimechangeDatepicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	OnChange();
	*pResult = 0;
}

void CLatestAdditionsPP::GetFirstDate(SYSTEMTIME* fDate)
{
	ASSERT(fDate != NULL);
	UpdateData();
	m_Date.GetTime(fDate);
}
void CLatestAdditionsPP::SetFirstDate(SYSTEMTIME* fDate)
{
	ASSERT(fDate != NULL);
	if (m_Date.m_hWnd)
	{
		m_Date.SetTime(fDate);
		UpdateData(FALSE);
	}
	else
		m_st = *fDate;
}
