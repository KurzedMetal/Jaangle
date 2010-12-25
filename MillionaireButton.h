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


// CMillionaireButton

class CMillionaireButton : public CButton
{
	DECLARE_DYNAMIC(CMillionaireButton)

public:
	CMillionaireButton();
	virtual ~CMillionaireButton();

	enum States
	{
		ST_Normal,
		ST_Over,
		ST_Correct,
		ST_Wrong,
		ST_Disabled,
		ST_Last
	};
	enum Colors
	{
		COL_Frame,

		COL_NormalBk,
		COL_OverBk,
		COL_CorrectBk,
		COL_WrongBk,
		COL_DisabledBk,

		COL_NormalText,
		COL_OverText,
		COL_CorrectText,
		COL_WrongText,
		COL_DisabledText,
		
		COL_Last
	};

	void SetFont(CFont* pFont)								{m_pFont = pFont;}
	void SetState(States state)								{m_currentState = state;}
	void SetColors(Colors colID, COLORREF textColor);
	enum PolygonStyles
	{
		POL_Rect,
		POL_RoundRect,
		POL_Ellipse,
		POL_Diamond,
		POL_Last
	};
	void SetPolygonStyle(PolygonStyles ps, INT psxParam = 0);
	void SetTextAlign(INT textAlign)					{m_textAlign = textAlign;}
	void SetMargin(INT margin)							{m_margin = margin;}
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
private:
	void UpdateWindowRegion(INT cx, INT cy);
	COLORREF m_colors[COL_Last];
	//COLORREF m_borderColor;
	//COLORREF m_textColor;
	States m_currentState;
	//BOOL m_blinkState;
	CString m_windowText;
	CRgn m_rgn;
	CRgn m_rgnWnd;
	PolygonStyles m_ps;
	INT m_psxParam;

	CFont* m_pFont;
	INT m_textAlign;
	INT m_margin;

	static int StateToColor[];
};


