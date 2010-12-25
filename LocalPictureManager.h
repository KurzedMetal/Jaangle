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
class CachedImageInfoProvider;

#include "DataRecords.h"
#include "GuiControls/GdiPlusBitmapCache.h"
#include "GuiControls/GdiPlusPicDrawer.h"

class LocalPictureManager
{
public:
	LocalPictureManager();
	~LocalPictureManager();

public:
	BOOL Init(LPCTSTR StoragePath);

	void LoadState(LPCTSTR stateRoot);
	void SaveState(LPCTSTR stateRoot);

	BOOL AddArtistPicture(const ArtistRecord& rec, LPCTSTR imagePath);
	BOOL GetArtistPictures(const ArtistRecord& rec, std::tstringvector& col);
	LPCTSTR GetMainArtistPicture(const ArtistRecord& rec);
	void RemoveArtistPicture(const ArtistRecord& rec, LPCTSTR imagePath);
	void ResetArtistCache(const ArtistRecord& rec);
	void SetMainArtistPicture(const ArtistRecord& rec, LPCTSTR path);


	BOOL AddAlbumPicture(const FullAlbumRecord& rec, LPCTSTR imagePath);
	BOOL GetAlbumPictures(const FullAlbumRecord& rec, std::tstringvector& col);
	LPCTSTR GetMainAlbumPicture(const FullAlbumRecord& rec);
	void RemoveAlbumPicture(const FullAlbumRecord& rec, LPCTSTR imagePath);
	void ResetAlbumCache(const FullAlbumRecord& rec);
	void SetMainAlbumPicture(const FullAlbumRecord& rec, LPCTSTR path);

	//=== Returns TRUE if a vaild picture have been found and correctly drawn
	BOOL DrawThumbnail(InfoItemTypeEnum iit, UINT itemID, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest);
	BOOL DrawThumbnail(InfoItemTypeEnum iit, UINT itemID, HDC hdc, RECT& rcDest);

	BOOL DrawDefaultThumbnail(InfoItemTypeEnum iit, Gdiplus::Graphics& g, Gdiplus::Rect& rcDest);
	BOOL DrawDefaultThumbnail(InfoItemTypeEnum iit, HDC hdc, RECT& rcDest);


	void EnableFolderImages(BOOL bEnable)		{m_bEnableFolderImages = bEnable;}
	BOOL IsFolderImagesEnabled()				{return m_bEnableFolderImages;}

	void SetDefaultThumbnail(InfoItemTypeEnum iit, Gdiplus::Image* pImage);
	Gdiplus::Image* GetDefaultThumbnail(InfoItemTypeEnum iit);

	//void ProcessFailedRequests();

private:
	LPCTSTR GetFirstArtistPicture(const ArtistRecord& rec);
	LPCTSTR GetFirstAlbumPicture(const FullAlbumRecord& rec);
	DWORD CalculateCacheUID(InfoItemTypeEnum iit, UINT itemID);
	//void RequestArtistPicDownload(const FullArtistRecordSP& rec);
	//void RequestAlbumPicDownload(const FullAlbumRecordSP& rec);
private:
	BOOL m_bEnableFolderImages;

	GdiPlusBitmapCache* m_pThumbnailCache; 



	CachedImageInfoProvider* m_pCIIP;
	DirImageInfoProvider* m_pDIIP;
	typedef std::map<UINT, std::tstring> CacheContainer;
	CacheContainer m_artists;
	CacheContainer m_albums;

	GdiPlusPicDrawer m_defGlobal;
	GdiPlusPicDrawer m_defAlbum;
	GdiPlusPicDrawer* m_defDrawer[IIT_Last];


	//std::list<FullArtistRecordSP> m_failedArtistRequests;
	//std::list<FullAlbumRecordSP> m_failedAlbumRequests;
};

#endif
