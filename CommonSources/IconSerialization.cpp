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
#include "IconSerialization.h"
#include "URLEncDecHelper.h"

HICON IconSerialization::LoadIcon(LPCSTR iconDescription)
{
	BYTE* b = URLEncDecHelper::DecodeData(iconDescription, -1, FALSE, TRUE);
	if (b == NULL)
	{
		TRACE(_T("IconSerialization::LoadIcon DecodeData Failed.\r\n"));
		return NULL;
	}
	//=== Text Structure
	//		ICONINFO
	//		BITMAP (color)
	//		BITMAP (mask)
	//		BitmapBits (color)
	//		BitmapBits (mask)

	ICONINFO iInfo;
	BITMAP bmpColor;
	BITMAP bmpMask;
	UINT pos = 0;
	memcpy(&iInfo, &b[pos], sizeof(ICONINFO));
	pos += sizeof(ICONINFO);
	memcpy(&bmpColor, &b[pos], sizeof(BITMAP));
	pos += sizeof(BITMAP);
	memcpy(&bmpMask, &b[pos], sizeof(BITMAP));
	pos += sizeof(BITMAP);
	iInfo.hbmColor = CreateBitmap(bmpColor.bmWidth, bmpColor.bmHeight, bmpColor.bmPlanes, bmpColor.bmBitsPixel, &b[pos]);
	pos += bmpColor.bmWidthBytes * bmpColor.bmHeight;
	iInfo.hbmMask = CreateBitmap(bmpMask.bmWidth, bmpMask.bmHeight, bmpMask.bmPlanes, bmpMask.bmBitsPixel, &b[pos]);
	pos += bmpMask.bmWidthBytes * bmpMask.bmHeight;


	HICON hIcon = CreateIconIndirect(&iInfo);
	if (hIcon == NULL)
		TRACE(_T("IconSerialization::LoadIcon CreateIconIndirect Failed.\r\n"));
	delete b;
	return hIcon;
}

LPSTR IconSerialization::SaveIcon(HICON hIcon)
{
	//=== Get the icon info
	ICONINFO iInfo;
	GetIconInfo(hIcon, &iInfo);

	//=== Get the bmpColor Info
	BITMAP bmpColor;
	::GetObject(iInfo.hbmColor, sizeof(BITMAP), &bmpColor);
	UINT bmpColorBitsLen = bmpColor.bmWidthBytes * bmpColor.bmHeight;

	//=== Get the bmpMask Info
	BITMAP bmpMask;
	::GetObject(iInfo.hbmMask, sizeof(BITMAP), &bmpMask);
	UINT bmpMaskBitsLen = bmpMask.bmWidthBytes * bmpMask.bmHeight;


	INT bfByteSize = sizeof(ICONINFO) + 2 * sizeof(BITMAP) + bmpColorBitsLen + bmpMaskBitsLen;
	BYTE* b = new BYTE[bfByteSize];

	UINT pos = 0;
	memcpy(&b[pos], &iInfo, sizeof(ICONINFO));
	pos += sizeof(ICONINFO);
	memcpy(&b[pos], &bmpColor, sizeof(BITMAP));
	pos += sizeof(BITMAP);
	memcpy(&b[pos], &bmpMask, sizeof(BITMAP));
	pos += sizeof(BITMAP);
	::GetBitmapBits(iInfo.hbmColor, bmpColorBitsLen, &b[pos]);
	pos += bmpColorBitsLen;
	::GetBitmapBits(iInfo.hbmMask, bmpMaskBitsLen, &b[pos]);
	pos += bmpMaskBitsLen;

	//LPSTR hex = URLEncDecHelper::EncodeHex(b, bfByteSize);
	LPSTR ret = URLEncDecHelper::EncodeData(b, bfByteSize, FALSE, TRUE);
	delete b;
	return ret;
}
