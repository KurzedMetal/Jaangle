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
#include "hdcUtils.h"

HLS RGBtoHLS(COLORREF lRGBColor)
{
	WORD R,G,B;          /* input RGB values */ 
	WORD cMax,cMin;      /* max and min RGB values */ 
	WORD  Rdelta,Gdelta,Bdelta; /* intermediate value: % of spread from max

								*/ 
	/* get R, G, and B out of DWORD */
	HLS ret;
	R = GetRValue(lRGBColor);
	G = GetGValue(lRGBColor);
	B = GetBValue(lRGBColor);

	/* calculate lightness */ 
	cMax = max( max(R,G), B);
	cMin = min( min(R,G), B);
	ret.lum = ( ((cMax+cMin)*HLSMAX) + RGBMAX )/(2*RGBMAX);

	if (cMax == cMin) {           /* r=g=b --> achromatic case */ 
		ret.sat = 0;                     /* saturation */ 
		ret.hue = UNDEFINED;             /* hue */ 
	}
	else {                        /* chromatic case */ 
		/* saturation */ 
		if (ret.lum <= (HLSMAX/2))
			ret.sat = ( ((cMax-cMin)*HLSMAX) + ((cMax+cMin)/2) ) / (cMax+cMin);
		else
			ret.sat = ( ((cMax-cMin)*HLSMAX) + ((2*RGBMAX-cMax-cMin)/2) )
			/ (2*RGBMAX-cMax-cMin);

		/* hue */ 
		Rdelta = ( ((cMax-R)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
		Gdelta = ( ((cMax-G)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);
		Bdelta = ( ((cMax-B)*(HLSMAX/6)) + ((cMax-cMin)/2) ) / (cMax-cMin);

		if (R == cMax)
			ret.hue = Bdelta - Gdelta;
		else if (G == cMax)
			ret.hue = (HLSMAX/3) + Rdelta - Bdelta;
		else /* B == cMax */ 
			ret.hue = ((2*HLSMAX)/3) + Gdelta - Rdelta;

		if (ret.hue < 0)
			ret.hue += HLSMAX;
		if (ret.hue > HLSMAX)
			ret.hue -= HLSMAX;
	}
	return ret;
}
/* utility routine for HLStoRGB */ 
WORD HueToRGB(WORD n1, WORD n2, WORD hue)
{
	/* range check: note values passed add/subtract thirds of range */ 
	if (hue < 0)
		hue += HLSMAX;

	if (hue > HLSMAX)
		hue -= HLSMAX;

	/* return r,g, or b value from this tridrant */ 
	if (hue < (HLSMAX/6))
		return ( n1 + (((n2-n1)*hue+(HLSMAX/12))/(HLSMAX/6)) );
	if (hue < (HLSMAX/2))
		return ( n2 );
	if (hue < ((HLSMAX*2)/3))
		return ( n1 +    (((n2-n1)*(((HLSMAX*2)/3)-hue)+(HLSMAX/12))/(HLSMAX/6))
		);
	else
		return ( n1 );
}

COLORREF HLStoRGB(HLS hls)
{
	WORD R,G,B;                /* RGB component values */ 
	WORD  Magic1,Magic2;       /* calculated magic numbers (really!) */ 

	if (hls.sat == 0) {            /* achromatic case */ 
		R=G=B=(hls.lum*RGBMAX)/HLSMAX;
		if (hls.hue != UNDEFINED) {
			/* ERROR */ 
		}
	}
	else  {                    /* chromatic case */ 
		/* set up magic numbers */ 
		if (hls.lum <= (HLSMAX/2))
			Magic2 = (hls.lum*(HLSMAX + hls.sat) + (HLSMAX/2))/HLSMAX;
		else
			Magic2 = hls.lum + hls.sat - ((hls.lum*hls.sat) + (HLSMAX/2))/HLSMAX;
		Magic1 = 2*hls.lum-Magic2;

		/* get RGB, change units from HLSMAX to RGBMAX */ 
		R = (HueToRGB(Magic1,Magic2,hls.hue+(HLSMAX/3))*RGBMAX +
			(HLSMAX/2))/HLSMAX;
		G = (HueToRGB(Magic1,Magic2,hls.hue)*RGBMAX + (HLSMAX/2)) / HLSMAX;
		B = (HueToRGB(Magic1,Magic2,hls.hue-(HLSMAX/3))*RGBMAX +
			(HLSMAX/2))/HLSMAX;
	}
	return(RGB(R,G,B));
}

COLORREF GrayMirrorColor(COLORREF clrColor)
{
	BYTE nGrayColor = (BYTE)((GetBValue(clrColor) * 0.299) + (GetGValue(clrColor) * 0.587) + (GetRValue(clrColor) * 0.114));

	return RGB(nGrayColor, nGrayColor, nGrayColor);
} //End of GrayMirrorColor

COLORREF GrayColor(COLORREF clrColor)
{
	BYTE nGrayColor = (BYTE)((GetRValue(clrColor) * 0.299) + (GetGValue(clrColor) * 0.587) + (GetBValue(clrColor) * 0.114));

	return RGB(nGrayColor, nGrayColor, nGrayColor);
} //End GrayColor

COLORREF InvertColor(COLORREF clrColor)
{
	return RGB(255 - GetRValue(clrColor), 255 - GetGValue(clrColor), 255 - GetBValue(clrColor));
} //End InvertColor

COLORREF MaxContrastColor(COLORREF clrColor)
{
	if (GetRValue(clrColor) + GetGValue(clrColor) + GetBValue(clrColor) > 255 * 3 / 2)
		return RGB(0,0,0);
	return RGB(255,255,255);
}


COLORREF DarkenColor(COLORREF clrColor, double darken)
{
	if (darken >= 0.0 && darken < 1.0)
	{
		BYTE color_r, color_g, color_b;
		color_r = (BYTE)(GetRValue(clrColor) * darken);
		color_g = (BYTE)(GetGValue(clrColor) * darken);
		color_b = (BYTE)(GetBValue(clrColor) * darken);
		clrColor = RGB(color_r, color_g, color_b);
	} //if

	return clrColor;
} //End DarkenColor

COLORREF LightenColor(COLORREF clrColor, double lighten)
{
	if (lighten > 0.0 && lighten <= 1.0)
	{
		BYTE color_r, color_g, color_b;

		color_r = (BYTE)min((DWORD)GetRValue(clrColor) + lighten * 255, 255.0);
		color_g = (BYTE)min((DWORD)GetGValue(clrColor) + lighten * 255, 255.0);
		color_b = (BYTE)min((DWORD)GetBValue(clrColor) + lighten * 255, 255.0);
		clrColor = RGB(color_r, color_g, color_b);
		/*		
		lighten *= 255
		color_r = (BYTE)max(0, min(255, (int)((color_r - 128) * 2.0 + 128 + lighten)));
		color_g = (BYTE)max(0, min(255, (int)((color_g - 128) * 2.0 + 128 + lighten)));
		color_b = (BYTE)max(0, min(255, (int)((color_b - 128) * 2.0 + 128 + lighten)));
		clrColor = RGB(color_r, color_g, color_b);
		*/
	} //if

	return clrColor;
} //End LightenColor




void DitherBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth,
				int nHeight, HBITMAP hbm, int nXSrc, int nYSrc)
{
	ASSERT(hdcDest && hbm);
	ASSERT(nWidth > 0 && nHeight > 0);

	// Create a generic DC for all BitBlts
	HDC hDC = CreateCompatibleDC(hdcDest);
	ASSERT(hDC);

	if (hDC)
	{
		// Create a DC for the monochrome DIB section
		HDC bwDC = CreateCompatibleDC(hDC);
		ASSERT(bwDC);

		if (bwDC)
		{
			// Create the monochrome DIB section with a black and white palette
			struct {
				BITMAPINFOHEADER bmiHeader;
				RGBQUAD 		 bmiColors[2];
			} RGBBWBITMAPINFO = {

				{		// a BITMAPINFOHEADER
					sizeof(BITMAPINFOHEADER),	// biSize 
						nWidth, 				// biWidth; 
						nHeight,				// biHeight; 
						1,						// biPlanes; 
						1,						// biBitCount 
						BI_RGB, 				// biCompression; 
						0,						// biSizeImage; 
						0,						// biXPelsPerMeter; 
						0,						// biYPelsPerMeter; 
						0,						// biClrUsed; 
						0						// biClrImportant; 
				},

				{
					{ 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }
					}
			};
			VOID *pbitsBW;
			HBITMAP hbmBW = CreateDIBSection(bwDC,
				(LPBITMAPINFO)&RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
			ASSERT(hbmBW);

			if (hbmBW)
			{
				// Attach the monochrome DIB section and the bitmap to the DCs
				SelectObject(bwDC, hbmBW);
				SelectObject(hDC, hbm);

				// BitBlt the bitmap into the monochrome DIB section
				BitBlt(bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);

				// Paint the destination rectangle in gray
				FillRect(hdcDest, CRect(nXDest, nYDest, nXDest + nWidth, nYDest +
					nHeight), GetSysColorBrush(COLOR_3DFACE));

				// BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT bits in the destination DC
				// The magic ROP comes from the Charles Petzold's book
				HBRUSH hb = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdcDest, hb);
				BitBlt(hdcDest, nXDest + 1, nYDest + 1, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);

				// BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW bits in the destination DC
				hb = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
				DeleteObject(SelectObject(hdcDest, hb));
				BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
				DeleteObject(SelectObject(hdcDest, oldBrush));
			}

			VERIFY(DeleteDC(bwDC));
		}

		VERIFY(DeleteDC(hDC));
	}
}


void GradientFillRect( HDC hdc, CRect &rect, COLORREF col_from, COLORREF col_to, bool vert_grad )
{
	TRIVERTEX        vert[2];
	GRADIENT_RECT    mesh;

	vert[0].x      = rect.left;
	vert[0].y      = rect.top;
	vert[0].Alpha  = 0x0000;
	vert[0].Blue   = GetBValue(col_from) << 8;
	vert[0].Green  = GetGValue(col_from) << 8;
	vert[0].Red    = GetRValue(col_from) << 8;

	vert[1].x      = rect.right;
	vert[1].y      = rect.bottom; 
	vert[1].Alpha  = 0x0000;
	vert[1].Blue   = GetBValue(col_to) << 8;
	vert[1].Green  = GetGValue(col_to) << 8;
	vert[1].Red    = GetRValue(col_to) << 8;

	mesh.UpperLeft  = 0;
	mesh.LowerRight = 1;

	GradientFill( hdc, vert, 2, &mesh, 1, vert_grad ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H );
}

HICON CreateIconHLS(HICON srchIcon, INT hue, INT lum, INT sat)
{
	ICONINFO iconInfo;
	GetIconInfo(srchIcon, &iconInfo);
	BITMAP bmpColor;
	int result = GetObject(iconInfo.hbmColor, sizeof(BITMAP), (void*) &bmpColor);
	if (result == 0)
		return 0;

	INT iconCX = bmpColor.bmWidth;
	INT iconCY = bmpColor.bmHeight;
	INT bmpColorSize = iconCX * iconCY * bmpColor.bmBitsPixel / 8;

	RGBQUAD* bf = new RGBQUAD[iconCX * iconCY];
	GetBitmapBits(iconInfo.hbmColor, bmpColorSize, bf);

	for (int i = 0 ; i < iconCX * iconCY ; i++) 
	{
		//Quick Grey Filter
		//bf[i].rgbRed = bf[i].rgbGreen = bf[i].rgbBlue = ((bf[i].rgbRed + bf[i].rgbGreen + bf[i].rgbBlue) / 3);
		HLS hls = RGBtoHLS(RGB(bf[i].rgbRed, bf[i].rgbGreen, bf[i].rgbBlue));
		hls.hue += hue;
		hls.sat += sat;
		hls.lum += lum;
		COLORREF rgb = HLStoRGB(hls);
		bf[i].rgbRed = GetRValue(rgb);
		bf[i].rgbGreen = GetGValue(rgb);
		bf[i].rgbBlue = GetBValue(rgb);
	}
	SetBitmapBits(iconInfo.hbmColor, bmpColorSize, bf);
	delete bf;
	return  CreateIconIndirect(&iconInfo);
}

HICON CreateIconOPERATOR(HICON srchIcon, CIOPERATOR oper, INT red, INT green, INT blue)
{
	ICONINFO iconInfo;
	GetIconInfo(srchIcon, &iconInfo);
	BITMAP bmpColor;
	int result = GetObject(iconInfo.hbmColor, sizeof(BITMAP), (void*) &bmpColor);
	if (result == 0)
		return 0;

	INT iconCX = bmpColor.bmWidth;
	INT iconCY = bmpColor.bmHeight;
	INT bmpColorSize = iconCX * iconCY * bmpColor.bmBitsPixel / 8;

	RGBQUAD* bf = new RGBQUAD[iconCX * iconCY];
	GetBitmapBits(iconInfo.hbmColor, bmpColorSize, bf);

	for (int i = 0 ; i < iconCX * iconCY ; i++) 
	{
		//Quick Grey Filter
		//bf[i].rgbRed = bf[i].rgbGreen = bf[i].rgbBlue = ((bf[i].rgbRed + bf[i].rgbGreen + bf[i].rgbBlue) / 3);
		if (oper == CIO_and)
		{
			bf[i].rgbRed &= red;
			bf[i].rgbGreen &= green;
			bf[i].rgbBlue &= blue;
		}
		else if (oper == CIO_or)
		{
			bf[i].rgbRed |= red;
			bf[i].rgbGreen |= green;
			bf[i].rgbBlue |= blue;
		}
		else if (oper == CIO_xor)
		{
			bf[i].rgbRed ^= red;
			bf[i].rgbGreen ^= green;
			bf[i].rgbBlue ^= blue;
		}
		else if (oper == CIO_not)
		{
			bf[i].rgbRed = !bf[i].rgbRed;
			bf[i].rgbGreen = !bf[i].rgbGreen;
			bf[i].rgbBlue = !bf[i].rgbBlue;
		}
	}
	SetBitmapBits(iconInfo.hbmColor, bmpColorSize, bf);
	delete bf;
	return  CreateIconIndirect(&iconInfo);

}


BOOL DrawIconExHLS(HDC hdc, int xLeft, int yTop, HICON hIcon, int cxWidth, int cyWidth, UINT diFlags,
				   INT hue, INT lum, INT sat)
{
	HICON newIcon = CreateIconHLS(hIcon, hue, lum, sat);
	if (newIcon == 0)
		return FALSE;
	BOOL ret = DrawIconEx(hdc, xLeft, yTop, newIcon, cxWidth, cyWidth, 0, 0, diFlags);
	DeleteObject(newIcon);
	return ret;
}


COLORREF MixColors(COLORREF col1, COLORREF col2, INT part1/* = 1*/, INT part2/* = 1*/)
{
	return RGB( 
		(part1 * GetRValue(col1) + part2 * GetRValue(col2)) / (part1 + part2),
		(part1 * GetGValue(col1) + part2 * GetGValue(col2)) / (part1 + part2),
		(part1 * GetBValue(col1) + part2 * GetBValue(col2)) / (part1 + part2)
		);
}

