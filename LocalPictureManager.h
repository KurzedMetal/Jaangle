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
#ifndef _LocalPictureManager_h_
#define _LocalPictureManager_h_

class IInfoProvider;
class DirImageInfoProvider;
//class CachedImageInfoProvider;

#include "DataRecords.h"
#include "GuiControls/GdiPlusBitmapCache.h"
#include "GuiControls/GdiPlusPicDrawer.h"
#include "InformationProviders/CachedImageInfoProvider/CachedImageInfoProvider.h"
#include "CRC32.h"
#include "UINTtoStringMapping.h"





class LocalPictureManager
{
public:
	LocalPictureManager();
	~LocalPictureManager();

public:
	BOOL Init(LPCTSTR StoragePath);

	BOOL AddArtistPicture(LPCTSTR artist, LPCTSTR imagePath);
	BOOL GetArtistPictures(LPCTSTR artist, std::tstringvector& col);
	BOOL GetArtistThumbnail(LPCTSTR artist, INT width, INT height, LPTSTR bf, UINT bfLen);
	LPCTSTR GetMainArtistPicture(LPCTSTR artist, BOOL bUseAnyPictureIfNotAvailable);
	void RemoveArtistPicture(LPCTSTR artist, LPCTSTR imagePath);
	//void ResetArtistCache(LPCTSTR artist);
	void SetMainArtistPicture(LPCTSTR artist, LPCTSTR path);


	BOOL AddAlbumPicture(LPCTSTR artist, LPCTSTR album, LPCTSTR imagePath);
	BOOL GetAlbumPictures(LPCTSTR artist, LPCTSTR album, std::tstringvector& col);
	BOOL GetAlbumThumbnail(LPCTSTR artist, LPCTSTR album, INT width, INT height, LPTSTR bf, UINT bfLen);
	LPCTSTR GetMainAlbumPicture(LPCTSTR artist, LPCTSTR album, BOOL bUseAnyPictureIfNotAvailable);
	void RemoveAlbumPicture(LPCTSTR artist, LPCTSTR album, LPCTSTR imagePath);
	//void ResetAlbumCache(LPCTSTR artist, LPCTSTR album);
	void SetMainAlbumPicture(LPCTSTR artist, LPCTSTR album, LPCTSTR path);

	//=== Returns TRUE if a vaild picture have been found and correctly drawn
	BOOL DrawArtistThumbnail(LPCTSTR artist, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest);
	BOOL DrawAlbumThumbnail(LPCTSTR artist, LPCTSTR album, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest);
	BOOL DrawDefaultThumbnail(InfoItemTypeEnum iit, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest);


	void EnableFolderImages(BOOL bEnable)		{m_bEnableFolderImages = bEnable;}
	BOOL IsFolderImagesEnabled()				{return m_bEnableFolderImages;}

	//void SetDefaultThumbnail(InfoItemTypeEnum iit, Gdiplus::Image* pImage);
	//Gdiplus::Image* GetDefaultThumbnail(InfoItemTypeEnum iit);

	//void ProcessFailedRequests();

private:
	LPCTSTR GetFirstArtistPicture(LPCTSTR artist);
	LPCTSTR GetFirstAlbumPicture(LPCTSTR artist, LPCTSTR album);
	DWORD CalculateCacheUID(InfoItemTypeEnum iit, UINT itemID);
	void DeleteArtistThumbnails(LPCTSTR artist);
	void DeleteAlbumThumbnails(LPCTSTR artist, LPCTSTR album);
	UINT CRC32forArtist(LPCTSTR artist);
	UINT CRC32forAlbum(LPCTSTR artist, LPCTSTR album);
	DWORD CalcCRC32(LPCTSTR text);
	//void RequestArtistPicDownload(const FullArtistRecordSP& rec);
	//void RequestAlbumPicDownload(const FullAlbumRecordSP& rec);
private:
	BOOL m_bEnableFolderImages;

	//GdiPlusBitmapCache* m_pThumbnailCache; 



	CachedImageInfoProvider* m_pCIIP;
	DirImageInfoProvider* m_pDIIP;
	//typedef std::map<UINT, std::tstring> CacheContainer;
	//CacheContainer m_artists;
	//CacheContainer m_albums;

	GdiPlusPicDrawer m_defGlobal;
	GdiPlusPicDrawer m_defAlbum;
	GdiPlusPicDrawer* m_defDrawer[IIT_Last];

	CachedImageInfoProvider m_thumbnails;


	UINTtoStringMapping m_mainPictures;
	std::basic_string<TCHAR> m_mainPicturesPath;



	CRC32 m_crc32;
	GdiPlusPicDrawer m_picDrawer;


	//std::list<FullArtistRecordSP> m_failedArtistRequests;
	//std::list<FullAlbumRecordSP> m_failedAlbumRequests;
};

#endif