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
#ifndef _FullTextListCtrl_h_
#define _FullTextListCtrl_h_

#include "FlexListCtrl.h"
#include "DataRecords.h"

class CFullTextListCtrl: public CFlexListCtrl
{
public:
	CFullTextListCtrl();
	virtual ~CFullTextListCtrl();
	void SetFilter(LPCTSTR searchString, InfoItemTypeEnum iit);
	FullTrackRecordSP GetFullTrackRecord(INT nItem);

	void SetMessageOnEmpty(LPCTSTR message);
	LPCTSTR GetMessageOnEmpty()				{return m_messageOnEmpty.c_str();}
protected:
	virtual void DrawItem(Gdiplus::Graphics& g, INT nItem, Gdiplus::Rect& itemRC);
	virtual void DrawBackground(Gdiplus::Graphics& g, const Gdiplus::Rect& r);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	FullTrackRecordCollection m_tracks;
	std::map<UINT, std::tstring > m_textFragments;
	InfoItemTypeEnum m_iit;


	Gdiplus::Font* m_pMainTextFont;
	Gdiplus::Font* m_pTitleTextFont;

	Gdiplus::SolidBrush* m_pNormalBrush;
	Gdiplus::SolidBrush* m_pAlternateBrush;
	Gdiplus::SolidBrush* m_pNormalTextBrush;
	Gdiplus::SolidBrush* m_pSelectedBrush;
	Gdiplus::SolidBrush* m_pSelectedTextBrush;

	Gdiplus::StringFormat* m_pMainStringFormat;
private:
	HFONT m_hFont;
	std::tstring m_searchString;
	std::tstring m_messageOnEmpty;

};

#endif
