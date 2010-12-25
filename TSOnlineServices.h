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
class MultiThreadJobQueue;
#include <string>
#include <WinInet.h>

class TSOnlineServices
{
public:
#ifdef _UNITTESTING
	static BOOL UnitTest(); 
#endif
	enum ErrorEnum
	{
		ERR_None,
		ERR_CommunicationError,
		ERR_ServerError,
		ERR_Last
	};
public:
	TSOnlineServices(LPCSTR systemID, HINTERNET hNet);
	~TSOnlineServices();

	BOOL SendDebugLogFile(LPCTSTR fileName);
	BOOL SendDebugLogData(LPCTSTR logData);
	BOOL SendGameResults(LPCSTR gameID, INT value, INT trackCount, UINT timeStamp, BOOL bBrowserMode);

	struct TrackInfo
	{
		TrackInfo()
		{
			memset(this, 0, sizeof(TrackInfo));
		}
		LPCTSTR artist;
		LPCTSTR title;
		LPCTSTR album;
		LPCTSTR tag;
		INT rating;
		LPCTSTR lyrics;
		LPCTSTR tablatures;
		LPCTSTR comments;
	};

	BOOL SendTrackInfo(TrackInfo& ti, UINT tsDatePlayed = 0);
	void SetAsyncMode(BOOL bAsync)			{m_bAsync = bAsync;}
	BOOL GetAsyncMode()						{return m_bAsync;}
private:
	BOOL SendData(LPCSTR data, BOOL bBrowserMode);
	static void text2xml(std::string& text);
	MultiThreadJobQueue* m_pMTCommunicationQueue;
	std::string m_uid;
	BOOL m_bAsync;
	HINTERNET m_hNet;

};
