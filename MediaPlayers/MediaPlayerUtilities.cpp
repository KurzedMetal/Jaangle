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
#include "MediaPlayerUtilities.h"
#include "TextFileDocument/textfile.h"
#include "cStringUtils.h"


#define M3U_HEADER		_T("#EXTM3U")
#define M3U_EXTINF		_T("#EXTINF")


INT MediaPlayerUtilities::InsertM3U(MediaPlayer& player, LPCTSTR filename, INT AddToPosition/* = 0*/)
{
	TRACEBENCH(_T("MediaPlayerUtilities::InsertM3U"), 1);
	ASSERT(filename != NULL && AddToPosition >= -1);
	std::basic_string<TCHAR> line;
	CTextFileRead f(filename);
	if (!f.IsOpen())
		return -1;
	
	//=== Get relative m3u Path
	TCHAR m3uPath[MAX_PATH];
	m3uPath[0] = 0;
	LPCTSTR pos = _tcsrchr(filename, '\\');
	if (pos != NULL)
	{
		_tcsncpy(m3uPath, filename, pos - filename + 1);
		m3uPath[pos - filename + 1] = 0;
	}
	//=== Get relative m3u Drive / Net Share
	TCHAR m3uDrive[MAX_PATH];
	m3uDrive[0] = 0;
	if (filename[1] == ':')
	{
		m3uDrive[0] = filename[0];
		m3uDrive[1] = filename[1];
		m3uDrive[2] = 0;
	}
	else if (filename[1] == '\\')
	{
		LPCTSTR firstSlash = _tcsrchr(&filename[2], '\\');
		if (firstSlash != NULL)
		{
			_tcsncpy(m3uDrive, filename, firstSlash - filename);
			m3uDrive[firstSlash - filename] = 0;
		}
	}

	UINT trackLimit = 0;
	TCHAR bf[MAX_PATH];
	INT TracksAdded = 0;
	std::basic_string<TCHAR> artist;
	std::basic_string<TCHAR> title;

	MediaPlayListItem mpli;
	while (f.ReadLine(line))
	{
		if (line[0] == '#')
		{
			TCHAR* ln = (TCHAR*)line.c_str();
			TCHAR* tok = _tcstok(ln, _T(":"));
			if ((tok != NULL) && (_tcscmp(tok, M3U_EXTINF) == 0))
			{
				//Duration
				tok = _tcstok(NULL, _T(","));
				if (tok == NULL)
					continue;
				mpli.length = _ttoi(tok);
				//Artist
				tok = _tcstok(NULL, _T("-"));
				if (tok == NULL)
					continue;
				_tcsncpy(bf, _tcsrtrim(_tcsltrim(tok)), MAX_PATH);
				bf[MAX_PATH - 1] = 0;
				artist = bf;
				//Track
				tok = _tcstok(NULL, _T("\0"));
				if (tok == NULL)
					continue;
				_tcsncpy(bf, _tcsrtrim(_tcsltrim(tok)), MAX_PATH);
				bf[MAX_PATH - 1] = 0;
				title = bf;
			}
		}
		else if (line[0] != 0)
		{
			if (line.size() > 1)
			{
				//=== Check if we have an absolute path
				if (line[1] == ':' || line[1] == '\\')
				{
					//=== Absolute path. No need to fix it
				}
				else if (line[0] == '\\')
				{
					//=== Needs the drive/net share to fix it
					line = m3uDrive + line;
				}
				else
				{
					//=== Needs the relative path
					line = m3uPath + line;
				}
				TCHAR longPath[MAX_PATH];//There is a problem with Unicode path names when are stored in the ANSI m3u
				if (GetLongPathName(line.c_str(), longPath, MAX_PATH) != 0)
					mpli.url = longPath;
				else
					mpli.url = line.c_str();
				mpli.artist = artist.c_str();
				mpli.title = title.c_str();
				player.InsertMedia(mpli, AddToPosition);
				artist.clear();
				title.clear();
				mpli.length = 0;
				if (AddToPosition == -1)
					AddToPosition = 0;
				AddToPosition++;
				TracksAdded++;
			}
		}
	}
	return TracksAdded;

}
BOOL MediaPlayerUtilities::SaveM3U(MediaPlayer& player, LPCTSTR filename)
{
	TRACEST(_T("MediaPlayerUtilities::SaveM3U"));
	ASSERT(filename != NULL);
	INT count = player.GetPlayListCount();
	if (count > 0)
	{
		CFile f;
		if (f.Open(filename, CFile::modeWrite | CFile::modeCreate))
		{
			CHAR lineBf[2000];
			TCHAR lineBfU[2000];
			sprintf(lineBf, "%S\n", M3U_HEADER);
			f.Write(lineBf, strlen(lineBf));
			for (int i = 0; i < count ; i++)
			{
				MediaPlayListItem mpli;
				if (player.GetPlayListItem(mpli, i))
				{
					if (PathFileExists(mpli.url))
					{
						_sntprintf(lineBfU, 2000, 
							_T("%s:%d,%s - %s\n%s\n"),
							M3U_EXTINF,
							mpli.length, 
							mpli.artist != NULL ? mpli.artist : _T(""), 
							mpli.title != NULL ? mpli.title : _T(""),
							mpli.url);
						UINT len = WideCharToMultiByte(CP_ACP, 0, lineBfU, -1, lineBf, 2000, 0,0);
						f.Write(lineBf, strlen(lineBf));
					}

				}
			}
			f.Close();
		}
	}
	return TRUE;
}
