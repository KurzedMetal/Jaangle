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
#ifndef _CFullTextSearchCHdlg_h_
#define _CFullTextSearchCHdlg_h_

#include "SearchPropertyPages.h"

class CFullTextSearchCHdlg : public CSearchPropertyPages
{
	DECLARE_DYNAMIC(CFullTextSearchCHdlg)

public:
	CFullTextSearchCHdlg();
	virtual ~CFullTextSearchCHdlg();

	enum TEXT_TYPES
	{
		TT_First,
		TT_Lyrics,
		TT_Comments,
		TT_Personal,
		TT_AlbumReview,
		TT_ArtistBio,
		TT_Path,
		TT_Last
	};
// Dialog Data
	enum { IDD = IDD_PPFULLTEXT };

	void GetSearchText(LPTSTR bf, UINT len);
	void SetSearchText(LPCTSTR bf);
	TEXT_TYPES GetTextTypeOption();
	void SetTextTypeOption(TEXT_TYPES fto);
	virtual void ApplyTranslation(ITranslation& skin); //ICtrlStrings
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//afx_msg void OnChange();
	DECLARE_MESSAGE_MAP()

private:
	TEXT_TYPES m_tto;
};

#endif
