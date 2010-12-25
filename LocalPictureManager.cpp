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
#include "LocalPictureManager.h"
#include "PrgAPI.h"
#include "SQLManager.h"

#include "InformationProviders/CachedImageInfoProvider/CachedImageInfoProvider.h"
#include "InformationProviders/DirImageInfoProvider/DirImageInfoProvider.h"
//#include "InformationProviders/CompositeInfoProvider/CompositeInfoProvider.h"
//#include "InformationProviders/InfoProviderRequestHelper.h"
#include "InformationProviders/InfoProviderFactory.h"
#include "InfoDownloadManager.h"
#include "resource.h"


#define MAX_FAILED_ARTISTS_REQUESTS_QUEUE 15
#define MAX_FAILED_ALBUMS_REQUESTS_QUEUE 15

LocalPictureManager::LocalPictureManager():
m_pCIIP(NULL),
m_pDIIP(NULL),
m_bEnableFolderImages(TRUE),
m_pThumbnailCache(NULL)
{
	for (INT i = IIT_Unknown; i < IIT_Last; i++)
		m_defDrawer[i] = &m_defGlobal;
	m_defDrawer[IIT_AlbumPicture] = &m_defAlbum;
}

LocalPictureManager::~LocalPictureManager()
{
	delete m_pThumbnailCache;
	//delete m_pCIIP;
	//delete m_pDIIP;
}

BOOL LocalPictureManager::Init(LPCTSTR storagePath)
{	
	if (m_pCIIP == NULL)
	{
		TRACEST(_T("LocalPictureManager::Init"));
		PrgAPI* pAPI = PRGAPI();
		InfoProviderFactory* pIPF = pAPI->GetInfoProviderFactory();
		ASSERT(storagePath != NULL);
		m_pCIIP = new CachedImageInfoProvider;
		pIPF->RegisterInfoProvider(m_pCIIP, FALSE);
		m_pDIIP = new DirImageInfoProvider;
		m_pDIIP->SetSQLManager(pAPI->GetSQLManager());
		pIPF->RegisterInfoProvider(m_pDIIP, FALSE);



		m_defGlobal.LoadResourceID(IDR_PNG_ARTIST, _T("png"));
		m_defGlobal.SetBkColor(RGB(0,0,0), 0);
		m_defGlobal.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
		m_defAlbum.LoadResourceID(IDR_ALBUM, _T("jpg"));
		m_defAlbum.SetBkColor(RGB(0,0,0), 0);
		m_defAlbum.GetDrawParams().zoomLockMode = GdiPlusPicDrawer::ZLM_FillArea;
	}
	m_pCIIP->SetStoragePath(storagePath);

	m_pThumbnailCache = new GdiPlusBitmapCache(64, 64, 80);
	//m_pThumbnailCache = new GdiPlusBitmapCache(96, 96, 50);
	//m_pThumbnailCache = new GdiPlusBitmapCache(128, 128, 100);


	return TRUE;
}

static const LPCTSTR sThumbsDB = _T("thumbs.db");

void LocalPictureManager::LoadState(LPCTSTR stateRoot)
{
	TRACEST(_T("LocalPictureManager::LoadState"));
	ASSERT(m_pThumbnailCache != NULL);//=== You must call Init first
	if (m_pThumbnailCache != NULL)
	{
		std::tstring thumbsDB(stateRoot);
		thumbsDB += sThumbsDB;
		m_pThumbnailCache->LoadState(thumbsDB.c_str());
	}
}

void LocalPictureManager::SaveState(LPCTSTR stateRoot)
{
	TRACEST(_T("LocalPictureManager::SaveState"));
	ASSERT(m_pThumbnailCache != NULL);//=== You must call Init first
	if (m_pThumbnailCache != NULL)
	{
		std::tstring thumbsDB(stateRoot);
		thumbsDB += sThumbsDB;
		m_pThumbnailCache->SaveState(thumbsDB.c_str());
	}
}

void LocalPictureManager::RemoveArtistPicture(const ArtistRecord& rec, LPCTSTR imagePath)
{
	ResetArtistCache(rec);
	DeleteFile(imagePath);
	//=== No need to check if this is the default picture. GetMain...Picture will detect it
}
void LocalPictureManager::RemoveAlbumPicture(const FullAlbumRecord& rec, LPCTSTR imagePath)
{
	ResetAlbumCache(rec);
	DeleteFile(imagePath);
	//=== No need to check if this is the default picture. GetMain...Picture will detect it
}



BOOL LocalPictureManager::AddArtistPicture(const ArtistRecord& rec, LPCTSTR imagePath)
{
	TRACEST(_T("LocalPictureManager::AddArtistPicture"));
	ASSERT(rec.IsValid() && imagePath != NULL);
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL || !rec.IsValid() || imagePath == NULL)	return FALSE;

	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = rec.name.c_str();

	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		res.main = imagePath;
		res.additionalInfo = _T("");
		res.service = IInfoProvider::SRV_ArtistImage;
		if (m_pCIIP->AddResult(res))
		{
			if (GetMainArtistPicture(rec) == NULL)
				ResetArtistCache(rec);
			return TRUE;
		}
	}

	return FALSE;


}
BOOL LocalPictureManager::AddAlbumPicture(const FullAlbumRecord& rec, LPCTSTR imagePath)
{
	TRACEST(_T("LocalPictureManager::AddAlbumPicture"));
	ASSERT(rec.IsValid() && imagePath != NULL);
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL || !rec.IsValid() || imagePath == NULL)	return FALSE;

	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = rec.artist.name.c_str();
	req.album = rec.album.name.c_str();

	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		res.main = imagePath;
		res.additionalInfo = _T("");
		res.service = IInfoProvider::SRV_AlbumImage;
		if (m_pCIIP->AddResult(res))
		{
			if (GetMainAlbumPicture(rec) == NULL)
				ResetAlbumCache(rec);
			return TRUE;
		}
	}
	return FALSE;
}

void LocalPictureManager::SetMainArtistPicture(const ArtistRecord& rec, LPCTSTR path)
{
	ResetArtistCache(rec);
	PicRecord pr(IIT_ArtistPicture, rec.ID);
	pr.path = path;
	pr.pictureType = PT_Main;
	PRGAPI()->GetSQLManager()->AddOrUpdatePicRecord(pr);
}

void LocalPictureManager::SetMainAlbumPicture(const FullAlbumRecord& rec, LPCTSTR path)
{
	ResetAlbumCache(rec);
	PicRecord pr(IIT_AlbumPicture, rec.album.ID);
	pr.path = path;
	pr.pictureType = PT_Main;
	PRGAPI()->GetSQLManager()->AddOrUpdatePicRecord(pr);
}


BOOL LocalPictureManager::GetArtistPictures(const ArtistRecord& rec, std::tstringvector& col)
{
	//TRACEST(_T("LocalPictureManager::GetArtistPictures"));
	ASSERT(rec.IsValid());
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL || !rec.IsValid())	return FALSE;
	if (rec.name.empty())					return FALSE;
	if (rec.name[0] == '[')					return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = rec.name.c_str();
	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		INT i = 0;
		while (m_pCIIP->GetNextResult(res))
		{
			col.push_back(std::tstring(res.main));
			i++;
		}
		return i > 0;
	}
	return FALSE;
}
BOOL LocalPictureManager::GetAlbumPictures(const FullAlbumRecord& rec, std::tstringvector& col)
{
	//TRACEST(_T("LocalPictureManager::GetAlbumPictures"));
	ASSERT(rec.IsValid());
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL || !rec.IsValid())	return FALSE;
	if (rec.album.name.empty())				return FALSE;
	if (rec.album.name[0] == '[')			return FALSE;
	if (rec.artist.name.empty())			return FALSE;
	if (rec.artist.name[0] == '[')			return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = rec.artist.name.c_str();
	req.album = rec.album.name.c_str();
	IInfoProvider::Result res;
	INT images = 0;
	if (m_bEnableFolderImages && m_pDIIP != NULL)
	{
		if (m_pDIIP->OpenRequest(req))
		{
			while (m_pDIIP->GetNextResult(res))
			{
				col.push_back(std::tstring(res.main));
				images++;
			}
		}

	}
	if (TRUE)
	{
		if (m_pCIIP->OpenRequest(req))
		{
			while (m_pCIIP->GetNextResult(res))
			{
				col.push_back(std::tstring(res.main));
				images++;
			}
		}
	}
	return images > 0;
}

//void LocalPictureManager::RequestArtistPicDownload(const FullArtistRecordSP& rec)
//{
//	while (m_failedArtistRequests.size() >= MAX_FAILED_ARTISTS_REQUESTS_QUEUE)
//		m_failedArtistRequests.pop_front();
//	m_failedArtistRequests.push_back(rec);
//}
//void LocalPictureManager::RequestAlbumPicDownload(const FullAlbumRecordSP& rec)
//{
//	while (m_failedAlbumRequests.size() >= MAX_FAILED_ALBUMS_REQUESTS_QUEUE)
//		m_failedAlbumRequests.pop_front();
//	m_failedAlbumRequests.push_back(rec);
//}
//
//void LocalPictureManager::ProcessFailedRequests()
//{
//	InfoDownloadManager* pIDM = PRGAPI()->GetInfoDownloadManager();
//	if (pIDM->GetPendingJobs() == 0)
//	{
//		if (m_failedArtistRequests.size() > 0)
//		{
//			pIDM->RequestArtistPic(m_failedArtistRequests.back()->artist, NULL);
//			m_failedArtistRequests.pop_back();
//		}
//		if (m_failedAlbumRequests.size() > 0)
//		{
//			pIDM->RequestAlbumPic(m_failedAlbumRequests.back()->album, m_failedAlbumRequests.back()->artist.name.c_str(), NULL);
//			m_failedAlbumRequests.pop_back();
//		}
//	}
//}

LPCTSTR LocalPictureManager::GetFirstArtistPicture(const ArtistRecord& rec)
{
	ASSERT(rec.IsValid());
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL || !rec.IsValid())	return FALSE;
	if (rec.name.empty())					return FALSE;
	if (rec.name[0] == '[')					return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = rec.name.c_str();
	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		if (m_pCIIP->GetNextResult(res))
			return res.main;//res.main is just a pointer to the internal m_pCIIP object which is not destroyed
	}
	return NULL;
}
LPCTSTR LocalPictureManager::GetFirstAlbumPicture(const FullAlbumRecord& rec)
{
	if (m_pCIIP == NULL || !rec.IsValid())	return FALSE;
	if (rec.album.name.empty())				return FALSE;
	if (rec.album.name[0] == '[')			return FALSE;
	if (rec.artist.name.empty())			return FALSE;
	if (rec.artist.name[0] == '[')			return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = rec.artist.name.c_str();
	req.album = rec.album.name.c_str();

	if (m_bEnableFolderImages && m_pDIIP != NULL)
	{
		if (m_pDIIP->OpenRequest(req))
		{
			IInfoProvider::Result res;
			if (m_pDIIP->GetNextResult(res))
				return res.main;//res.main is just a pointer to the internal m_pDIIP object which is not destroyed
		}
	}
	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		if (m_pCIIP->GetNextResult(res))
			return res.main;//res.main is just a pointer to the internal m_pCIIP object which is not destroyed
	}
	return NULL;
}

LPCTSTR LocalPictureManager::GetMainArtistPicture(const ArtistRecord& rec)
{
	ASSERT(rec.IsValid());
	ASSERT(rec.IsValid());
	if (!rec.IsValid())	
		return FALSE;
	if (rec.name[0] == '[')
		return NULL;
	CacheContainer::iterator it = m_artists.find(rec.ID);
	if (it == m_artists.end())
	{
		//TRACEST(_T("@4 LocalPictureManager::GetMainArtistPicture (Uncached)"));
		//=== This is the first request. We will try to find one
		//=== First Try the ArtistsPic Table
		SQLManager* pSM = PRGAPI()->GetSQLManager();
		PicRecord apr(IIT_ArtistPicture, rec.ID);
		if (pSM->GetPicRecord(apr))
		{
			//=== A db entry already exists. Lets check if the file exists.
			if (_taccess(apr.path.c_str(), 4) == 0)
			{
				//=== Exists. All OK. Add to cache and return the path
				m_artists[rec.ID] = apr.path;
				//TRACE(_T("@3 LocalPictureManager::GetMainArtistPicture. Found In DB\r\n"));
				return m_artists[rec.ID].c_str();
			}
			else
			{
				//=== Does NOT Exist. Maybe it is deleted from the FS. Adjust the database				apr.path.clear();
				TRACE(_T("@1 LocalPictureManager::GetMainArtistPicture. Found In DB but not in FS\r\n"));
				pSM->DeletePicRecord(apr.infoType, rec.ID);
			}
		}
		//=== The Picture does NOT exist in the database (OR it is does not exist in the FS anymore)
		//=== Get The Picture with the normal way
		LPCTSTR pic = GetFirstArtistPicture(rec);
		if (pic != NULL)
		{
			//TRACE(_T("@3 LocalPictureManager::GetMainArtistPicture. Found In Folder\r\n"));
			//=== We have a picture. Let's cache it to the database.
			SetMainArtistPicture(rec, pic);
			//=== Let's cache it here and return the record
			m_artists[rec.ID] = pic;
			return m_artists[rec.ID].c_str();
		}
		//=== No Pictures found
		//TRACE(_T("@3 LocalPictureManager::GetMainArtistPicture. Picture Not Found\r\n"));
		m_artists[rec.ID] = _T("");
		return NULL;
	}
	else if (!(*it).second.empty())
	{
		if (_taccess((*it).second.c_str(), 4) == 0)
			return (*it).second.c_str();//Return Cached answer
		TRACE(_T("@1 LocalPictureManager::GetMainArtistPicture. A previously cached Picture has ot been found\r\n"));
		m_artists.erase(it);
	}
	return NULL;
}
LPCTSTR LocalPictureManager::GetMainAlbumPicture(const FullAlbumRecord& rec)
{
	//TRACEST(_T("LocalPictureManager::GetMainAlbumPicture"));
	ASSERT(rec.IsValid());
	if (!rec.IsValid())	
		return FALSE;
	if (rec.album.name[0] == '[' || rec.artist.name[0] == '[' )
		return NULL;
	CacheContainer::iterator it = m_albums.find(rec.album.ID);
	if (it == m_albums.end())
	{
		//TRACEST(_T("LocalPictureManager::GetMainAlbumPicture (Uncached)"));
		//=== This is the first request. We will try to find one
		//=== First Try the AlbumsPic Table
		SQLManager* pSM = PRGAPI()->GetSQLManager();
		PicRecord apr(IIT_AlbumPicture, rec.album.ID);
		if (pSM->GetPicRecord(apr))
		{
			//=== A db entry already exists. Lets check if the file exists.
			if (_taccess(apr.path.c_str(), 4) == 0)
			{
				//=== Exists. All OK. Add to cache and return the path
				m_albums[rec.album.ID] = apr.path;
				return m_albums[rec.album.ID].c_str();
			}
			else
			{
				//=== Does NOT Exist. Maybe it is deleted from the FS. Adjust the database				apr.path.clear();
				pSM->DeletePicRecord(apr.infoType, rec.album.ID);
			}
		}
		//=== The Picture does NOT exist in the database (OR it is does not exist in the FS anymore)
		//=== Get The Picture with the normal way
		LPCTSTR pic = GetFirstAlbumPicture(rec);
		if (pic != NULL)
		{
			//=== We have a picture. Let's cache it to the database.
			apr.path = pic;
			apr.pictureType = PT_Main;
			pSM->AddOrUpdatePicRecord(apr);
			//=== Let's cache it here and return the record
			m_albums[rec.album.ID] = pic;
			return m_albums[rec.album.ID].c_str();
		}
		//=== No Pictures found
		m_albums[rec.album.ID] = _T("");
		return NULL;
	}
	else if (!(*it).second.empty())
	{
		if (_taccess((*it).second.c_str(), 4) == 0)
			return (*it).second.c_str();//Return Cached answer
		TRACE(_T("@1 LocalPictureManager::GetMainAlbumPicture. A previously cached Picture has not been found\r\n"));
		m_albums.erase(it);
	}
	return NULL;

}

void LocalPictureManager::ResetArtistCache(const ArtistRecord& rec)
{
	//=== Reset Session Cache
	TRACEST(_T("LocalPictureManager::ClearCachedArtistPicture"));
	ASSERT(rec.IsValid());
	if (rec.IsValid())	
	{
		CacheContainer::iterator it = m_artists.find(rec.ID);
		if (it != m_artists.end())
			m_artists.erase(it);
	}

	//=== Reset DB Cache
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	pSM->DeletePicRecord(IIT_ArtistPicture, rec.ID);
}
void LocalPictureManager::ResetAlbumCache(const FullAlbumRecord& rec)
{
	//=== Reset Session Cache
	TRACEST(_T("LocalPictureManager::ClearCachedAlbumPicture"));
	ASSERT(rec.IsValid());
	if (rec.IsValid())
	{
		CacheContainer::iterator it = m_albums.find(rec.album.ID);
		if (it != m_albums.end())
			m_albums.erase(it);
	}
	//=== Reset DB Cache
	SQLManager* pSM = PRGAPI()->GetSQLManager();
	pSM->DeletePicRecord(IIT_AlbumPicture, rec.album.ID);

}

DWORD LocalPictureManager::CalculateCacheUID(InfoItemTypeEnum iit, UINT itemID)
{
	return (itemID & 0xFFFFFF) | ((iit & 0xff) << 24);//=== 1 byte iit + 3bytes for the ID
	//return MAKELONG(iit, itemID);//=== 2 bytes iit + 2bytes ID
}

BOOL LocalPictureManager::DrawDefaultThumbnail(InfoItemTypeEnum iit, HDC hdc, RECT& rcDest)
{
	return DrawDefaultThumbnail(iit, Gdiplus::Graphics(hdc), Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top));
}

BOOL LocalPictureManager::DrawDefaultThumbnail(InfoItemTypeEnum iit, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest)
{
	return m_defDrawer[iit]->Draw(g, rcDest);
}

BOOL LocalPictureManager::DrawThumbnail(InfoItemTypeEnum iit, UINT itemID, HDC hdc, RECT& rcDest)
{
	return DrawThumbnail(iit, itemID, Gdiplus::Graphics(hdc), Gdiplus::Rect(rcDest.left, rcDest.top, rcDest.right - rcDest.left, rcDest.bottom - rcDest.top));
}


BOOL LocalPictureManager::DrawThumbnail(InfoItemTypeEnum iit, UINT itemID, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest)
{
	DWORD uid = CalculateCacheUID(iit, itemID);
	PrgAPI* pAPI = PRGAPI();
	InfoDownloadManager* pDM = pAPI->GetInfoDownloadManager();
	BOOL bRet = FALSE;
	if (itemID > 0)
	{
		switch (iit)
		{
		case IIT_ArtistPicture:
			bRet = m_pThumbnailCache->Draw(uid, g, rcDest);
			if (bRet == FALSE)
			{
				ArtistRecord rec;
				if (pAPI->GetSQLManager()->GetArtistRecord(itemID, rec))
				{
					LPCTSTR picPath = GetMainArtistPicture(rec);
					if (picPath != NULL)
					{
						INT nItem = m_pThumbnailCache->SetByPath(uid, picPath);
						if (nItem != -1)
							bRet = m_pThumbnailCache->DrawItem(nItem, g, rcDest);
						else
							TRACE(_T("@1 LocalPictureManager::DrawThumbnail Failed at '%s'\r\n"), picPath);
					}
					else
					{
						//=== We don't have the picture.
						//=== Ask for a download
						pDM->RequestArtistPic(rec);
					}
				}
			}
			break;
		case IIT_AlbumPicture:
			bRet = m_pThumbnailCache->Draw(uid, g, rcDest);
			if (bRet == FALSE)
			{
				FullAlbumRecordSP rec;
				if (pAPI->GetSQLManager()->GetFullAlbumRecordByID(rec, itemID))
				{
					LPCTSTR picPath = GetMainAlbumPicture(*rec);
					if (picPath != NULL)
					{
						INT nItem = m_pThumbnailCache->SetByPath(uid, picPath);
						if (nItem != -1)
							bRet = m_pThumbnailCache->Draw(uid, g, rcDest);
						else
							TRACE(_T("@1 LocalPictureManager::DrawThumbnail Failed at '%s'\r\n"), picPath);
					}
					else
					{
						//=== We don't have the picture. Ask for a download
						pDM->RequestAlbumPic(rec->album, rec->artist.name.c_str());
					}
				}
			}
			break;
		default:
			break;
			//bRet = FALSE;
		}
	}
	else
	{
		//Should be various
		INT debug = 0;
	}
	return bRet;
}

