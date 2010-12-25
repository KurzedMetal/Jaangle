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

#include <GdiPlus.h>
#include "FlexDblBuffer.h"

class FlexScrollbar
{
public:
	FlexScrollbar();
	~FlexScrollbar();
	void SetParent(CWnd* pWndParent)			{m_pWndParent = pWndParent;}
	void SetButtonSize(INT px)					{m_buttonSize = px; UpdateRects();}
	void SetIcons(HICON hIcon1, HICON hIcon2, INT cx, INT cy);
	void SetScrollStep(INT px)					{m_scrollStep = px;}

	//void SetAlignment(BOOL bDefault);//Right on Vertical - Down On Horizontal

	void Draw(Gdiplus::Graphics& g);

	void SetFullScroll(INT pos)				{m_fullScroll = pos; UpdateRects();}
	void SetScrollPos(INT pos);
	INT GetScrollPos()						{return m_scrollPos;}
	BOOL ContainsPoint(INT x, INT y);

	//void GetClientRect(RECT* rc);

	void SetScrollerRect(Gdiplus::Rect& rc);
	const Gdiplus::Rect& GetScrollerRect()	{return m_scrollerRect;}
	
	//===Events
	BOOL OnLButtonDown(INT x, INT y);
	BOOL OnLButtonUp(INT x, INT y);
	BOOL OnMouseMove(INT x, INT y);
	BOOL OnLButtonDblClk(INT x, INT y);

	BOOL HasScroll();
private:
	void UpdateRects();
	//void GetButton1Rect(Gdiplus::Rect& rc);
	//void GetButton2Rect(Gdiplus::Rect& rc);
	//void GetSliderRect(Gdiplus::Rect& rc);

	CWnd* m_pWndParent;
	INT m_scrollPos;
	INT m_fullScroll;
	INT m_buttonSize;
	HICON m_hIcon1,m_hIcon2;
	Gdiplus::Bitmap* m_pBmpIcon1;
	Gdiplus::Bitmap* m_pBmpIcon2; 
	INT m_cx,m_cy;
	INT m_scrollStep;
	BOOL m_bMouseDownState;
	//INT m_PostionOnMouseDown;
	//INT m_ScrollPosOnMouseDown;
	INT m_dyFromSliderOnMouseDown;
	FlexDblBuffer m_dblBuffer;

	enum SBButtons
	{
		SBB_None,
		SBB_Button1,
		SBB_Button2,
		SBB_Slider,
		SBB_Empty1,
		SBB_Empty2,
		SBB_Last
	} m_mouseOverSBB;
	SBButtons m_mouseDownSBB;
	SBButtons GetSBBFromPoint(INT x, INT y);
	void SetMouseOverSBB(SBButtons btn);
	void SetMouseDownSBB(SBButtons btn);

	enum Brushes
	{
		BR_Bk,
		BR_Btn,
		BR_BtnOver,
		BR_BtnDown,
		BR_Last
	};
	Gdiplus::Brush* m_pBrushes[BR_Last];
	void CreateBrushes();
	void ClearBrushes();
	Gdiplus::Brush* GetBrush(SBButtons btn);
	void InvalidateScroller();


	//NEW
	//BOOL m_bScrollerInvalidated;
	Gdiplus::Rect m_scrollerRect;
	Gdiplus::Rect m_button1Rect;
	Gdiplus::Rect m_button2Rect;
	Gdiplus::Rect m_sliderRect;

	INT ScrollPosBySliderPos(INT sliderPos) const;
	INT SliderPosByScrollPos(INT scrollPos) const;

};
