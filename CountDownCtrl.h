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

#include "FlickerFree.h"
// CCountDownCtrl

class CCountDownCtrl : public CProgressCtrl
{
	DECLARE_DYNAMIC(CCountDownCtrl)

public:
	CCountDownCtrl();
	virtual ~CCountDownCtrl();

	enum COLORS
	{
		COL_Bk,
		COL_Text,
		COL_Fg1,
		COL_Fg2,
		COL_Last 
	};
	void SetColor(COLORS colID, COLORREF value);

	enum Unit
	{
		UN_Unknown,
		UN_ms,
		UN_Last
	};
	void SetUnit(Unit unit)				{m_unit = unit;}

	void SetReverseBar(BOOL bReverse)	{m_bReverseBar = bReverse;}

	int SetPos(int nPos);


protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	COLORREF GetGradientColor(DOUBLE relPos);
	COLORREF m_colors[COL_Last];
	BOOL m_bReverseBar;
	Unit m_unit;
	CFlickerFree m_ff;

};


