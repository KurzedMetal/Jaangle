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
#pragma once

#include <string>
#include <list>
#include <Wininet.h>
#include <afxmt.h>

struct LastFMSubmitTrack
{
public:
	LastFMSubmitTrack():unixTimeStamp(0),length(0),trackNo(0)
	{
	}
	std::basic_string<TCHAR> artist;
	std::basic_string<TCHAR> album;
	std::basic_string<TCHAR> track;
	
	UINT unixTimeStamp;
	UINT length;
	UINT trackNo;
	std::basic_string<TCHAR> mbID;
	BOOL IsValid()
	{
		return !artist.empty() && !track.empty() && length >= 30;
	}
};

class LastFMServices
{
public:
	enum State
	{
		ST_First,
		ST_Error,
		ST_Idle,
		ST_UnInitialized,
		ST_Connecting,
		ST_Disconnecting,
		ST_Working,
		ST_Last
	};
	enum Error
	{
		ERR_None,
		ERR_Banned,
		ERR_BadAuth,
		ERR_BadTime,
		ERR_BadSession,
		ERR_Failed,
		ERR_General,
		ERR_CommunicationError,
		ERR_Last
	};

public:
	LastFMServices(void);
	virtual ~LastFMServices(void);

	void Init(HINTERNET hNet);
	BOOL CheckLogin();
	LPCTSTR GetUserName()		{return m_username.c_str();}
	LPCTSTR GetMD5Password()	{return m_MD5password.c_str();}
	void SetUserName(LPCTSTR username);
	void SetMD5Password(LPCTSTR MD5password);


	void Submit(LastFMSubmitTrack& track);

	State GetState()			{return m_state;}
	Error GetError()			{return m_error;}
	LPCSTR GetErrorString()		{return m_errorString.c_str();}

private:
	static UINT WorkingThreadST(LPVOID pParam);
	UINT WorkingThread();
	BOOL HandShake(std::basic_string<TCHAR>& sessionID, 
		std::basic_string<TCHAR>& nowPlayingURL, 
		std::basic_string<TCHAR>& submissionURL);
	BOOL SubmitTracks(LPCTSTR sessionID, LPCTSTR submissionURL);
	BOOL IsTrackAvailable();
	void SetFailure(Error err);
	static BOOL LeaveThread(HANDLE hThread, UINT timeOut);
	static BOOL ConvertString(LPCTSTR src, LPSTR bf, UINT bfLen);

private:
	std::basic_string<TCHAR> m_username;
	std::basic_string<TCHAR> m_MD5password;
	State m_state;
	Error m_error;
	HINTERNET m_hNet;
	CWinThread* m_pThread;
	BOOL volatile m_bExitThread;
	std::string m_errorString;
	std::list<LastFMSubmitTrack> m_jobs;
	CCriticalSection m_cs;
	UINT m_lastHandShakeTickCount;

};
