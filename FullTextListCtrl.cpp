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
#include "FullTextListCtrl.h"
#include "PrgAPI.h"
#include "SQLManager.h"
#include "stlStringUtils.h"
#include "cStringUtils.h"


CFullTextListCtrl::CFullTextListCtrl():
m_hFont(NULL)
{
	SetItemHeight(32);

	LOGFONT lf;
	HDC hdc = ::GetDC(0);
	//	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
	memset(&lf, 0, sizeof(LOGFONT));
	_tcscpy(lf.lfFaceName, _T("Tahoma"));
	lf.lfWeight = 400;
	lf.lfHeight = 13;
	m_pMainTextFont = new Gdiplus::Font(hdc, &lf);
	lf.lfWeight += 200;
	lf.lfHeight += 2;
	m_pTitleTextFont = new Gdiplus::Font(hdc, &lf);
	::ReleaseDC(0, hdc);


	m_pNormalBrush = new Gdiplus::SolidBrush(Gdiplus::Color::MakeARGB(255, 255, 255, 255));
	m_pAlternateBrush = new Gdiplus::SolidBrush(Gdiplus::Color::MakeARGB(255, 250, 250, 252));
	m_pNormalTextBrush = new Gdiplus::SolidBrush(Gdiplus::Color::MakeARGB(255, 0, 0, 0));

	m_pSelectedBrush = new Gdiplus::SolidBrush(Gdiplus::Color::MakeARGB(255, 125, 155, 225));
	m_pSelectedTextBrush = new Gdiplus::SolidBrush(Gdiplus::Color::MakeARGB(255, 255, 255, 255));

	m_pMainStringFormat = new Gdiplus::StringFormat();
	m_pMainStringFormat->SetLineAlignment(Gdiplus::StringAlignmentCenter);
	m_pMainStringFormat->SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

}

CFullTextListCtrl::~CFullTextListCtrl()
{
	delete m_pNormalBrush;
	delete m_pAlternateBrush;
	delete m_pNormalTextBrush;
	delete m_pSelectedBrush;
	delete m_pSelectedTextBrush;
	delete m_pMainStringFormat;
}

void CFullTextListCtrl::DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r)
{
	g.FillRectangle(m_pNormalBrush, r);
	if (r.Y == 0)
	{
		Gdiplus::StringFormat centeredStringFormat;
		centeredStringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
		centeredStringFormat.SetFormatFlags(Gdiplus::StringFormatFlagsNoWrap);

		g.DrawString(m_messageOnEmpty.c_str(), 
			m_messageOnEmpty.size(), 
			m_pMainTextFont, 
			Gdiplus::RectF(
			(Gdiplus::REAL)(r.X + 5), 
			(Gdiplus::REAL)(r.Y + 5), 
			(Gdiplus::REAL)(r.Width), 
			(Gdiplus::REAL)(r.Y + 30)), 
			&centeredStringFormat, 
			m_pNormalTextBrush);
	}


}

void CFullTextListCtrl::DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC)
{
	ASSERT(nItem < (INT)m_tracks.size());
	Gdiplus::Brush* pBkBrush = (nItem % 2 == 0) ? m_pNormalBrush : m_pAlternateBrush;
	Gdiplus::Brush* pTextBrush = m_pNormalTextBrush;
	if (IsItemSelected(nItem))
	{
		pBkBrush = m_pSelectedBrush;
		pTextBrush = m_pSelectedTextBrush;
	}
	g.FillRectangle(pBkBrush, itemRC);

	TCHAR bf[1000];
	_sntprintf(bf, 1000, _T("%s | %s"), 
		m_tracks[nItem]->track.name.c_str(),
		m_tracks[nItem]->artist.name.c_str());
	g.DrawString(bf, 
		-1, 
		m_pTitleTextFont, 
		Gdiplus::RectF(
		(Gdiplus::REAL)(itemRC.X), 
		(Gdiplus::REAL)(itemRC.Y), 
		(Gdiplus::REAL)(itemRC.Width), 
		(Gdiplus::REAL)(itemRC.Height / 2)), 
		m_pMainStringFormat, 
		pTextBrush);



	std::map<UINT, std::tstring>::iterator it = m_textFragments.find(nItem);
	LPCTSTR txtFrag = NULL;
	if (it == m_textFragments.end())
	{
		UINT infoID = 0;
		switch (m_iit)
		{
		case IIT_TrackComment:
		case IIT_TrackLyrics:
		case IIT_TrackPersonal:
			{
				InfoRecord ir;
				if (PRGAPI()->GetSQLManager()->GetTrackInfoRecord(m_tracks[nItem]->track.ID, m_iit, ir))
				{
					replace(ir.name, _T("\r"), _T("."));
					replace(ir.name, _T("\n"), _T("."));
					const TCHAR* fin = _tcsistr(ir.name.c_str(), m_searchString.c_str());
					m_textFragments[nItem].clear();
					if (INT(fin - ir.name.c_str()) > 20)//If its less than 20 then show all
					{
						fin -= 15;//Go Back 15 chars;
						m_textFragments[nItem].append(_T("..."));
						if (_tcslen(fin) > 100)
							m_textFragments[nItem].append(fin, 100);
						else
							m_textFragments[nItem].append(fin);
					}
					else
					{
						if (ir.name.size() > 100)
							m_textFragments[nItem].append(ir.name.c_str(), 100);
						else
							m_textFragments[nItem] = ir.name;
					}
					txtFrag = m_textFragments[nItem].c_str();
				}
			}
			break;
		}
	}
	else
		txtFrag = (*it).second.c_str();
	g.DrawString(txtFrag, 
		-1, 
		m_pMainTextFont, 
		Gdiplus::RectF(
		(Gdiplus::REAL)(itemRC.X), 
		(Gdiplus::REAL)(itemRC.Y + itemRC.Height / 2), 
		(Gdiplus::REAL)(itemRC.Width), 
		(Gdiplus::REAL)(itemRC.Height / 2)), 
		m_pMainStringFormat, 
		pTextBrush);
	
}

void CFullTextListCtrl::SetFilter(LPCTSTR searchString, InfoItemTypeEnum iit)
{
	ASSERT(searchString != NULL);
	if (searchString == NULL)
		searchString = _T("");
	TRACEST(_T("CFullTextListCtrl::SetFilter"), searchString);
	m_searchString = searchString;
	m_iit = iit;
	m_textFragments.clear();

	SQLManager* pSM = PRGAPI()->GetSQLManager();
	m_tracks.clear();
	if (pSM->GetFullTrackRecordCollectionByInfoLikeness(m_tracks, iit, searchString))
	{
	}
	ClearItems();
	InsertItems(0, m_tracks.size());
	Refresh();
}

FullTrackRecordSP CFullTextListCtrl::GetFullTrackRecord(INT nItem)
{
	if (nItem >= 0 && nItem < (INT)m_tracks.size())
		return m_tracks[nItem];
	return FullTrackRecordSP();
}

LRESULT CFullTextListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_SETFONT)
		m_hFont = (HFONT) wParam;
	return CFlexListCtrl::WindowProc(message, wParam, lParam);
}
void CFullTextListCtrl::SetMessageOnEmpty(LPCTSTR message)		
{
	m_messageOnEmpty = message;
	if (m_hWnd)
		Invalidate(TRUE);
}
