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
#include "RatingPP.h"
#include "PrgApi.h"


// CRatingPP dialog

IMPLEMENT_DYNAMIC(CRatingPP, CPropertyPage)

CRatingPP::CRatingPP()
	: CSearchPropertyPages(CRatingPP::IDD),
	m_Rating(100),
	m_bOver(TRUE)
{

}

CRatingPP::~CRatingPP()
{
}

void CRatingPP::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLD_RATING, m_sldRating);
}


BEGIN_MESSAGE_MAP(CRatingPP, CPropertyPage)
	//ON_BN_CLICKED(IDC_RD3STAR, &CSearchPropertyPages::OnChange)
	//ON_BN_CLICKED(IDC_RD2STAR, &CSearchPropertyPages::OnChange)
	//ON_BN_CLICKED(IDC_RD1STAR, &CSearchPropertyPages::OnChange)
	ON_BN_CLICKED(IDC_CHK_OVER, &CSearchPropertyPages::OnChange)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

void CRatingPP::ApplyTranslation(ITranslation& translation)
{
	if (m_hWnd)
	{
		PrgAPI* pAPI = PRGAPI();
		SetDlgItemText(IDC_INFO, pAPI->GetString(IDS_RATINGTEXT));
		SetDlgItemText(IDC_CHK_OVER , pAPI->GetString(IDS_OVER));
		//SetDlgItemText(IDC_RATINGDESC, pAPI->GetStringForStars(m_sldRating.GetPos()));
		//SetDlgItemText(IDC_RD3STAR, pAPI->GetStringForStars(5));
		//SetDlgItemText(IDC_RD2STAR, pAPI->GetStringForStars(4));
		//SetDlgItemText(IDC_RD1STAR, pAPI->GetStringForStars(3));
		//SetDlgItemText(IDC_RD0STAR, pAPI->GetStringForStars(1));
	}
}

BOOL CRatingPP::OnInitDialog()
{
	CSearchPropertyPages::OnInitDialog();
	//ApplyTranslation();
	m_sldRating.SetRange(45, 235);
	if (m_Rating < 1)
		m_Rating = 100;
	SetRatingOption(m_Rating, m_bOver);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CRatingPP::SetRatingOption(INT rating, BOOL bOver)
{
	if (m_sldRating.m_hWnd != NULL)
	{
		m_sldRating.SetPos(rating);
		CheckDlgButton(IDC_CHK_OVER, bOver ? 1 : 0);
		//ApplyTranslation();
	}
	else
	{
		m_Rating = rating;
		m_bOver = bOver;
	}
}
void CRatingPP::GetRatingOption(INT& rating, BOOL& bOver)
{
	rating = m_sldRating.GetPos();
	bOver = IsDlgButtonChecked(IDC_CHK_OVER);
}

//void CRatingPP::OnChange()
//{
//	//m_pParent->SendMessage(WM_PAGEUPDATED);
//}

void CRatingPP::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	TRACE(_T("@@nSBCode: %d (%d)\r\n"), nSBCode, nPos);
	if (nSBCode == 8)
	{
		OnChange();
		//INT pos = m_sldRating.GetPos();
		//FLOAT stars = Rating2Stars(pos);
		//SetDlgItemText(IDC_RATINGDESC, PRGAPI()->GetStringForStars((UINT)stars));
	}
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}
