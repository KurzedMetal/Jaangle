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
#ifndef _InfoDownloadManager_h_
#define _InfoDownloadManager_h_

#include "InformationProviders/InfoProvider.h"
#include "DataRecords.h"
#include <set>	
#include "PrgAPI.h"
class MultiThreadJobQueue;
class InfoProviderFactory;

class InfoDownloadManager
{
public:
	InfoDownloadManager(InfoProviderFactory& ipf);
	~InfoDownloadManager();

public:
	BOOL RequestArtistInfo(ArtistRecord& rec, LPCTSTR providerName = NULL, BOOL bForce = FALSE, BOOL bSync = FALSE);
	BOOL RequestArtistPic(ArtistRecord& rec, LPCTSTR providerName = NULL, BOOL bForce = FALSE, BOOL bSync = FALSE);
	BOOL RequestAlbumInfo(AlbumRecord& rec, LPCTSTR artist, LPCTSTR providerName = NULL, BOOL bForce = FALSE, BOOL bSync = FALSE);
	BOOL RequestAlbumPic(AlbumRecord& rec, LPCTSTR artist, LPCTSTR providerName = NULL, BOOL bForce = FALSE, BOOL bSync = FALSE);
	BOOL RequestLyrics(TrackRecord& rec, LPCTSTR artist, LPCTSTR providerName = NULL, BOOL bForce = FALSE, BOOL bSync = FALSE);
	BOOL RequestTablatures(TrackRecord& rec, LPCTSTR artist, LPCTSTR providerName = NULL, BOOL bForce = FALSE, BOOL bSync = FALSE);


	void ResetArtistCache(ArtistRecord& rec);
	void ResetAlbumCache(AlbumRecord& rec, LPCTSTR artist);
	void ResetTrackCache(TrackRecord& rec, LPCTSTR artist);


	BOOL IsWorking();
	INT GetPendingJobs();

	void ConsumeResult();

private:
	//Create On Demand (RequestInfo - CountProviders. This function may need to start dlls
	//and may become time consuming.
	//void CreateInfoProviderContainer();
	//void RequestInfo(InfoItemTypeEnum itemType, LPCTSTR providerName, LPCTSTR params, BOOL bForce, BOOL bSync);
	BOOL IsFirstRequest(const IInfoProvider::Request& request);


	BOOL RequestInfo(IInfoProvider::Request& request, 
		UINT helpID, LPCTSTR providerName, BOOL bForce, BOOL bSync, 
		AppOptionsEnum optionToCheck, InfoItemTypeEnum iit);
	BOOL HandleArtistInfo(UINT artID, const IInfoProvider::Result& result);
	BOOL HandleArtistPic(UINT artID, const IInfoProvider::Result& result);
	BOOL HandleAlbumInfo(UINT albID, const IInfoProvider::Result& result);
	BOOL HandleAlbumPic(UINT albID, const IInfoProvider::Result& result);
	BOOL HandleTrackInfo(UINT traID, const IInfoProvider::Result& result, InfoItemTypeEnum iit);

	void NotifyUserForSuccess(LPCTSTR infoType, LPCTSTR reqInfo, LPCTSTR addInfo);
	static UINT CalcStringHash(LPCTSTR str);
	static UINT CalcRequestHash(const IInfoProvider::Request& request);


private:
	MultiThreadJobQueue* m_pMTQueue;
	InfoProviderFactory& m_IPFactory;
	BOOL m_bAcceptAll;
	BOOL m_bCancelAll;
	BOOL m_bReEntranceProtection;
	std::set<UINT> m_RequestHashes;//Used by IsFirstRequest
	BOOL m_bOldInfoRequestsRead;


};

#endif
