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
#include "GdiplusUtilities.h"



Gdiplus::Bitmap* GdiplusUtilities::FromHICON32(HICON hIcon)
{
	Gdiplus::Bitmap* ret = NULL;
	ICONINFO iconInfo;
	GetIconInfo(hIcon, &iconInfo);
	BITMAP bitmapData;
	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bitmapData);

	if (bitmapData.bmBitsPixel != 32)
		ret = Gdiplus::Bitmap::FromHICON(hIcon);
	else
	{
		ret = new Gdiplus::Bitmap(bitmapData.bmWidth, bitmapData.bmHeight, PixelFormat32bppARGB);
		Gdiplus::BitmapData bmpData;
		ret->LockBits(&Gdiplus::Rect(0,0,bitmapData.bmWidth, bitmapData.bmHeight), Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bmpData);
#ifndef GetDIBitsVERSION
		//===Version GetBitmapBits
		// THIS FUNCTION IS UNDER TESTING. WHAT IF THE bitmap stride is different? Where will the new data go?
		ASSERT(bmpData.Stride == bmpData.Width * 4);
		::GetBitmapBits(iconInfo.hbmColor, 4 * bitmapData.bmWidth * bitmapData.bmHeight, bmpData.Scan0);
		//===Version GetBitmapBits===END
#else
		//===Version GetDIBits (incomplete)
		::GetDIBits(GetDC(), iconInfo.hbmColor, 0, bitmapData.bm)
			//===Version GetDIBits
#endif
			ret->UnlockBits(&bmpData);
	} 
	DeleteObject(iconInfo.hbmColor);
	DeleteObject(iconInfo.hbmMask);
	return ret;
}

Gdiplus::Color GdiplusUtilities::COLORREF2Color(COLORREF value, BYTE opacity/* = 255*/)
{
	return Gdiplus::Color::MakeARGB(opacity, GetRValue(value), GetGValue(value), GetBValue(value));
}

Gdiplus::Rect GdiplusUtilities::RECT2GdiplusRect(const RECT& value)
{
	return Gdiplus::Rect(value.left, value.top, value.right - value.left, value.bottom - value.top);
}

RECT GdiplusUtilities::GdiplusRect2RECT(const Gdiplus::Rect& value)
{
	RECT rc;
	rc.left = value.X;
	rc.top = value.Y;
	rc.right = value.X + value.Width;
	rc.bottom = value.Y + value.Height;
	return rc;
}



BOOL GdiplusUtilities::GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& p, 
			int x, int y, int width, int height, int radius, RectangleCorners corners)
{
	int xw = x + width;
	int yh = y + height;
	int xwr = xw - radius;
	int yhr = yh - radius;
	int xr = x + radius;
	int yr = y + radius;
	int r2 = radius * 2;
	int xwr2 = xw - r2;
	int yhr2 = yh - r2;

	p.StartFigure();

	//Top Left Corner
	if ((TopLeft & corners)	== TopLeft)
	{
		p.AddArc(x, y, r2, r2, 180, 90);
	}
	else
	{
		p.AddLine(x, yr, x, y);
		p.AddLine(x, y, xr, y);
	}

	//Top Edge
	p.AddLine(xr, y, xwr, y);

	//Top Right Corner
	if ((TopRight & corners)
		== TopRight)
	{
		p.AddArc(xwr2, y, r2, r2, 270, 90);
	}
	else
	{
		p.AddLine(xwr, y, xw, y);
		p.AddLine(xw, y, xw, yr);
	}

	//Right Edge
	p.AddLine(xw, yr, xw, yhr);

	//Bottom Right Corner
	if ((BottomRight & corners)
		== BottomRight)
	{
		p.AddArc(xwr2, yhr2, r2, r2, 0, 90);
	}
	else
	{
		p.AddLine(xw, yhr, xw, yh);
		p.AddLine(xw, yh, xwr, yh);
	}

	//Bottom Edge
	p.AddLine(xwr, yh, xr, yh);

	//Bottom Left Corner
	if ((BottomLeft & corners)
		== BottomLeft)
	{
		p.AddArc(x, yhr2, r2, r2, 90, 90);
	}
	else
	{
		p.AddLine(xr, yh, x, yh);
		p.AddLine(x, yh, x, yhr);
	}

	//Left Edge
	p.AddLine(x, yhr, x, yr);

	p.CloseFigure();
	return TRUE;
}

BOOL GdiplusUtilities::GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& p, 
					Gdiplus::Rect rect, int radius, RectangleCorners corners)
{
	return GetRoundRectGraphicsPath(p, rect.X, rect.Y, rect.Width,	rect.Height, radius, corners);
}

BOOL GdiplusUtilities::GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& p, 
					int x, int y, int width, int height, int radius)
{
	return GetRoundRectGraphicsPath(p, x, y, width, height, radius,	All);
}

BOOL GdiplusUtilities::GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& p, 
					Gdiplus::Rect rect, int radius)
{
	return GetRoundRectGraphicsPath(p, rect.X, rect.Y, rect.Width,
		rect.Height, radius);
}

BOOL GdiplusUtilities::GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& p, 
					int x, int y,
								  int width, int height)
{ 
	return GetRoundRectGraphicsPath(p, x, y, width, height, 5); 
}

BOOL GdiplusUtilities::GetRoundRectGraphicsPath(Gdiplus::GraphicsPath& p, Gdiplus::Rect rect)
{ 
	return GetRoundRectGraphicsPath(p, rect.X, rect.Y, rect.Width, rect.Height); 
}

BOOL GdiplusUtilities::DrawTextOutline(Gdiplus::Graphics& graphics, 
					 LPCTSTR lpchText, int cchText, const Gdiplus::Rect& rc, const Gdiplus::StringFormat& format, 
					 const Gdiplus::Font& font, Gdiplus::Color fill, Gdiplus::Color outline, INT outlineWidth,
					 BOOL bCalcOnly/* = FALSE*/, Gdiplus::Rect* rcCalc/* = NULL*/)
{
	Gdiplus::FontFamily fontFamily;
	font.GetFamily(&fontFamily);
	Gdiplus::GraphicsPath path;
	path.AddString(lpchText, cchText, &fontFamily, font.GetStyle(), font.GetHeight(&graphics), rc, &format);

	if (rcCalc != NULL)
		path.GetBounds(rcCalc);
	if (bCalcOnly)
		return TRUE;

	Gdiplus::Pen pen(outline, (Gdiplus::REAL) outlineWidth);
	if (graphics.DrawPath(&pen, &path) == Gdiplus::Ok)
	{
		Gdiplus::SolidBrush brush(fill);
		return graphics.FillPath(&brush, &path) == Gdiplus::Ok;
	}
	return FALSE;
}



BOOL GdiplusUtilities::DrawTextOutline(Gdiplus::Graphics& graphics, 
									   LPCTSTR lpchText, int cchText, const RECT* lprc, UINT format, 
									   const LOGFONT& lf, COLORREF fill, COLORREF outline, INT outlineWidth,
									   BOOL bCalcOnly /*= FALSE*/, RECT* rcCalc/* = NULL*/)
{
	HDC hdc = graphics.GetHDC();
	Gdiplus::Font font(hdc, &lf);
	graphics.ReleaseHDC(hdc);
	Gdiplus::StringFormat sFormat;
	if (format & DT_VCENTER)
		sFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	else if (format & DT_BOTTOM)
		sFormat.SetLineAlignment(Gdiplus::StringAlignmentFar);
	else
		sFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);

	if (format & DT_CENTER)
		sFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	else if (format & DT_RIGHT)
		sFormat.SetAlignment(Gdiplus::StringAlignmentFar);
	else
		sFormat.SetAlignment(Gdiplus::StringAlignmentNear);

	Gdiplus::Rect rcForCalc;
	Gdiplus::Rect* pRCForCalc = &rcForCalc;
	if (rcCalc == NULL)
		pRCForCalc = NULL;
	if (DrawTextOutline(graphics, lpchText, cchText, RECT2GdiplusRect(*lprc), sFormat, font, 
		COLORREF2Color(fill), COLORREF2Color(outline), outlineWidth, bCalcOnly, pRCForCalc))
	{
		if (pRCForCalc != NULL)
			*rcCalc = GdiplusRect2RECT(*pRCForCalc);
		return TRUE;
	}
	return FALSE;

}

Gdiplus::RectF GdiplusUtilities::DrawTextMeasure(Gdiplus::Graphics& graphics, LPCTSTR lpchText, int cchText, const Gdiplus::Font& font)
{
	Gdiplus::PointF p;
	Gdiplus::RectF rc;
	if (graphics.MeasureString(lpchText, cchText, &font, p, &rc))
	{
	}
	return rc;
}

Gdiplus::RectF GdiplusUtilities::DrawTextMeasure(Gdiplus::Graphics& graphics, LPCTSTR lpchText, int cchText, const LOGFONT& lf)
{
	HDC hdc = graphics.GetHDC();
	Gdiplus::Font font(hdc, &lf);
	graphics.ReleaseHDC(hdc);
	return DrawTextMeasure(graphics, lpchText, cchText, font);
}



