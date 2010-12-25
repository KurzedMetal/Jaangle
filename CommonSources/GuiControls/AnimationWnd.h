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

#include "GdiPlusPicDrawer.h"

class CAnimationWnd : public CWnd
{
	DECLARE_DYNAMIC(CAnimationWnd)

public:
	CAnimationWnd();
	virtual ~CAnimationWnd();

	BOOL LoadFile(LPCTSTR imgFile);
	BOOL LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst = NULL);
	BOOL LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst = NULL);

	BOOL HasPicture()									{return m_picDrawer.GetPictureSize().cx > 0;}

	void SetFrameSize(INT cx, INT cy)					{m_cx=cx;	m_cy=cy; CalculateFrames();}
	CSize GetFrameSize()								{return CSize(m_cx,m_cy);}
	void SetBkColor(COLORREF rgb, BYTE opacity = 255);
	void SetAutoSize(BOOL bAutoSize);
	BOOL GetAutoSize();
	
	void SetAnimationDelay(UINT delayMS);


	void Start();
	void Stop();

protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

	void DrawAnimation(CDC& dc, CRect& rc);
private:
	void ClearImage();
	void CalculateFrames();
	void InitializeDrawer();
private:
	INT m_cx;
	INT m_cy;
	UINT m_delayMS;
	INT m_curFrame;
	INT m_TotalFrames;
	BOOL m_bStarted;
	GdiPlusPicDrawer m_picDrawer;
	GdiPlusPicDrawer m_picLoader;
	Color m_bkColor;
};


