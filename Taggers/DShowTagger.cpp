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
#include "DShowTagger.h"
#include <DShow.h>

DShowTagger::DShowTagger():
m_readDepth(50)
{

}
DShowTagger::~DShowTagger()
{
}


UINT DShowTagger::Read(LPCTSTR fileName, TagInfo& tagInfo)
{
	TRACEST(_T("DShowTagger::Read"));
	ASSERT(fileName != NULL);
	
	//This is a "safer" approach. DSHOW will only try to handle files that are defined here
	//There seems to be a problem with m4a files
	if (m_readDepth <= 50)//Try only these types. Otherwise try every type...
	{
		TCHAR ext[5];
		_tcsncpy(ext, &fileName[_tcslen(fileName) - 4], 4);
		ext[4] = 0;
		_tcslwr(ext);
		LPCTSTR f = _tcsstr(_T(".mpgmpeg.avi.wmv.ogm.mp3.mp2.wma.cda.ogg.ac3.ape"), ext);
		if (f== NULL)	
			return NULL;
	}

	CComPtr<IGraphBuilder> pGraphBuilder = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder,(void**)&pGraphBuilder);
	if (FAILED(hr))		return TagInfo_None;
	try
	{
		TRACEST(_T("DShowTagger::Read RenderFile"));
		hr = pGraphBuilder->RenderFile((LPCWSTR)CT2CW(fileName), NULL);
	}
	catch (...)
	{
		TRACE(_T("@0 DShowTagger::Read Error catched!!!\r\n"));
		return TagInfo_None;
	}
	if (FAILED(hr))		return TagInfo_None;

	CComPtr<IMediaPosition> pMediaPosition = NULL;
	if (SUCCEEDED(pGraphBuilder->QueryInterface(IID_IMediaPosition, (void**)&pMediaPosition)))
	{
		REFTIME length;
		if (SUCCEEDED(pMediaPosition->get_Duration(&length)))
		{
			tagInfo.Length = (UINT)length;
			return TagInfo_Length;
		}
	}
	return 0;
}

void DShowTagger::SetConfig(Config conf, INT val)
{
	if (conf == CONF_ReadDepth)
		m_readDepth = val;
}
INT DShowTagger::GetConfig(Config conf)
{
	if (conf = CONF_ReadDepth)
		return m_readDepth;
	return -1;
}
