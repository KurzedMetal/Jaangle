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
#include "ResourceBasedLanguage.h"


#pragma warning(push)
#pragma warning(disable: 4200)

struct StringResource
{
	WORD nLength;
#ifdef _UNICODE
	WCHAR pString[];
#else
	CHAR pString[];
#endif
};
#pragma warning(pop)	// C4200


LPCTSTR ResourceBasedLanguage::GetString(UINT ResourceNumber)
{
	std::map<int, std::tstring>::const_iterator it = m_mapStrings.find(ResourceNumber);
	if (it != m_mapStrings.end())
		return (*it).second.c_str();

	HRSRC hResource = ::FindResource(m_hInstance, MAKEINTRESOURCE( ((ResourceNumber>>4)+1) ), RT_STRING );
	if( hResource == NULL )
		return( NULL );
	//CString tst;
	//tst.LoadString(ResourceNumber);
	HGLOBAL hGlobal = ::LoadResource(m_hInstance, hResource);
	if( hGlobal == NULL )
		return( NULL );
	const StringResource* pImage = (const StringResource*)::LockResource( hGlobal );

	ULONG nResourceSize = ::SizeofResource(m_hInstance, hResource);
	const StringResource* pImageEnd = (const StringResource*)(LPBYTE( pImage )+nResourceSize);
	UINT iIndex = ResourceNumber & 0x000f;

	while( (iIndex > 0) && (pImage < pImageEnd) )
	{
		pImage = (const StringResource*)(LPBYTE( pImage )+(sizeof( StringResource )+(pImage->nLength*sizeof( TCHAR ))));
		iIndex--;
	}
	if( pImage >= pImageEnd || pImage->nLength == 0)
		return( NULL );
	m_mapStrings[ResourceNumber].insert(0, pImage->pString, pImage->nLength);
	return m_mapStrings[ResourceNumber].c_str();
}
