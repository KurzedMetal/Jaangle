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
#ifndef _FlickerFree_h_
#define _FlickerFree_h_

//Class Objective
//Make an efficient memory DC for double buffering graphics

//Class Usage

//	1. Make a Member Variable in the CWnd declaration
//	CFlickerFree m_ff; 

//	2. Write OnPaint like this. [Use dc for drawing. m_ff is a CFlickerFree member var]
//	CPaintDC _dc(this); // device context for painting
//	CFlickerFreeDC dc(m_ff, _dc);

//	---Optional---
//	3. Override OnEraseBkgnd like this. [Draw everything in OnPaint. Can cause flickering]
//	BOOL CWnd::OnEraseBkgnd(CDC* pDC)
//	{
//		return TRUE;
//	}


class CFlickerFree
{
public:
	CFlickerFree();
	~CFlickerFree();

	CDC& GetMemDC();
	void SetTargetDC(CDC& targetDC);
	void Draw();
private:
	void SetSize(INT cx, INT cy);
private:
	CDC m_dc;
	CBitmap m_bitmap;
	CBitmap* m_pOldBitmap;
	CDC* m_targetDC;
	CRect m_rect;
	CRect m_BitmapRect;
};

class CFlickerFreeDC:public CDC
{
public:
	explicit CFlickerFreeDC(CFlickerFree& ff, CDC& dc);
	virtual ~CFlickerFreeDC();

private:
	CFlickerFree* m_ff;
};
#endif
