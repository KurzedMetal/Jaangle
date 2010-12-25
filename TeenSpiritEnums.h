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
#ifndef _TeenSpiritEnums_h_
#define _TeenSpiritEnums_h_

//------------------------------------------------------------------------
//-----------Enums------------------------------------
//------------------------------------------------------------------------

enum ItemTypeEnum
{
	IT_Unknown = 0,
	IT_Various,
	IT_Artist,
	IT_Album,
	IT_Genre,
	IT_Year,
	IT_AlbumCollection,
	IT_Collection,
	IT_Track,
	IT_MultipleTracks,
	IT_QuickSearch,
	IT_Last
};

enum SectionTypeEnum
{
	ST_Root = 0,
	ST_Unknown,
	ST_Artist,
	ST_Album,
	ST_Genre,
	ST_Year,
	ST_Collection,
	ST_AlbumByName,
	ST_MonthAdded,
	ST_Directory,
	ST_Last
};


enum SectionModesEnum
{
	SMODE_First,
	SMODE_ArtistsAlbums,
	SMODE_ArtistsYears,
	SMODE_ArtistsGenres,
	SMODE_GenreArtistsAlbums,
	SMODE_GenreYears,
	SMODE_YearAlbums,
	SMODE_YearArtistsAlbums,
	SMODE_CollectionsArtistsAlbums,
	SMODE_CollectionsGenresArtistsAlbums,
	SMODE_AlbumsByName,
	SMODE_MonthAddedArtistAlbum,
	SMODE_CollectionsAlbums,
	//SMODE_Directories,
	SMODE_Last
};

enum SectionFiltersEnum
{
	SFILT_First,
	SFILT_All,
	SFILT_Audio,
	SFILT_Video,
	SFILT_New,
	SFILT_Last
};

enum SectionStylesEnum
{
	SSTYL_First,
	SSTYL_1_Small,
	SSTYL_2,
	SSTYL_3,
	SSTYL_4,
	SSTYL_5,
	SSTYL_6,
	SSTYL_7,
	SSTYL_Last
};

enum CollectionTypesEnum
{
	CTYPE_Unknown,
	CTYPE_LocalFolder,	//CAUTION: This are hardcoded in Database - DO NOT CHANGE THEIR ORDER
	CTYPE_Media,	
	CTYPE_NetFolder,
	CTYPE_Virtual,
	CTYPE_Removable,
	CTYPE_AudioCD,
	CTYPE_HTTPLinks,
	CTYPE_Last			//ADD NEW COLLECTION TYPES BEFORE 'CTYPE_Last'
};

enum MediaPlayerActionsEnum
{
	MPA_Default,
	MPA_Play,
	MPA_Enqueue,
	MPA_EnqueueAfter,
	MPA_Last
};

enum AppCommandsEnum
{
	ACMD_First = 0x2000,
	ACMD_DefaultPlayerAction,
	ACMD_Play,
	ACMD_Enqueue,
	ACMD_EnqueueAfter,
	ACMD_PlayExternal,
	ACMD_EnqueueExternal,
	ACMD_OpenFolder,
	ACMD_History,
	ACMD_Delete,
	ACMD_Properties,
	ACMD_CopyPath,
	ACMD_CopyTo,
	ACMD_ReadTags,
	ACMD_WriteTags,
	ACMD_LocateAlbum,
	ACMD_PurchaseInfo,
	ACMD_Renamer,
	ACMD_GetTagsFromFilename,
	ACMD_InsertRandomTracks,
	ACMD_GotoPersonalPage,
	ACMD_GotoWebSite,
	ACMD_GotoForum,
	ACMD_GotoDonation,
	ACMD_CheckForUpdates,
	ACMD_Last
};


enum HistoryLogActionsEnum
{
	//These values are hard coded in Database
	HLA_Unknown = 0,
	HLA_Played,
	HLA_Clicked,
	HLA_Last
};

#define TTYPE_FIRSTVIDEO 100
#define TTYPE_FIRSTVIRTUAL 200



enum TrackTypesEnum
{
	TTYPE_Unsupported = 0,
	
	TTYPE_mp3,//.mp3.mp2.wma.cda.ogg
	TTYPE_mp2,
	TTYPE_wma,
	TTYPE_cda,
	TTYPE_ogg,

	TTYPE_mp1,//.mp1aiff.xm .it .s3m
	TTYPE_aiff,
	TTYPE_xm,
	TTYPE_it,
	TTYPE_s3m,
	
	TTYPE_mod,//.mod.mtm.umx.mo3.mpc
	TTYPE_mtm,
	TTYPE_umx,
	TTYPE_mo3,
	TTYPE_mpc,
	
	TTYPE_flac,//flac.midmidi.ac3.mp4
	TTYPE_mid,
	TTYPE_midi,
	TTYPE_ac3,
	TTYPE_mp4,
	
	TTYPE_aac,//.aac.ape.m4a.wav.oga
	TTYPE_ape,
	TTYPE_m4a,
	TTYPE_wav,
	TTYPE_oga,
	
	TTYPE_wv,//.wv .spx.tta.m4b.m4p
	TTYPE_spx,
	TTYPE_tta,
	TTYPE_m4b,
	TTYPE_m4p,
	
	TTYPE_3g2,//.3g2.aif
	TTYPE_aif,
	
	TTYPE_mpg = TTYPE_FIRSTVIDEO,//.mpgmpeg.avi.wmv.ogm
	
	TTYPE_mpeg,
	TTYPE_avi,
	TTYPE_wmv,
	TTYPE_ogm,
	
	TTYPE_http = TTYPE_FIRSTVIRTUAL,
	
	TTYPE_Last
};
static TrackTypesEnum GetTrackType(LPCTSTR fileName)
{
	ASSERT(fileName != NULL);
	if (fileName != NULL)
	{
		if (_tcsncmp(fileName, _T("http://"), 7) == 0)
			return TTYPE_http;
		if (_tcsrchr(fileName, '.') == 0) 
			return TTYPE_Unsupported;//Don't accept files without extension
		size_t len = _tcslen(fileName);
		if (len <= 4)// "a.mp3" //
			return TTYPE_Unsupported;//Don't accept so small files
		TCHAR ext[5];
		_tcsncpy(ext, &fileName[len - 4], 4);
		ext[4] = 0;
		_tcslwr(ext);
		const TCHAR* const audioFileTypes = _T(".mp3.mp2.wma.cda.ogg.mp1aiff.xm .it .s3m.mod.mtm.umx.mo3.mpcflac.midmidi.ac3.mp4.aac.ape.m4a.wav.oga.wv .spx.tta.m4b.m4p.3g2.aif");
		const TCHAR* const videoFileTypes = _T(".mpgmpeg.avi.wmv.ogm");
		LPCTSTR f = _tcsstr(audioFileTypes, ext);
		if (f != NULL)
		{
			DOUBLE pos = (f - audioFileTypes) / 4.0;
			if (pos == DOUBLE(INT(pos)))
				return TrackTypesEnum(INT(pos) + 1);
			else
				f = NULL;//Found such a file
		}
		f = _tcsstr(videoFileTypes, ext);
		if (f != NULL)
		{
			DOUBLE pos = (f - videoFileTypes) / 4.0;
			if (pos == DOUBLE(INT(pos)))
				return TrackTypesEnum(INT(pos) + 100);
			else
				f = NULL;//Found such a file
		}
	}
	return TTYPE_Unsupported;
}

enum TrackSyncStateEnum
{
	TSS_InSync = 0,
	TSS_NotInSync = 1
};

enum TrackWriteableStateEnum
{
	TWS_Writable = 0,
	TWS_ReadOnly = 1
};

enum InfoItemTypeEnum
{
	IIT_Unknown,
	//INFO------------
	IIT_ArtistBio,		//The Info Provided must be in the form 'Artist'
	IIT_AlbumReview,	//The Info Provided must be in the form 'Artist|||Album'
	IIT_TrackComment,	//The Info Provided must be in the form 'Artist|||Track'
	IIT_TrackLyrics,
	IIT_TrackPersonal,
	IIT_TrackTablatures,
	//PICTURES------------
	IIT_ArtistPicture,
	IIT_AlbumPicture,	
	IIT_GenrePicture,
	IIT_YearPicture,
	IIT_CollectionPicture,
	IIT_MonthPicture,

	IIT_Last
};

enum RandomTrackModeEnum
{
	RTM_All,
	RTM_Artist,
	RTM_Album,
	RTM_Genre,
	RTM_Year,
	RTM_Collection,
	RTM_Newest,
	RTM_Last
};

enum TrackTextType
{
	TTT_Title,
	TTT_Album,
	TTT_Artist,
	TTT_Last
};

#endif
