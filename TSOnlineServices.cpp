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
#include "TSOnlineServices.h"
#include "URLEncDecHelper.h"
#include "MultiThreadJobQueue.h"
#include "stlStringUtils.h"
#include "CommunicationJob.h"
#include "SystemTimeFunctions.h"


#ifdef _UNITTESTING
#include "PrgAPI.h"
BOOL TSOnlineServices::UnitTest()
{
	PrgAPI* pAPI = PRGAPI();
	TSOnlineServices* pTS = pAPI->GetTSOnlineServices();
	SYSTEMTIME st;
	GetSystemTime(&st);
	UINT ts = SystemTime2UnixTimeStamp(st);
	do
	{
		pTS->SendDebugLogData(_T("This is the first log line\r\nThis is the second line"));
		pTS->SendDebugLogData(_T("This is the first log line\r\nκαι με Ελληνικά πιά"));
		pTS->SendGameResults("Test|30-5-0", 1, 22132, ts, TRUE);
		//Send track info
		TrackInfo ti;
		ti.artist = _T("Art1");
		ti.title = _T("Tra1");
		pTS->SendTrackInfo(ti);
		ti.album = _T("Alb1");
		pTS->SendTrackInfo(ti);
		ti.tag = _T("Tag1");
		pTS->SendTrackInfo(ti);
		ti.rating = 55;
		pTS->SendTrackInfo(ti);
		ti.lyrics =  _T("Όσο υπάρχεις θα υπάρχω\r\nκαι δε πρόκειται ποτέ να ξεχαστώ");
		pTS->SendTrackInfo(ti);
		pTS->SendTrackInfo(ti, ts);
		//===Send Just Lyrics
		ti = TrackInfo();
		ti.artist = _T("Art2");
		ti.title = _T("Tra4");
		ti.lyrics =  _T("Όσο υπάρχεις θα υπάρχω\r\nκαι δε πρόκειται ποτέ να ξεχαστώ (2)");
		pTS->SendTrackInfo(ti);
		//===Send Just Rating
		ti = TrackInfo();
		ti.artist = _T("Art2");
		ti.title = _T("Tra3");
		ti.rating = 55;
		pTS->SendTrackInfo(ti);
		//===Send Just History
		ti = TrackInfo();
		ti.artist = _T("Art2");
		ti.title = _T("Tra2");
		pTS->SendTrackInfo(ti, ts);


	}
	while (MessageBox(NULL, _T("Repeat?"), NULL, MB_YESNO) == IDYES);
	return TRUE;
}
#endif


TSOnlineServices::TSOnlineServices(LPCSTR systemID, HINTERNET hNet):
m_pMTCommunicationQueue(NULL),
m_uid(systemID),
m_bAsync(TRUE),
m_hNet(hNet)
{

}

TSOnlineServices::~TSOnlineServices()
{
	delete m_pMTCommunicationQueue;
}

BOOL TSOnlineServices::SendData(LPCSTR data, BOOL bBrowserMode)
{
#ifdef _DEBUG
	static LPCTSTR const sWebServiceURL = _T("http://services.artificialspirit.com/tsservice2.php");
#else
	static LPCTSTR const sWebServiceURL = _T("http://services.artificialspirit.com/tsservice.php");
#endif
	//static LPCTSTR const sWebServiceURL = _T("http://localhost/services/tsservice.php");

	std::string request = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	request += "<request>";
	request += "<info uid=\"";
	request += m_uid;
	request += "\" />";
	request += data;
	request += "</request>";


	INT dataSize = request.size();
	LPSTR encData = URLEncDecHelper::EncodeData((LPBYTE)request.c_str(), dataSize, TRUE, TRUE);

	std::string params;
	params += "data=";
	params += encData;
	delete[] encData;
	if (bBrowserMode)
	{
		params += "&browsermode=1";
		std::string browseUrl = CT2CA(sWebServiceURL);
		browseUrl += "?";
		browseUrl += params;
		ShellExecuteA(NULL, "open", browseUrl.c_str(), 0,0,0);
	}
	else
	{
#ifdef _DEBUG
		if (FALSE)
		{
			params += "&debug=2";
			std::string browseUrl = CT2CA(sWebServiceURL);
			browseUrl += "?";
			browseUrl += params;
			ShellExecuteA(NULL, "open", browseUrl.c_str(), 0,0,0);
		}
		else
		{
			if (m_pMTCommunicationQueue == NULL)
				m_pMTCommunicationQueue = new MultiThreadJobQueue;
			m_pMTCommunicationQueue->AddJob(JobSP(CommunicationJob::CreateJob(sWebServiceURL, params.c_str(), m_hNet)));
		}

#else
		if (m_bAsync == TRUE)
		{
			if (m_pMTCommunicationQueue == NULL)
				m_pMTCommunicationQueue = new MultiThreadJobQueue;
			m_pMTCommunicationQueue->AddJob(JobSP(CommunicationJob::CreateJob(sWebServiceURL, params.c_str(), m_hNet)));
		}
		else
		{
			CommunicationJob* pJob = CommunicationJob::CreateJob(sWebServiceURL, params.c_str(), m_hNet);
			pJob->Execute();
			delete pJob;
		}
#endif
	}

	return FALSE;
}


void TSOnlineServices::text2xml(std::string& text)
{
	replace(text, "&", "&amp;");
	replace(text, "<", "&lt;");
	replace(text, ">", "&gt;");
	replace(text, "'", "&apos;");
	replace(text, "\"", "&quot;");
}

//-----------------------------------------------------------------------------
//--------------------------SERVICES-------------------------------------------
//-----------------------------------------------------------------------------
BOOL TSOnlineServices::SendDebugLogFile(LPCTSTR fileName)//_T("debug.crs")
{
	TRACEST(_T("TSOnlineServices::SendDebugLogFile."));
	HANDLE f = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f == INVALID_HANDLE_VALUE)
		return FALSE;
	DWORD fSize = GetFileSize(f, 0);
	if (fSize != INVALID_FILE_SIZE)
	{
		CHAR* bf = new CHAR[fSize + 1];
		DWORD numBytesRead = 0;
		ReadFile(f, bf, fSize, &numBytesRead, NULL);
		CloseHandle(f);
		bf[fSize] = 0;

		INT len = MultiByteToWideChar(CP_ACP, 0, bf, fSize + 1, 0, 0);
		BOOL bRet = FALSE;
		if (len > 0)
		{
			WCHAR* bfu = new WCHAR[len + 1];
			len = MultiByteToWideChar(CP_ACP, 0, bf, fSize + 1, bfu, len + 1);
			if (len > 0)
				bRet = SendDebugLogData(bfu);
			delete bfu;
		}
		delete bf;
		return bRet;
	}
	return FALSE;
}

BOOL TSOnlineServices::SendDebugLogData(LPCTSTR logData)
{
	std::string request = "<operation name=\"senddebuglog\"><log>";

	const INT cMaxString = 30000;
	CHAR utf8bf[cMaxString];
	INT len = WideCharToMultiByte(CP_UTF8, 0, logData, -1, utf8bf, cMaxString, 0, 0);
	if (len == 0)
	{
		TRACE(_T("@1 TSOnlineServices::SendDebugLogData. WideCharToMultiByte Failed\r\n"));
		return FALSE;
	}
	utf8bf[len] = 0;
	std::string xmlstring = utf8bf;
	text2xml(xmlstring);
	request += xmlstring;

	request += "</log></operation>";
	return SendData(request.c_str(), FALSE);
}


BOOL TSOnlineServices::SendTrackInfo(TrackInfo& ti, UINT tsDatePlayed/* = 0*/)
{
	TRACEST(_T("TSOnlineServices::SendTrackInfo."));
	const INT cMaxString = 30000;
	CHAR utf8bf[cMaxString];

	std::string request = "<operation name=\"sendtrackinfo\" ";

	std::string xmlstring;
	if (ti.artist != NULL)
	{
		request += " artist=\"";
		INT len = WideCharToMultiByte(CP_UTF8, 0, ti.artist, -1, utf8bf, cMaxString, 0, 0);
		utf8bf[len] = 0;
		xmlstring = utf8bf;
		text2xml(xmlstring);
		request += xmlstring;
		request += "\"";
	}

	if (ti.title != NULL)
	{
		request += " title=\"";
		INT len = WideCharToMultiByte(CP_UTF8, 0, ti.title, -1, utf8bf, cMaxString, 0, 0);
		utf8bf[len] = 0;
		xmlstring = utf8bf;
		text2xml(xmlstring);
		request += xmlstring;
		request += "\"";
	}

	if (ti.album != NULL)
	{
		request += " album=\"";
		INT len = WideCharToMultiByte(CP_UTF8, 0, ti.album, -1, utf8bf, cMaxString, 0, 0);
		utf8bf[len] = 0;
		xmlstring = utf8bf;
		text2xml(xmlstring);
		request += xmlstring;
		request += "\"";
	}

	if (ti.title != NULL)
	{
		request += " tag=\"";
		INT len = WideCharToMultiByte(CP_UTF8, 0, ti.tag, -1, utf8bf, cMaxString, 0, 0);
		utf8bf[len] = 0;
		xmlstring = utf8bf;
		text2xml(xmlstring);
		request += xmlstring;
		request += "\"";
	}

	if (ti.rating > 0)
	{
		CHAR bf[100];
		_snprintf(bf, 50, " rating=\"%d\" ", ti.rating);
		request += bf;
	}

	if (tsDatePlayed > 0)
	{
		CHAR bf[100];
		_snprintf(bf, 100, " dateplayed=\"%d\"", tsDatePlayed);
		request += bf;
	}


	request += " >";


	if (ti.lyrics != NULL)
	{
		request += "<lyrics>";
		INT len = WideCharToMultiByte(CP_UTF8, 0, ti.lyrics, -1, utf8bf, cMaxString, 0, 0);
		utf8bf[len] = 0;
		xmlstring = utf8bf;
		text2xml(xmlstring);
		request += xmlstring;
		request += "</lyrics>";
	}

	if (ti.tablatures != NULL)
	{
		request += "<tablatures>";
		INT len = WideCharToMultiByte(CP_UTF8, 0, ti.tablatures, -1, utf8bf, cMaxString, 0, 0);
		utf8bf[len] = 0;
		xmlstring = utf8bf;
		text2xml(xmlstring);
		request += xmlstring;
		request += "</tablatures>";
	}

	if (ti.comments != NULL)
	{
		request += "<comments>";
		INT len = WideCharToMultiByte(CP_UTF8, 0, ti.comments, -1, utf8bf, cMaxString, 0, 0);
		utf8bf[len] = 0;
		xmlstring = utf8bf;
		text2xml(xmlstring);
		request += xmlstring;
		request += "</comments>";
	}

	request += "</operation>";

	return SendData(request.c_str(), FALSE);
}


BOOL TSOnlineServices::SendGameResults(LPCSTR gameID, INT value, INT trackCount, UINT timeStamp, BOOL bBrowserMode)
{
	TRACEST(_T("TSOnlineServices::SendGameResults."));
	CHAR bf[500];
	_snprintf(bf, 500, 
		"<operation name=\"sendgameresults\" gamename=\"%s\" value=\"%d\" trackcount=\"%d\" timestamp=\"%d\"/>",
		gameID,
		value,
		trackCount,
		timeStamp);
	return SendData(bf, bBrowserMode);
}



