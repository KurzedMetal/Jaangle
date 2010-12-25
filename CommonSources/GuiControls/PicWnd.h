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

using namespace Gdiplus;

class CPicWnd : public CWnd
{
	DECLARE_DYNAMIC(CPicWnd)

public:
	CPicWnd();
	virtual ~CPicWnd();

	BOOL LoadFile(LPCTSTR imgFile);
	BOOL LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst = NULL);
	BOOL LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst = NULL);
	void ClearImage();

	void SetBkColor(COLORREF rgb, BYTE opacity = 255);
	GdiPlusPicDrawer::DrawParams& GetDrawParams()		{return m_picDrawer.GetDrawParams();}
	CSize GetPictureSize();

protected:
	void Redraw();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
private:
	GdiPlusPicDrawer m_picDrawer;
};


