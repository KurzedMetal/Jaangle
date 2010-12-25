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
#include "GoogleLyricsProvider.h"
#include "stlStringUtils.h"
#include "cStringUtils.h"
#include "WebPageUtilities.h"

#ifdef _USRDLL
std::vector<IInfoProvider*>	s_createdIPs;
extern "C" __declspec(dllexport) UINT GetInfoProviderCount()
{
	return 1;
}
extern "C" __declspec(dllexport) IInfoProvider* CreateInfoProvider(UINT idx)
{
	if (idx == 0)
	{
		IInfoProvider* pIP = new GoogleLyricsProvider;
		s_createdIPs.push_back(pIP);
		return pIP;
	}
	return NULL;
}
extern "C" __declspec(dllexport) BOOL DeleteInfoProvider(IInfoProvider* pIP)
{
	std::vector<IInfoProvider*>::iterator it = s_createdIPs.begin();
	for (; it != s_createdIPs.end(); it++)
	{
		if (*it == pIP)
		{
			delete pIP;
			s_createdIPs.erase(it);
			return TRUE;
		}
	}
	return FALSE;
}
#endif

#ifdef _UNITTESTING

const int lastSizeOf = 180;

BOOL GoogleLyricsProvider::UnitTest()
{
	if (lastSizeOf != sizeof(GoogleLyricsProvider))
		TRACE(_T("TestGoogleLyricsProvider. Object Size Changed. Was: %d - Is: %d\r\n"), lastSizeOf, sizeof(GoogleLyricsProvider));
	GoogleLyricsProvider g;
	std::tstring agent = _T("Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9) Gecko/2008052906 Firefox/3.0");

	HINTERNET hNet = InternetOpen(agent.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	g.SetInternetHandle(hNet);
	Request req(IInfoProvider::SRV_TrackLyrics);
	req.artist = _T("Afghan Whigs");
	req.title = _T("My Enemy");


	UNITTEST(g.OpenRequest(req));
	Result res;
	while (g.GetNextResult(res))
	{
		if (MessageBox(0, 
			res.main, 
			res.additionalInfo, 
			MB_YESNO)
			!=IDYES)
			break;
	}

	req.service = IInfoProvider::SRV_TrackTablatures;

	UNITTEST(g.OpenRequest(req));
	while (g.GetNextResult(res))
	{
		if (MessageBox(0, 
			res.main, 
			res.additionalInfo, 
			MB_YESNO)
			!=IDYES)
			break;
	}

	req.service = IInfoProvider::SRV_TrackLyrics;
	req.title = _T("Kapoio asxeto");
	UNITTEST(g.OpenRequest(req));
	while (g.GetNextResult(res))
	{
		if (MessageBox(0, 
			res.main, 
			res.additionalInfo, 
			MB_YESNO)
			!=IDYES)
			break;
	}




	InternetCloseHandle(hNet);
	req.artist = _T("Nirvana");
	req.title = _T("Smells like teen spirit");
	UNITTEST(g.OpenRequest(req));
	UNITTEST(!g.GetNextResult(res));
	g.SetInternetHandle(0);
	return TRUE;
}
#endif

GoogleLyricsProvider::GoogleLyricsProvider():
m_MinAllowedLyricsLen(100),
m_curResult(-1),
m_hNet(NULL),
m_request(SRV_First)
{

}

GoogleLyricsProvider::~GoogleLyricsProvider()
{
}

IInfoProvider* GoogleLyricsProvider::Clone() const
{
	IInfoProvider* pIP = new GoogleLyricsProvider;
	pIP->SetInternetHandle(GetInternetHandle());
	IConfigurableHelper::TransferConfiguration(*this, *pIP);
	return pIP;
}


BOOL GoogleLyricsProvider::CanHandle(ServiceEnum service) const
{
	switch (service)
	{
	case IInfoProvider::SRV_TrackLyrics:
	case IInfoProvider::SRV_TrackTablatures:
		return TRUE;
	}
	return FALSE;
}


BOOL GoogleLyricsProvider::OpenRequest(const Request& request)
{
	m_curResult = -1;
	ASSERT(request.IsValid());
	if (!request.IsValid())
		return FALSE;
	switch (request.service)
	{
	case SRV_TrackLyrics:
	case SRV_TrackTablatures:
		m_Artist = request.artist;
		m_Track = request.title;
		m_request.artist = m_Artist.c_str();
		m_request.title = m_Track.c_str();
		break;
	default:
		m_Artist.clear();
		m_Track.clear();
		return FALSE;
	}
	m_curResult = 0;
	m_googleLinks.clear();
	m_request.service = request.service;
	return TRUE;
}

//void GoogleLyricsProvider::FixURLParameter(std::tstring& str)
//{
//	replace(str, _T("\""), _T("%22"));
//	replace(str, _T("&"), _T("%26"));
//	replace(str, _T(" "), _T("+"));
//}

BOOL GoogleLyricsProvider::GetNextResult(Result& result)
{
	m_result.clear();
	m_resultAdditionalInfo.clear();
	ASSERT(m_curResult > -1);//Should be Initialized
	ASSERT(!m_Artist.empty() && !m_Track.empty());
	if (m_Artist.empty() || m_Track.empty())
		return FALSE;

	if (m_curResult == 0)
	{
		TRACE(_T("@3 GoogleLyricsProvider::GetResult(). Dowloading Google Page\r\n"));
		//std::tstring query = _T("http://www.google.com/search?q=");
		std::tstring query = _T("allintitle:\"");
		query += m_Artist;
		query += _T("\" \"");
		query += m_Track;
		switch (m_request.service)
		{
		case SRV_TrackLyrics:
			query += _T("\" lyrics");
			break;
		case SRV_TrackTablatures:
			query += _T("\" chords");
			break;
		default:
			ASSERT(0);
			return FALSE;
		}
		std::tstring fixString;
		URLEncode(fixString, query.c_str());
		query = _T("http://www.google.com/search?q=");
		query += fixString;




		//URLEncode(fixString, m_Artist.c_str());
		//query += fixString;
		//query += _T("\"+\"");
		//URLEncode(fixString, m_Track.c_str();
		//query += fixString;
		//switch (m_request.service)
		//{
		//case SRV_TrackLyrics:
		//	query += _T("\"+lyrics");
		//	break;
		//case SRV_TrackTablatures:
		//	query += _T("\"+chords");
		//	break;
		//default:
		//	ASSERT(0);
		//	return FALSE;
		//}

		
		//fixString.clear();
		//URLEncode(fixString, query.c_str());
		

		//CHAR bf[1000];
		//WideCharToMultiByte(CP_ACP, 0, query.c_str(), -1, bf, 1000, 0, 0);

		//CHAR url[1000];
		//DWORD bfLen = 1000;
		//BOOL ret = InternetCanonicalizeUrlA(bf, url, &bfLen, 0);
		//query = bf;

		//query = _T("http://www.google.com/search?q=allintitle%3A%22%CE%98%CE%B1%CE%BD%CE%AC%CF%83%CE%B7%CF%82%2B%CE%A0%CE%B1%CF%80%CE%B1%CE%BA%CF%89%CE%BD%CF%83%CF%84%CE%B1%CE%BD%CF%84%CE%AF%CE%BD%CE%BF%CF%85%22%2B%22%CE%89%CE%BC%CE%B5%CF%81%CE%BF%CF%82%2B%CE%8E%CF%80%CE%BD%CE%BF%CF%82%22%2Blyrics");
		std::string page;
		if (DownloadWebPage(page, m_hNet, query.c_str()))
		{
			const INT cLinkLen = 1000;
			CHAR linkBuffer[cLinkLen];
			LPCSTR startPos = page.c_str(); 
			while (startPos != NULL)
			{
				startPos = strstr(startPos, " class=g>");
				if (startPos)
				{
					startPos = strstr(startPos, "http:");
					if (startPos)
					{
						LPCSTR endPos = NULL;
						if (startPos[-1] == '"')
							endPos = strchr(startPos + 1, '"');
						else
							endPos = strchr(startPos + 1, '&');
						if (endPos)
						{
							if (endPos - startPos < (INT)cLinkLen)
							{
								strncpy(linkBuffer, startPos, endPos - startPos);
								linkBuffer[endPos - startPos] = 0;
								if (IsBlackListed(linkBuffer) == FALSE)
									m_googleLinks.push_back(linkBuffer);
							}
							else
								TRACE(_T("@1 GoogleLyricsProvider::NextResult. link bigger than expected (FAILED)\r\n"));
						}
						else
							TRACE(_T("@1 GoogleLyricsProvider::NextResult. Can't find endPos '%0.150s'\r\n"), CA2CT(&startPos[-1]));
						startPos = endPos;
					}
					else
						TRACE(_T("@1 GoogleLyricsProvider::NextResult. Can't find startPos (2) (FAILED)\r\n"));
				}
				else
					TRACE(_T("@1 GoogleLyricsProvider::NextResult. Can't find startPos (1) (FAILED)\r\n"));
			}
			if (m_googleLinks.empty())
			{
				startPos = page.c_str(); 
				//=== This happens on iGoogle
				while (startPos != NULL)
				{
					startPos = strstr(startPos, " class=r>");
					if (startPos)
					{
						startPos = strstr(startPos, "http:");
						if (startPos)
						{
							LPCSTR endPos = NULL;
							if (startPos[-1] == '"')
								endPos = strchr(startPos + 1, '"');
							else
								endPos = strchr(startPos + 1, '&');
							if (endPos)
							{
								if (endPos - startPos < (INT)cLinkLen)
								{
									strncpy(linkBuffer, startPos, endPos - startPos);
									linkBuffer[endPos - startPos] = 0;
									if (IsBlackListed(linkBuffer) == FALSE)
										m_googleLinks.push_back(linkBuffer);
								}
								else
									TRACE(_T("@1 GoogleLyricsProvider::NextResult. (r) link bigger than expected (FAILED)\r\n"));
							}
							else
								TRACE(_T("@1 GoogleLyricsProvider::NextResult. (r) Can't find endPos '%0.150s'\r\n"), CA2CT(&startPos[-1]));
							startPos = endPos;
						}
						else
							TRACE(_T("@1 GoogleLyricsProvider::NextResult. (r) Can't find startPos (2) (FAILED)\r\n"));
					}
					else
						TRACE(_T("@1 GoogleLyricsProvider::NextResult. (r) Can't find startPos (1) (FAILED)\r\n"));
				}

			}

		}
		else
			TRACE(_T("@1 GoogleLyricsProvider::NextResult(). Dowloading Google Page (FAILED)\r\n"));

	}

	std::wstring page;
	while (m_curResult < (INT)m_googleLinks.size())
	{
		BOOL bResultFound = FALSE;
		if (DownloadWebPageUnicode(page, m_hNet, (LPCTSTR)CA2CT(m_googleLinks[m_curResult].c_str())))
		{
			RemoveEnclosedString(page, _T("<!--"), _T("-->"));
			RemoveEnclosedString(page, _T("<head"), _T("/head>"));
			RemoveEnclosedString(page, _T("<script"), _T("/script>"));
			RemoveEnclosedString(page, _T("<a href="), _T("/a>"));
			ReplaceHtmlEntities(page);
			if (ExtractLyrics(page))
			{

				//InlineHTML2Text(pResult.get());
				//m_result = pResult.get();
				m_result = page;
				LPCTSTR delims = _T(" \t\n\r");
				m_result.erase(0, m_result.find_first_not_of(delims));		//Trim Left
				m_result.erase(m_result.find_last_not_of(delims) + 1);		//Trim Right
				if (m_result.size() > 40)
				{
					//Keep Additional Info (The Web Site Provider)
					LPCSTR pLink = m_googleLinks[m_curResult].c_str();
					LPCSTR sp = &pLink[7];
					LPCSTR ep = strchr(&pLink[8], '/');
					if (ep != 0)
					{
						CHAR SourceSiteU[MAX_PATH];
						strncpy(SourceSiteU, sp, INT(ep - sp) > 99 ? 99 : INT(ep - sp));
						SourceSiteU[INT(ep - sp) > 99 ? 99 : INT(ep - sp)] = 0;
						m_resultAdditionalInfo = (LPCTSTR)CA2CT(SourceSiteU);
					}
					else
						m_resultAdditionalInfo.clear();
					bResultFound = TRUE;
				}
			}
			else
				TRACE(_T("@1 GoogleLyricsProvider::GetResult(). ExtractLyrics (FAILED)\r\n"));
		}		
		else
			TRACE(_T("@1 GoogleLyricsProvider::GetResult(). Dowloading Lyrics Page (FAILED) '%s'\r\n"), (LPCTSTR)CA2CT(m_googleLinks[m_curResult].c_str()));

		m_curResult++;
		if (bResultFound)
			break;
	}
	if (m_result.empty())
		return FALSE;
	result.main = m_result.c_str();
	result.additionalInfo = m_resultAdditionalInfo.c_str();
	result.service = m_request.service;
	return TRUE;
}


LPCTSTR GoogleLyricsProvider::GetModuleInfo(ModuleInfo mi) const
{
	switch (mi)
	{
	case IPI_UniqueID:		return _T("GOLY");
	case IPI_Name:			return _T("Google Lyrics");
	case IPI_Author:		return _T("Alex Economou");
	case IPI_VersionStr:	return _T("0.8");
	case IPI_VersionNum:	return _T("6");
	case IPI_Description:	return _T("Downloads lyrics & tablatures using google search");
	case IPI_HomePage:		return _T("http://teenspirit.artificialspirit.com");
	case IPI_Email:			return _T("alex@artificialspirit.com");
	}
	return NULL;
}


//***************************************************************
//OLD CODE
//***************************************************************

LPCSTR const BlacListedSitesLyrics[] =
{
	"leoslyrics.com",	//This needs login
	"azchords.com",		//This has chords (RadioHead - No Surprises)
	"lyricsdir.com",	//This shows disclaimer  (RadioHead - No Surprises)
	"songlyrics.com",	//Needs to press button  (Linkin Park - From the inside)
	"actionext.com",	//Usually asks to add the lyrics yourself
	NULL
};
LPCSTR const BlacListedSitesTabs[] =
{
	NULL
};

BOOL GoogleLyricsProvider::IsBlackListed(LPCSTR link)
{
	const LPCSTR* blackList = NULL;
	switch (m_request.service)
	{
	case SRV_TrackLyrics:
		blackList = BlacListedSitesLyrics;
		break;
	case SRV_TrackTablatures:
		blackList = BlacListedSitesTabs;
		break;
	}
	INT i = 0;
	while (blackList[i] != NULL)
	{
		if (strstr(link, blackList[i]) != NULL)
			return TRUE;
		i++;
	}
	return FALSE;

}



BOOL GoogleLyricsProvider::ExtractLyrics(std::tstring& page)
{
	UINT maxCharSeqLen = 0;
	LPCTSTR maxFirstSeqChar = 0;
	LPCTSTR maxLastSeqChar = 0;
	UINT curCharSeqLen = 0;
	LPCTSTR curFirstSeqChar = 0;

	LPCTSTR curChar = page.c_str();
	BOOL bInTag = FALSE;
	BOOL bBRHasBeenFound = FALSE;

	std::tstring ansi;
	for (int pass = 0; pass < 2; pass++)
	{
		if (pass == 1)
		{
			//SecondPass
			if (maxCharSeqLen <= m_MinAllowedLyricsLen)
				return FALSE;
			curChar = maxFirstSeqChar;
			bInTag = FALSE;
		}
		while (*curChar != 0)
		{
			if (maxLastSeqChar <= curChar && pass == 1)
			{
				page = ansi;
				return TRUE;
			}

			switch (*curChar)
			{
			case '<':
				//ASSERT(bInTag == FALSE);
				bInTag = TRUE;
				if (_tcsnicmp(curChar, _T("<br"), 3) == 0 && curCharSeqLen > 0)
				{
					bBRHasBeenFound = TRUE;
					curCharSeqLen+=2;
					if (pass == 1)
						ansi += _T("\r\n");
					//strcat(ansi, "\r\n");
				}
				else
				{
					//Finished...
					if (curCharSeqLen > maxCharSeqLen)
					{
						maxCharSeqLen = curCharSeqLen;
						maxFirstSeqChar = curFirstSeqChar;
						maxLastSeqChar = curChar;
					}
					curCharSeqLen = 0;
					bBRHasBeenFound = FALSE;
				}
				break;
			case '>':
				//ASSERT(bInTag == TRUE);
				bInTag = FALSE;
				break;
			case 10:
				if (!bBRHasBeenFound && curCharSeqLen > 0)//This is a lonesome LF. He can t be here if it was CRLF (look 13)
				{
					curCharSeqLen+=2;
					if (pass==1)
						ansi += _T("\r\n");
					//strcat(ansi, "\r\n");
				}
				break;
			case 13:
				if (!bBRHasBeenFound && curCharSeqLen > 0)
				{
					curCharSeqLen+=2;
					if (pass==1)
						ansi += _T("\r\n");
					if(*curChar == 10)//Handle the case that you get CR without LF following
						curChar++;
				}
				break;
			default:
				if (bInTag == FALSE)
				{
					if (curCharSeqLen == 0)
						curFirstSeqChar = curChar;
					if (pass == 1)
						ansi += *curChar;
					curCharSeqLen++;
				}
			}
			curChar++;
		}
	}
	return NULL;
}
