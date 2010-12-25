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


class CMediaSlider;

class MediaSliderListener
{
public:
	virtual void OnMediaSliderClick(CMediaSlider* pMediaSlider) = 0;
};

class CMediaSlider : public CWnd
{
	DECLARE_DYNAMIC(CMediaSlider)

public:
	CMediaSlider();
	virtual ~CMediaSlider();

	void SetListener(MediaSliderListener* pListener)		{m_pListener = pListener;}

	BOOL Create(MediaSliderListener* pListener, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	void SetMaxPos(INT maxPos);
	INT GetMaxPos()								{return m_maxPos;}
	void SetPos(INT value);
	INT GetPos()								{return m_curPos;}

	void SetTickInterval(INT tickInterval)		{m_tickInterval = tickInterval;}
	INT GetTickInterval(INT tickInterval)		{return m_tickInterval;}

	//Cursor width must be set to an ODD number for proper results
	void SetCursorWidth(INT width)				{ASSERT(width % 2 == 1); m_cursorWidth = width;}
	void SetHorizontal(BOOL bHorizontal)		{m_bHorizontal = bHorizontal;}
	void UseHandCursor(BOOL bUseHandCursor)		{m_bUseHandCursor = bUseHandCursor;}

	void SetTransparentMode(BOOL bTransparentMode)	{m_bTransparentMode = bTransparentMode;}
	BOOL IsInTransparentMode()						{return m_bTransparentMode;}

	enum COLORS
	{
		COL_Bk,
		COL_Border,
		COL_FirstPart,
		COL_SecondPart,
		COL_CursorBorder,
		COL_TickColor,
		COL_Last
	};
	void SetColor(COLORS col, COLORREF value);
	COLORREF GetColor(COLORS col);

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	DECLARE_MESSAGE_MAP()
private:
	COLORREF m_colors[COL_Last];
	INT m_maxPos;
	INT m_curPos;
	INT m_cursorWidth;
	BOOL m_bHorizontal;
	BOOL m_bMouseIsDown;
	BOOL m_bUseHandCursor;
	MediaSliderListener* m_pListener;
	INT m_tickInterval;
	BOOL m_bTransparentMode;

	INT GetPosByPoint(INT x, INT y);
	static COLORREF MixColors(COLORREF col1, COLORREF col2, INT part1 = 1, INT part2 = 1);

};


