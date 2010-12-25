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
#include "FileNameTagger.h"
#include "FileNameParser.h"
#include <shlwapi.h>

UINT FileNameTagger::Read(LPCTSTR fileName, TagInfo& tagInfo)
{
	TRACEST(_T("FileNameTagger::Read"));
	return FileNameTagger::CustomRead(fileName, tagInfo, NULL);
}

UINT FileNameTagger::CustomRead(LPCTSTR fileName, TagInfo& tagInfo, LPCTSTR format)
{
	ASSERT(fileName != NULL);
	if (fileName == NULL)
		return TagInfo_None;
	//It can only read these flags
	if (PathIsDirectory(fileName))	
		return TagInfo_None;
	TRACE(_T("@2FileNameTagger::Read \r\n"));
	FileNameParser fnParser;
	UINT aquiredTiFlags = TagInfo_None;
	if (fnParser.ParseFileName(fileName, format))
	{
		tagInfo.Artist = fnParser.GetArtist();
		if (!tagInfo.Artist.empty())			
			aquiredTiFlags |= TagInfo_Artist;
		tagInfo.Album = fnParser.GetAlbum();
		if (!tagInfo.Album.empty())				
			aquiredTiFlags |= TagInfo_Album;
		tagInfo.Title = fnParser.GetTitle();
		if (!tagInfo.Title.empty())				
			aquiredTiFlags |= TagInfo_Title;
		tagInfo.TrackNo = fnParser.GetTrackNo();
		if (tagInfo.TrackNo > 0)				
			aquiredTiFlags |= TagInfo_TrackNo;
		tagInfo.Year = fnParser.GetYear();
		if (tagInfo.Year > 0)				
			aquiredTiFlags |= TagInfo_Year;
	}
	return aquiredTiFlags;
}
UINT FileNameTagger::CustomWrite(LPTSTR newPathName, LPCTSTR fileName, LPCTSTR format,
				 LPCTSTR artist, LPCTSTR album, INT trackNo, LPCTSTR title, INT year)
{
	//--- Renamer
	ASSERT(format != NULL);
	FileNameParser fnParser;
	TCHAR bf[MAX_PATH];
	if (!TranslatePattern(bf, MAX_PATH, format, 
		artist,
		album,
		trackNo,
		title,
		year,
		NULL
		))
		return 0;
	if (!MakeMewPath(newPathName, bf, fileName))
		return 0;
	
	if (!::MoveFile(fileName, newPathName))
		return 0;

	return TagInfo_Artist | TagInfo_Album | TagInfo_Title | TagInfo_TrackNo | TagInfo_Year;
}

BOOL FileNameTagger::TranslatePattern(LPTSTR newName, UINT bfLen,
									  LPCTSTR pattern, 
									  LPCTSTR artist, LPCTSTR album, LPCTSTR trackNo, LPCTSTR title, LPCTSTR year, LPCTSTR discard) 
{
	return FileNameParser::TranslatePattern(newName, bfLen, pattern, artist, album, trackNo, title, year, discard);
}
BOOL FileNameTagger::TranslatePattern(LPTSTR newName, UINT bfLen,
									  LPCTSTR pattern, 
									  LPCTSTR artist, LPCTSTR album, INT trackNo, LPCTSTR title, INT year, LPCTSTR discard)
{
	TCHAR sTrackNo[20];
	_sntprintf(sTrackNo, 20, _T("%02d"), trackNo);
	TCHAR sYear[20];
	_itot(year, sYear, 10);
	return FileNameParser::TranslatePattern(newName, bfLen, pattern, artist, album, title == NULL ? NULL : sTrackNo, title, title == NULL ? NULL : sYear, discard);

}

LPCTSTR FileNameTagger::GetReadPredifinedFormat(INT idx)
{
	return FileNameParser::GetReadPredefinedFormat(idx);
}

LPCTSTR FileNameTagger::GetWritePredifinedFormat(INT idx)
{
	return FileNameParser::GetWritePredefinedFormat(idx);
}


BOOL FileNameTagger::MakeMewPath(LPTSTR outPath, LPCTSTR newName, LPCTSTR originalPath)
{
	ASSERT(outPath != NULL);
	ASSERT(newName != NULL);
	ASSERT(originalPath != NULL);
	const TCHAR* pos = _tcsrchr(originalPath, '\\');
	if (pos != NULL)
	{
		pos++;
		_tcsncpy(outPath, originalPath, pos - originalPath);
		outPath[pos - originalPath] = 0;

		_tcsncat(outPath, newName, MAX_PATH - _tcslen(outPath));
		pos = _tcsrchr(originalPath, '.');
		if (pos != NULL)
		{
			_tcsncat(outPath, pos, MAX_PATH - _tcslen(outPath));
			return TRUE;
		}
	}
	return FALSE;
}




