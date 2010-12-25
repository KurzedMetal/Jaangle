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
#ifndef _DataRecords_h_
#define _DataRecords_h_

#include <boost/shared_ptr.hpp>
#include "TeenSpiritEnums.h"

struct InfoRecord
{
	InfoRecord():ID(0)
	{
		memset(&dateAdded, 0, sizeof(SYSTEMTIME));
	}
	UINT ID;
	std::tstring name;
	SYSTEMTIME dateAdded;
	BOOL IsValid()const{return ID != 0 && !name.empty();}
};

struct GenreRecord
{
	GenreRecord():ID(0),trackCount(-1){}
	UINT ID;
	std::tstring name;
	INT trackCount;
	BOOL IsValid()const{return ID != 0;}// && !name.empty()

	//---On Second thought i think i need a more custom equality (r.g not counting ID or trackCount
	//bool operator==(const GenreRecord &ref) const	{
	//	return ID==ref.ID && name==ref.name && trackCount==ref.trackCount;
	//}
	//bool operator!=(const GenreRecord &ref) const	{
	//	return !(*this == ref);
	//}
};


struct DirectoryRecord
{
	DirectoryRecord(){}
	std::tstring path;
	BOOL IsValid()const{return !path.empty();}// && !name.empty()
};


struct ArtistRecord
{
	ArtistRecord():ID(0),genID(0),rating(0),trackCount(-1){}
	UINT ID;
	std::tstring name;
	UINT genID;
	INT rating;
	INT trackCount;
	BOOL IsValid()const{return ID != 0 && !name.empty() && genID != 0;}
};
struct FullArtistRecord
{
	FullArtistRecord(){}
	ArtistRecord artist;
	GenreRecord genre;
	BOOL IsValid()const{return artist.IsValid() && genre.IsValid();}
};
struct AlbumRecord
{
	AlbumRecord():ID(0),artID(0),rating(0),year(0),trackCount(-1){}
	UINT ID;
	std::tstring name;
	UINT artID;
	INT rating;
	INT year;
	INT trackCount;
	BOOL IsValid()const{return ID != 0 && !name.empty() && artID != 0;}
};
struct FullAlbumRecord
{
	FullAlbumRecord(){}
	AlbumRecord album;
	ArtistRecord artist;
	BOOL IsValid()const{return artist.IsValid() && album.IsValid();}
};
struct CollectionRecord
{
	CollectionRecord():ID(0),serial(0),type(CTYPE_Unknown),/*status(CSTAT_Unknown),*/
		trackCount(-1),sumSize(0), sumDuration(0)
	{
		memset(&dateAdded, 0, sizeof(SYSTEMTIME));
		memset(&dateUpdated, 0, sizeof(SYSTEMTIME));
	}
	UINT ID;
	std::tstring name;
	std::tstring location;
	UINT serial;
	CollectionTypesEnum type;
	//CollectionStatusEnum status;
	SYSTEMTIME dateAdded;
	SYSTEMTIME dateUpdated;
	INT trackCount;
	UINT sumSize;
	UINT sumDuration;
	BOOL IsValid()const{return ID != 0 && !name.empty() && !location.empty() && 
		/*status > CSTAT_Unknown && status < CSTAT_Last &&*/
		type > CTYPE_Unknown && type < CTYPE_Last;}
};

enum TrackRecordFlags
{
	TRF_Uninitialized = 0x0,
	TRF_NoFlags = 0x1,
	TRF_HasComments = 0x2,
	TRF_HasLyrics  = 0x4,
	TRF_HasPersonal = 0x8,
	TRF_HasTablatures = 0x10,
	TRF_Last = 0x20
};

struct TrackRecord
{
	TrackRecord():ID(0),artID(0),albID(0),colID(0),genID(0),bitrate(0),
	duration(0),size(0),trackNo(0),year(0),rating(0),trackType(TTYPE_Unsupported),flags(0){}
	UINT ID;
	std::tstring name;
	TrackTypesEnum trackType;
	SYSTEMTIME dateAdded;
	UINT albID;
	UINT artID;
	UINT colID;
	UINT genID;
	std::tstring location;
	INT bitrate;
	UINT duration;
	UINT size;
	INT trackNo;
	INT year;
	INT rating;
	UINT flags;
	BOOL IsValid()const{return ID != 0 && !name.empty() && !location.empty() && 
		albID != 0 && artID != 0 && genID != 0 && colID != 0 && 
		trackType > TTYPE_Unsupported && trackType < TTYPE_Last;}

};

struct FullTrackRecord
{
#ifdef _DEBUG
	static int FullTrackRecords;
	//FullTrackRecord()	{TRACE(_T("@D+FullTrackRecord. #: %d\r\n"), ++FullTrackRecords);}
	//~FullTrackRecord()	{TRACE(_T("@D-FullTrackRecord. #: %d\r\n"), --FullTrackRecords);}
#endif
	TrackRecord track;
	AlbumRecord album;
	ArtistRecord artist;
	GenreRecord genre;
	CollectionRecord collection;
	BOOL IsValid()const{return track.IsValid() && album.IsValid() && 
		artist.IsValid() && genre.IsValid() && collection.IsValid();}

};

struct YearRecord
{
	YearRecord():year(0), trackCount(-1){}
	INT year;
	INT trackCount;
	BOOL IsValid()const{return TRUE;}
};

struct MonthAddedRecord
{
	MonthAddedRecord():month(0), trackCount(-1){}
	INT month;
	INT trackCount;
	BOOL IsValid()const{return month > (2000 * 12);}
};

enum PictureTypeEnum
{
	PT_First = 0,
	PT_Main,
	PT_Normal,
	PT_Last
};

struct PicRecord
{
	PicRecord(InfoItemTypeEnum iit, UINT parentID):parID(parentID),pictureType(PT_First),infoType(iit)
	{
		ASSERT(IsValid());
	}
	UINT parID;
	std::tstring path;
	PictureTypeEnum pictureType;
	InfoItemTypeEnum infoType;
	BOOL IsValid()const{return parID > 0 && infoType >= IIT_Unknown && infoType<IIT_Last;}
};

enum InfoRecordRequestResultEnum
{
	IRRR_Unknown,
	IRRR_GeneralFailure,
	IRRR_Rejection,
	IRRR_Last
};

struct InfoRecordRequest
{
	InfoRecordRequest(UINT parentID, InfoItemTypeEnum infoItemType):parID(parentID),infoType(infoItemType),result(IRRR_Unknown)
	{
		ASSERT(IsValid());
	}
	UINT parID;
	InfoItemTypeEnum infoType;
	UINT requestTimeStamp;
	InfoRecordRequestResultEnum result;
	BOOL IsValid()const{return parID > 0 && infoType > IIT_Unknown && infoType < IIT_Last;}
};

//struct AlbumPicRecord
//{
//	AlbumPicRecord(UINT albumID):albID(albumID),pictureType(PT_First)
//	{
//		memset(&dateAdded, 0, sizeof(SYSTEMTIME));
//		ASSERT(IsValid());
//	}
//	UINT albID;
//	std::tstring path;
//	SYSTEMTIME dateAdded;
//	PictureTypeEnum pictureType;
//	BOOL IsValid(){return albID > 0;}
//};


struct HistArtistRecord
{
	HistArtistRecord():ID(0){}
	UINT ID;
	std::tstring name;
	BOOL IsValid()const{return ID != 0 && !name.empty();}
};

struct HistTrackRecord
{
	HistTrackRecord():ID(0),artID(0){}
	UINT ID;
	std::tstring name;
	UINT artID;
	BOOL IsValid()const{return ID != 0 && !name.empty() && artID != 0;}
};
struct HistLogRecord
{
	HistLogRecord():ID(0),actID(HLA_Played),trackID(0){memset(&dateAdded, 0, sizeof(SYSTEMTIME));}
	UINT ID;
	HistoryLogActionsEnum actID;
	SYSTEMTIME dateAdded;
	UINT trackID;
	BOOL IsValid()const{return ID != 0 && trackID != 0 && actID > HLA_Unknown && actID < HLA_Last;}
};

struct FullHistLogRecord
{
	HistLogRecord log;
	HistTrackRecord track;
	HistArtistRecord artist;
	BOOL IsValid()const{return log.IsValid() && track.IsValid() && artist.IsValid();}
};

struct FullHistTrackRecord
{
	HistTrackRecord track;
	HistArtistRecord artist;
	UINT hits;
	SYSTEMTIME firstTime;
	SYSTEMTIME lastTime;
	BOOL IsValid()const{return hits > 0 && track.IsValid() && artist.IsValid();}
};

struct FullHistArtistRecord
{
	HistArtistRecord artist;
	UINT hits;
	SYSTEMTIME firstTime;
	SYSTEMTIME lastTime;
	BOOL IsValid()const{return hits > 0 && artist.IsValid();}
};

typedef boost::shared_ptr<TrackRecord> TrackRecordSP;
typedef std::vector<TrackRecordSP> TrackRecordCollection;

typedef boost::shared_ptr<FullTrackRecord> FullTrackRecordSP;
typedef std::vector<FullTrackRecordSP> FullTrackRecordCollection;

typedef boost::shared_ptr<FullArtistRecord> FullArtistRecordSP;
typedef std::vector<FullArtistRecordSP> FullArtistRecordCollection;

typedef boost::shared_ptr<FullAlbumRecord> FullAlbumRecordSP;
typedef std::vector<FullAlbumRecordSP> FullAlbumRecordCollection;

typedef boost::shared_ptr<GenreRecord> GenreRecordSP;
typedef std::vector<GenreRecordSP> GenreRecordCollection;

typedef boost::shared_ptr<CollectionRecord> CollectionRecordSP;
typedef std::vector<CollectionRecordSP> CollectionRecordCollection;

typedef boost::shared_ptr<YearRecord> YearRecordSP;
typedef std::vector<YearRecordSP> YearRecordCollection;

typedef boost::shared_ptr<MonthAddedRecord> MonthAddedRecordSP;
typedef std::vector<MonthAddedRecordSP> MonthAddedRecordCollection;

typedef boost::shared_ptr<InfoRecord> InfoRecordSP;

typedef boost::shared_ptr<DirectoryRecord> DirectoryRecordSP;
typedef std::vector<DirectoryRecordSP> DirectoryRecordCollection;

typedef boost::shared_ptr<HistTrackRecord> HistTrackRecordSP;
typedef std::vector<HistTrackRecordSP> HistTrackRecordCollection;

typedef boost::shared_ptr<HistArtistRecord> HistArtistRecordSP;
typedef std::vector<HistArtistRecordSP> HistArtistRecordCollection;

typedef boost::shared_ptr<HistLogRecord> HistLogRecordSP;
typedef std::vector<HistLogRecordSP> HistLogRecordCollection;

typedef boost::shared_ptr<FullHistLogRecord> FullHistLogRecordSP;
typedef std::vector<FullHistLogRecordSP> FullHistLogRecordCollection;

typedef boost::shared_ptr<FullHistTrackRecord> FullHistTrackRecordSP;
typedef std::vector<FullHistTrackRecordSP> FullHistTrackRecordCollection;

typedef boost::shared_ptr<FullHistArtistRecord> FullHistArtistRecordSP;
typedef std::vector<FullHistArtistRecordSP> FullHistArtistRecordCollection;




#endif

