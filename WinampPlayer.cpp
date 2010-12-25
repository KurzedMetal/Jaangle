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
#include "StdAfx.h"
#include "winampplayer.h"
#include "TracksViewSchema.h"
#include "ObjFactory.h"
#include "DBManager.h"

#define PLAY_COMMAND _T("Play")
#define ENQUEUE_COMMAND _T("Enqueue")

WinampPlayer::WinampPlayer(void)
{
}

WinampPlayer::~WinampPlayer(void)
{
}

BOOL WinampPlayer::Play(LPCTSTR str)
{
	ASSERT(str != NULL);
	return ShellExecute(NULL, PLAY_COMMAND, str, NULL, NULL, SW_SHOWDEFAULT) != 0;
}

BOOL WinampPlayer::Enqueue(LPCTSTR str)
{
	ASSERT(str != NULL);
	return ShellExecute(NULL, ENQUEUE_COMMAND, str, NULL, NULL, SW_SHOWDEFAULT) != 0;
}

//void WinampPlayer::AddTracks(std::vector<UINT>& ListTracks, BOOL Enqueue)
//{
//	TCHAR* command = Enqueue ? ENQUEUE_COMMAND : PLAY_COMMAND;
//	switch (ListTracks.size())
//	{
//	case 0:
//		break;
//	case 1:
//		{
//			TCHAR bf[_MAX_PATH];
//			TracksViewSchema tvs;
//			if (DBHelper::GetTracksViewByTrackID(ListTracks[0], tvs))
//			{
//				tvs.GetLocation(bf, sizeof(bf)/sizeof(TCHAR));
//				ShellExecute(NULL, command, bf, NULL, NULL, SW_SHOWDEFAULT);
//			}
//		}
//		break;
//	default:
//		{
//			TCHAR tempPath[_MAX_PATH];
//			GetTempPath(_MAX_PATH, tempPath);
//			_tcscat(tempPath, _T("~TSTemp.m3u"));
//			CFile f;
//			CFileException e;
//			if (f.Open(tempPath, CFile::modeCreate | CFile::modeWrite, &e))
//			{
//				TCHAR bf[_MAX_PATH + 2];
//				size_t NumTracks = ListTracks.size();
//				for (size_t i = 0; i < NumTracks; i++)
//				{
//					TracksViewSchema tvs;
//					if (DBHelper::GetTracksViewByTrackID(ListTracks[i], tvs))
//					{
//						tvs.GetLocation(bf, sizeof(bf)/sizeof(TCHAR));
//						_tcscat(bf, _T("\r\n"));
//						f.Write(CW2A(bf), (UINT)_tcslen(bf));
//					}
//				}
//				f.Close();
//				ShellExecute(NULL, command, tempPath, NULL, NULL, SW_SHOWDEFAULT);
//			}
//			else
//			{
//				TRACE(_T("Cannot Create File: %s\n\t Error: %d\n"), tempPath, e.m_cause);
//			}
//		}
//	}
//}

//Control Playback
void WinampPlayer::PlayPause()
{
	if (GetPlayState() == PS_Playing)
		CommandWinamp(WA_PAUSE);
	else
		CommandWinamp(WA_PLAY);
}

void WinampPlayer::Stop()
{
	CommandWinamp(WA_STOP);
}
void WinampPlayer::Next()
{
	CommandWinamp(WA_NEXT);
}
void WinampPlayer::Previous()
{
	CommandWinamp(WA_PREVIOUS);
}

//Check Status
DOUBLE WinampPlayer::GetMediaPos()
{
	return SendUserMsg(0, 105) / 1000.0;
}
void WinampPlayer::SetMediaPos(DOUBLE newPosition)
{
	SendUserMsg((WPARAM)newPosition * 1000, 106);
}

//TrackInfo
DOUBLE WinampPlayer::GetCurrentMediaLength()
{
	return (DOUBLE)SendUserMsg(1, 105);

}
void WinampPlayer::GetCurrentMediaInfo(TCHAR* artist, int artistBufferLen, TCHAR* track, int trackBufferLen)
{
	ASSERT(!(artist == NULL && track == NULL));
	ASSERT(!(artist != NULL && artistBufferLen < 1));
	ASSERT(!(track != NULL && trackBufferLen < 1));
	if (artist != NULL)
		artist[0] = 0;
	if (track != NULL)
		track[0] = 0;
	HWND wndWinamp = FindWinamp();
	if (wndWinamp != NULL)
	{
		TCHAR szTitle[120];
		GetWindowText(wndWinamp, szTitle, sizeof(szTitle));
		TCHAR* endPos = _tcsrchr(szTitle, '-');//Clean the " - Winamp" at the end of the title
		if (endPos > szTitle + 1)
		{
			endPos--;
			*endPos = 0;
		}

		size_t sLen = _tcslen(szTitle);
		if (sLen > 3)
		{
			TCHAR* ArtistPos = _tcschr(szTitle, ' ');
			if (ArtistPos != NULL)
			{
				ArtistPos += 1;
				TCHAR* TrackPos = _tcschr(ArtistPos, '-') ;
				if (TrackPos != NULL)
				{
					TrackPos[-1] = 0; 
					TrackPos += 2;
					if (artist != NULL)
						_tcsncpy(artist, ArtistPos, artistBufferLen);
					if (track != NULL)
						_tcsncpy(track, TrackPos, trackBufferLen);
				}
			}
		}
	}
}

void WinampPlayer::StartWinamp()
{
	TCHAR winampPath[_MAX_PATH];
	UINT ret = 100;
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, winampPath)))
	{
		TCHAR strCommand[2 * _MAX_PATH];
		_stprintf(strCommand, TEXT("\"%s%s\""), winampPath, TEXT("\\Winamp\\Winamp.exe"));
		ret = WinExec(CW2A(strCommand), SW_SHOWNOACTIVATE);
	}
	if (ret != 0)
		TRACE(_T("StartWinamp Failed (%d)\n"), ret);
}

HWND WinampPlayer::FindWinamp()
{
	return ::FindWindow(_T("Winamp v1.x"), NULL);
}

void WinampPlayer::CommandWinamp(WINAMP_COMMANDS iCommand)
{
	HWND wndWinamp = FindWinamp();
	if (wndWinamp != NULL)
	{
		::PostMessage(wndWinamp, WM_COMMAND, iCommand, 0);
	}
}

LRESULT	WinampPlayer::SendUserMsg(WPARAM data, LPARAM id) 
{
	HWND wndWinamp = FindWinamp();
	if(wndWinamp != NULL) 
		return ::SendMessage(wndWinamp, WM_USER, data, id); 
	else
		return -1;	
};

MediaPlayer::PlayState WinampPlayer::GetPlayState()
{
	LRESULT res = SendUserMsg(0, 104);
	switch (res)
	{
	case -1:
		return PS_Inactive;
		break;
	case 1:
		return PS_Playing;
		break;
	case 3:
		return PS_Paused;
		break;
	default:
		TRACE("WinampPlayer::GetPlayState ret: %d\n", res);
	}
	return PS_Stopped;
}

void WinampPlayer::PostPlaylist(INT iCommand)
{
	HWND wndWinamp = FindWinamp();
	if(wndWinamp != NULL) 
		::PostMessage(wndWinamp, WM_COMMAND, iCommand, 0); 
}
