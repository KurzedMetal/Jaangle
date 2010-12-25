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
#ifndef _SQLManager_h_
#define _SQLManager_h_

#include "DataRecords.h"
#include "TracksFilter.h"
#include "TagInfo.h"


class ADODatabase;
class ADORecordset;



static const TCHAR* const TS_UnknownString = _T("[Unknown]");


class SQLManager
{
public:
#ifdef _UNITTESTING
	static BOOL UnitTest();
#endif
public:
	SQLManager():m_pDB(NULL)					{}
	virtual ~SQLManager();

	BOOL Init(LPCTSTR DatabasePath);

	//BOOL TrackRecordCollectionByCollectionID(TrackRecordCollection& col, UINT collectionID);

	//=======================Public Utilities
	INT AddNewTrackFromTagInfo(UINT collectionID, LPCTSTR location, TagInfo& info);
	BOOL UpdateTrackFromTagInfo(UINT ID, UINT collectionID, LPCTSTR location, TagInfo& info);

	//===============AddNew-Xxx-Record: Adds a new Xxx record
	//REQUIRE:	xxx with ID==0 && name != "" && foreign keys valid
	//RETURN:	FALSE if xxx exists || failure
	//			TRUE on success. Modifies ID with the new ID
	BOOL AddNewTrackRecord(TrackRecord& rec);
	BOOL AddNewAlbumRecord(AlbumRecord& rec);
	BOOL AddNewArtistRecord(ArtistRecord& rec);
	BOOL AddNewGenreRecord(GenreRecord& rec);
	//BOOL AddNewInfoRecord(InfoRecord& rec);
	BOOL AddNewCollectionRecord(CollectionRecord& rec);
	BOOL AddNewHistTrackRecord(HistTrackRecord& rec);
	BOOL AddNewHistLogRecord(HistLogRecord& rec);
	BOOL AddNewHistArtistRecord(HistArtistRecord& rec);


	//===============Update-Xxx-Record: Updates a Xxx record
	//REQUIRE:	xxx with ID!=0 && name != "" && foreign keys valid
	//RETURN:	FALSE on failure
	//			TRUE on success.
	BOOL UpdateTrackRecord(TrackRecord& rec);
	BOOL UpdateAlbumRecord(AlbumRecord& rec, BOOL bForce);
	BOOL UpdateArtistRecord(ArtistRecord& rec, BOOL bForce);
	BOOL UpdateGenreRecord(GenreRecord& rec, BOOL bForce);
	//BOOL UpdateInfoRecord(InfoRecord& rec);
	BOOL UpdateCollectionRecord(CollectionRecord& rec);
	BOOL UpdateHistTrackRecord(HistTrackRecord& rec);
	BOOL UpdateHistLogRecord(HistLogRecord& rec);
	BOOL UpdateHistArtistRecord(HistArtistRecord& rec);

	//Add/Update/Deletes automatically. It may modify the rec.ID
	//BOOL AdjustInfoRecord(InfoRecord& rec);


	//===============Get-Xxx-Record: Gets a record from ID
	//REQUIRE:	ID!=0
	//RETURN:	FALSE if xxx does not exists || failure
	//			TRUE on success.
	BOOL GetTrackRecord(UINT ID, TrackRecord& rec);
	BOOL GetAlbumRecord(UINT ID, AlbumRecord& rec);
	BOOL GetArtistRecord(UINT ID, ArtistRecord& rec);
	BOOL GetGenreRecord(UINT ID, GenreRecord& rec);
	//BOOL GetInfoRecord(UINT ID, InfoRecord& rec);
	BOOL GetCollectionRecord(UINT ID, CollectionRecord& rec);
	BOOL GetHistTrackRecord(UINT ID, HistTrackRecord& rec);
	BOOL GetHistArtistRecord(UINT ID, HistArtistRecord& rec);
	BOOL GetHistLogRecord(UINT ID, HistLogRecord& rec);
	//BOOL GetInfoRecordRequestRecord(InfoRecordRequest& rec);//rec.parID && rec.infoType
	BOOL GetPicRecord(PicRecord& rec);



	BOOL AddOrUpdatePicRecord(PicRecord& rec);
	BOOL DeletePicRecord(InfoItemTypeEnum infoType, UINT parID);

	BOOL DeleteInfoRequestsOlderThan(UINT timeStampLimit);
	BOOL GetInfoRequests(std::set<UINT>& requestSet);
	BOOL AddInfoRequest(UINT timeStamp, UINT hash);


	//BOOL DeleteInfoRecordResult(UINT ID, InfoItemTypeEnum iit);


	BOOL GetFullTrackRecordByID(FullTrackRecordSP& rec, UINT id);
	BOOL GetFullAlbumRecordByID(FullAlbumRecordSP& rec, UINT id);
	BOOL GetFullArtistRecordByID(FullArtistRecordSP& rec, UINT id);

	//===============Get-Xxx-RecordUnique: Gets a record from ID
	//REQUIRE:	ID!=0
	//RETURN:	FALSE if xxx does not exists || failure
	//			TRUE on success.
	BOOL GetTrackRecordUnique(UINT colID, LPCTSTR location, TrackRecord& rec);
	BOOL GetAlbumRecordUnique(UINT artID, LPCTSTR name, AlbumRecord& rec);
	BOOL GetArtistRecordUnique(LPCTSTR name, ArtistRecord& rec);
	BOOL GetGenreRecordUnique(LPCTSTR name, GenreRecord& rec);
	BOOL GetCollectionRecordUnique(UINT serial, LPCTSTR location, CollectionRecord& rec);
	BOOL GetHistTrackRecordUnique(UINT artID, LPCTSTR name, HistTrackRecord& rec);
	BOOL GetHistArtistRecordUnique(LPCTSTR name, HistArtistRecord& rec);

	//===============Get-Xxx-UD: Gets the ID
	//REQUIRE:	...
	//RETURN:	0 if xxx does not exists || failure
	//			Database ID on success.
	UINT GetTrackID(UINT colID, LPCTSTR location);
	UINT GetAlbumID(UINT artID, LPCTSTR name);
	UINT GetArtistID(LPCTSTR name);
	UINT GetGenreID(LPCTSTR name);
	UINT GetCollectionID(UINT serial, LPCTSTR location);
	UINT GetHistTrackID(UINT artID, LPCTSTR name);
	UINT GetHistArtistID(LPCTSTR name);


	//===============Xxx-RecordExists: Test if a record exist
	//REQUIRE:	ID!=0
	//RETURN:	FALSE if xxx does not exists || failure
	//			TRUE on success.
	BOOL TrackRecordExists(UINT ID);
	BOOL AlbumRecordExists(UINT ID);
	BOOL ArtistRecordExists(UINT ID);
	BOOL GenreRecordExists(UINT ID);
	//BOOL InfoRecordExists(UINT ID);
	BOOL CollectionRecordExists(UINT ID);
	BOOL HistTrackRecordExists(UINT ID);
	BOOL HistArtistRecordExists(UINT ID);
	BOOL HistLogRecordExists(UINT ID);


	//===============Delete-Xxx-Record: Deletes
	//REQUIRE:	ID!=0
	//RETURN:	FALSE if xxx does not exists || failure
	//			TRUE on success.
	BOOL DeleteTrackRecord(UINT ID);
	BOOL DeleteAlbumRecord(UINT ID);
	BOOL DeleteArtistRecord(UINT ID);
	BOOL DeleteGenreRecord(UINT ID);
	//===DeleteInfoRecord Force will also delete the associated Tracks
	BOOL DeleteCollectionRecord(UINT ID, BOOL bForce);
	//===DeleteCollectionRecord Force will also reset the Tracks/Albums/Artists info flags
	//BOOL DeleteInfoRecord(UINT ID);

	BOOL DeleteHistLogRecord(UINT ID);


	//====CONTRACTS for Updates
	//1. WHEN artist.ID = 0;
	//		REQUEST: "Add the record (if does not exist)"
	//		POSSIBLE RESULTS:	(UR_Error)"Failure"					---record is not modified 
	//							(UR_DataAdded)"Added OK"			---record is not modified 
	//							(UR_DataMerged)"Same name (unique) exists."  ---Record.ID gets the value of the existing record
	//								* "Other Record fields has been ignored"
	//2. When artist.ID > 0;
	//		REQUEST: "Update the record with ID to the new fields"
	//		POSSIBLE RESULTS:	(UR_Error)"Failure"					---record is not modified
	//							(UR_DataUpdated)"Updated OK"		---record is not modified
	//							(UR_DataMerged)"Name (unique) has changed && New Name exists." ---Record.ID gets the value of the existing record.
	//								* "Other Record fields has been ignored"
	//UpdateResult UpdateArtistRecord(ArtistRecord& artist);
	//UpdateResult UpdateAlbumRecord(AlbumRecord& artist);
	//UpdateResult UpdateGenreRecord(GenreRecord& genre);
	//UpdateResult UpdateCollectionRecord(CollectionRecord& collection);

	struct ChangeStyle
	{
		ChangeStyle():additions(0),removals(0),updates(0){}
		INT additions;
		INT removals;
		INT updates;
	};
	struct Changes
	{
		ChangeStyle tracks;
		ChangeStyle albums;
		ChangeStyle artists;
		ChangeStyle genres;
		ChangeStyle collections;
		ChangeStyle info;
	};
	const Changes& GetChanges()			{return m_changes;}
	void ResetChanges()					{m_changes = Changes();}


	//UpdateFullArtistRecord: May Modify the genreID || the artist ID
	BOOL UpdateFullArtistRecord(FullArtistRecord& artist, BOOL bForce);
	//UpdateFullAlbumRecord: May Modify the albumID || the artist ID
	BOOL UpdateFullAlbumRecord(FullAlbumRecord& album, BOOL bForce);
	//UpdateFullTrackRecord: May Modify ALL the foreign keys
	BOOL UpdateFullTrackRecord(FullTrackRecord& track);

	//BOOL InfoRecordByID(InfoRecordSP& rec, UINT id);
	//BOOL GetFullTrackRecordByID(FullTrackRecordSP& rec, UINT id);
	BOOL GetFullTrackRecordByLocation(FullTrackRecordSP& rec, LPCTSTR Location, UINT CollectionID = 0);
	//BOOL GetFullAlbumRecordByID(FullAlbumRecordSP& rec, UINT id);
	//BOOL GetFullArtistRecordByID(FullArtistRecordSP& rec, UINT id);


	//====================Retrieve Collections By SQL
	BOOL GetTrackRecordCollectionBySQL(TrackRecordCollection& col, LPCTSTR sql, INT limitResults = 0);
	BOOL GetGenreRecordCollectionBySQL(GenreRecordCollection& col, LPCTSTR SQL);
	BOOL GetCollectionRecordCollectionBySQL(CollectionRecordCollection& col, LPCTSTR SQL);
	BOOL GetYearRecordCollectionBySQL(YearRecordCollection& col, LPCTSTR SQL);
	BOOL GetFullTrackRecordCollectionBySQL(FullTrackRecordCollection& col, LPCTSTR SQL, INT limitResults = 0);
	BOOL GetFullAlbumRecordCollectionBySQL(FullAlbumRecordCollection& col, LPCTSTR SQL);
	BOOL GetFullArtistRecordCollectionBySQL(FullArtistRecordCollection& col, LPCTSTR SQL);
	BOOL GetMonthAddedRecordCollectionBySQL(MonthAddedRecordCollection& col, LPCTSTR sql);

	//====================Retrieve Collections By TracksFilter
	BOOL GetTrackRecordCollectionByTracksFilter(TrackRecordCollection& col, TracksFilter& Filter);
	BOOL GetGenreRecordCollectionByTracksFilter(GenreRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks);
	BOOL GetCollectionRecordCollectionByTracksFilter(CollectionRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks);
	BOOL GetYearRecordCollectionByTracksFilter(YearRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks);
	BOOL GetFullTrackRecordCollectionByTracksFilter(FullTrackRecordCollection& col, TracksFilter& Filter, INT limitResults = 0);
	BOOL GetFullAlbumRecordCollectionByTracksFilter(FullAlbumRecordCollection& col, TracksFilter& Filter);
	BOOL GetFullArtistRecordCollectionByTracksFilter(FullArtistRecordCollection& col, TracksFilter& Filter);
	BOOL GetFullAlbumGPNRecordCollectionByTracksFilter(FullAlbumRecordCollection& col, TracksFilter& Filter);
	BOOL GetMonthAddedRecordCollectionByTracksFilter(MonthAddedRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks);

	//====================Retrieve Collections [Special]
	BOOL GetFullTrackRecordCollectionBySimpleSearch(FullTrackRecordCollection& col, LPCTSTR searchStr, INT limitResults = 0);
	BOOL GetFullTrackRecordCollectionBySmartSearch(FullTrackRecordCollection& col, LPCTSTR searchStr, INT limitResults = 0);
	BOOL GetFullTrackRecordCollectionByInfoLikeness(FullTrackRecordCollection& col, InfoItemTypeEnum iit, LPCTSTR searchString);
	BOOL GetFullTrackRecordCollectionByLocation(FullTrackRecordCollection& col, LPCTSTR Location);
	BOOL GetFullAlbumRecordCollectionByTracksFilterForCollections(FullAlbumRecordCollection& col, TracksFilter& Filter);
	BOOL GetCollectionRecordCollectionByTypeID(CollectionRecordCollection& col, CollectionTypesEnum type);

	//=====================Get/Set Info
	//UINT GetTrackInfoID(UINT trackID, InfoItemTypeEnum iit);
	//UINT GetAlbumInfoID(UINT albID, InfoItemTypeEnum iit);
	//UINT GetArtistInfoID(UINT artID, InfoItemTypeEnum iit);

	BOOL AddNewTrackInfo(UINT trackID, InfoItemTypeEnum iit, LPCTSTR info);
	BOOL AddNewAlbumInfo(UINT albID, InfoItemTypeEnum iit, LPCTSTR info);
	BOOL AddNewArtistInfo(UINT artID, InfoItemTypeEnum iit, LPCTSTR info);

	BOOL UpdateTrackInfo(UINT trackID, InfoItemTypeEnum iit, LPCTSTR info);
	BOOL UpdateAlbumInfo(UINT albID, InfoItemTypeEnum iit, LPCTSTR info);
	BOOL UpdateArtistInfo(UINT artID, InfoItemTypeEnum iit, LPCTSTR info);

	BOOL DeleteTrackInfo(UINT trackID, InfoItemTypeEnum iit);
	BOOL DeleteAlbumInfo(UINT albID, InfoItemTypeEnum iit);
	BOOL DeleteArtistInfo(UINT artID, InfoItemTypeEnum iit);

	BOOL DeleteAllTrackInfo(UINT trackID);
	BOOL DeleteAllAlbumInfo(UINT albID);
	BOOL DeleteAllArtistInfo(UINT artID);

	BOOL AdjustTrackInfo(UINT trackID, InfoItemTypeEnum iit, LPCTSTR info);
	BOOL AdjustAlbumInfo(UINT albID, InfoItemTypeEnum iit, LPCTSTR info);
	BOOL AdjustArtistInfo(UINT artID, InfoItemTypeEnum iit, LPCTSTR info);

	BOOL GetTrackInfoRecord(UINT trackID, InfoItemTypeEnum iit, InfoRecord& ir);
	BOOL GetAlbumInfoRecord(UINT albID, InfoItemTypeEnum iit, InfoRecord& ir);
	BOOL GetArtistInfoRecord(UINT artID, InfoItemTypeEnum iit, InfoRecord& ir);

	UINT GetTrackInfoFlags(UINT trackID);


	//Other Statistical Info
	UINT GetTrackCount(TracksFilter& tf);
	BOOL GetTrackIDList(TracksFilter& tf, std::vector<UINT>& idList);


	//enum RandomTrackModeEnum
	//{
	//	RTM_All,
	//	RTM_Artist,
	//	RTM_Album,
	//	RTM_Genre,
	//	RTM_Year,
	//	RTM_Collection,
	//	RTM_Newest,
	//	RTM_Last
	//};
	//BOOL GetRandomTrackIDList(std::vector<UINT>& idList, UINT numItems, RandomTrackModeEnum rtm, UINT rtmHelperID, UINT avoidID, UINT minRating);

	BOOL GetFullLogRecordCollection(FullHistLogRecordCollection& col,
		SYSTEMTIME* startDate/* = NULL*/,
		SYSTEMTIME* endDate /*= NULL*/,
		TCHAR* ArtistFilter /*= NULL*/,
		TCHAR* TrackFilter /*= NULL*/,
		HistoryLogActionsEnum actID,
		UINT limitResults /*= 0*/);

	BOOL GetFullHistTrackRecordCollection(FullHistTrackRecordCollection& col,
		SYSTEMTIME* startDate/* = NULL*/,
		SYSTEMTIME* endDate /*= NULL*/,
		TCHAR* ArtistFilter /*= NULL*/,
		TCHAR* TrackFilter /*= NULL*/,
		HistoryLogActionsEnum actID,
		UINT limitResults /*= 0*/);

	BOOL GetFullHistArtistRecordCollection(FullHistArtistRecordCollection& col,
		SYSTEMTIME* startDate/* = NULL*/,
		SYSTEMTIME* endDate /*= NULL*/,
		TCHAR* ArtistFilter /*= NULL*/,
		TCHAR* TrackFilter /*= NULL*/,
		HistoryLogActionsEnum actID,
		UINT limitResults /*= 0*/);

	struct Rank
	{
		UINT Hits;
		UINT HitsCount;
	};
	BOOL GetHistTrackRankCollection(std::vector<Rank>& col);
	BOOL GetHistArtistRankCollection(std::vector<Rank>& col);

	BOOL LogTrackInHistory(LPCTSTR Artist, LPCTSTR Track, HistoryLogActionsEnum actID, SYSTEMTIME& localTime);

	struct HistoryLogStats
	{
		HistoryLogStats():actions(0),plays(0)
		{
			memset(&firstTime, 0, sizeof(SYSTEMTIME));
			memset(&lastTime, 0, sizeof(SYSTEMTIME));
		}
		UINT actions;
		UINT plays;
		SYSTEMTIME firstTime;
		SYSTEMTIME lastTime;
	};
	BOOL GetHistoryLogStats(HistoryLogStats& stats);


	struct HistoryTrackStats
	{
		HistoryTrackStats():rank(0),hits(0)
		{
			memset(&firstTime, 0, sizeof(SYSTEMTIME));
			memset(&lastTime, 0, sizeof(SYSTEMTIME));
		}
		UINT rank;
		UINT hits;
		SYSTEMTIME firstTime;
		SYSTEMTIME lastTime;
	};
	BOOL GetHistoryTrackStats(LPCTSTR Artist, LPCTSTR Track, SYSTEMTIME* startDate, SYSTEMTIME* endDate,//IN
		HistoryTrackStats& stats);//OUT

	BOOL GetVirtualTrackCollectionRecord(CollectionRecord& cr);


	struct ValuedID
	{
		ValuedID(UINT artid, DOUBLE val):ID(artid),value(val),appearanceCount(1){}
		UINT ID;
		DOUBLE value;
		UINT appearanceCount;
	};
	BOOL GetSimilarArtists(std::vector<ValuedID>& col, UINT artistID, INT maxTagsToExamine = 5, INT maxArtistsPerGenreToExamine = 5);
	
	//BOOL UpdateTrack(FullTrackRecordSP track);

private:
	//BOOL GetInfo(LPCTSTR fldName, LPCTSTR tblName, InfoItemTypeEnum iit, UINT ID, InfoRecord& rec);
	//BOOL SetInfo(LPCTSTR fldName, LPCTSTR tblName, InfoItemTypeEnum iit, UINT ID, LPCTSTR info);
	BOOL RecordExists(LPCTSTR sql, UINT ID);
	void AddSQLStrParameter(CString& ret, TextMatch tm, LPCTSTR str);
	void AddSQLNumParameter(CString& ret, NumericMatch nm, LPCTSTR str);
	void AddSQLCountParameter(CString& ret, NumericMatch nm, LPCTSTR fieldName);
	void AddSQLDateParameter(CString& ret, DateMatch dm, LPCTSTR str);
	
	CString GetWhereSQL(TracksFilter& Filter);
	CString GetHavingSQL(NumericMatch nm);

	static UINT FixStringField(LPCTSTR source, LPTSTR dest, UINT destLen);
	static void GetSQLDateForQueries(SYSTEMTIME& input, LPTSTR bf, UINT bfLen);

	//Appends the data from the current recordset (current position) to rec.
	//Requests recordset fields using their index starting from fieldID
	void GetTrackRecordFields(TrackRecord& rec, ADORecordset& rs, short& startIdx);
	BOOL SetTrackRecordFields(TrackRecord& rec, ADORecordset& rs);
	void GetAlbumRecordFields(AlbumRecord& rec, ADORecordset& rs, short& startIdx);
	BOOL SetAlbumRecordFields(AlbumRecord& rec, ADORecordset& rs);
	void GetArtistRecordFields(ArtistRecord& rec, ADORecordset& rs, short& startIdx);
	BOOL SetArtistRecordFields(ArtistRecord& rec, ADORecordset& rs);
	void GetGenreRecordFields(GenreRecord& rec, ADORecordset& rs, short& startIdx);
	BOOL SetGenreRecordFields(GenreRecord& rec, ADORecordset& rs);
	void GetCollectionRecordFields(CollectionRecord& rec, ADORecordset& rs, short& startIdx);
	BOOL SetCollectionRecordFields(CollectionRecord& rec, ADORecordset& rs);
	//void GetInfoRecordFields(InfoRecord& rec, ADORecordset& rs, short& startIdx);
	//BOOL SetInfoRecordFields(InfoRecord& rec, ADORecordset& rs);
	void GetHistArtistRecordFields(HistArtistRecord& rec, ADORecordset& rs, short& startIdx);
	void GetHistLogRecordFields(HistLogRecord& rec, ADORecordset& rs, short& startIdx);
	void GetHistTrackRecordFields(HistTrackRecord& rec, ADORecordset& rs, short& startIdx);

	BOOL GetFirstVal(LPCTSTR sql, INT& val);

	void AddSmartSearchCondition(std::tstring& SQL, LPCTSTR condition);

	BOOL EvaluateTrackInfo(InfoItemTypeEnum iit);
	BOOL EvaluateAlbumInfo(InfoItemTypeEnum iit);
	BOOL EvaluateArtistInfo(InfoItemTypeEnum iit);
	//UINT GetInfoID(LPCTSTR tblName, LPCTSTR fldName, UINT ID, InfoItemTypeEnum iit);
	BOOL AddNewInfo(LPCTSTR tblName, LPCTSTR fldName, UINT ID, InfoItemTypeEnum iit, LPCTSTR info);



	//===================================COLLECTIONS
	//BOOL GetCollectionRecord(CollectionRecord& collection, UINT ID);
	//BOOL GetCollectionRecordUnique(CollectionRecord& genre, LPCTSTR location, UINT serial);


	//===================================GENRES
	//BOOL GetGenreRecord(GenreRecord& genre, UINT ID);
	//BOOL GetGenreRecordUnique(GenreRecord& genre, LPCTSTR name);

	//===================================ARTISTS








	//BOOL GetArtistRecordUnique(ArtistRecord& artist, LPCTSTR name);

	//===================================ALBUMS
	//BOOL AddNewAlbumRecord(AlbumRecord& album);

	//BOOL GetAlbumRecordUnique(AlbumRecord& album, LPCTSTR name, UINT artID);





	//BOOL SetCollectionRecordFields(CollectionRecord& collection, ADORecordset& rs);


private:
	ADODatabase* m_pDB;
	Changes m_changes;

};

#endif

