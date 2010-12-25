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
#include "NotificationWindowsMediaPlayer.h"
#include "ObserverPat/Notification.h"
#include "SubjectMessages.h"

NotificationWindowsMediaPlayer::NotificationWindowsMediaPlayer()
{
	m_bCurItemInfoInvalidated = TRUE;
	m_CurPath[0] = 0;
	m_CurTitle[0] = 0;
	m_CurArtist[0] = 0;
	m_CulLen = 0.0;
	m_MediaCount = -1;
	m_CurIndex = -1;
}


DOUBLE NotificationWindowsMediaPlayer::GetCurrentMediaLength()
{
	if (m_bCurItemInfoInvalidated == TRUE)
		ReadCurMediaInfo();
	//else
	//	TRACE(_T("\tGetCurrentMediaLength [Cache Hit]\n"));
	//TRACE(_T("\tGetCurrentMediaLength: Returned %3.2f\n"), m_CulLen);
	return m_CulLen;
}

void NotificationWindowsMediaPlayer::GetCurrentMediaInfo(TCHAR* artist, int artistBufferLen, 
						 TCHAR* track, int trackBufferLen)
{
	if (m_bCurItemInfoInvalidated == TRUE)
		ReadCurMediaInfo();
	else
		TRACE(_T("\tGetCurrentMediaInfo [Cache Hit]\n"));

	if (artist != NULL)
		_tcsncpy(artist, m_CurArtist, artistBufferLen);
	if (track != NULL)
		_tcsncpy(track, m_CurTitle, trackBufferLen);

}
void NotificationWindowsMediaPlayer::GetCurrentMediaPath(TCHAR* path, int pathBufferLen)
{
	ASSERT(path != NULL);
	ASSERT(pathBufferLen >= 0);
	if (m_bCurItemInfoInvalidated == TRUE)
		ReadCurMediaInfo();
	else
		TRACE(_T("\tGetCurrentMediaPath [Cache Hit]\n"));
	_tcsncpy(path, m_CurPath, pathBufferLen);
}

void NotificationWindowsMediaPlayer::ReadCurMediaInfo()
{
	TRACEST(_T("@5NotificationWindowsMediaPlayer::ReadCurMediaInfo\n"));
	CComPtr<IWMPMedia> media;
	m_spWMPPlayer->get_currentMedia(&media);
	if (media != NULL)
	{
		CComBSTR name;
		HRESULT hr = media->get_sourceURL(&name);
		if (SUCCEEDED(hr))
			_tcsncpy(m_CurPath, name.m_str, MAX_PATH);
		m_CurPath[MAX_PATH - 1] = 0;
		WindowsMediaPlayer::GetMediaInfoPriv(media, m_CurArtist, cMaxFiledLen, m_CurTitle, cMaxFiledLen);

		//hr = media->getItemInfo(CComBSTR(_T("Author")), &name);
		//if (SUCCEEDED(hr))
		//	_tcsncpy(m_CurArtist, name.m_str, cMaxFiledLen);
		//m_CurArtist[cMaxFiledLen - 1] = 0;
		//hr = media->getItemInfo(CComBSTR(_T("Title")), &name);
		//if (SUCCEEDED(hr))
		//	_tcsncpy(m_CurTitle, name.m_str, cMaxFiledLen);
		//m_CurTitle[cMaxFiledLen - 1] = 0;
		hr = media->get_duration(&m_CulLen);
		if (FAILED(hr) || m_CulLen == 0.0)
		{
			m_CulLen = 0.0;
		}
		else
			m_bCurItemInfoInvalidated = FALSE;

	}
	else
		m_bCurItemInfoInvalidated = FALSE;

}
UINT NotificationWindowsMediaPlayer::GetMediaCount()
{
	TRACEST(_T("@5NotificationWindowsMediaPlayer::GetMediaCount\n"));
	if (m_MediaCount == -1)
		m_MediaCount = WindowsMediaPlayer::GetMediaCount();
	else
		TRACE(_T("@4NotificationWindowsMediaPlayer::GetMediaCount[Cache Hit]\n"));
	return m_MediaCount;
}

INT NotificationWindowsMediaPlayer::GetCurrentMediaIndex()
{
	TRACEST(_T("NotificationWindowsMediaPlayer::GetCurrentMediaIndex\n"));
	if (m_CurIndex == -1)
		m_CurIndex = WindowsMediaPlayer::GetCurrentMediaIndex();
	else
		TRACE(_T("NotificationWindowsMediaPlayer::GetCurrentMediaIndex [Cache Hit]\n"));
	return m_CurIndex;
}

void NotificationWindowsMediaPlayer::PlayStateChangeMediaplayer(long NewState)
{
	TRACEST(_T("@5EVENT: PlayStateChange (NewState: %d)\n"), NewState);
	WindowsMediaPlayer::PlayStateChangeMediaplayer(NewState);
	UINT mc = WindowsMediaPlayer::GetMediaCount();
	if (m_MediaCount != mc)
	{
		CurrentPlaylistChangeMediaplayer(0);
	}

	m_MediaCount = -1;
	switch (NewState)
	{
	case 3:
		NewState = MediaPlayer::PS_Playing;
		break;
	case 1:
		NewState = MediaPlayer::PS_Stopped;
		break;
	case 2:
		NewState = MediaPlayer::PS_Paused;
		break;
	default:
		NewState = MediaPlayer::PS_Inactive;
	}
	NotifyObservers(BaseNotification(SM_PlayStateChanged, NewState));

}

void NotificationWindowsMediaPlayer::CurrentItemChangeMediaplayer(LPDISPATCH pdispMedia)
{
	TRACE(_T("@5EVENT: CurrentItemChange (Media: %d)\n"), pdispMedia);
	//Invalidate values
	m_bCurItemInfoInvalidated = TRUE; 
	m_CurIndex = -1;

	WindowsMediaPlayer::CurrentItemChangeMediaplayer(pdispMedia);
	NotifyObservers(BaseNotification(SM_MediaChanged));
}

void NotificationWindowsMediaPlayer::CurrentPlaylistChangeMediaplayer(long change)
{
	TRACE(_T("@5--------EVENT: CurrentPlaylistChange (Change:%d)\n"), change);
	//Invalidate values
	m_bCurItemInfoInvalidated = TRUE; 
	m_MediaCount = -1;
	m_CurIndex = -1;

	WindowsMediaPlayer::CurrentPlaylistChangeMediaplayer(change);
	NotifyObservers(BaseNotification(SM_PlayListChanged));
}
