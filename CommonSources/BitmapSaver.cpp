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
#include "BitmapSaver.h"


void BitmapSaver::SetFormat(SaveFormat f)
{
	ASSERT(f > F_First && f < F_Last);
	m_format = f;
}
void BitmapSaver::SetQuality(INT quality)
{
	ASSERT(quality > 0 && quality <= 100);
	m_quality = quality;
}

BOOL BitmapSaver::SaveHBITMAP(LPCTSTR fName, HBITMAP hBitmap)
{
	return SaveFile(fName, hBitmap, m_format, m_quality);
}
BOOL BitmapSaver::SaveClipboard(LPCTSTR fName)
{
	BOOL bRet = FALSE;
	if (OpenClipboard(AfxGetMainWnd()->m_hWnd))
	{
		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
		if (hBitmap != 0)
			bRet = SaveHBITMAP(fName, hBitmap);
		CloseClipboard();
	}
	return bRet;

}

BOOL BitmapSaver::IsPictureInClipboard()
{
	BOOL bRet = FALSE;
	if (OpenClipboard(NULL))
	{
		//Get the clipboard data
		bRet = (HBITMAP)GetClipboardData(CF_BITMAP) > 0;
		CloseClipboard();
	}
	return bRet;

}

BOOL BitmapSaver::SaveFile(LPCTSTR fileName, HBITMAP hBitmap, SaveFormat format, INT quality)
{
	PBITMAPINFO pbi = CreateBitmapInfoStruct(hBitmap);
	PBITMAPINFOHEADER pbih;			//bitmap info-header 
	pbih = (PBITMAPINFOHEADER) pbi;
	LPBYTE lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
	if (!lpBits) 
		return FALSE;
	// Retrieve the color table (RGBQUAD array) and the bits 
	// (array of palette indices) from the DIB. 
	if (!GetDIBits(::GetDC(0), hBitmap, 0, (WORD) pbih->biHeight, lpBits, pbi, 
		DIB_RGB_COLORS)) 
		return FALSE;
	BITMAPFILEHEADER hdr;       // bitmap file-header 
	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
	// Compute the size of the entire file. 
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + 
		pbih->biClrUsed * sizeof(RGBQUAD) + 
		pbih->biSizeImage); 
	hdr.bfReserved1 = 0; 
	hdr.bfReserved2 = 0; 
	// Compute the offset to the array of color indices. 
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
		pbih->biSize + 
		pbih->biClrUsed * sizeof (RGBQUAD); 

	HGLOBAL hgl = GlobalAlloc(GMEM_MOVEABLE, 
		sizeof(BITMAPFILEHEADER) + // Copy the BITMAPFILEHEADER into the .BMP file. 
		sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD) +// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
		pbih->biSizeImage);// Copy the array of color indices into the .BMP file.
	if (hgl == NULL)
		return FALSE;
	void* gMem = GlobalLock(hgl);
	//Copy the BITMAPFILEHEADER into the Global Memory
	BYTE* curPos = (BYTE*)gMem;
	UINT curWriteLen = sizeof(BITMAPFILEHEADER);
	memcpy(curPos, (LPVOID) &hdr, curWriteLen);
	//Copy the BITMAPINFOHEADER and RGBQUAD array into the Global Memory
	curPos += curWriteLen;
	curWriteLen = sizeof(BITMAPINFOHEADER) + pbih->biClrUsed * sizeof (RGBQUAD);
	memcpy(curPos, (LPVOID) pbih, curWriteLen);
	// Copy the array of color indices into the .BMP file. 
	curPos += curWriteLen;
	curWriteLen = pbih->biSizeImage;
	memcpy(curPos, (LPVOID) lpBits, curWriteLen);

	IStream* pInIStream = NULL;
	if(CreateStreamOnHGlobal(hgl, TRUE, (LPSTREAM*)&pInIStream) == S_OK)
	{
		//Here we load the bmp Image from the strem (memory)
		Gdiplus::Image image(pInIStream);
		//pInIStream->Release();
		//Now We Create the Output stream
		IStream* pOutIStream	= NULL;
		if(CreateStreamOnHGlobal(NULL, TRUE, (LPSTREAM*)&pOutIStream) == S_OK)
		{
			// Get encoder class id for jpg compression
			// for other compressions use 
			//     image/bmp 
			//     image/jpeg 
			//     image/gif 
			//     image/tiff 
			//     image/png 

			CLSID clsid;
			INT ret = -1;
			switch (format)
			{
			case F_Bmp:
				ret = GetEncoderCLSIDByDescription(L"BMP", clsid);
				break;
			case F_Gif:
				ret = GetEncoderCLSIDByDescription(L"GIF", clsid);
				break;
			case F_Tiff:
				ret = GetEncoderCLSIDByDescription(L"TIFF", clsid);
				break;
			case F_Png:
				ret = GetEncoderCLSIDByDescription(L"PNG", clsid);
				break;
			case F_Jpg:
			default:
				ret = GetEncoderCLSIDByDescription(L"JPEG", clsid);
				break;
			}
			if (ret == -1)
			{
				pOutIStream->Release();
				if (pInIStream != NULL)
					pInIStream->Release();//This should free the memory
				else
				{
					GlobalUnlock(gMem);
					GlobalFree(gMem);
				}
				return FALSE;
			}

			// Setup encoder parameters
			Gdiplus::EncoderParameters encoderParameters;
			encoderParameters.Count = 1;
			encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
			encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			encoderParameters.Parameter[0].NumberOfValues = 1;
			// setup compression level
			ULONG quality = 70;
			encoderParameters.Parameter[0].Value = &quality;

			//  Save the image to the stream
			Gdiplus::Status SaveStatus = image.Save(pOutIStream, &clsid, &encoderParameters);
			if(SaveStatus == Gdiplus::Ok)
			{
				//image.Save(pOutIStream, &pngClsid, &encoderParameters);
				ULARGE_INTEGER ulnSize;
				LARGE_INTEGER lnOffset;
				lnOffset.QuadPart = 0;
				int ret = pOutIStream->Seek(lnOffset, STREAM_SEEK_END, &ulnSize);
				// now move the pointer to the beginning of the file
				if (ret == S_OK)
					ret = pOutIStream->Seek(lnOffset, STREAM_SEEK_SET, NULL);
				// here you can do what ever you want
				/* 
				1. You can use global memory
				HGLOBAL hg;
				if(GetHGlobalFromStream(pIStream, &hg) = S_OK)
				... use hg for something

				2. Copy it into some other buffer
				char *pBuff = new char[ulnSize.QuadPart];

				// Read the stream directly into the buffer
				ULONG ulBytesRead;
				if(pIStream->Read(pBuff, ulnSize.QuadPart, &ulBytesRead) != S_OK)
				{
				pIStream->Release();
				return;
				}
				*/
				if (ret == S_OK)
				{
					CFile fFile;
					if(fFile.Open(fileName, CFile::modeCreate | CFile::modeWrite))
					{
						char *pBuff = new char[int(ulnSize.QuadPart)];
						// Read the stream directly into the buffer
						ULONG ulBytesRead;
						if(pOutIStream->Read(pBuff, ULONG(ulnSize.QuadPart), &ulBytesRead) == S_OK)
						{
							fFile.Write(pBuff, ulBytesRead);
							fFile.Close();
						}
						else
						{
							pOutIStream->Release();
						}

						delete pBuff;
					}
					else 
						TRACE(_T("@0CPicturePage::SaveJpg. Failed to save data to the disk!"));

				}
			}
			else
			{
				TRACE(_T("@0CPicturePage::SaveJpg. Failed to save to stream!"));
			}
			pOutIStream->Release();


			// get the size of the stream

		}
		else
			TRACE(_T("@0CPicturePage::SaveJpg. Failed to create stream on global memory!"));
		//pInIStream->Release();
	}
	else
	{
		TRACE(_T("@0CPicturePage::SaveJpg. Failed to create stream on global memory!"));
	}
	if (pInIStream != NULL)
		pInIStream->Release();//This should free the memory
	else
	{
		GlobalUnlock(gMem);
		GlobalFree(gMem);
	}

	return TRUE;
}

PBITMAPINFO BitmapSaver::CreateBitmapInfoStruct(HBITMAP handle)
{ 
	BITMAP bmp; 
	PBITMAPINFO pbmi; 
	WORD    cClrBits; 

	// Retrieve the bitmap color format, width, and height. 
	if (!GetObject(handle, sizeof(BITMAP), (LPSTR)&bmp)) 
		return NULL;//errhandler("GetObject", hwnd); 

	// Convert the color format to a count of bits. 
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
	if (cClrBits == 1) 
		cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 

	// Allocate memory for the BITMAPINFO structure. (This structure 
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
	// data structures.) 

	if (cClrBits != 24) 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER) + 
		sizeof(RGBQUAD) * (1<< cClrBits)); 

	// There is no RGBQUAD array for the 24-bit-per-pixel format. 

	else 
		pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
		sizeof(BITMAPINFOHEADER)); 

	// Initialize the fields in the BITMAPINFO structure. 

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = bmp.bmWidth; 
	pbmi->bmiHeader.biHeight = bmp.bmHeight; 
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
	if (cClrBits < 24) 
		pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB; 

	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	// For Windows NT, the width must be DWORD aligned unless 
	// the bitmap is RLE compressed. This example shows this. 
	// For Windows 95/98/Me, the width must be WORD aligned unless the 
	// bitmap is RLE compressed.
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbmi->bmiHeader.biHeight; 
	// Set biClrImportant to 0, indicating that all of the 
	// device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 
	return pbmi; 
} 

int BitmapSaver::GetEncoderCLSIDByDescription(const WCHAR* description, CLSID& clsid)
{
	using namespace Gdiplus;
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
	
	INT ret = -1;
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].FormatDescription, description) == 0 )
		{
			clsid = pImageCodecInfo[j].Clsid;
			ret = j;
			break;
		}    
	}
	free(pImageCodecInfo);
	return ret;
}

int BitmapSaver::GetEncoderClsidByMIMETYPE(const WCHAR* mimeType, CLSID& clsid)
{
	using namespace Gdiplus;
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

	INT ret = -1;
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, mimeType) == 0 )
		{
			clsid = pImageCodecInfo[j].Clsid;
			ret = j;
			break;
		}    
	}
	free(pImageCodecInfo);
	return ret;
}

BOOL BitmapSaver::SaveScreenshot(LPCTSTR savePath, HWND hwndTarget, HWND hwndHide/* = NULL*/, BOOL bClient/* = FALSE*/)
{
	ASSERT(hwndTarget != NULL);
	if (hwndTarget == NULL)
		return FALSE;
	if (hwndHide != NULL)
	{
		SetWindowPos(hwndHide, 0,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);
		RedrawWindow(hwndTarget, 0, 0, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
	}

	RECT rcTarget;
	HDC hTargetDC = NULL;
	if (bClient)
	{
		GetClientRect(hwndTarget, &rcTarget);
		POINT p;
		p.x = rcTarget.left;
		p.y = rcTarget.top;
		ClientToScreen(hwndTarget, &p);
		rcTarget.left = p.x;
		rcTarget.top = p.y;
		p.x = rcTarget.right;
		p.y = rcTarget.bottom;
		ClientToScreen(hwndTarget, &p);
		rcTarget.right = p.x;
		rcTarget.bottom = p.y;
		hTargetDC = ::GetDC(hwndTarget);
	}
	else
	{
		GetWindowRect(hwndTarget, &rcTarget);
		hTargetDC = ::GetWindowDC(hwndTarget);
	}


	HDC	hBmpFileDC = CreateCompatibleDC(hTargetDC);
	HBITMAP	hBmpFileBitmap = CreateCompatibleBitmap(hTargetDC, rcTarget.right - rcTarget.left, rcTarget.bottom - rcTarget.top);
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(hBmpFileDC, hBmpFileBitmap);
	BitBlt(hBmpFileDC, 0, 0, rcTarget.right - rcTarget.left, rcTarget.bottom - rcTarget.top, hTargetDC, 0, 0, SRCCOPY|CAPTUREBLT);
	SelectObject(hBmpFileDC, hOldBitmap);

	SaveHBITMAP(savePath, hBmpFileBitmap);

	DeleteDC(hBmpFileDC);
	DeleteObject(hBmpFileBitmap);
	::ReleaseDC(hwndTarget, hTargetDC);

	if (hwndHide != NULL)
		SetWindowPos(hwndHide, 0,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

		//SetWindowPos(hwndHide, 0, rcHide.left, rcHide.top, rcHide.right - rcHide.left, rcHide.bottom - rcHide.top, SWP_NOZORDER|SWP_SHOWWINDOW);
	return TRUE;
}







