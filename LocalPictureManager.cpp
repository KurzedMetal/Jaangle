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
m_bEnableFolderImages(TRUE)/*,
m_pThumbnailCache(NULL)*/
{
	for (INT i = IIT_Unknown; i < IIT_Last; i++)
		m_defDrawer[i] = &m_defGlobal;
	m_defDrawer[IIT_AlbumPicture] = &m_defAlbum;
}

LocalPictureManager::~LocalPictureManager()
{
	if (m_mainPictures.IsDirty())
		m_mainPictures.Save(m_mainPicturesPath.c_str());
	//delete m_pThumbnailCache;
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

	//m_pThumbnailCache = new GdiPlusBitmapCache(64, 64, 80);
	//m_pThumbnailCache = new GdiPlusBitmapCache(96, 96, 50);
	//m_pThumbnailCache = new GdiPlusBitmapCache(128, 128, 100);

	TCHAR thumbnailStorage[MAX_PATH];
	_sntprintf(thumbnailStorage, MAX_PATH, _T("%sthumbnails\\"), storagePath);

	m_thumbnails.SetStoragePath(thumbnailStorage);
	m_thumbnails.SetThumbnailMode(TRUE);
	m_thumbnails.SetThumbnailSize(32,32);

	m_mainPicturesPath = storagePath;
	m_mainPicturesPath += _T("main.inf");
	m_mainPictures.Load(m_mainPicturesPath.c_str());

	return TRUE;
}

void LocalPictureManager::RemoveArtistPicture(LPCTSTR artist, LPCTSTR imagePath)
{
	DeleteFile(imagePath);
	DeleteArtistThumbnails(artist);
	//=== No need to check if this is the main picture. GetMain...Picture will detect it and it will erase it automatically
}
void LocalPictureManager::RemoveAlbumPicture(LPCTSTR artist, LPCTSTR album, LPCTSTR imagePath)
{
	DeleteFile(imagePath);
	DeleteAlbumThumbnails(artist, album);
	//=== No need to check if this is the main picture. GetMain...Picture will detect it and it will erase it automatically
}



BOOL LocalPictureManager::AddArtistPicture(LPCTSTR artist, LPCTSTR imagePath)
{
	TRACEST(_T("LocalPictureManager::AddArtistPicture"));
	ASSERT(artist != NULL && imagePath != NULL);
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL || imagePath == NULL)	return FALSE;

	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = artist;

	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		res.main = imagePath;
		res.additionalInfo = _T("");
		res.service = IInfoProvider::SRV_ArtistImage;
		if (m_pCIIP->AddResult(res))
		{
			//if (GetMainArtistPicture(artist) == NULL)
			//	ResetArtistCache(artist);
			return TRUE;
		}
	}

	return FALSE;


}
BOOL LocalPictureManager::AddAlbumPicture(LPCTSTR artist, LPCTSTR album, LPCTSTR imagePath)
{
	TRACEST(_T("LocalPictureManager::AddAlbumPicture"));
	ASSERT(artist != NULL && album != NULL && imagePath != NULL);
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL || artist==NULL || album==NULL || imagePath == NULL)	return FALSE;

	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = artist;
	req.album = album;

	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		res.main = imagePath;
		res.additionalInfo = _T("");
		res.service = IInfoProvider::SRV_AlbumImage;
		if (m_pCIIP->AddResult(res))
		{
			//if (GetMainAlbumPicture(artist, album) == NULL)
			//	ResetAlbumCache(artist, album);
			return TRUE;
		}
	}
	return FALSE;
}

UINT LocalPictureManager::CRC32forArtist(LPCTSTR artist)
{
	TCHAR bf[MAX_PATH];
	_sntprintf(bf, MAX_PATH, _T("AR_%s"), artist);
	bf[MAX_PATH - 1] = 0;
	return CalcCRC32(bf);
}
UINT LocalPictureManager::CRC32forAlbum(LPCTSTR artist, LPCTSTR album)
{
	TCHAR bf[MAX_PATH];
	_sntprintf(bf, MAX_PATH, _T("AL_%s_%s"), artist, album);
	bf[MAX_PATH - 1] = 0;
	return CalcCRC32(bf);
}

DWORD LocalPictureManager::CalcCRC32(LPCTSTR text)
{
	return m_crc32.CalcCRC((LPVOID)text, _tcslen(text) * sizeof(TCHAR));
}


void LocalPictureManager::SetMainArtistPicture(LPCTSTR artist, LPCTSTR path)
{
	//ResetArtistCache(artist);
	m_mainPictures.Set(CRC32forArtist(artist), path);
	DeleteArtistThumbnails(artist);
}

void LocalPictureManager::SetMainAlbumPicture(LPCTSTR artist, LPCTSTR album, LPCTSTR path)
{
	//ResetAlbumCache(artist, album);
	m_mainPictures.Set(CRC32forAlbum(artist, album), path);
	DeleteAlbumThumbnails(artist, album);
}


BOOL LocalPictureManager::GetArtistPictures(LPCTSTR artist, std::tstringvector& col)
{
	//TRACEST(_T("LocalPictureManager::GetArtistPictures"));
	ASSERT(artist != NULL);
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL)					return FALSE;
	if (artist == NULL)						return FALSE;
	if (artist[0] == '[')					return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = artist;
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
BOOL LocalPictureManager::GetAlbumPictures(LPCTSTR artist, LPCTSTR album, std::tstringvector& col)
{
	//TRACEST(_T("LocalPictureManager::GetAlbumPictures"));
	ASSERT(artist != NULL && album != NULL);
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL)	return FALSE;
	if (artist == NULL)				return FALSE;
	if (artist[0] == '[')			return FALSE;
	if (album == NULL)				return FALSE;
	if (album[0] == '[')			return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = artist;
	req.album = album;
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

LPCTSTR LocalPictureManager::GetFirstArtistPicture(LPCTSTR artist)
{
	ASSERT(artist != NULL);
	ASSERT(m_pCIIP != NULL);
	if (m_pCIIP == NULL)				return FALSE;
	if (artist == NULL)					return FALSE;
	if (artist[0] == '[')				return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = artist;
	if (m_pCIIP->OpenRequest(req))
	{
		IInfoProvider::Result res;
		if (m_pCIIP->GetNextResult(res))
			return res.main;//res.main is just a pointer to the internal m_pCIIP object which is not destroyed
	}
	return NULL;
}
LPCTSTR LocalPictureManager::GetFirstAlbumPicture(LPCTSTR artist, LPCTSTR album)
{
	if (m_pCIIP == NULL)			return FALSE;
	if (artist == NULL)				return FALSE;
	if (artist[0] == '[')			return FALSE;
	if (album == NULL)				return FALSE;
	if (album[0] == '[')			return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = artist;
	req.album = album;

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

BOOL LocalPictureManager::GetArtistThumbnail(LPCTSTR artist, INT width, INT height, LPTSTR bf, UINT bfLen)
{
	ASSERT(artist != NULL);
	ASSERT(bf != NULL && bfLen >= MAX_PATH);
	ASSERT(m_pCIIP != NULL);
	ASSERT(artist != NULL);
	if (m_pCIIP == NULL)			return FALSE;
	if (artist[0] == '[')			return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = artist;
	m_thumbnails.SetThumbnailSize(width, height);
	if (m_thumbnails.OpenRequest(req))
	{
		IInfoProvider::Result res;
		if (m_thumbnails.GetNextResult(res))
		{
			_tcsncpy(bf, res.main, bfLen);
			return TRUE;
		}
		LPCTSTR imagePath = GetMainArtistPicture(artist, TRUE);
		if (imagePath != NULL)
		{
			IInfoProvider::Result res;
			res.main = imagePath;
			res.additionalInfo = _T("");
			res.service = IInfoProvider::SRV_ArtistImage;
			if (m_thumbnails.AddResult(res))
				return GetArtistThumbnail(artist, width, height, bf, bfLen);
		}
	}
	return FALSE;
}

void LocalPictureManager::DeleteArtistThumbnails(LPCTSTR artist)
{
	IInfoProvider::Request req(IInfoProvider::SRV_ArtistImage);
	req.artist = artist;
	if (m_thumbnails.OpenRequest(req))
	{
		IInfoProvider::Result res;
		if (m_thumbnails.GetNextResult(res))
		{
			TCHAR bf[MAX_PATH];
			_tcsncpy(bf, res.main, MAX_PATH);
			LPTSTR undPos = _tcsrchr(bf, _T('_'));
			if (undPos != NULL)
			{
				undPos[1] = 0;
				_tcscat(bf, _T("*.*"));
				CFileFind finder;
				BOOL bWorking = finder.FindFile(bf);
				while (bWorking)
				{
					bWorking = finder.FindNextFile();
					DeleteFile(finder.GetFilePath());
				}
			}
		}
	}
}
void LocalPictureManager::DeleteAlbumThumbnails(LPCTSTR artist, LPCTSTR album)
{
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = artist;
	req.album = album;
	if (m_thumbnails.OpenRequest(req))
	{
		IInfoProvider::Result res;
		if (m_thumbnails.GetNextResult(res))
		{
			TCHAR bf[MAX_PATH];
			_tcsncpy(bf, res.main, MAX_PATH);
			LPTSTR undPos = _tcsrchr(bf, _T('_'));
			if (undPos != NULL)
			{
				undPos[1] = 0;
				_tcscat(bf, _T("*.*"));
				CFileFind finder;
				BOOL bWorking = finder.FindFile(bf);
				while (bWorking)
				{
					bWorking = finder.FindNextFile();
					DeleteFile(finder.GetFilePath());
				}
			}
		}
	}
}





BOOL LocalPictureManager::GetAlbumThumbnail(LPCTSTR artist, LPCTSTR album, INT width, INT height, LPTSTR bf, UINT bfLen)
{
	if (artist == NULL || album == NULL)		return FALSE;
	if (artist[0] == '[')						return FALSE;
	if (album[0] == '[')						return FALSE;
	IInfoProvider::Request req(IInfoProvider::SRV_AlbumImage);
	req.artist = artist;
	req.album = album;
	m_thumbnails.SetThumbnailSize(width, height);
	if (m_thumbnails.OpenRequest(req))
	{
		IInfoProvider::Result res;
		if (m_thumbnails.GetNextResult(res))
		{
			_tcsncpy(bf, res.main, bfLen);
			return TRUE;
		}
		LPCTSTR imagePath = GetMainAlbumPicture(artist, album, TRUE);
		if (imagePath != NULL)
		{
			IInfoProvider::Result res;
			res.main = imagePath;
			res.additionalInfo = _T("");
			res.service = IInfoProvider::SRV_AlbumImage;
			if (m_thumbnails.AddResult(res))
				return GetAlbumThumbnail(artist, album, width, height, bf, bfLen);
		}

	}
	return FALSE;
}



LPCTSTR LocalPictureManager::GetMainArtistPicture(LPCTSTR artist, BOOL bUseAnyPictureIfNotAvailable)
{
	ASSERT(artist != NULL);
	if (artist == NULL)					return NULL;
	if (artist[0] == '[')				return NULL;

	UINT crc32 = CRC32forArtist(artist);
	LPCTSTR path = m_mainPictures.Get(crc32);
	if (path != NULL)
	{
		if (PathFileExists(path))
			return path;
		m_mainPictures.Remove(crc32);
	}
	if (bUseAnyPictureIfNotAvailable)
		return GetFirstArtistPicture(artist);
	return NULL;
}


LPCTSTR LocalPictureManager::GetMainAlbumPicture(LPCTSTR artist, LPCTSTR album, BOOL bUseAnyPictureIfNotAvailable)
{
	ASSERT(artist != NULL && album != NULL);
	if (artist == NULL || album == NULL)					return NULL;
	if (artist[0] == '[' || album[0] == '[')				return NULL;

	UINT crc32 = CRC32forAlbum(artist, album);
	LPCTSTR path = m_mainPictures.Get(crc32);
	if (path != NULL)
	{
		if (PathFileExists(path))
			return path;
		m_mainPictures.Remove(crc32);
	}
	if (bUseAnyPictureIfNotAvailable)
		return GetFirstAlbumPicture(artist, album);
	return NULL;
}

//void LocalPictureManager::ResetArtistCache(LPCTSTR artist)
//{
//	ASSERT(0);
//	//=== Reset Session Cache
//	//TRACEST(_T("LocalPictureManager::ClearCachedArtistPicture"));
//	//ASSERT(rec.IsValid());
//	//if (rec.IsValid())	
//	//{
//	//	CacheContainer::iterator it = m_artists.find(rec.ID);
//	//	if (it != m_artists.end())
//	//		m_artists.erase(it);
//	//}
//
//	////=== Reset DB Cache
//	//SQLManager* pSM = PRGAPI()->GetSQLManager();
//	//pSM->DeletePicRecord(IIT_ArtistPicture, rec.ID);
//}
//void LocalPictureManager::ResetAlbumCache(LPCTSTR artist, LPCTSTR album)
//{
//	ASSERT(0);
//
//	////=== Reset Session Cache
//	//TRACEST(_T("LocalPictureManager::ClearCachedAlbumPicture"));
//	//ASSERT(rec.IsValid());
//	//if (rec.IsValid())
//	//{
//	//	CacheContainer::iterator it = m_albums.find(rec.album.ID);
//	//	if (it != m_albums.end())
//	//		m_albums.erase(it);
//	//}
//	////=== Reset DB Cache
//	//SQLManager* pSM = PRGAPI()->GetSQLManager();
//	//pSM->DeletePicRecord(IIT_AlbumPicture, rec.album.ID);
//
//}

BOOL LocalPictureManager::DrawDefaultThumbnail(InfoItemTypeEnum iit, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest)
{
	return m_defDrawer[iit]->Draw(g, rcDest);
}

BOOL LocalPictureManager::DrawArtistThumbnail(LPCTSTR artist, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest)
{
	TCHAR picPath[MAX_PATH];
	if (GetArtistThumbnail(artist, rcDest.Width, rcDest.Height, picPath, MAX_PATH))
	{
		if (m_picDrawer.LoadFile(picPath))
			return m_picDrawer.Draw(g, rcDest);

		//INT nItem = m_pThumbnailCache->SetByPath(CRC32forArtist(artist), picPath);
		//if (nItem != -1)
		//	bRet = m_pThumbnailCache->DrawItem(nItem, g, rcDest);
		//else
		//	TRACE(_T("@1 LocalPictureManager::DrawArtistThumbnail Failed at '%s'\r\n"), picPath);
	}
	else
	{
		//=== We don't have the picture.
		//=== Ask for a download
		PRGAPI()->GetInfoDownloadManager()->RequestArtistPic(artist);
	}
	return FALSE;
}


BOOL LocalPictureManager::DrawAlbumThumbnail(LPCTSTR artist, LPCTSTR album, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest)
{
	TCHAR picPath[MAX_PATH];
	if (GetAlbumThumbnail(artist, album, rcDest.Width, rcDest.Height, picPath, MAX_PATH))
	{
		if (m_picDrawer.LoadFile(picPath))
			return m_picDrawer.Draw(g, rcDest);
		//INT nItem = m_pThumbnailCache->SetByPath(CRC32forAlbum(artist, album), picPath);
		//if (nItem != -1)
		//	bRet = m_pThumbnailCache->DrawItem(nItem, g, rcDest);
		//else
		//	TRACE(_T("@1 LocalPictureManager::DrawAlbumThumbnail Failed at '%s'\r\n"), picPath);
	}
	else
	{
		//=== We don't have the picture.
		//=== Ask for a download
		PRGAPI()->GetInfoDownloadManager()->RequestAlbumPic(artist, album);
	}
	return FALSE;
}

//BOOL LocalPictureManager::DrawThumbnail(InfoItemTypeEnum iit, UINT itemID, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest)
//{
//	DWORD uid = CalculateCacheUID(iit, itemID);
//	PrgAPI* pAPI = PRGAPI();
//	InfoDownloadManager* pDM = pAPI->GetInfoDownloadManager();
//	BOOL bRet = FALSE;
//	if (itemID > 0)
//	{
//		switch (iit)
//		{
//		case IIT_ArtistPicture:
//			{
//				DWORD tick1, tick2, tick3, tick4, tick5, tick6;
//				tick1 = GetTickCount();
//				bRet = m_pThumbnailCache->Draw(uid, g, rcDest);
//				tick2 = GetTickCount();
//				
//				if (bRet == FALSE)
//				{
//					ArtistRecord rec;
//					if (pAPI->GetSQLManager()->GetArtistRecord(itemID, rec))
//					{
//						tick3 = GetTickCount();
//						TCHAR picPath[MAX_PATH];
//						if (GetArtistThumbnail(rec, rcDest.Width, rcDest.Height, picPath, MAX_PATH))
//						{
//							tick4 = GetTickCount();
//
//							INT nItem = m_pThumbnailCache->SetByPath(uid, picPath);
//							tick5 = GetTickCount();
//
//							if (nItem != -1)
//								bRet = m_pThumbnailCache->DrawItem(nItem, g, rcDest);
//							else
//								TRACE(_T("@1 LocalPictureManager::DrawThumbnail Failed at '%s'\r\n"), picPath);
//							tick6 = GetTickCount();
//
//							TCHAR bf[1000];
//							_sntprintf(bf, 1000, _T("LocalPictureManager::DrawThumbnail timings...\r\n")
//								_T("  m_pThumbnailCache->Draw: %d\r\n" )
//								_T("  GetSQLManager()->GetArtistRecord: %d\r\n" )
//								_T("  GetArtistThumbnail: %d\r\n" )
//								_T("  m_pThumbnailCache->SetByPath: %d\r\n" )
//								_T("  m_pThumbnailCache->DrawItem: %d\r\n" ),
//								tick2 - tick1,
//								tick3 - tick2,
//								tick4 - tick3,
//								tick5 - tick4,
//								tick6 - tick5);
//							TRACE(bf);
//						}
//						else
//						{
//							//=== We don't have the picture.
//							//=== Ask for a download
//							pDM->RequestArtistPic(rec);
//						}
//					}
//
//			}
//			}
//			break;
//		case IIT_AlbumPicture:
//			bRet = m_pThumbnailCache->Draw(uid, g, rcDest);
//			if (bRet == FALSE)
//			{
//				FullAlbumRecordSP rec;
//				if (pAPI->GetSQLManager()->GetFullAlbumRecordByID(rec, itemID))
//				{
//					LPCTSTR picPath = GetMainAlbumPicture(*rec);
//					if (picPath != NULL)
//					{
//						INT nItem = m_pThumbnailCache->SetByPath(uid, picPath);
//						if (nItem != -1)
//							bRet = m_pThumbnailCache->Draw(uid, g, rcDest);
//						else
//							TRACE(_T("@1 LocalPictureManager::DrawThumbnail Failed at '%s'\r\n"), picPath);
//					}
//					else
//					{
//						//=== We don't have the picture. Ask for a download
//						pDM->RequestAlbumPic(rec->album, rec->artist.name.c_str());
//					}
//				}
//			}
//			break;
//		default:
//			break;
//			//bRet = FALSE;
//		}
//	}
//	else
//	{
//		//Should be various
//		INT debug = 0;
//	}
//	return bRet;
//}

