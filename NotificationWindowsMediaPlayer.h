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
#ifndef _NotificationWindowsMediaPlayer_h_
#define _NotificationWindowsMediaPlayer_h_

#include "WindowsMediaPlayer.h"
#include "ObserverPat/BaseSubject.h"

class NotificationWindowsMediaPlayer: public WindowsMediaPlayer, public BaseSubject
{
public:
	NotificationWindowsMediaPlayer();
	virtual ~NotificationWindowsMediaPlayer()			{}


	virtual DOUBLE GetCurrentMediaLength();
	virtual void GetCurrentMediaInfo(TCHAR* artist = NULL, int artistBufferLen = 0, 
		TCHAR* track = NULL, int trackBufferLen = 0);
	virtual void GetCurrentMediaPath(TCHAR* path, int pathBufferLen);

	virtual UINT GetMediaCount();
	virtual INT GetCurrentMediaIndex();



protected:
	//Events
	virtual void PlayStateChangeMediaplayer(long NewState);
	virtual void CurrentItemChangeMediaplayer(LPDISPATCH pdispMedia);
	virtual void CurrentPlaylistChangeMediaplayer(long change);
	//virtual void OpenStateChangeMediaplayer(long change);
private:

	void ReadCurMediaInfo();

	enum MAXFIELDLEN
	{
		cMaxFiledLen = 70
	};
	TCHAR m_CurPath[MAX_PATH];
	TCHAR m_CurTitle[cMaxFiledLen];
	TCHAR m_CurArtist[cMaxFiledLen];
	BOOL m_bCurItemInfoInvalidated;
	DOUBLE m_CulLen;
	UINT m_MediaCount;
	INT m_CurIndex;

};

#endif
