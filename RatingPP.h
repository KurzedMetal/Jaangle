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
#pragma once

#include "SearchPropertyPages.h"

class CRatingPP : public CSearchPropertyPages
{
	DECLARE_DYNAMIC(CRatingPP)

public:
	CRatingPP();
	virtual ~CRatingPP();

// Dialog Data
	enum { IDD = IDD_PPBESTWORSE };


	void SetRatingOption(INT rating, BOOL bOver);
	void GetRatingOption(INT& rating, BOOL& bOver);
	//void SetRealParent(CWnd* pParent){m_pParent = pParent;}
	virtual void ApplyTranslation(ITranslation& skin); //ICtrlStrings
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//afx_msg void OnChange();
protected:
	INT m_Rating;
	BOOL m_bOver;

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_sldRating;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
