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
#include "TeenSpirit.h"
#include "CaptionBordStatic.h"


// CCaptionBordStatic

IMPLEMENT_DYNAMIC(CCaptionBordStatic, CLabelEx)

CCaptionBordStatic::CCaptionBordStatic():
m_bMouseDown(FALSE)
{

}

CCaptionBordStatic::~CCaptionBordStatic()
{
}


BEGIN_MESSAGE_MAP(CCaptionBordStatic, CLabelEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CCaptionBordStatic message handlers



void CCaptionBordStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseDown = TRUE;
	SetCapture();
	m_mouseDownPoint = point;
	CLabelEx::OnLButtonDown(nFlags, point);
}

void CCaptionBordStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bMouseDown )
	{
		CSize s = m_mouseDownPoint - point;
		CRect r;
		GetParent()->GetWindowRect(&r);
		r = s - &r;
		GetParent()->MoveWindow(r);
	}
	CLabelEx::OnMouseMove(nFlags, point);
}

void CCaptionBordStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDown = FALSE;
	ReleaseCapture();
	CLabelEx::OnLButtonUp(nFlags, point);
}
