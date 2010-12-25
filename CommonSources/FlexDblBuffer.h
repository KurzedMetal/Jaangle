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
#include "GdiPlusInstance.h"

class FlexDblBuffer
{
public:
	FlexDblBuffer():m_pG(NULL),m_pB(NULL),m_cx(0),m_cy(0),m_smoothingMode(Gdiplus::SmoothingModeDefault){}
	~FlexDblBuffer();

	//You must not store this objects as they may be temporary
	//Typical Usage...
	//GetGraphics(...) at the beginning of the OnPaint
	//Render(...) at the end
	Gdiplus::Graphics* GetGraphics(INT cx, INT cy);
	Gdiplus::Bitmap* GetBitmap();
	void Render(Gdiplus::Graphics& destination, INT X = 0, INT Y = 0);
	void ConfigBuffer(Gdiplus::SmoothingMode smoothingMode);
		
private:
	Gdiplus::Graphics* m_pG;
	Gdiplus::Bitmap* m_pB;
	GdiPlusInstance inst;
	UINT m_cx;
	UINT m_cy;
	Gdiplus::SmoothingMode m_smoothingMode;

};

