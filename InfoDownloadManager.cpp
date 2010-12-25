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
#include "InfoDownloadManager.h"
#include "PrgAPI.h"

#include "InformationProviders/InfoProviderFactory.h"
#include "InformationProviders/InfoProviderJob.h"

#include "ActionManager.h"
#include "SQLManager.h"
#include "LocalPictureManager.h"

#include "resource.h"
#include "InfoQueryDlg.h"

#include "TagInfo.h"
#include "ReEntryLock.h"

#include "TSOnlineServices.h"

#include "SystemTimeFunctions.h"

#include "StateManager.h"

#define TABLES_NAME_MAX_LEN 100
 
InfoDownloadManager::InfoDownloadManager(InfoProviderFactory& ipf):
m_pMTQueue(NULL),
m_IPFactory(ipf),
m_bAcceptAll(FALSE),
m_bCancelAll(FALSE),
m_bReEntranceProtection(FALSE),
m_bOldInfoRequestsRead(FALSE)
{
}

InfoDownloadManager::~InfoDownloadManager()
{
	TRACEST(_T("InfoDownloadManager::~InfoDownloadManager"));
	if (m_pMTQueue != NULL)
		delete m_pMTQueue;
}
BOOL InfoDownloadManager::RequestArtistInfo(ArtistRecord& rec, LPCTSTR providerName, BOOL bForce, BOOL bSync)
{
	//TRACEST(_T("InfoDownloadManager::RequestArtistInfo"));
	ASSERT(rec.IsValid());
	if (!rec.IsValid())
		return FALSE;
	if (rec.name.empty())						return FALSE;
	if (rec.name[0] == '[')						return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistBio);
	req.artist = rec.name.c_str();
	return RequestInfo(req, rec.ID, providerName, bForce, bSync, OPT_IM_AutoDlArtistBio, IIT_ArtistBio);
}
BOOL InfoDownloadManager::RequestArtistPic(ArtistRecord& rec, LPCTSTR providerName, BOOL bForce, BOOL bSync)
{
	//TRACEST(_T("InfoDownloadManager::RequestArtistPic"));
	ASSERT(rec.IsValid());
	if (!rec.IsValid())
		return FALSE;
	if (rec.name.empty())						return FALSE;
	if (rec.name[0] == '[')						return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = rec.name.c_str();
	return RequestInfo(req, rec.ID, providerName, bForce, bSync, OPT_IM_AutoDlArtistPicture, IIT_ArtistPicture);
}
BOOL InfoDownloadManager::RequestAlbumInfo(AlbumRecord& rec, LPCTSTR artist, LPCTSTR providerName, BOOL bForce, BOOL bSync)
{
	//TRACEST(_T("InfoDownloadManager::RequestAlbumInfo"));
	ASSERT(artist != NULL);
	ASSERT(rec.IsValid());
	if (!rec.IsValid())							return FALSE;
	if (rec.name.empty())						return FALSE;
	if (rec.name[0] == '[')						return FALSE;
	if (artist == NULL)							return FALSE;
	if (artist[0] == '[')						return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumReview);
	req.artist = artist;
	req.album = rec.name.c_str();
	return RequestInfo(req, rec.ID, providerName, bForce, bSync, OPT_IM_AutoDlAlbumReview, IIT_AlbumReview);
}
BOOL InfoDownloadManager::RequestAlbumPic(AlbumRecord& rec, LPCTSTR artist, LPCTSTR providerName, BOOL bForce, BOOL bSync)
{
	//TRACEST(_T("InfoDownloadManager::RequestAlbumPic"));
	ASSERT(rec.IsValid());
	if (!rec.IsValid())							return FALSE;
	if (rec.name.empty())						return FALSE;
	if (rec.name[0] == '[')						return FALSE;
	if (artist == NULL)							return FALSE;
	if (artist[0] == '[')						return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = artist;
	req.album = rec.name.c_str();
	return RequestInfo(req, rec.ID, providerName, bForce, bSync, OPT_IM_AutoDlAlbumPicture, IIT_AlbumPicture);
}
BOOL InfoDownloadManager::RequestLyrics(TrackRecord& rec, LPCTSTR artist, LPCTSTR providerName, BOOL bForce, BOOL bSync)
{
	//TRACEST(_T("InfoDownloadManager::RequestLyrics"));
	ASSERT(artist != NULL);
	ASSERT(rec.IsValid());
	if (!rec.IsValid())							return FALSE;

	if (rec.name.empty())						return FALSE;
	if (rec.name[0] == '[')						return FALSE;
	if (artist == NULL)							return FALSE;
	if (artist[0] == '[')						return FALSE;

	m_bCancelAll = FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_TrackLyrics);
	req.artist = artist;
	req.title = rec.name.c_str();
	return RequestInfo(req, rec.ID, providerName, bForce, bSync, OPT_Unknown, IIT_TrackLyrics);
}
BOOL InfoDownloadManager::RequestTablatures(TrackRecord& rec, LPCTSTR artist, LPCTSTR providerName, BOOL bForce, BOOL bSync)
{
	//TRACEST(_T("InfoDownloadManager::RequestTablatures"));
	ASSERT(artist != NULL);
	ASSERT(rec.IsValid());
	if (!rec.IsValid())							return FALSE;

	if (rec.name.empty())						return FALSE;
	if (rec.name[0] == '[')						return FALSE;
	if (artist == NULL)							return FALSE;
	if (artist[0] == '[')						return FALSE;

	IInfoProvider::Request req(IInfoProvider::SRV_TrackTablatures);
	req.artist = artist;
	req.title = rec.name.c_str();
	return RequestInfo(req, rec.ID, providerName, bForce, bSync, OPT_Unknown, IIT_TrackTablatures);
}

void InfoDownloadManager::ResetArtistCache(ArtistRecord& rec)
{
	TRACEST(_T("InfoDownloadManager::ResetArtistCache"));
	//=== Reset Session Cache
	IInfoProvider::Request request(IInfoProvider::SRV_ArtistBio);
	request.artist = rec.name.c_str();
	UINT hash = CalcRequestHash(request);
	std::set<UINT>::iterator it = m_RequestHashes.find(hash);
	if (it != m_RequestHashes.end())
		m_RequestHashes.erase(it);
	request.service = IInfoProvider::SRV_ArtistImage;
	hash = CalcRequestHash(request);
	it = m_RequestHashes.find(hash);
	if (it != m_RequestHashes.end())
		m_RequestHashes.erase(it);
	////=== Reset DB Cache
	//SQLManager* pSM = PRGAPI()->GetSQLManager();
	//pSM->DeleteInfoRecordResult(rec.ID, IIT_ArtistPicture);
	//pSM->DeleteInfoRecordResult(rec.ID, IIT_ArtistBio);
}
void InfoDownloadManager::ResetAlbumCache(AlbumRecord& rec, LPCTSTR artist)
{
	TRACEST(_T("InfoDownloadManager::ResetAlbumCache"));
	//=== Reset Session Cache
	IInfoProvider::Request request(IInfoProvider::SRV_AlbumReview);
	request.artist = artist;
	request.album = rec.name.c_str();
	UINT hash = CalcRequestHash(request);
	std::set<UINT>::iterator it = m_RequestHashes.find(hash);
	if (it != m_RequestHashes.end())
		m_RequestHashes.erase(it);
	request.service = IInfoProvider::SRV_AlbumImage;
	hash = CalcRequestHash(request);
	it = m_RequestHashes.find(hash);
	if (it != m_RequestHashes.end())
		m_RequestHashes.erase(it);
	//=== Reset DB Cache
	//SQLManager* pSM = PRGAPI()->GetSQLManager();
	//pSM->DeleteInfoRecordResult(rec.ID, IIT_AlbumPicture);
	//pSM->DeleteInfoRecordResult(rec.ID, IIT_AlbumReview);
}

void InfoDownloadManager::ResetTrackCache(TrackRecord& rec, LPCTSTR artist)
{
	TRACEST(_T("InfoDownloadManager::ResetTrackCache"));
	//=== Reset Session Cache
	IInfoProvider::Request request(IInfoProvider::SRV_TrackLyrics);
	request.artist = artist;
	request.title = rec.name.c_str();
	UINT hash = CalcRequestHash(request);
	std::set<UINT>::iterator it = m_RequestHashes.find(hash);
	if (it != m_RequestHashes.end())
		m_RequestHashes.erase(it);
	request.service = IInfoProvider::SRV_TrackTablatures;
	hash = CalcRequestHash(request);
	it = m_RequestHashes.find(hash);
	if (it != m_RequestHashes.end())
		m_RequestHashes.erase(it);
	//=== Reset DB Cache
	//SQLManager* pSM = PRGAPI()->GetSQLManager();
	//pSM->DeleteInfoRecordResult(rec.ID, IIT_TrackLyrics);
	//pSM->DeleteInfoRecordResult(rec.ID, IIT_TrackTablatures);

}



UINT InfoDownloadManager::CalcStringHash(LPCTSTR str)
{
	UINT hash = 0;
	if (str != NULL)
	{
		UINT paramsLen = _tcslen(str);
		for (UINT i = 0; i < paramsLen; i++)
			hash += str[i] * i;
	}
	return hash;
}

UINT InfoDownloadManager::CalcRequestHash(const IInfoProvider::Request& request)
{
	UINT hash = (UINT) request.service;
	hash += CalcStringHash(request.artist);
	hash += CalcStringHash(request.album);
	hash += CalcStringHash(request.title);
	return hash;
}

BOOL InfoDownloadManager::IsFirstRequest(const IInfoProvider::Request& request)
{
	ASSERT(request.IsValid());
	UINT hash = CalcRequestHash(request);
	//TRACE(_T("InfoDownloadManager::IsFirstRequestInThisSession Art: %s Hash: %d\r\n"), request.artist, hash);

	if (m_bOldInfoRequestsRead == FALSE)
	{
		TRACEST(_T("InfoDownloadManager::IsFirstRequest Getting old requests"));
		m_bOldInfoRequestsRead = TRUE;
		SQLManager* pSM = PRGAPI()->GetSQLManager();
		UINT reqLimit = GetCurrentUnixTimeStamp() - 10 * 24 * 3600;
		pSM->DeleteInfoRequestsOlderThan(reqLimit);
		pSM->GetInfoRequests(m_RequestHashes);
	}
	if (m_RequestHashes.find(hash) != m_RequestHashes.end())
		return FALSE;
	PRGAPI()->GetSQLManager()->AddInfoRequest(GetCurrentUnixTimeStamp(), hash);
	m_RequestHashes.insert(hash);
	return TRUE;
}


BOOL InfoDownloadManager::RequestInfo(IInfoProvider::Request& request, 
			UINT helpID, LPCTSTR providerName, BOOL bForce, BOOL bSync, 
			AppOptionsEnum optionToCheck, InfoItemTypeEnum iit)
{
	ASSERT(request.IsValid());
	if (!bForce)
	{
		//=== Check if the user has select not to download pictures/info
		PrgAPI* pAPI = PRGAPI();
		if (optionToCheck > 0 && !pAPI->GetOption(optionToCheck))
			return FALSE;	//No Need
		//=== Check if we have already tried this request in this session
		if (!IsFirstRequest(request))
		{
			//TRACE(_T("@3 InfoDownloadManager::RequestInfo. Already Tried this request in this session. '%s'\r\n"), pAPI->GetStringForInfoItemType(IIT_ArtistPicture));
			return FALSE;
		}
		//=== Check if we have previously tried this request and we have failed (Database)
		//if (!IsFirstRequestInTheLastSessions())
		//InfoRecordRequest irr(helpID, iit);
		//SQLManager* pSM = pAPI->GetSQLManager();
		//if (pSM->GetInfoRecordRequestRecord(irr))
		//{
		//	UINT curTime = GetCurrentUnixTimeStamp();
		//	if (curTime - irr.requestTimeStamp < 10 * 24 * 3600)
		//	{
		//		//TRACE(_T("@3 InfoDownloadManager::RequestInfo. Already Tried this request in the last 10 days. '%s'\r\n"), pAPI->GetStringForInfoItemType(IIT_ArtistPicture));
		//		return FALSE;//=== We have checked for such info less than 10 days before and it failed
		//	}
		//}
		//=== Result: Proceed to download.
	}
	TRACEST(_T("InfoDownloadManager::RequestInfo"), iit);

	//=== Clean Up the request
	static LPCTSTR const trimChars = _T(" ");
	std::tstring art;
	if (request.artist != NULL)
	{
		static LPCTSTR const cutChars = _T("()[]<>");
		//TRACE(_T("@2 InfoDownloadManager::RequestInfo. Filtering :'%s'\r\n"), request.artist);
		art = request.artist;
		//=== trim left
		art.erase(0, art.find_first_not_of(trimChars));
		//=== trim "the "
		if (_tcsnicmp(art.c_str(), _T("the "), 4) == 0)
			art.erase(0, 4);
		//=== Cut when find the first of the cut chars
		size_t pos = art.find_first_of(cutChars);
		if (pos != std::tstring::npos)
			art.erase(pos);
		//=== trim right
		art.erase(art.find_last_not_of(trimChars) + 1);
		//=== Filter Out the request if less than 3 chars
		if (art.size() < 3)
		{
			TRACE(_T("@2 InfoDownloadManager::RequestInfo. Artist Filtered Out '%s'\r\n"), request.artist);
			return FALSE;
		}
#ifdef _DEBUG
		if (_tcslen(request.artist) != art.size())
			TRACE(_T("@3 InfoDownloadManager::RequestInfo. Artist Filtered TO :'%s'\r\n"), art.c_str());
#endif
		request.artist = art.c_str();
	}
	std::tstring alb;
	if (request.album != NULL)
	{
		static LPCTSTR const cutChars = _T("()[]-<>");
		//TRACE(_T("@2 InfoDownloadManager::RequestInfo. Filtering :'%s'\r\n"), request.album);
		alb = request.album;
		//=== trim left
		alb.erase(0, alb.find_first_not_of(trimChars));
		//=== Cut when find the first of the cut chars
		size_t pos = alb.find_first_of(cutChars);
		if (pos != std::tstring::npos)
			alb.erase(pos);
		//=== trim right
		alb.erase(alb.find_last_not_of(trimChars) + 1);
		//=== Filter Out the request if less than 3 chars
		if (alb.size() < 3)
		{
			TRACE(_T("@2 InfoDownloadManager::RequestInfo. Album Filtered Out '%s'\r\n"), request.album);
			return FALSE;
		}
#ifdef _DEBUG
		if (_tcslen(request.album) != alb.size())
			TRACE(_T("@3 InfoDownloadManager::RequestInfo. Album Filtered TO :'%s'\r\n"), alb.c_str());
#endif
		request.album = alb.c_str();
	}
	std::tstring tit;
	if (request.title != NULL)
	{
		static LPCTSTR const cutChars = _T("()[]-<>");
		//TRACE(_T("@2 InfoDownloadManager::RequestInfo. Filtering :'%s'\r\n"), request.title);
		tit = request.title;
		//=== trim left
		tit.erase(0, tit.find_first_not_of(trimChars));
		//=== Cut when find the first of the cut chars
		size_t pos = tit.find_first_of(cutChars);
		if (pos != std::tstring::npos)
			tit.erase(pos);
		//=== trim right
		tit.erase(tit.find_last_not_of(trimChars) + 1);
		//=== Filter Out the request if less than 3 chars
		if (tit.size() < 3)
		{
			TRACE(_T("@2 InfoDownloadManager::RequestInfo. Title Filtered Out '%s'\r\n"), request.title);
			return FALSE;
		}
#ifdef _DEBUG
		if (_tcslen(request.title) != tit.size())
			TRACE(_T("@3 InfoDownloadManager::RequestInfo. Title Filtered TO :'%s'\r\n"), tit.c_str());
#endif
		request.title = tit.c_str();
	}


	if (providerName == NULL)
	{
		switch (request.service)
		{
		case IInfoProvider::SRV_ArtistBio:
		case IInfoProvider::SRV_ArtistImage:
			providerName = _T("LAFM");
			break;
		case IInfoProvider::SRV_AlbumReview:
		case IInfoProvider::SRV_AlbumImage:
			providerName = _T("AMZN");
			break;
		case IInfoProvider::SRV_TrackLyrics:
		case IInfoProvider::SRV_TrackTablatures:
			providerName = _T("GOLY");
			break;
		default:
			TRACE(_T("@1 InfoDownloadManager::RequestInfo. No Default provider for %d\r\n"), request.service);
			return FALSE;
		}
	}

	IInfoProvider* pIP = NULL;
	if (providerName != NULL)
		pIP = m_IPFactory.CreateInfoProviderByUniqueID(providerName);
	if (pIP == NULL)
		pIP = m_IPFactory.CreateInfoProviderByService(request.service);
	if (pIP != NULL)
	{
		if (pIP->OpenRequest(request))
		{
			JobSP job(InfoProviderJob::CreateInfoProviderJob(pIP, helpID));
			if (job.get())
			{
				if (bSync)
					job->Execute();
				else
				{
					if (m_pMTQueue == NULL)
					{
						m_pMTQueue = new MultiThreadJobQueue;
						m_pMTQueue->SetMaxThreads(3);
						m_pMTQueue->SetAssignJobDelay(1000);
					}
					return m_pMTQueue->AddJob(job);
				}
				return TRUE;
			}
		}
		delete pIP;
	}
	TRACE(_T("@2 InfoDownloadManager::RequestInfo. Cannot create this job.\r\n"));
	return FALSE;
}

void InfoDownloadManager::NotifyUserForSuccess(LPCTSTR infoType, LPCTSTR reqInfo, LPCTSTR addInfo)
{
	ASSERT(infoType != NULL && reqInfo != NULL && addInfo != NULL);
	if (infoType != NULL && reqInfo != NULL && addInfo != NULL)
	{
		PrgAPI* pAPI = PRGAPI();
		TCHAR msg[1000];
		_sntprintf(msg, 1000, 
			_T("%s | %s: %s | %s | %s: %s"),
			pAPI->GetString(IDS_SUCCESS),
			pAPI->GetString(IDS_TYPE), infoType,
			reqInfo, 
			pAPI->GetString(IDS_PROVIDER), addInfo);
		pAPI->NotifyUser(msg, 0, SEV_InfoL1);
	}
}


BOOL InfoDownloadManager::HandleArtistInfo(UINT artID, const IInfoProvider::Result& result)
{
	//TRACEST(_T("InfoDownloadManager::HandleArtistInfo"));
	ASSERT(artID != 0 && result.IsValid());
	IInfoProvider::Result res;
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	ArtistRecord ar;
	if (pSM->GetArtistRecord(artID, ar))
	{
		pSM->AdjustArtistInfo(artID, IIT_ArtistBio, result.main);
		pAPI->SendMessage(SM_InfoChanged);
		NotifyUserForSuccess(pAPI->GetString(IDS_ARTISTBIO), 
			ar.name.c_str(), 
			result.additionalInfo);
	}
	return FALSE;
}
BOOL InfoDownloadManager::HandleArtistPic(UINT artID, const IInfoProvider::Result& result)
{
	//TRACEST(_T("InfoDownloadManager::HandleArtistPic"));
	ASSERT(artID != 0 && result.IsValid());
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	LocalPictureManager* pLPM = pAPI->GetLocalPictureManager();
	ArtistRecord ar;
	if (pSM->GetArtistRecord(artID, ar))
	{
		if (!pLPM->AddArtistPicture(ar, result.main))
		{
			//It happens when the same picture exists
			return TRUE;//Request more results
		}
		PRGAPI()->SendMessage(SM_PictureChanged);
		NotifyUserForSuccess(pAPI->GetString(IDS_ARTISTPICTURE), 
			ar.name.c_str(), 
			result.additionalInfo);
	}
	return FALSE;//DO NOT REQUEST more results
}
BOOL InfoDownloadManager::HandleAlbumInfo(UINT albID, const IInfoProvider::Result& result)
{
	//TRACEST(_T("InfoDownloadManager::HandleAlbumInfo"));
	ASSERT(albID != 0 && result.IsValid());
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	FullAlbumRecordSP rec;
	if (pSM->GetFullAlbumRecordByID(rec, albID))
	{
		pSM->AdjustAlbumInfo(albID, IIT_AlbumReview, result.main);
		pAPI->SendMessage(SM_InfoChanged);
		TCHAR msg[500];
		_sntprintf(msg, 500, _T("%s: %s"), rec->artist.name.c_str(), rec->album.name.c_str());
		NotifyUserForSuccess(pAPI->GetString(IDS_ALBUMREVIEW), 
			msg, 
			result.additionalInfo);
	}
	return FALSE;
}
BOOL InfoDownloadManager::HandleAlbumPic(UINT albID, const IInfoProvider::Result& result)
{
	//TRACEST(_T("InfoDownloadManager::HandleAlbumPic"));
	ASSERT(albID != 0 && result.IsValid());
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	LocalPictureManager* pLPM = pAPI->GetLocalPictureManager();
	FullAlbumRecordSP rec;
	if (pSM->GetFullAlbumRecordByID(rec, albID))
	{
		if (!pLPM->AddAlbumPicture(*rec, result.main))
		{
			//It happens when the same picture exists
			return TRUE;//Request more results
		}
		pAPI->SendMessage(SM_PictureChanged);
		TCHAR msg[500];
		_sntprintf(msg, 500, _T("%s: %s"), rec->artist.name.c_str(), rec->album.name.c_str());
		NotifyUserForSuccess(pAPI->GetString(IDS_ALBUMPICTURE), 
			msg, 
			result.additionalInfo);
	}
	return FALSE;//DO NOT REQUEST more results
}
BOOL InfoDownloadManager::HandleTrackInfo(UINT traID, const IInfoProvider::Result& result, InfoItemTypeEnum iit)
{
	//TRACEST(_T("InfoDownloadManager::HandleTrackInfo"));
	ASSERT(traID != 0 && result.IsValid());
	if (m_bCancelAll)
		return FALSE;
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	FullTrackRecordSP tr;
	if (pSM->GetFullTrackRecordByID(tr, traID))
	{
		BOOL bRefuse = FALSE;
		std::tstring info = result.main;
		if (!m_bAcceptAll)
		{
			ActionManager* pAM = pAPI->GetActionManager();
			CInfoQueryDlg dlgQuery;//(AfxGetMainWnd());
			TCHAR msg[1000];
			_sntprintf(msg, 1000, 
				_T("%s: %s - %s\r\n%s: %s"),
				pAPI->GetString(IDS_TITLE), tr->track.name.c_str(), tr->artist.name.c_str(),
				pAPI->GetString(IDS_PROVIDER), result.additionalInfo);
			msg[999] = 0;
			int resID = IDS_LYRICS;
			switch (iit)
			{
			case IIT_TrackLyrics:
				resID = IDS_LYRICS;
				break;
			case IIT_TrackTablatures:
				resID = IDS_TABLATURES;
				dlgQuery.EnableFixedFont(TRUE);
				break;
			default:
				ASSERT(0);
				return FALSE;
				break;
			}
			//Clears the context menu
			//It does not work 100%.
			//	WORKS: if the user has focused on tree/list and have a context
			//	DONT WORK if the user have clicked the 'TS' button and the menu appears 
			HWND hwndFous = ::GetFocus();
			//TRACEHWND(_T("Focused Window"), hwndFous);
			SendNotifyMessage(hwndFous, WM_CANCELMODE, 0, 0);

			dlgQuery.SetExtraInfo(pAPI->GetString(resID), msg, result.additionalInfo);
			dlgQuery.SetMainText(result.main);

			switch (dlgQuery.DoModal())
			{
			case CInfoQueryDlg::RES_AcceptAll:
				m_bAcceptAll = TRUE;
				info = dlgQuery.GetMainText();
				break;
			case CInfoQueryDlg::RES_Accept:
				info = dlgQuery.GetMainText();
				break;
			case CInfoQueryDlg::RES_Retry:
				return TRUE;
			case CInfoQueryDlg::RES_Cancel:
				bRefuse = TRUE;
				break;
			case CInfoQueryDlg::RES_CancelAll:
				m_pMTQueue->AbortJobs();
				m_bCancelAll = TRUE;
				bRefuse = TRUE;
				break;
			}

		}
		if (!bRefuse)
		{
			pSM->AdjustTrackInfo(traID, iit, info.c_str());
			if (iit == IIT_TrackLyrics)
			{
				TagInfo ti;
				ti.Lyrics = info;
				ti.validFields = TagInfo_Lyrics;
				pAPI->GetActionManager()->TagTrack(tr->track.location.c_str(), ti);
				if (pAPI->GetOption(OPT_GEN_UseNativeOnlineServices))
				{
					TSOnlineServices* pTS = pAPI->GetTSOnlineServices();
					TSOnlineServices::TrackInfo ti;
					ti.artist = tr->artist.name.c_str();
					ti.title = tr->track.name.c_str();
					ti.album = tr->album.name.c_str();
					ti.tag = tr->genre.name.c_str();
					ti.rating = tr->track.rating;
					ti.lyrics = info.c_str();
					pTS->SendTrackInfo(ti);
				}
			}
			pAPI->SendMessage(SM_InfoChanged);
			TCHAR msg[500];
			_sntprintf(msg, 500, _T("%s: %s"), tr->artist.name.c_str(), tr->track.name.c_str());
			NotifyUserForSuccess(pAPI->GetStringForInfoItemType(iit), 
				msg, 
				result.additionalInfo);
		}
	}
	return FALSE;
}


void InfoDownloadManager::ConsumeResult()
{
	if (m_pMTQueue == NULL)
		return;
	ReEntryLock lock(m_bReEntranceProtection);
	if (lock.WasAlreadyLocked())
	{
		TRACE(_T("@2InfoDownloadManager::ConsumeResult. Re-entrance protection\r\n"));
		return;
	}
	PrgAPI* pAPI = PRGAPI();
	SQLManager* pSM = pAPI->GetSQLManager();
	JobSP job;
	while (m_pMTQueue->RetrieveFinishedJob(job))
	{
		InfoProviderJob* pInfoJob = dynamic_cast<InfoProviderJob*>(job.get());
		ASSERT(pInfoJob != NULL);
		if (pInfoJob == NULL)
			continue;
		IInfoProvider::Result result;
		IInfoProvider::Request request;
		UINT itemID = pInfoJob->GetItemID();
		pInfoJob->GetLastRequest(request);
		BOOL bSuccess = pInfoJob->GetLastResult(result);
		if (bSuccess)
		{
			TRACEST(_T("InfoDownloadManager::ConsumeResult. Success (p=srv)"), result.service);
			InfoItemTypeEnum iit;
			BOOL bContinueGettingResults = FALSE;
			switch (result.service)
			{
			case IInfoProvider::SRV_ArtistBio:
				iit = IIT_ArtistBio;
				bContinueGettingResults = HandleArtistInfo(pInfoJob->GetItemID(), result);
				break;
			case IInfoProvider::SRV_ArtistImage:
				iit = IIT_ArtistPicture;
				bContinueGettingResults = HandleArtistPic(pInfoJob->GetItemID(), result);
				break;
			case IInfoProvider::SRV_AlbumReview:
				iit = IIT_AlbumReview;
				bContinueGettingResults = HandleAlbumInfo(pInfoJob->GetItemID(), result);
				break;
			case IInfoProvider::SRV_AlbumImage:
				iit = IIT_AlbumPicture;
				bContinueGettingResults = HandleAlbumPic(pInfoJob->GetItemID(), result);
				break;
			case IInfoProvider::SRV_TrackLyrics:
				iit = IIT_TrackLyrics;
				bContinueGettingResults = HandleTrackInfo(pInfoJob->GetItemID(), result, iit);
				break;
			case IInfoProvider::SRV_TrackTablatures:
				iit = IIT_TrackTablatures;
				bContinueGettingResults = HandleTrackInfo(pInfoJob->GetItemID(), result, iit);
				break;
			default:
				ASSERT(0);//None handle it
			}
			if (bContinueGettingResults)
				m_pMTQueue->AddJob(job);
			//else
			//	pSM->DeleteInfoRecordResult(pInfoJob->GetItemID(), iit);
		}
		else
		{
			TRACEST(_T("InfoDownloadManager::ConsumeResult. Failure (p=srv)"), result.service);
			InfoItemTypeEnum iit = IIT_Unknown;
			TCHAR msg[500];
			switch (request.service)
			{
			case IInfoProvider::SRV_ArtistBio:
				iit = IIT_ArtistBio;
				_tcscpy(msg, request.artist);
				break;
			case IInfoProvider::SRV_ArtistImage:
				{
					iit = IIT_ArtistPicture;
					_tcscpy(msg, request.artist);
					//Add a Download "FAILURE" record in the database
				}
				break;
			case IInfoProvider::SRV_AlbumReview:
				iit = IIT_AlbumReview;
				_sntprintf(msg, 500, _T("%s - %s"), 
					request.artist, 
					request.album);
				break;
			case IInfoProvider::SRV_AlbumImage:
				{
					iit = IIT_AlbumPicture;
					_sntprintf(msg, 500, _T("%s - %s"), 
						request.artist, 
						request.album);

				}
				break;
			case IInfoProvider::SRV_TrackLyrics:
				iit = IIT_TrackLyrics;
				_sntprintf(msg, 500, _T("%s - %s"), 
					request.artist, 
					request.title);
				break;
			case IInfoProvider::SRV_TrackTablatures:
				iit = IIT_TrackTablatures;
				_sntprintf(msg, 500, _T("%s - %s"), 
					request.artist, 
					request.title);
				break;
			}
			ASSERT(iit != IIT_Unknown);
			if (iit != IIT_Unknown)
			{
				////===Add A Failure Record
				//InfoRecordRequest irr(itemID, iit);
				//irr.result = IRRR_GeneralFailure;
				//irr.requestTimeStamp = GetCurrentUnixTimeStamp();
				//pSM->AddOrUpdateInfoRecordRequestRecord(irr);

				TCHAR bf[1000];
				_sntprintf(bf, 1000, _T("%s | %s: %s | %s"),	
					pAPI->GetString(IDS_FAILURE), 
					pAPI->GetString(IDS_TYPE), pAPI->GetStringForInfoItemType(iit),
					msg);
				pAPI->NotifyUser(bf, 0, SEV_Warning);
			}
		}
	}
}

BOOL InfoDownloadManager::IsWorking()		
{
	if (m_pMTQueue != NULL)
	{
		MultiThreadJobQueue::JobStatisics stats = m_pMTQueue->GetStatistics();
		return (stats.waiting + stats.working) > 0;
	}
	return FALSE;
}

INT InfoDownloadManager::GetPendingJobs()
{
	if (m_pMTQueue != NULL)
	{
		MultiThreadJobQueue::JobStatisics stats = m_pMTQueue->GetStatistics();
		return stats.waiting + stats.working;
	}
	return 0;

}

