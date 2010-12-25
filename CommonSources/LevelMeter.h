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


// CLevelMeter

class CLevelMeter : public CWnd
{
	DECLARE_DYNAMIC(CLevelMeter)

public:
	CLevelMeter();
	virtual ~CLevelMeter();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void SetMaxMin(INT maxVal, INT minVal);
	void SetLevel(INT value);
	INT GetLevel()							{return m_curVal;}

	enum COLORS
	{
		COL_Bk,
		COL_Border,
		COL_ActiveLow,
		COL_ActiveMid,
		COL_ActiveHigh,
		COL_Lines,
		COL_Last
	};
	void SetColor(COLORS col, COLORREF value);
	COLORREF GetColor(COLORS col);
	void ShowLines(BOOL bShow)	{m_bShowLines = bShow;}
	void ShowBorder(BOOL bShow)	{m_bShowBorder = bShow;}

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_colors[COL_Last];
	INT m_maxLevel;
	INT m_maxVal;
	INT m_minVal;
	INT m_curVal;
	BOOL m_bShowLines;
	BOOL m_bShowBorder;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


