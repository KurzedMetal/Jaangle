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
#include "GdiPlusPicDrawer.h"

GdiPlusPicDrawer::GdiPlusPicDrawer():
m_pImage(NULL),
m_bkBrush(Color(255,0,0,0)),
m_hBuffer(0),
m_pGraphics(NULL)
{
}

GdiPlusPicDrawer::~GdiPlusPicDrawer()
{
	Destroy();
}

BOOL GdiPlusPicDrawer::Draw(HDC hdc, CRect& trgRC, CRect* pSrcRC/* = NULL*/)
{
	Graphics g(hdc);
	return Draw(g, 
		Rect(trgRC.left, trgRC.top, trgRC.Width(), trgRC.Height()), 
		(pSrcRC == NULL ? NULL : &Rect(pSrcRC->left, pSrcRC->top, pSrcRC->Width(), pSrcRC->Height()))
		);
}

BOOL GdiPlusPicDrawer::Draw(Graphics& g, Rect& trgRC, Rect* pSrcRC/* = NULL*/)
{
	if (m_drawParams.bTransparent == FALSE)
		g.FillRectangle(&m_bkBrush, trgRC.X, trgRC.Y, trgRC.Width, trgRC.Height);
	if (m_pImage != NULL && m_pImage->GetWidth() && m_pImage->GetHeight())
	{
		Rect srcRC(0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
		if (pSrcRC != NULL)
			srcRC = *pSrcRC;
		Rect realSourceRC(srcRC);
		Rect realTargetRC(trgRC);

		switch (m_drawParams.drawMode)
		{
		case DM_Normal:
			{
				//===In Normal Mode you have 3 options
				//	1. Zoom Lock None. It will display the source target to
				//		the target rect with the specified zoom. Source Rect will count mainly as the starting x,y
				//	2. Fill Area. The image from the source rect will fill
				//		the specified target. If it isn't enough the zoom will increase
				//	3. Fill Image. The full image source target will be displayed.
				//		If it isn't enough the zoom will get less.

				//===Calculate the desired zoom
				DOUBLE reqZoom = m_drawParams.reqZoom;
				switch (m_drawParams.zoomLockMode)
				{
				case ZLM_FillArea:
					{
						DOUBLE magnificationW = trgRC.Width / (DOUBLE)srcRC.Width;
						DOUBLE magnificationH = trgRC.Height / (DOUBLE)srcRC.Height;
						reqZoom = max(magnificationW, magnificationH);
					}
					break;
				case ZLM_FullImage:
					{
						DOUBLE magnificationW = trgRC.Width / (DOUBLE)srcRC.Width;
						DOUBLE magnificationH = trgRC.Height / (DOUBLE)srcRC.Height;
						reqZoom = min(magnificationW, magnificationH);
					}
					break;
				}
				if (m_drawParams.maxZoom > 0.0 && reqZoom > m_drawParams.maxZoom)
					reqZoom = m_drawParams.maxZoom;	//===Decreases the magnification
				else if (m_drawParams.minZoom > 0.0 && reqZoom < m_drawParams.minZoom)
					reqZoom = m_drawParams.minZoom;	//===Increases the magnification


				//===If we expand the srcRC to the target zoom it will have...
				Rect vTrgRC(srcRC);
				vTrgRC.Width = INT(vTrgRC.Width * reqZoom);
				vTrgRC.Height = INT(vTrgRC.Height * reqZoom);
				if (vTrgRC.Width > trgRC.Width)
				{
					realSourceRC.Width = INT(trgRC.Width / reqZoom);
					INT freeW = srcRC.Width - realSourceRC.Width;
					realSourceRC.X = INT((freeW * m_drawParams.positionX) / 100.0);
				}
				else if (vTrgRC.Width < trgRC.Width)
				{
					realTargetRC.Width = vTrgRC.Width;
					INT freeW = trgRC.Width - realTargetRC.Width;
					realTargetRC.X += INT(freeW / 2);
				}
				if (vTrgRC.Height > trgRC.Height)
				{
					realSourceRC.Height = INT(trgRC.Height / reqZoom);
					INT freeH = srcRC.Height - realSourceRC.Height;
					realSourceRC.Y += INT((freeH * m_drawParams.positionY) / 100.0);
				}
				else if (vTrgRC.Height < trgRC.Height)
				{
					realTargetRC.Height = vTrgRC.Height;
					INT freeH = trgRC.Height - realTargetRC.Height;
					realTargetRC.Y += INT(freeH / 2);
				}
			}
			break;
		case DM_Stretch:
			//===It will draw the source part of the image stretched (without keeping AR)
			break;
		}
		Status st = g.DrawImage(m_pImage, realTargetRC, 
			realSourceRC.X, realSourceRC.Y, realSourceRC.Width, realSourceRC.Height, 
			UnitPixel, 0, 0, 0);
		ASSERT(st==Ok);
		return st==Ok;
	}
	return FALSE;
}

DOUBLE GdiPlusPicDrawer::GetZoomForLockMode(Rect& trgRC, ZoomLockMode zlm)
{
	if (m_pImage != NULL && m_pImage->GetWidth() && m_pImage->GetHeight())
	{
		Rect srcRC(0, 0, m_pImage->GetWidth(), m_pImage->GetHeight());
		switch (zlm)
		{
		case ZLM_FillArea:
			{
				DOUBLE magnificationW = trgRC.Width / (DOUBLE)srcRC.Width;
				DOUBLE magnificationH = trgRC.Height / (DOUBLE)srcRC.Height;
				return max(magnificationW, magnificationH);
			}
			break;
		case ZLM_FullImage:
			{
				DOUBLE magnificationW = trgRC.Width / (DOUBLE)srcRC.Width;
				DOUBLE magnificationH = trgRC.Height / (DOUBLE)srcRC.Height;
				return min(magnificationW, magnificationH);
			}
			break;
		}
	}
	return 0.0;
}

void GdiPlusPicDrawer::SetBkColor(COLORREF bkColor, BYTE opacity)
{
	m_bkBrush.SetColor(Color(opacity, GetRValue(bkColor), GetGValue(bkColor), GetBValue(bkColor)));
}

BOOL GdiPlusPicDrawer::Create(INT cx, INT cy)
{
	Destroy();
	m_pImage = new Gdiplus::Bitmap(cx, cy, PixelFormat32bppARGB);
	if (m_pImage == NULL)
		return FALSE;
	return m_pImage->GetLastStatus() == Ok;
}

BOOL GdiPlusPicDrawer::SaveFile(LPCTSTR imgFile)
{
	if (m_pImage == NULL)
		return FALSE;
	CLSID pngClsid;
	Status st = GenericError;
	if (GetEncoderClsid(L"image/png", &pngClsid))
		st = m_pImage->Save(imgFile, &pngClsid, NULL);
	return st == Ok;
}



BOOL GdiPlusPicDrawer::LoadFile(LPCTSTR imgFile)
{
	ASSERT(imgFile != NULL);
	Destroy();
	HANDLE f = CreateFile(imgFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (f == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("@1 GdiPlusPicDrawer::LoadFile. Cannot read file\r\n"));
		return FALSE;
	}
	DWORD imageSize = GetFileSize(f, 0);
	if (imageSize == INVALID_FILE_SIZE)
	{
		TRACE(_T("@1 GdiPlusPicDrawer::LoadFile. Cannot get file size\r\n"));
		return FALSE;
	}
	m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);
		if (pBuffer)
		{
			DWORD numBytesRead = 0;
			ReadFile(f, pBuffer, imageSize, &numBytesRead, NULL);
			CloseHandle(f);
			return CreateImageFromHGlobal(m_hBuffer);
		}
		TRACE(_T("@1 GdiPlusPicDrawer::LoadFile. GlobalLock failure\r\n"));
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	else
		TRACE(_T("@1 GdiPlusPicDrawer::LoadFile. GlobalAlloc failure\r\n"));
	return FALSE;
}

BOOL GdiPlusPicDrawer::CreateImageFromHGlobal(HGLOBAL hBuffer)
{
	ASSERT(hBuffer != NULL);
	IStream* pStream = NULL;
	if (::CreateStreamOnHGlobal(hBuffer, FALSE, &pStream) == S_OK)
	{
		m_pImage = Gdiplus::Bitmap::FromStream(pStream);
		Status lastStatus = m_pImage->GetLastStatus();
		pStream->Release();
		if (m_pImage != NULL && lastStatus == Ok)
			return TRUE;
		TRACE(_T("@1 GdiPlusPicDrawer::CreateImageFromHGlobal. Bitmap::FromStream failed :%d\r\n"), lastStatus);
	}
	else
		TRACE(_T("@1 GdiPlusPicDrawer::CreateImageFromHGlobal. CreateStreamOnHGlobal failed\r\n"));
	Destroy();
	return FALSE;
}

BOOL GdiPlusPicDrawer::LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst)
{
	ASSERT(resName != NULL);
	ASSERT(resType != NULL);
	Destroy();
	//m_pImage = new Image(imgFile);
	HRSRC hResource = ::FindResource(hInst, resName, resType);
	if (!hResource)			return FALSE;

	DWORD imageSize = ::SizeofResource(hInst, hResource);
	if (!imageSize)			return FALSE;

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
	if (!pResourceData)		return FALSE;

	m_hBuffer = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if (m_hBuffer)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);
		if (pBuffer)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);
			return CreateImageFromHGlobal(m_hBuffer);
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	return FALSE;
}

BOOL GdiPlusPicDrawer::LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst)
{
	return LoadResourceStr(MAKEINTRESOURCE(resID), resType, hInst);
}

void GdiPlusPicDrawer::Destroy()
{
	if (m_pImage != NULL)
	{
		delete m_pGraphics;
		m_pGraphics = NULL;
		delete m_pImage;
		m_pImage = NULL;
	}
	if (m_hBuffer)
	{
		::GlobalUnlock(m_hBuffer);
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
}

CSize GdiPlusPicDrawer::GetPictureSize()
{
	CSize ret(0,0);
	if (m_pImage != NULL)
	{
		ret.cx = m_pImage->GetWidth();
		ret.cy = m_pImage->GetHeight();
	}
	return ret;
}


Graphics* GdiPlusPicDrawer::GetInternalGraphics()
{
	if (m_pGraphics == NULL)
	{
		if (m_pImage != NULL)
			m_pGraphics = new Graphics(m_pImage);
	}
	ASSERT(m_pGraphics != NULL);//Don't ask it if PicDrawer is uninitialized
	return m_pGraphics;
}

INT GdiPlusPicDrawer::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

