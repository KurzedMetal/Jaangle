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
#ifndef _BitmapSaver_h_
#define _BitmapSaver_h_

class BitmapSaver
{
public:
	BitmapSaver():m_format(F_Jpg),m_quality(70){}
	~BitmapSaver(){}

	enum SaveFormat
	{
		F_First,
		F_Bmp,
		F_Jpg,
		F_Gif,
		F_Tiff,
		F_Png,
		F_Last
	};
	void SetFormat(SaveFormat f);
	void SetQuality(INT quality);

	BOOL SaveHBITMAP(LPCTSTR fName, HBITMAP hBitmap);
	BOOL SaveClipboard(LPCTSTR fName);
	BOOL IsPictureInClipboard();

	BOOL SaveScreenshot(LPCTSTR savePath, HWND hwndTarget, HWND hwndHide = NULL, BOOL bClient = FALSE);

private:
	BOOL SaveFile(LPCTSTR fileName, HBITMAP hBitmap, SaveFormat format, INT quality);
	PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBitmap);
	int GetEncoderClsidByMIMETYPE(const WCHAR* mimeType, CLSID& clsid);
	int GetEncoderCLSIDByDescription(const WCHAR* description, CLSID& clsid);

	SaveFormat m_format;
	INT m_quality;



};

#endif
