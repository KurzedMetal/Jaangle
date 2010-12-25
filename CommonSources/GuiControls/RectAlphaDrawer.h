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

#include "IAlphaDrawer.h"
class RectAlphaDrawer:public IAlphaDrawer
{
public:
	RectAlphaDrawer(void);
	virtual ~RectAlphaDrawer(void);
	
	virtual void SetColor(COLORREF col);
	virtual void Draw(CDC& dc, CRect& rc, BYTE Alpha);

	//Advanced Functions
	virtual CDC* GetInternalDC();
	virtual CRect* GetInternalRect();

	virtual void SetDrawOptions(UINT drawOptions);

private:
	virtual void CreateInternalDC(CDC* pParentDC = NULL);

	CDC* m_pDC;
	CBitmap* m_pBMP;
	CBitmap* m_pOldBMP;
	static CRect s_InternalRect;
	COLORREF m_bkColor;
	UINT m_drawOptions;

};
