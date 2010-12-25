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

enum RectangleCorners
{
	None = 0, TopLeft = 1, TopRight = 2,
	BottomLeft = 4, BottomRight = 8,
	All = TopLeft | TopRight | BottomLeft | BottomRight
};

class GdiplusUtilities
{
public:
	//Creates a bitmap from an ICON. This corrects the GDI+ bug in the Icons alpha channel
	static Gdiplus::Bitmap* FromHICON32(HICON hIcon);

	static Gdiplus::Color COLORREF2Color(COLORREF value, BYTE opacity = 255);
	static Gdiplus::Rect RECT2GdiplusRect(const RECT& value);
	static RECT GdiplusRect2RECT(const Gdiplus::Rect& value);

	//This gets a GraphicsPath for a round rectangle which is missing from GDI+
	static BOOL GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& path, int x, int y, int width, int height, int radius, RectangleCorners corners);
	static BOOL GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& path, Gdiplus::Rect rect, int radius, RectangleCorners corners);
	static BOOL GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& path, int x, int y, int width, int height, int radius);
	static BOOL GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& path, Gdiplus::Rect rect, int radius);
	static BOOL GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& path, int x, int y, int width, int height);
	static BOOL GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& path, Gdiplus::Rect rect);

	static BOOL DrawTextOutline(Gdiplus::Graphics& graphics, 
		LPCTSTR lpchText, int cchText, const Gdiplus::Rect& rc, const Gdiplus::StringFormat& format, 
		const Gdiplus::Font& font, Gdiplus::Color fill, Gdiplus::Color outline, INT outlineWidth,
		BOOL bCalcOnly = FALSE, Gdiplus::Rect* rcCalc = NULL);
	static BOOL DrawTextOutline(Gdiplus::Graphics& graphics, 
		LPCTSTR lpchText, int cchText, const RECT* lprc, UINT format, 
		const LOGFONT& lf, COLORREF fill, COLORREF outline, INT outlineWidth,
		BOOL bCalcOnly = FALSE, RECT* rcCalc = NULL);

	static Gdiplus::RectF DrawTextMeasure(Gdiplus::Graphics& graphics, LPCTSTR lpchText, int cchText, const Gdiplus::Font& font);
	static Gdiplus::RectF DrawTextMeasure(Gdiplus::Graphics& graphics, LPCTSTR lpchText, int cchText, const LOGFONT& lf);


};



