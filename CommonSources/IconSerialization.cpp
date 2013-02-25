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
