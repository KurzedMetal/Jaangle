#include "stdafx.h"
#include "FreeDb.h"
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")
#include "WebPageUtilities.h"
#include "stlStringUtils.h"


#ifdef _UNITTESTING
BOOL FreeDB::UnitTest()
{
	UNITTESTSTART;

	//Read Disc
	if (MessageBox(NULL, _T("Do You want to test invalid CDs?"), NULL, MB_YESNO) == IDYES)
	{
		AudioCDInfo	cdInfo;
		UNITTEST(!FreeDB::ReadDisc(_T("c:\\windows"), cdInfo));
		UNITTEST(!FreeDB::ReadDisc(_T("c:\\"), cdInfo));
		MessageBox(NULL, _T("CD-Rom (G:\\) must be empty. Press OK to continue."), NULL, MB_OK);
		UNITTEST(!FreeDB::ReadDisc(_T("g:\\"), cdInfo));
		MessageBox(NULL, _T("Put a Data CD in the CD-Rom (G:\\). Press OK to continue."), NULL, MB_OK);
		UNITTEST(!FreeDB::ReadDisc(_T("g:\\"), cdInfo));
	}
	HINTERNET hNet = InternetOpen(_T("UnitTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	do 
	{
		AudioCDInfo	cdInfo;
		MessageBox(NULL, _T("Put an Audio CD in the CD-Rom (G:\\). Press OK to continue."), NULL, MB_OK);
		UNITTEST(FreeDB::ReadDisc(_T("g:\\"), cdInfo));
		UNITTEST(cdInfo.discID != 0);
		//else
		//{
		//	//"Me lene Popi"
		//	cdInfo.discID = 185678866;
		//	cdInfo.totalLength = 4412;
		//	AudioCDTrackInfo track;
		//	track.length = MSF(3, 20, 20);
		//	for (int i=0; i<18; i++)
		//	{
		//		track.offset = MSF(0, 0, 2);
		//		cdInfo.tracks.push_back(track);
		//		track.offset += track.length;
		//		track.offset += MSF(0, 0, 2);
		//	}
		//}

		FreeDB fdb;
		fdb.Init(_T("UC"), _T("0.1"), hNet);
		UNITTEST(fdb.RetrieveFreeDbServers());
		UNITTEST(fdb.GetServerCount() > 0);
		INT srvCount = fdb.GetServerCount();
		TRACE(_T("FreeDB Servers\r\n"));
		for (UINT i = 0; i<fdb.GetServerCount(); i++)
			TRACE(_T("%d. %s\r\n"), i, fdb.GetServer(i));
		std::vector<FreeDBResult> results;
		UNITTEST(fdb.Query(results, cdInfo, fdb.GetServer(0)));
		TRACE(_T("FreeDB Results for DiscID: %d\r\n"), cdInfo.discID);
		for (UINT i = 0; i<results.size(); i++)
		{
			TRACE(_T("%d. %s\r\n"), i, results[i].title.c_str());
			TRACE(_T("\t%s - %s\r\n"), results[i].genre.c_str(), results[i].discID.c_str());
		}
		
		UNITTEST(fdb.Read(results[0], cdInfo, fdb.GetServer(0)));
		TRACE(_T("AudioCDInfo... Title:'%s' Genre:'%s'\r\n"), 
			cdInfo.title.c_str(), 
			cdInfo.genre.c_str());
		TRACE(_T("\tYear:'%d' TotalLegth:'%d'\r\n"), 
			cdInfo.year, 
			cdInfo.totalLength);
		std::tstring artist, album;
		FreeDB::GetFirstPart(artist, cdInfo.title.c_str());
		FreeDB::GetSecondPart(album, cdInfo.title.c_str());
		TRACE(_T("\tArtist: '%s' - Album: '%s'\r\n"), artist.c_str(), album.c_str());


		for (size_t i = 0; i < cdInfo.tracks.size(); i++)
		{
			AudioCDTrackInfo& tr = cdInfo.tracks[i];
			TRACE(_T("%d '%s'\r\n"), i, tr.title.c_str());
			TRACE(_T("\tlen:%d offs:%d\r\n"), tr.length.toSec(), tr.offset.toSec());
		}
	} while(MessageBox(NULL, _T("Do you want to try more?"), NULL, MB_YESNO) == IDYES);

	InternetCloseHandle(hNet);
	return TRUE;
}
#endif


const TCHAR* defFreeDBServer = _T("http://freedb.freedb.org/~cddb/cddb.cgi");

//-------------------------------------------------------------------
//MSF Helper Class --------------------------------------------------------------
//-------------------------------------------------------------------
MSF::MSF(INT min /*= 0*/, INT sec /*= 0*/, INT Frame /*= 0*/):
m_min(min),
m_sec(sec),
m_frame(Frame)
{}

MSF MSF::operator+(const MSF& other )
{
	return int2msf(msf2int(other) + msf2int(*this));
}

MSF& MSF::operator+=(const MSF& other )
{
	*this = int2msf(msf2int(other) + msf2int(*this));
	return *this;
}

void MSF::operator=(const MSF &other )
{
	m_min = other.m_min;
	m_sec = other.m_sec;
	m_frame = other.m_frame;
}

INT MSF::msf2int(const MSF& msf)
{
	return msf.m_min * 60 * 75 + msf.m_sec * 75 + msf.m_frame;
}
MSF MSF::int2msf(INT frames)
{
	MSF msf;
	msf.m_min = frames / (60 * 75);
	frames -= msf.m_min * 60 * 75;
	msf.m_sec = frames / 75;
	msf.m_frame = frames - msf.m_sec * 75;
	return msf;
}

UINT MSF::toFrame()
{
	return msf2int(*this);
}
UINT MSF::toSec()
{
	return m_min * 60 + m_sec;
}

//-------------------------------------------------------------------
//FreeDB   -------------------------------------------------------------------
//-------------------------------------------------------------------

FreeDB::FreeDB():m_hNet(NULL)
{

}

FreeDB::~FreeDB()
{

}

void FreeDB::Init(LPCTSTR clientname, LPCTSTR clientversion, HINTERNET hNet)
{
	ASSERT(hNet != NULL);

	const LPCTSTR sUnknown = _T("Unknown");
	TCHAR sUser[MAX_PATH];
	DWORD retLen = MAX_PATH;
	if (!::GetUserName(sUser, &retLen))
		_tcscpy(sUser, sUnknown);
	else
		NormalizeString(sUser);
	//CHAR sTempHost[MAX_PATH];
	TCHAR sHost[MAX_PATH];
	//if (!::gethostname(sTempHost, MAX_PATH))
		_tcscpy(sHost, sUnknown);
	//else
	//{
	//	_tcscpy(sHost, CA2T(sTempHost));
	//	NormalizeString(sHost);
	//}
	TCHAR sClientName[50];
	if (clientname == NULL)
		_tcscpy(sClientName, sUnknown);
	else
	{
		_tcscpy(sClientName, clientname);
		NormalizeString(sClientName);
	}
	TCHAR sClientVersion[50];
	if (clientversion == NULL)
		_tcscpy(sClientVersion, sUnknown);
	else
	{
		_tcscpy(sClientVersion, clientname);
		NormalizeString(sClientVersion);
	}

	_sntprintf(m_ident, MAX_IDENT_LEN, _T("%s+%s+%s+%s&proto=5"), 
		sUser, 
		sHost, 
		clientname, 
		clientversion);

	m_ident[MAX_IDENT_LEN - 1] = 0;
	m_hNet = hNet;
}


UINT FreeDB::GetServerCount()
{
	return (UINT) m_serverList.size();
}

LPCTSTR FreeDB::GetServer(UINT idx)
{
	ASSERT(idx >= 0 && idx < GetServerCount());
	if (idx >= 0 && idx < GetServerCount())
		return m_serverList[idx].c_str();
	return NULL;
}



BOOL FreeDB::RetrieveFreeDbServers()
{
	//http://freedb.freedb.org/~cddb/cddb.cgi?cmd=sites&hello=Alex+outcast_desk+MfcCDDB+1.27&proto=5
	ASSERT(m_hNet != NULL);
	if (m_hNet == NULL)
		return FALSE;
	TCHAR urlCmd[500];
	_sntprintf(urlCmd, 500, _T("%s?cmd=sites&hello=%s"), defFreeDBServer, m_ident);
	std::string page;
	if (!DownloadWebPage(page, m_hNet, urlCmd))
		return FALSE;

	INT errCode = GetPageErrorCode(page.c_str());
	if (errCode != 210)
	{
		TRACE(_T("@1 RetrieveFreeDbServers Error: '%s'\r\n"), CA2CT(page.c_str()));
		return FALSE;
	}
	INT startPos = 0;
	BOOL bFirstRow = TRUE;
	std::string lineEnd("\r\n");
	std::string space(" ");
	std::string line;
	do 
	{
		startPos = getToken(page, startPos, lineEnd, line);
		if (line == ".")
			break;
		if (!bFirstRow)
		{
			std::vector<std::string> tokens;
			if (splitString(line, space, tokens, TRUE) > 2)
			{
				if (tokens[1] == "http")
				{
					std::tstring server(_T("http://"));
					server += CA2CT(tokens[0].c_str());
					server += _T(":");
					server += CA2CT(tokens[2].c_str());
					server += CA2CT(tokens[3].c_str());
					m_serverList.push_back(server);
				}

			}
		}
		bFirstRow = FALSE;
	}
	while (startPos != std::string::npos);
	return m_serverList.size() > 0;
}

void FreeDB::NormalizeString(LPTSTR str)
{
	ASSERT(str!= NULL);
	UINT i = 0;
	while (str[i] != 0)
	{
		BOOL bIsAlpha = (((str[i]>=_T('A')) && (str[i]<=_T('Z'))) ||   //Definitively between A-Z or a-z and nothing else!
			((str[i]>=_T('a')) && (str[i]<=_T('z')))); 
		BOOL bIsDigit = ((str[i]>=_T('0')) && (str[i]<=_T('9')));
		if (!(bIsAlpha || bIsDigit))
			str[i] = _T('_');
		i++;
	}
}

UINT FreeDB::GetPageErrorCode(LPCSTR page)
{
	CHAR errCodeStr[4];
	strncpy(errCodeStr, page, 3);
	errCodeStr[3] = 0;
	return atoi(errCodeStr);
}

FreeDB::QueryResult FreeDB::Query(std::vector<FreeDBResult>& results, AudioCDInfo& CDInfo, LPCTSTR freeDbServer /*= NULL*/)
{
	ASSERT(CDInfo.tracks.size() > 0 && CDInfo.discID != 0);//You must first initialize CDInfo using GetRawDiscInfo()
	if (CDInfo.tracks.size() == 0 || CDInfo.discID == 0)
	{
		TRACE(_T("FreeDB::Query. CDInfo.tracks.size() == 0"));
		return QR_NotFound;
	}
	std::tstring sOffsets;
	TCHAR offset[100];
	for (UINT i=0; i < CDInfo.tracks.size(); i++)
	{
		//Form the frame number
		//CString sFrame;
		//sFrame.Format(_T("%d"), tracks[i].m_nMinute*60*75 + tracks[i].m_nSecond*75 + tracks[i].m_nFrame);
		_sntprintf(offset, 100, _T("%d"), CDInfo.tracks[i].offset.toFrame());
		//Accumulate the frame number into the string
		sOffsets += offset;
		//Add a space between each offset
		if (i<(CDInfo.tracks.size()-1))
			sOffsets += _T("+");
	}

	TCHAR urlCmd[2000];
	_sntprintf(urlCmd, 2000, _T("%s?cmd=cddb+query+%08x+%d+%s+%d&hello=%s"),
		(freeDbServer == NULL) ? defFreeDBServer : freeDbServer,
		CDInfo.discID,
		CDInfo.tracks.size(),
		sOffsets.c_str(),
		CDInfo.totalLength,
		m_ident
		);
	std::string page;
	//ASSERT(0);
	//sPage = "200 misc d20cc90f Ãéþñãïò Óôáõñéáíüò / ÙÑÅÓ ÌÏÕ ×ÑÙÌÁÔÉÓÔÅÓ";
	if (!DownloadWebPage(page, m_hNet, urlCmd))
		return QR_NotFound;
	INT errCode = GetPageErrorCode(page.c_str());
	switch (errCode)
	{
	case 200:
		{
			std::string space(" ");
			std::string buffer;
			INT linePos = getToken(page, 0, space, buffer);
			if (linePos != std::string::npos)
			{
				linePos = getToken(page, linePos, space, buffer);
				if (linePos != std::string::npos)
				{
					FreeDBResult res;
					res.genre = CA2CT(buffer.c_str());
					linePos = getToken(page, linePos, space, buffer);
					if (linePos != std::string::npos)
					{
						res.discID = CA2CT(buffer.c_str());
						buffer = page.substr(linePos);
						res.title = CA2CT(buffer.c_str());
						results.push_back(res);
					}
				}
			}
		}
		return QR_FoundExactMatch;
		break;
	case 210:
	case 211:
		{
			BOOL bFirstRow = TRUE;
			std::string lineEnd("\r\n");
			std::string space(" ");
			std::string line;
			INT startPos = 0;
			do 
			{
				startPos = getToken(page, startPos, lineEnd, line);
				if (line == ".")
					break;
				if (!bFirstRow)
				{
					std::string buffer;
					INT linePos = getToken(line, 0, space, buffer);
					if (linePos != std::string::npos)
					{
						FreeDBResult res;
						res.genre = CA2CT(buffer.c_str());
						linePos = getToken(line, linePos, space, buffer);
						if (linePos != std::string::npos)
						{
							res.discID = CA2CT(buffer.c_str());
							buffer = line.substr(linePos);
							res.title = CA2CT(buffer.c_str());
							results.push_back(res);
						}
					}
				}
				bFirstRow = FALSE;
			}
			while (startPos != std::string::npos);
			if (errCode == 211)
				return QR_FoundInexactMatches;
			return QR_FoundExactMatches;
		}
		break;
	case 202: //202 No match for disc ID 550d6a18.
	default:
		TRACE(_T("@1 FreeDB::Query Error: '%s'\r\n"), CA2CT(page.c_str()));
		return QR_NotFound;
		break;
	}
	return QR_NotFound;
}

BOOL FreeDB::Read(FreeDBResult result, AudioCDInfo& CDInfo, LPCTSTR freeDbServer/* = NULL*/)
{
	TCHAR urlCmd[2000];
	_sntprintf(urlCmd, 2000, _T("%s?cmd=cddb+read+%s+%s&hello=%s"),
		(freeDbServer == NULL) ? defFreeDBServer : freeDbServer, 
		result.genre.c_str(),
		result.discID.c_str(),
		m_ident);

	std::string page;
	//ASSERT(0);
	//sPage = "200 misc d20cc90f Ãéþñãïò Óôáõñéáíüò / ÙÑÅÓ ÌÏÕ ×ÑÙÌÁÔÉÓÔÅÓ";
	if (!DownloadWebPage(page, m_hNet, urlCmd))
		return FALSE;
	INT errCode = GetPageErrorCode(page.c_str());
	BOOL bFirstLine = TRUE;
	switch (errCode)
	{
	case 210://The normal results
		{
			BOOL bFirstRow = TRUE;
			std::string lineEnd("\r\n");
			std::string equal("=");
			std::string line;
			INT startPos = 0;
			do 
			{
				startPos = getToken(page, startPos, lineEnd, line);
				if (line == ".")
					break;
				if (!bFirstRow && line[0] != '#')
				{
					LPCSTR token = line.c_str();
					if (strncmp(token, "DYEAR", 5) == 0)
					{
						CDInfo.year = atoi(&token[6]);
					}
					else if (strncmp(token, "DGENRE", 6) == 0)
					{
						CDInfo.genre = CA2CT(&token[7]);
					}
					else if (strncmp(token, "DTITLE", 6) == 0)
					{
						CDInfo.title = CA2CT(&token[7]);
					}
					else if (strncmp(token, "TTITLE", 6) == 0)
					{
						LPCSTR eq = strchr(token, '=');
						if (eq)
						{
							CHAR num[10];
							strncpy(num, &token[6], eq - &token[6]);
							num[eq - &token[6]] = 0;
							INT trackNo = atoi(&token[6]);
							if (trackNo < (INT)CDInfo.tracks.size())
								CDInfo.tracks[trackNo].title = CA2CT(eq + 1);
							else
								ASSERT(0);
						}
					}
				}
				bFirstRow = FALSE;
			}
			while (startPos != std::string::npos);
		}
		break;
	case 212://Found InExact match
	default:
		TRACE(_T("@1 FreeDB::Read Error: '%s'\r\n"), CA2CT(page.c_str()));
		return FALSE;
		break;
	}
	return TRUE;

}

BOOL FreeDB::ReadDisc(LPCTSTR drive, AudioCDInfo& cdInfo)
{
	//===Open the specified "cdaudio" MCI device
	MCI_OPEN_PARMS mciOpenParms;
	mciOpenParms.lpstrDeviceType = _T("cdaudio");
	mciOpenParms.lpstrElementName = drive;
	mciOpenParms.wDeviceID = 0;
	MCIERROR err = ::mciSendCommand(0, MCI_OPEN, MCI_OPEN_SHAREABLE | MCI_OPEN_TYPE | (drive ? MCI_OPEN_ELEMENT : 0), (DWORD_PTR) &mciOpenParms);
	if (err == 0)
	{
		BOOL bSuccess = TRUE;
		//Set Time Format---------------
		MCI_SET_PARMS mciSetParms;
		mciSetParms.dwTimeFormat = MCI_FORMAT_MSF;
		err = ::mciSendCommand(mciOpenParms.wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR) &mciSetParms);
		if (err)
		{
			bSuccess = FALSE;
			TRACE(_T("@1 FreeDB::ReadDisc. Failed to set time format\r\n"));
		}
		INT numTracks = 0;
		//Get the total track count-----------------------
		if (bSuccess)
		{
			MCI_STATUS_PARMS mciStatusParms;
			mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
			err = ::mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR) &mciStatusParms);
			if (err)
			{
				bSuccess = FALSE;
				TRACE(_T("@1 FreeDB::ReadDisc. Failed to get the track count\n"));
			}
			else
				numTracks = (DWORD)mciStatusParms.dwReturn;
		}

		//check that disk contain at least one audio track-----------------------
		if (bSuccess)
		{
			MCI_STATUS_PARMS mciStatusParms;
			BOOL bAudioTrackPresent = FALSE;
			for (INT i=1; (i <= numTracks) && (!bAudioTrackPresent); i++)
			{                      
				mciStatusParms.dwItem = MCI_CDA_STATUS_TYPE_TRACK;
				mciStatusParms.dwTrack = i;
				err = mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR) &mciStatusParms);
				if (err)
				{
					bSuccess = FALSE;
					TRACE(_T("@1 AudioCDInfo::ReadDisc. Failed to get a track type\r\n"));
					break;
				}
				if (mciStatusParms.dwReturn == MCI_CDA_TRACK_AUDIO)
					bAudioTrackPresent = TRUE;
			}
			if (!bAudioTrackPresent)  
			{
				bSuccess = FALSE;
				TRACE(_T("@1 AudioCDInfo::ReadDisc. The CD does not contain any audio tracks!\r\n"));
			} 
		}
		//Iterate through all the tracks getting their starting position and length
		if (bSuccess)
		{
			int n = 0, t = 0;
			MCI_STATUS_PARMS mciStatusParms;
			for (INT i=1; i <= numTracks; i++)
			{
				mciStatusParms.dwItem = MCI_STATUS_POSITION;
				mciStatusParms.dwTrack = i;
				err = mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR) &mciStatusParms);
				if (err)
				{
					bSuccess = FALSE;
					TRACE(_T("@1 AudioCDInfo::ReadDisc. Failed to get track %d's starting position\r\n"), i);
					break;
				}
				AudioCDTrackInfo track;
				track.offset.m_min = MCI_MSF_MINUTE(mciStatusParms.dwReturn);
				track.offset.m_sec = MCI_MSF_SECOND(mciStatusParms.dwReturn);
				track.offset.m_frame = MCI_MSF_FRAME(mciStatusParms.dwReturn);
				mciStatusParms.dwItem = MCI_STATUS_LENGTH;
				err = mciSendCommand(mciOpenParms.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR) &mciStatusParms);
				if (err)
				{
					bSuccess = FALSE;
					TRACE(_T("@1 AudioCDInfo::ReadDisc. Failed to Get the track's length\r\n"));
					break;
				}
				track.length.m_min = MCI_MSF_MINUTE(mciStatusParms.dwReturn);
				track.length.m_sec = MCI_MSF_SECOND(mciStatusParms.dwReturn);
				track.length.m_frame = MCI_MSF_FRAME(mciStatusParms.dwReturn);

				cdInfo.tracks.push_back(track);
			}
		}
		::mciSendCommand(mciOpenParms.wDeviceID, MCI_CLOSE, 0, 0);
		if (bSuccess)
		{
			UINT totalSum = 0;
			for (UINT i = 0; i < cdInfo.tracks.size(); i++)
			{
				int trackSec = cdInfo.tracks[i].offset.m_min * 60 + cdInfo.tracks[i].offset.m_sec;
				int sum = 0;
				while (trackSec > 0)
				{
					sum += trackSec % 10;
					trackSec /= 10;
				}
				totalSum += sum;
			}
			AudioCDTrackInfo& track = cdInfo.tracks[cdInfo.tracks.size() - 1];
			UINT totalLengthInFrames = (track.offset + track.length).toFrame() + 1;
			INT frames = totalLengthInFrames % 75;
			totalLengthInFrames /= 75;
			INT sec = totalLengthInFrames % 60;
			totalLengthInFrames /= 60;
			INT min = totalLengthInFrames;
			AudioCDTrackInfo& firstTrack = cdInfo.tracks[0];
			cdInfo.totalLength = min * 60 + sec - firstTrack.offset.m_min * 60 - firstTrack.offset.m_sec;
			cdInfo.discID = ((totalSum % 0xFF) << 24 | cdInfo.totalLength << 8 | cdInfo.tracks.size());
		}
		return bSuccess && cdInfo.discID != 0;
	}
	TRACE(_T("@1@E FreeDB::ReadDisc, Failed to open cdaudio."));
	return FALSE;
}

BOOL FreeDB::GetFirstPart(std::tstring& buffer, LPCTSTR title)
{
	ASSERT(title != NULL);
	LPCTSTR divider = _tcschr(title, '/');
	if (divider != NULL)
	{
		UINT cpyLen = divider - title;
		buffer.clear();
		buffer.insert(0, title, cpyLen);
		return TRUE;
	}
	return FALSE;
}
BOOL FreeDB::GetSecondPart(std::tstring& buffer, LPCTSTR title)
{
	ASSERT(title != NULL);
	LPCTSTR divider = _tcschr(title, '/');
	if (divider != NULL)
	{
		divider++;
		buffer = divider;
		return TRUE;
	}
	return FALSE;
}



