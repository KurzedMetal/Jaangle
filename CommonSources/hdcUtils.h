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
#ifndef _hdcUtils_h_
#define _hdcUtils_h_

//-------------HLS 2 RGB 2 HLS conversion utilities----------------------------------
#define  HLSMAX   100 /* H,L, and S vary over 0-HLSMAX */ 
#define  RGBMAX   255   /* R,G, and B vary over 0-RGBMAX */ 
/* HLSMAX BEST IF DIVISIBLE BY 6 */ 
/* RGBMAX, HLSMAX must each fit in a byte. */ 

/* Hue is undefined if Saturation is 0 (grey-scale) */ 
/* This value determines where the Hue scrollbar is */ 
/* initially set for achromatic colors */ 
#define UNDEFINED (HLSMAX*2/3)
struct HLS
{
	WORD hue;
	WORD lum;
	WORD sat;
};
HLS RGBtoHLS(COLORREF lRGBColor);
COLORREF HLStoRGB(HLS hls);

COLORREF MixColors(COLORREF col1, COLORREF col2, INT part1 = 1, INT part2 = 1);

COLORREF GrayMirrorColor(COLORREF clrColor);
COLORREF GrayColor(COLORREF clrColor);
COLORREF DarkenColor(COLORREF clrColor, double darken);
COLORREF LightenColor(COLORREF clrColor, double lighten);
COLORREF InvertColor(COLORREF clrColor);
COLORREF MaxContrastColor(COLORREF clrColor);


//----------------------------------------------------------------------
//      DitherBlt :     Draw a bitmap dithered (3D grayed effect like disabled buttons in toolbars) into a destination DC
//      Author :        Jean-Edouard Lachand-Robert (iamwired@geocities.com), June 1997.
//
//      hdcDest :       destination DC
//      nXDest :        x coordinate of the upper left corner of the destination rectangle into the DC
//      nYDest :        y coordinate of the upper left corner of the destination rectangle into the DC
//      nWidth :        width of the destination rectangle into the DC
//      nHeight :       height of the destination rectangle into the DC
//      hbm :           the bitmap to draw (as a part or as a whole)
//      nXSrc :         x coordinates of the upper left corner of the source rectangle into the bitmap
//      nYSrc :         y coordinates of the upper left corner of the source rectangle into the bitmap
//
void DitherBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth,
				int nHeight, HBITMAP hbm, int nXSrc, int nYSrc);



void GradientFillRect( HDC hdc, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad);

HICON CreateIconHLS(HICON srchIcon, INT hue, INT lum, INT sat);
enum CIOPERATOR
{
	CIO_and = 0,
	CIO_or,
	CIO_xor,
	CIO_not,
	CIO_last
};
HICON CreateIconOPERATOR(HICON srchIcon, CIOPERATOR oper, INT red, INT green, INT blue);
BOOL DrawIconExHLS(HDC hdc, int xLeft, int yTop, HICON hIcon, int cxWidth, int cyWidth, UINT diFlags, INT hue, INT lum, INT sat);




#endif
