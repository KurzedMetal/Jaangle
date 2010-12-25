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
#include "FlexScrollBar.h"
#include "GdiplusUtilities.h"

using namespace Gdiplus;

FlexScrollbar::FlexScrollbar():
m_pWndParent(NULL),
m_scrollPos(0),
m_fullScroll(0),
m_buttonSize(30),
m_hIcon1(NULL),
m_hIcon2(NULL),
m_pBmpIcon1(NULL),
m_pBmpIcon2(NULL),
m_cx(0),
m_cy(0),
m_scrollStep(5),
m_bMouseDownState(FALSE),
//m_PostionOnMouseDown(0),
//m_ScrollPosOnMouseDown(0),
m_dyFromSliderOnMouseDown(0),
m_mouseOverSBB(SBB_None),
m_mouseDownSBB(SBB_None)
{
	for (int i=0; i<BR_Last; i++)
		m_pBrushes[i] = 0;
}

void FlexScrollbar::CreateBrushes()
{
	Point p1(0 ,0);
	Point p2(m_scrollerRect.Width, 0);
	m_pBrushes[BR_Bk] = new LinearGradientBrush(p1, p2, Color::MakeARGB(255, 195,195,195), Color::MakeARGB(255,240,240,240));
	m_pBrushes[BR_Btn] = new LinearGradientBrush(p1, p2, Color::MakeARGB(255, 255,255,255), Color::MakeARGB(255,210,210,210));
	m_pBrushes[BR_BtnOver] = new LinearGradientBrush(p1, p2, Color::MakeARGB(255, 255,255,255), Color::MakeARGB(255,250,250,250));
	m_pBrushes[BR_BtnDown] = new LinearGradientBrush(p1, p2, Color::MakeARGB(255, 150,150,180), Color::MakeARGB(255,210,210,240));
}

void FlexScrollbar::ClearBrushes()
{
	for (int i=0; i<BR_Last; i++)
	{
		delete m_pBrushes[i];
		m_pBrushes[i] = 0;
	}
}


FlexScrollbar::~FlexScrollbar()
{
	ClearBrushes();
	delete m_pBmpIcon1;
	delete m_pBmpIcon2;
}


void FlexScrollbar::SetIcons(HICON hIcon1, HICON hIcon2, INT cx, INT cy)
{
	m_hIcon1 = hIcon1;
	m_hIcon2 = hIcon2;
	m_cx = cx;
	m_cy = cy;
	delete m_pBmpIcon1;
	m_pBmpIcon1 = NULL;
	delete m_pBmpIcon2;
	m_pBmpIcon2 = NULL;
}

Gdiplus::Brush* FlexScrollbar::GetBrush(SBButtons btn)
{
	Brushes br = BR_Last;
	if (btn == SBB_Empty1 || btn == SBB_Empty2)
		br = BR_Bk;
	else
	{
		if (btn == m_mouseDownSBB)
			br = BR_BtnDown;
		else if (btn == m_mouseOverSBB)
			br = BR_BtnOver;
		else
			br = BR_Btn;

	}
	if (br == BR_Last)
	{
		ASSERT(0);//???
		return NULL;
	}
	if (m_pBrushes[0] == 0)
		CreateBrushes();
	return m_pBrushes[br];
}


void FlexScrollbar::Draw(Gdiplus::Graphics& dest)
{
	if (m_scrollerRect.Width != 0 && m_scrollerRect.Height != 0)
	{
		Graphics*pg = m_dblBuffer.GetGraphics(m_scrollerRect.Width, m_scrollerRect.Height);
		if (pg == NULL)
		{
			TRACE(_T("@0 FlexScrollbar::Draw Can't get dbl buffer\r\n\r\n"));
			return;
		}
		Graphics& g = *pg;
		Rect rcRect(m_scrollerRect);
		rcRect.X = 0;
		//g.ResetTransform();
		//g.TranslateTransform(REAL(-m_scrollerRect.X), REAL(-m_scrollerRect.Y));
		//===Draw Background
		g.FillRectangle(GetBrush(SBB_Empty1), rcRect);
		//===Draw Background border
		Pen borderLine(Color::MakeARGB(200,180,180,180));
		rcRect.Width--;
		rcRect.Height--;
		g.DrawRectangle(&borderLine, rcRect);

		//INT fullScroll = m_fullScroll - m_scrollerRect.Height + 2 * m_buttonSize;
		if (HasScroll())
		{
			Pen btnLine(Color::MakeARGB(255,180,180,180));
			rcRect = m_button1Rect;	rcRect.X = 0;
			g.FillRectangle(GetBrush(SBB_Button1), rcRect);
			if (m_hIcon1 != NULL)
			{
				if (m_pBmpIcon1 == NULL)
					m_pBmpIcon1 = GdiplusUtilities::FromHICON32(m_hIcon1);
				ASSERT(m_pBmpIcon1);
				g.DrawImage(m_pBmpIcon1, rcRect.X + (rcRect.Width - m_cx) / 2,
					rcRect.Y + (m_buttonSize - m_cy) / 2, 0,0,m_cx - 1, m_cy -1, Gdiplus::UnitPixel);
			}
			rcRect.Width-=1;
			rcRect.Height-=1;
			g.DrawRectangle(&btnLine, rcRect);

			//===Draw Button 2
			rcRect = m_button2Rect;	rcRect.X = 0;
			g.FillRectangle(GetBrush(SBB_Button2), rcRect);
			if (m_hIcon2 != NULL)
			{
				if (m_pBmpIcon2 == NULL)
					m_pBmpIcon2 = GdiplusUtilities::FromHICON32(m_hIcon2);
				ASSERT(m_pBmpIcon2);
				g.DrawImage(m_pBmpIcon2, rcRect.X + (rcRect.Width - m_cx) / 2,
					rcRect.Y + (m_buttonSize - m_cy) / 2, 0,0,m_cx - 1, m_cy -1, Gdiplus::UnitPixel);
			}
			rcRect.Width-=1;
			rcRect.Height-=1;
			g.DrawRectangle(&btnLine, rcRect);

			//===Draw Slider
			rcRect = m_sliderRect;	rcRect.X = 0;
			if (0)
			{
				//===Draw Slider (Rectangle)
				g.FillRectangle(GetBrush(SBB_Slider), m_sliderRect);
				rcRect.Width -= 1;
				rcRect.Height -= 1;
				g.DrawRectangle(&btnLine, rcRect);
			}
			else
			{
				//===Draw Slider (rounded)
				GraphicsPath p;
				rcRect.Width -= 1;
				rcRect.Height -= 1;
				GdiplusUtilities::GetRoundRectGraphicsPath(p, rcRect, 3, All);
				g.FillPath(GetBrush(SBB_Slider), &p);
				g.DrawPath(&btnLine, &p);
			}

		}
		m_dblBuffer.Render(dest, m_scrollerRect.X, m_scrollerRect.Y);
	}
}

void FlexScrollbar::SetScrollPos(INT pos)
{
	//if (m_fullScroll == 0)
	//	INT a = 0;
	if (pos >= m_fullScroll - m_scrollerRect.Height)
		pos = m_fullScroll - m_scrollerRect.Height;
	if (pos < 0)
		pos = 0;
	if (m_scrollPos != pos)
	{
		m_scrollPos = pos;
		UpdateRects();
		if (m_pWndParent->m_hWnd)
			m_pWndParent->Invalidate();
	}
}
BOOL FlexScrollbar::HasScroll()
{
	return (m_fullScroll - m_scrollerRect.Height) > 0;
}

BOOL FlexScrollbar::ContainsPoint(INT x, INT y)
{
	return HasScroll() && m_scrollerRect.Contains(x, y);
}


BOOL FlexScrollbar::OnLButtonDown(INT x, INT y)
{
	if (!ContainsPoint(x, y))
		return FALSE;

	SBButtons btn = GetSBBFromPoint(x, y);
	SetMouseDownSBB(btn);
	switch (btn)
	{
	case SBB_Button1:	
		SetScrollPos(GetScrollPos() - m_scrollStep);
		break;
	case SBB_Button2:	
		SetScrollPos(GetScrollPos() + m_scrollStep);
		break;
	case SBB_Slider:
		m_bMouseDownState = TRUE;
		m_dyFromSliderOnMouseDown = y - SliderPosByScrollPos(GetScrollPos());
		//m_PostionOnMouseDown = y;
		//m_ScrollPosOnMouseDown = m_scrollPos;
		break;
	case SBB_Empty1:
		SetScrollPos(GetScrollPos() - GetScrollerRect().Height);
		break;
	case SBB_Empty2:
		SetScrollPos(GetScrollPos() + GetScrollerRect().Height);
		break;
	}
	return TRUE;
}

BOOL FlexScrollbar::OnLButtonDblClk(INT x, INT y)
{
	if (!ContainsPoint(x, y))
		return FALSE;
	SBButtons btn = GetSBBFromPoint(x, y);
	switch (btn)
	{
	case SBB_Button1:	
		SetScrollPos(GetScrollPos() - 2 * m_scrollStep);
		break;
	case SBB_Button2:	
		SetScrollPos(GetScrollPos() + 2 * m_scrollStep);
		break;
	case SBB_Empty1:
		SetScrollPos(GetScrollPos() - 2 * GetScrollerRect().Height);
		break;
	case SBB_Empty2:
		SetScrollPos(GetScrollPos() + 2 * GetScrollerRect().Height);
		break;
	}
	return TRUE;
}

BOOL FlexScrollbar::OnLButtonUp(INT x, INT y)
{
	if (m_bMouseDownState)
		m_bMouseDownState = FALSE;
	SetMouseDownSBB(SBB_None);
	if (!m_scrollerRect.Contains(x, y))
		return FALSE;
	return TRUE;
}

INT FlexScrollbar::ScrollPosBySliderPos(INT sliderPos) const
{
	INT minScrollPos = 0;
	INT maxScrollPos = m_fullScroll - m_scrollerRect.Height;
	INT dScroll = maxScrollPos - minScrollPos;

	INT minSliderPos = m_buttonSize + m_sliderRect.Height / 2;
	INT maxSliderPos = m_scrollerRect.Height - minSliderPos;
	INT dSlider = maxSliderPos - minSliderPos;
	if (sliderPos < minSliderPos)
		sliderPos = minSliderPos;
	if (sliderPos > maxSliderPos)
		sliderPos = maxSliderPos;
	return minScrollPos + (sliderPos - minSliderPos) * dScroll / dSlider;
}
INT FlexScrollbar::SliderPosByScrollPos(INT scrollPos) const
{
	INT minScrollPos = 0;
	INT maxScrollPos = m_fullScroll - m_scrollerRect.Height;
	INT dScroll = maxScrollPos - minScrollPos;

	INT minSliderPos = m_buttonSize + m_sliderRect.Height / 2;
	INT maxSliderPos = m_scrollerRect.Height - minSliderPos;
	INT dSlider = maxSliderPos - minSliderPos;
	
	return minSliderPos + (scrollPos - minScrollPos) * dSlider / dScroll;
}

BOOL FlexScrollbar::OnMouseMove(INT x, INT y)
{
	if (m_bMouseDownState)
	{
		//INT availableDist = m_scrollerRect.Height - 2 * m_buttonSize - m_sliderRect.Height;
		//SetScrollPos(m_ScrollPosOnMouseDown + 
		//	(y - m_PostionOnMouseDown) * m_fullScroll / availableDist);
		SetScrollPos(ScrollPosBySliderPos(y - m_dyFromSliderOnMouseDown));
		return TRUE;
	}
	if (!m_scrollerRect.Contains(x, y))
	{
		SetMouseDownSBB(SBB_None);
		SetMouseOverSBB(SBB_None);
		return FALSE;
	}
	SetMouseOverSBB(GetSBBFromPoint(x,y));
	return TRUE;
}

FlexScrollbar::SBButtons FlexScrollbar::GetSBBFromPoint(INT x, INT y)
{
	if (!m_scrollerRect.Contains(x,y))
		return SBB_None;
	if (m_button1Rect.Contains(x, y))//Button1 Clicked
		return SBB_Button1;
	if (m_button2Rect.Contains(x, y))//Button2 Clicked
		return SBB_Button2;
	if (m_sliderRect.Contains(x, y))//Slider Clicked
		return SBB_Slider;
	if (y < m_sliderRect.Y || x < m_sliderRect.X)
		return SBB_Empty1;
	return SBB_Empty2;
}

void FlexScrollbar::InvalidateScroller()
{
	RECT r;
	r.left = m_scrollerRect.X;
	r.top = m_scrollerRect.Y;
	r.right = m_scrollerRect.GetRight();
	r.bottom = m_scrollerRect.GetBottom();
	m_pWndParent->InvalidateRect(&r, FALSE);
}

void FlexScrollbar::SetMouseOverSBB(SBButtons btn)
{
	if (btn != m_mouseOverSBB)
	{
		m_mouseOverSBB = btn;
		InvalidateScroller();
	}
}
void FlexScrollbar::SetMouseDownSBB(SBButtons btn)
{
	if (btn != m_mouseDownSBB)
	{
		m_mouseDownSBB = btn;
		InvalidateScroller();
	}
}

void FlexScrollbar::SetScrollerRect(Gdiplus::Rect& rc)	
{
	if (rc.Width != m_scrollerRect.Width)
		ClearBrushes();
	m_scrollerRect = rc;
	UpdateRects();
	SetScrollPos(m_scrollPos);
}

void FlexScrollbar::UpdateRects()
{
	m_button1Rect = m_scrollerRect;
	m_button2Rect = m_scrollerRect;
	if (m_scrollerRect.Height / 2 < m_buttonSize)
	{
		m_button1Rect.Height = m_scrollerRect.Height / 2;
		m_button2Rect.Y = m_scrollerRect.GetBottom() - m_scrollerRect.Height / 2;
		m_button2Rect.Height = m_scrollerRect.Height / 2;
	}
	else
	{
		m_button1Rect.Height = m_buttonSize;
		m_button2Rect.Y = m_scrollerRect.GetBottom() - m_buttonSize;
		m_button2Rect.Height = m_buttonSize;
	}

	m_sliderRect = m_scrollerRect;
	if ((m_fullScroll - m_scrollerRect.Height) > 0)
	{
		INT availableDist = m_scrollerRect.Height - 2 * m_buttonSize;
		m_sliderRect.Height = (availableDist * m_scrollerRect.Height) / m_fullScroll;
		if (m_sliderRect.Height < 25)
			m_sliderRect.Height = 25;
		if (m_sliderRect.Height > availableDist)
			m_sliderRect.Height = 0;
		INT realScrollDist = availableDist - m_sliderRect.Height;
		//m_sliderRect.Y = m_buttonSize + (m_scrollPos * realScrollDist) / (m_fullScroll - availableDist);
		m_sliderRect.Y = m_buttonSize + (realScrollDist * (m_scrollPos + 1)) / (m_fullScroll - m_scrollerRect.Height);

	}
	else
		m_sliderRect = Rect();
}


