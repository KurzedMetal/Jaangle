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

class FileNameParser
{
public:
	FileNameParser();
	FileNameParser(LPCTSTR fileName);

	BOOL ParseFileName(LPCTSTR fileName, LPCTSTR pattern = NULL);
	LPCTSTR GetArtist()	{return m_Artist;}
	LPCTSTR GetAlbum()	{return m_Album;}
	INT GetTrackNo()	{return m_TrackNo;}
	LPCTSTR GetTitle()	{return m_Title;}
	INT GetYear()		{return m_Year;}

	static BOOL TranslatePattern(LPTSTR newName, UINT bfLen,
		LPCTSTR pattern, 
		LPCTSTR artist, LPCTSTR album, LPCTSTR trackNo, LPCTSTR title, LPCTSTR year, LPCTSTR discard);
	static LPCTSTR GetReadPredefinedFormat(INT idx);
	static LPCTSTR GetWritePredefinedFormat(INT idx);

#ifdef _UNITTESTING
	static BOOL UnitTest();
#endif

private:
	const static int MAX_STRING = 100;
	TCHAR m_Artist[MAX_STRING];
	TCHAR m_Album[MAX_STRING];
	TCHAR m_Title[MAX_STRING];
	INT m_TrackNo;
	INT m_Year;
	BOOL DecodeAs(LPCTSTR fileName, LPCTSTR pattern);
	//BOOL ExtractAttribute(TCHAR code, LPCTSTR startChar);
	INT ExtractAttribute(TCHAR code, TCHAR divider, LPCTSTR startChar);
	BOOL IsInteger(LPCTSTR str);
};
