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

#include <gdiplus.h>
#include "GdiPlusInstance.h"

using namespace Gdiplus;

class GdiPlusPicDrawer
{
public:
	GdiPlusPicDrawer();
	~GdiPlusPicDrawer();

	BOOL Create(INT cx, INT cy);

	BOOL LoadFile(LPCTSTR imgFile);
	BOOL LoadResourceStr(LPCTSTR resName, LPCTSTR resType, HMODULE hInst = NULL);
	BOOL LoadResourceID(UINT resID, LPCTSTR resType, HMODULE hInst = NULL);

	BOOL SaveFile(LPCTSTR imgFile);

	enum DrawMode
	{
		DM_Normal,
		DM_Stretch,
		DM_Last
	};
	enum ZoomLockMode
	{
		ZLM_NoLock,
		ZLM_FullImage,
		ZLM_FillArea,
		ZLM_Last
	};

	struct DrawParams
	{
		DrawParams()
			:drawMode(DM_Normal),
			zoomLockMode(ZLM_NoLock),
			positionX(50),positionY(50),
			reqZoom(1.0),maxZoom(0.0),minZoom(0.0),
			bTransparent(FALSE)
		{}
		DrawMode drawMode;
		ZoomLockMode zoomLockMode;
		INT positionX;
		INT positionY;
		DOUBLE reqZoom; // < 1.0 means that the original image will shrink
		DOUBLE maxZoom;
		DOUBLE minZoom;
		BOOL bTransparent;
	};
	inline DrawParams& GetDrawParams()					{return m_drawParams;}

	void SetBkColor(COLORREF rgb, BYTE opacity = 255);
	CSize GetPictureSize();

	void Destroy();

	BOOL Draw(HDC hdc, CRect& trgRC, CRect* pSrcRC = NULL);
	BOOL Draw(Graphics& g, Rect& trgRC, Rect* pSrcRC = NULL);

	BOOL HasImage()						{return m_pImage != NULL;}
	DOUBLE GetZoomForLockMode(Rect& trgRC, ZoomLockMode zlm);

	Graphics* GetInternalGraphics();
	Bitmap* GetInternalBitmap()				{return m_pImage;}

	static INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
private:
	BOOL CreateImageFromHGlobal(HGLOBAL hBuffer);
	GdiPlusInstance inst;
	Bitmap* m_pImage;
	Graphics* m_pGraphics;
	SolidBrush m_bkBrush;
	DrawParams m_drawParams;
	HGLOBAL m_hBuffer;
};




