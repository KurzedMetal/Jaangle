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
#include "LastFMServices.h"
#include "cMD5.h"
#include "SystemTimeFunctions.h"
#include "WebPageUtilities.h"
#include "stlStringUtils.h"

LPCTSTR sProtocolVer = _T("1.2.1");
LPCTSTR sClientID = _T("tsp");
LPCTSTR sClientVer = _T("1.0");
const INT sMaxTracksOnQueue = 10;

LastFMServices::LastFMServices(void):
m_state(ST_UnInitialized),
m_error(ERR_None),
m_hNet(0),
m_pThread(NULL),
m_bExitThread(FALSE),
m_lastHandShakeTickCount(0)
{
}

LastFMServices::~LastFMServices(void)
{
	m_bExitThread = TRUE;
	if (m_pThread != NULL)
	{
		LeaveThread(m_pThread->m_hThread, 500);
	}
}

void LastFMServices::Init(HINTERNET hNet)
{
	ASSERT(hNet != NULL);
	m_hNet = hNet;
	m_state = ST_Idle;
}

void LastFMServices::SetUserName(LPCTSTR username)
{
	ASSERT(username != NULL);
	if (username != NULL)
		m_username = username;
	else
		m_username.clear();

}
void LastFMServices::SetMD5Password(LPCTSTR MD5password)
{
	ASSERT(MD5password != NULL);
	if (MD5password != NULL)
		m_MD5password = MD5password;
	else
		m_MD5password.clear();
}

BOOL LastFMServices::CheckLogin()
{
	ASSERT(!m_username.empty());
	ASSERT(!m_MD5password.empty());
	if (m_username.empty() || m_MD5password.empty())
		return FALSE;
	std::basic_string<TCHAR> ses, now, sub;
	if (HandShake(ses, now, sub))
	{
		ASSERT(!ses.empty());
		return TRUE;
	}
	return FALSE;
}




void LastFMServices::Submit(LastFMSubmitTrack& track)
{
	ASSERT(m_state != ST_UnInitialized);
	//Check the track
	if (track.IsValid() && track.length >= 30)
	{
		if (track.unixTimeStamp == 0) // Automatically fix the "start playing" date if needed
		{
			track.unixTimeStamp = GetCurrentUnixTimeStamp();
			if (track.length < 480)
				track.unixTimeStamp -= (track.length/2);
			else
				track.unixTimeStamp -= 240;
		}
		CSingleLock lock(&m_cs);
		if (sMaxTracksOnQueue > m_jobs.size())
			m_jobs.push_back(track);
		if (m_pThread == NULL)
			m_pThread = AfxBeginThread(LastFMServices::WorkingThreadST, this, THREAD_PRIORITY_BELOW_NORMAL);

	}



}


BOOL LastFMServices::IsTrackAvailable()
{
	CSingleLock lock(&m_cs);
	return m_jobs.size() > 0;
}
//BOOL LastFM::GetTrack(LastFMSubmitTrack& track)
//{
//	CSingleLock lock(&m_cs);
//	if (m_jobs.size() > 0)
//	{
//		track = *m_jobs.begin();
//		return TRUE;
//	}
//	return FALSE;
//}
//void LastFM::PopTrack()
//{
//	CSingleLock lock(&m_cs);
//	if (m_jobs.begin() != m_jobs.end())
//		m_jobs.erase(m_jobs.begin());
//}
//BOOL LastFM::PushTrack(LastFMSubmitTrack& track)
//{
//	CSingleLock lock(&m_cs);
//	if (sMaxTracksOnQueue > m_jobs.size())
//	{
//		m_jobs.push_back(track);
//		return TRUE;
//	}
//	return FALSE;
//}


UINT LastFMServices::WorkingThreadST(LPVOID pParam)
{
	ASSERT(pParam != NULL);
	return ((LastFMServices*)pParam)->WorkingThread();
}
UINT LastFMServices::WorkingThread()
{
	std::basic_string<TCHAR> sessionID;
	std::basic_string<TCHAR> nowPlayingURL;
	std::basic_string<TCHAR> submissionURL;
	INT failCounter = 0;
	UINT lastTickCheck = 0;
	while (m_bExitThread == FALSE)
	{
		if (GetTickCount() - lastTickCheck > 1000)
		{
			if (IsTrackAvailable())
			{
				if (sessionID.empty())
				{
					if (GetTickCount() - m_lastHandShakeTickCount > 30000)
					{
						HandShake(sessionID, nowPlayingURL, submissionURL);
						m_lastHandShakeTickCount = GetTickCount();
					}
				}
				if (!sessionID.empty())
				{
					if (SubmitTracks(sessionID.c_str(), submissionURL.c_str()))
					{
						failCounter = 0;
						CSingleLock lock(&m_cs);
						m_jobs.clear();
					}
					else
					{
						failCounter++;
						if (failCounter == 3)
							sessionID.clear();
					}

				}
			}
			lastTickCheck = GetTickCount();
		}
		Sleep(10);
	}
	TRACE(_T("LastFM::WorkingThread. Exit\r\n"));
	return 0;
}

BOOL LastFMServices::HandShake(std::basic_string<TCHAR>& sessionID, 
							   std::basic_string<TCHAR>& nowPlayingURL, 
							   std::basic_string<TCHAR>& submissionURL)
{
	sessionID.clear();
	nowPlayingURL.clear();
	submissionURL.clear();
	if (m_username.empty() || m_MD5password.empty())
	{
		//TRACE(_T("@4 LastFMServices::HandShake. Empty Authorization.\r\n"));
		m_error = ERR_BadAuth;
		return FALSE;
	}
	SYSTEMTIME st;
	GetSystemTime(&st);
	UINT ts = SystemTime2UnixTimeStamp(st);
	LPCTSTR auth = _T("auth");
	cMD5 md5;
	CHAR authStr[100];
	_snprintf(authStr, 100, "%s%u", (LPCSTR)CT2CA(m_MD5password.c_str()), ts);


	//http://post.audioscrobbler.com/?hs=true&p=1.2.1&c=<client-id>&v=<client-ver>&u=<user>&t=<timestamp>&a=<auth>
	TCHAR url[2000];
	_sntprintf(url, 2000, 
		_T("http://post.audioscrobbler.com/?hs=true&p=%s&c=%s&v=%s&u=%s&t=%u&a=%.32s"),
		sProtocolVer,
		sClientID,
		sClientVer,
		m_username.c_str(),
		ts,
		(LPCTSTR)CA2CT(_strlwr(md5.CalcMD5FromString(authStr))));

	TRACE(_T("About to request: '%s'\r\n"), url);
	m_state = ST_Connecting;

	std::string page;
	if (DownloadWebPage(page, m_hNet, url))
	{
		std::string delimiter = "\n";
		std::string line;
		INT pos = getToken(page, 0, delimiter, line);
		INT count = 0;
		while (pos != -1)
		{
			switch (count)
			{
			case 0:
				m_errorString = line;
				if (line == "OK")
					m_error = ERR_None;
				else if (line == "BANNED")
					m_error = ERR_Banned;
				else if (line == "BADAUTH")
					m_error = ERR_BadAuth;
				else if (line == "BADTIME")
					m_error = ERR_BadTime;
				else 
					m_error = ERR_Failed;
				break;
			case 1:
				sessionID = (LPCTSTR)CA2CT(line.c_str());
				break;
			case 2:
				nowPlayingURL = (LPCTSTR)CA2CT(line.c_str());
				break;
			case 3:
				submissionURL = (LPCTSTR)CA2CT(line.c_str());
				break;
			default:
				break;
			}
			if (m_error != ERR_None)
				break;
			count++;
			pos = getToken(page, pos, delimiter, line);
		}
		if (m_error == ERR_None)
		{
			TRACE(_T("@4 LastFMServices::HandShake. success.\r\n"));
			m_state = ST_Idle;
		}
		else
		{
			TRACE(_T("@4 LastFMServices::HandShake. fail: '%s'.\r\n"), CA2CT(GetErrorString()));
			m_state = ST_Error;
		}
	}
	else //---Download page failed
	{
		m_error = ERR_CommunicationError;
		m_state = ST_Error;
		TRACE(_T("@4 LastFMServices::HandShake. fail: Communication Error.\r\n"));
	}
	return m_state == ST_Idle;

}


BOOL LastFMServices::ConvertString(LPCTSTR src, LPSTR bf, UINT bfLen)
{
	std::tstring temp = src;
	replace(temp, _T("&"), _T("%26"));
	INT ret = WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), (INT)temp.size() + 1, bf, bfLen, NULL, NULL);
	return ret > 0;
}




BOOL LastFMServices::SubmitTracks(LPCTSTR sessionID, LPCTSTR submissionURL)
{
	ASSERT(sessionID != NULL && submissionURL != NULL);
	if (!(sessionID != NULL && submissionURL != NULL))
		return FALSE;
	ASSERT(sessionID[0] != 0 && submissionURL[0]);
	if (!(sessionID[0] != 0 && submissionURL[0]))
		return FALSE;
	TRACEST(_T("LastFMServices::SubmitTracks"));
	std::string postData;
	const INT bfSize = 1000;
	postData.reserve(bfSize);
	CHAR bf[bfSize];
	CHAR utf8bf[bfSize];
	//s=<sessionID>
	//The Session ID string returned by the handshake request. Required
	_snprintf(bf, bfSize, "s=%s&", CT2CA(sessionID));
	postData = bf;
	{
		CSingleLock lock(&m_cs);
		std::list<LastFMSubmitTrack>::const_iterator it = m_jobs.begin();
		UINT i = 0;
		for (;it != m_jobs.end(); it++)
		{
			const LastFMSubmitTrack& track = *it;
			//a[0]=<artist>
			//The artist name. Required.
			ConvertString(track.artist.c_str(), utf8bf, bfSize);
			_snprintf(bf, bfSize, "a[%d]=%s&", i, utf8bf);
			bf[bfSize - 1] = 0;
			postData += bf;
			//t[0]=<track>
			//The track title. Required.
			ConvertString(track.track.c_str(), utf8bf, bfSize);
			_snprintf(bf, bfSize, "t[%d]=%s&", i, utf8bf);
			bf[bfSize - 1] = 0;
			postData += bf;
			//i[0]=<time>
			//The time the track started playing, in UNIX timestamp format. 
			//This must be in the UTC time zone, and is required.
			_snprintf(bf, bfSize, "i[%d]=%u&", i, track.unixTimeStamp);
			bf[bfSize - 1] = 0;
			postData += bf;
			//o[0]=<source>
			//The source of the track. Required
			_snprintf(bf, bfSize, "o[%d]=P&", i);
			postData += bf;
			//r[0]=<rating>
			//A single character denoting the rating of the track
			_snprintf(bf, bfSize, "r[%d]=&", i);
			bf[bfSize - 1] = 0;
			postData += bf;

			//l[0]=<secs>
			//The length of the track in seconds. Required when the source is P, optional otherwise.
			_snprintf(bf, bfSize, "l[%d]=%d&", i, track.length);
			bf[bfSize - 1] = 0;
			postData += bf;

			//b[0]=<album>
			//The album title, or an empty string if not known.
			ConvertString(track.album.c_str(), utf8bf, bfSize);
			_snprintf(bf, bfSize, "b[%d]=%s&", i, utf8bf);
			bf[bfSize - 1] = 0;
			postData += bf;

			//n[0]=<tracknumber>
			//The position of the track on the album, or an empty string if not known.
			_snprintf(bf, bfSize, "n[%d]=%d&", i, track.trackNo);
			bf[bfSize - 1] = 0;
			postData += bf;

			//m[0]=<mb-trackid>
			//The MusicBrainz Track ID, or an empty string if not known.
			_snprintf(bf, bfSize, "m[%d]=&", i);
			bf[bfSize - 1] = 0;
			postData += bf;
			i++;
		}
	}
	postData.erase(postData.size() - 1);

	TRACE(_T("@4 LastFMServices::SubmitTracks. postingData '%s'\n"), CA2CT(postData.c_str()));
	std::string page;
	if (DownloadWebPagePost(page, m_hNet, submissionURL, postData.c_str()))
	{
		m_errorString = page;
		if (strncmp(page.c_str(), "OK", 2) == 0)
		{
			//TRACE(_T("@4 LastFMServices::SubmitTracks. Track Succesfully submitted.\n"));
			m_error = ERR_None;
			m_state = ST_Idle;
		}
		else 
		{
			m_state = ST_Error;
			if (strncmp(page.c_str(), "BADSESSION", 10) == 0)
				m_error = ERR_BadSession;
			else 
				m_error = ERR_Failed;
			TRACE(_T("@1 LastFMServices::SubmitTracks. Track Failed (%d):'%s'.\n"), m_error, CA2CT(GetErrorString()));
			TRACE(_T("@1 ... postedData '%s'.\n"), CA2CT(postData.c_str()));
		}
	}
	else
	{
		m_state = ST_Error;
		m_error = ERR_CommunicationError;
		TRACE(_T("@4 LastFMServices::SubmitTracks. DownloadWebPage Failed\n"));
	}
	if (m_state == ST_Error)
		return FALSE;
	return TRUE;
}

BOOL LastFMServices::LeaveThread(HANDLE hThread, UINT timeOut)
{
	BOOL ret = TRUE;
	DWORD exitCode = 0;
	GetExitCodeThread(hThread, &exitCode);
	SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
	DWORD tickCount = GetTickCount();
	while (exitCode == STILL_ACTIVE)
	{
		Sleep(0);
		if (GetTickCount() - tickCount > timeOut)
		{
			TRACE(_T("@0 LeaveThread: Terminating Thread: %d\r\n"), (INT)hThread);
			TerminateThread(hThread, -1);
			return FALSE;
		}
		if (!GetExitCodeThread(hThread, &exitCode))
			return FALSE;
	}
	return TRUE;
}


