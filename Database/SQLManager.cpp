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
#include "stdafx.h"
#include "SQLManager.h"
#include "Ado/AdoWrap.h"
#include "stlStringUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//#define USE_INTEGRITY_CHECKS
#define USE_CACHED_QUERIES

#ifdef USE_CACHED_QUERIES
GenreRecord sGenreCache;
AlbumRecord sAlbumCache;
ArtistRecord sArtistCache;
#endif


#ifdef _UNITTESTING
BOOL SQLManager::UnitTest()
{
	TRACE(_T("@2 SQLManager:Unit-Testing\r\n"));
	LPCTSTR dbPath = _T("c:\\test.mdb");
	DeleteFile(dbPath);
	{
		SQLManager sm;
		UNITTEST(sm.Init(dbPath));
	}
	{
		SQLManager sm;
		UNITTEST(sm.Init(dbPath));

		//---Collection records----------------
		CollectionRecord col;
		col.location = _T("d:\\mmm.mp3");
		col.type = CTYPE_LocalFolder;
		//------Should Success -Add - Update - Exists - GetById - GetUnique
		UNITTEST(sm.AddNewCollectionRecord(col));
		UNITTEST(col.IsValid());
		col.name = _T("new col name");
		UNITTEST(sm.UpdateCollectionRecord(col));
		UNITTEST(col.IsValid());
		UNITTEST(sm.GetCollectionRecord(col.ID, col));
		UNITTEST(col.name == _T("new col name"));
		UNITTEST(col.IsValid());
		CollectionRecord col2;
		UNITTEST(sm.GetCollectionRecordUnique(col.serial, col.location.c_str(), col2));
		UNITTEST(col2.name == _T("new col name"));
		UNITTEST(col2.IsValid());
		UNITTEST(sm.CollectionRecordExists(col2.ID));
		UNITTEST(col2.name == _T("new col name"));
		UNITTEST(col.IsValid());

		//------Should Fail -Add - Update - Exists - GetById - GetUnique
		//col.ID = 0;
		//UNITTEST(!sm.AddNewCollectionRecord(col) && _T("Record is not Unique"));
		col.ID = 947;
		UNITTEST(!sm.UpdateCollectionRecord(col) && _T("Record is not Existent"));
		UNITTEST(!sm.GetCollectionRecord(col.ID, col) && _T("Record is not Existent"));
		UNITTEST(!sm.GetCollectionRecordUnique(12, col.location.c_str(), col) && _T("Record is not Existent"));
		UNITTEST(!sm.CollectionRecordExists(col.ID) && _T("Record is not Existent"));

		//---GenreRecord----------------
		GenreRecord gen;
		gen.name = _T("This is a new fucking genre with a very large name");
		gen.name += gen.name;
		gen.name += gen.name;
		gen.name += gen.name;
		INT len = gen.name.size();
		UNITTEST(sm.AddNewGenreRecord(gen));
		UNITTEST(gen.IsValid());
		gen.name = _T("new name");
		UNITTEST(sm.UpdateGenreRecord(gen, FALSE));
		UNITTEST(gen.name == _T("new name"));
		UNITTEST(gen.IsValid());
		UNITTEST(sm.GetGenreRecord(gen.ID, gen));
		UNITTEST(gen.name == _T("new name"));
		UNITTEST(gen.IsValid());
		GenreRecord gen2;
		UNITTEST(sm.GetGenreRecordUnique(gen.name.c_str(), gen2));
		UNITTEST(gen2.name == _T("new name"));
		UNITTEST(gen2.IsValid());
		UNITTEST(sm.GenreRecordExists(gen2.ID));
		UNITTEST(gen2.name == _T("new name"));
		UNITTEST(gen2.IsValid());
		//gen.ID = 0;
		//UNITTEST(!sm.AddNewGenreRecord(gen) && _T("Record is not Unique"));
		gen.ID = 947;
		UNITTEST(!sm.UpdateGenreRecord(gen, FALSE) && _T("Record is not Existent"));
		UNITTEST(!sm.GetGenreRecord(gen.ID, gen) && _T("Record is not Existent"));
		UNITTEST(!sm.GetGenreRecordUnique(_T("Not Existent"), gen) && _T("Record is not Existent"));
		UNITTEST(!sm.GenreRecordExists(gen.ID) && _T("Record is not Existent"));

		//---ArtistRecord----------------
		ArtistRecord art;
		art.name = _T("This is a new fucking artist with a very large name");
		art.name += art.name;
		art.name += art.name;
		art.name += art.name;
		art.genID = 1;
		UNITTEST(sm.AddNewArtistRecord(art));
		UNITTEST(art.IsValid());
		art.name = _T("new name");
		UNITTEST(sm.UpdateArtistRecord(art, FALSE));
		UNITTEST(art.IsValid());
		UNITTEST(art.name == _T("new name"));
		UNITTEST(sm.GetArtistRecord(art.ID, art));
		UNITTEST(art.IsValid());
		UNITTEST(art.name == _T("new name"));
		ArtistRecord art2;
		UNITTEST(sm.GetArtistRecordUnique(art.name.c_str(), art2));
		UNITTEST(art2.IsValid());
		UNITTEST(art2.name == _T("new name"));
		UNITTEST(sm.ArtistRecordExists(art2.ID));
		UNITTEST(art2.IsValid());
		UNITTEST(art2.name == _T("new name"));
		//art.ID = 0;
		//UNITTEST(!sm.AddNewArtistRecord(art) && _T("Record is not Unique"));
		art.ID = 947;
		UNITTEST(!sm.UpdateArtistRecord(art, FALSE) && _T("Record is not Existent"));
		UNITTEST(!sm.GetArtistRecord(art.ID, art) && _T("Record is not Existent"));
		UNITTEST(!sm.GetArtistRecordUnique(_T("Not Existent"), art) && _T("Record is not Existent"));
		UNITTEST(!sm.ArtistRecordExists(art.ID) && _T("Record is not Existent"));


		//---ArtistRecord----------------
		AlbumRecord alb;
		alb.name = _T("This is a new fucking album with a very large name");
		alb.name += alb.name;
		alb.name += alb.name;
		alb.name += alb.name;
		alb.artID = 1;
		UNITTEST(sm.AddNewAlbumRecord(alb));
		UNITTEST(sm.UpdateAlbumRecord(alb, FALSE));
		UNITTEST(sm.GetAlbumRecord(alb.ID, alb));
		UNITTEST(sm.GetAlbumRecordUnique(alb.artID, alb.name.c_str(), alb));
		UNITTEST(sm.AlbumRecordExists(alb.ID));
		//alb.ID = 0;
		//UNITTEST(!sm.AddNewAlbumRecord(alb) && _T("Record is not Unique"));
		alb.ID = 947;
		UNITTEST(!sm.UpdateAlbumRecord(alb, FALSE) && _T("Record is not Existent"));
		UNITTEST(!sm.GetAlbumRecord(alb.ID, alb) && _T("Record is not Existent"));
		UNITTEST(!sm.GetAlbumRecordUnique(456, _T("Not Existent"), alb) && _T("Record is not Existent"));
		UNITTEST(!sm.AlbumRecordExists(alb.ID) && _T("Record is not Existent"));


		TrackRecord tra;
		tra.name = _T("This is a new fucking track name with a very large name");
		tra.name += tra.name;
		tra.name += tra.name;
		tra.name += tra.name;
		tra.location = _T("This is a new fucking location with a very large name");
		tra.location += tra.location;
		tra.location += tra.location;
		tra.location += tra.location;
		tra.artID = 1;
		tra.genID = 1;
		tra.colID = 1;
		tra.albID = 1;
		tra.trackType = TTYPE_ac3;


		UNITTEST(sm.AddNewTrackRecord(tra));
		UNITTEST(sm.UpdateTrackRecord(tra));
		UNITTEST(sm.GetTrackRecord(tra.ID, tra));
		UNITTEST(sm.GetTrackRecordUnique(tra.colID, tra.location.c_str(), tra));
		UNITTEST(sm.TrackRecordExists(tra.ID));
		//tra.ID = 0;
		//UNITTEST(!sm.AddNewTrackRecord(tra) && _T("Record is not Unique"));
		tra.ID = 947;
		UNITTEST(!sm.UpdateTrackRecord(tra) && _T("Record is not Existent"));
		UNITTEST(!sm.GetTrackRecord(tra.ID, tra) && _T("Record is not Existent"));
		UNITTEST(!sm.GetTrackRecordUnique(456, _T("Not Existent"), tra) && _T("Record is not Existent"));
		UNITTEST(!sm.TrackRecordExists(tra.ID) && _T("Record is not Existent"));


		CollectionRecord collection;
		collection.location = _T("d:\\musi1");
		collection.type = CTYPE_LocalFolder;
		UNITTEST(sm.AddNewCollectionRecord(collection));
		UINT ID = collection.ID;

		GenreRecord genre;
		genre.name = _T("Genre#1");
		UNITTEST(sm.AddNewGenreRecord(genre));
		ID = genre.ID;
		genre.ID = ID;

		ArtistRecord artist;
		artist.name = _T("Artist#1");
		artist.genID = genre.ID;
		UNITTEST(sm.AddNewArtistRecord(artist));
		ID = artist.ID;	artist.ID = 0;
		artist.ID = ID;

		AlbumRecord album;
		album.name = _T("Album#1");
		album.artID = artist.ID;
		UNITTEST(sm.AddNewAlbumRecord(album));
		ID = album.ID;	album.ID = 0;
		album.ID = ID;

		TrackRecord track;
		track.location = _T("d:\\path#1.mp3");
		track.name = _T("Track#1");
		track.artID = artist.ID;
		track.genID = genre.ID;
		track.colID = collection.ID;
		track.albID = album.ID;
		track.trackType = GetTrackType(track.location.c_str());
		UNITTEST(sm.AddNewTrackRecord(track));
		ID = track.ID;	track.ID = 0;
		track.ID = ID;



		TagInfo tag;
		INT trackID = sm.AddNewTrackFromTagInfo(collection.ID, _T("xmm.mp3"), tag);
		UNITTEST(trackID != 0);
		tag.Title = _T("Title2#");
		//UNITTEST(!sm.AddNewTrackFromTagInfo(collection.ID, _T("xmm.mp3"), tag));
		UNITTEST(sm.UpdateTrackFromTagInfo(trackID, collection.ID, _T("xmm.mp3"), tag));
		UNITTEST(sm.UpdateTrackFromTagInfo(trackID, collection.ID, _T("222xmm.mp3"), tag));

		HistArtistRecord hart;
		hart.name = _T("Art name");
		UNITTEST(sm.AddNewHistArtistRecord(hart));
		HistArtistRecord hart2 = hart;
		hart2.ID = 0;
		//UNITTEST(!sm.AddNewHistArtistRecord(hart2));
		UNITTEST(hart.IsValid());
		UNITTEST(sm.GetHistArtistRecord(hart.ID, hart));
		UNITTEST(hart.IsValid());

		HistTrackRecord htra;
		htra.artID = hart.ID;
		htra.name = _T("track Name");
		UNITTEST(sm.AddNewHistTrackRecord(htra));
		UNITTEST(htra.IsValid());
		HistTrackRecord htra2 = htra;
		htra2.ID = 0;
		//UNITTEST(!sm.AddNewHistTrackRecord(htra2));
		UNITTEST(sm.GetHistTrackRecord(htra.ID, htra));
		UNITTEST(hart.IsValid());

		SYSTEMTIME st;
		GetLocalTime(&st);
		UNITTEST(sm.LogTrackInHistory(_T("ArtName#2"), _T("TraName#2"), HLA_Clicked, st));
		UNITTEST(sm.LogTrackInHistory(_T("ArtName#2"), _T("TraName#2"), HLA_Played, st));
		UNITTEST(sm.LogTrackInHistory(_T("ArtName#2"), _T("TraName#2"), HLA_Played, st));
		UNITTEST(sm.LogTrackInHistory(_T("ArtName#2"), _T("TraName#2"), HLA_Played, st));
		UNITTEST(sm.LogTrackInHistory(_T("ArtName#2"), _T("TraName#3"), HLA_Played, st));
		UNITTEST(sm.LogTrackInHistory(_T("ArtName#2"), _T("TraName#4"), HLA_Played, st));
		UNITTEST(sm.LogTrackInHistory(_T("ArtName#3"), _T("TraName#4"), HLA_Clicked, st));
		SQLManager::HistoryLogStats	stats;
		UNITTEST(sm.GetHistoryLogStats(stats));
		UNITTEST(stats.actions == 2);
		UNITTEST(stats.plays == 5);

	}
	DeleteFile(dbPath);
	return TRUE;

}
#endif

LPCTSTR const sDBPassword = _T("DontMessWithIt");



const INT cMaxStringLen = 256;

//==============================================================
//		INITIALIZATION - DESTRUCTION
//==============================================================

SQLManager::~SQLManager()
{
	delete m_pDB;
}


BOOL SQLManager::Init(LPCTSTR DatabasePath)
{
	TRACE(_T("@3 SQLManager::Init. DatabasePath: %s\r\n"), DatabasePath);

	ASSERT(m_pDB == NULL);//Initialize only once
	if (m_pDB != NULL)
		return FALSE;
	if (_taccess(_T("cdb.ins"), 00) == 0)
	{
		DeleteFile(_T("cdb.ins"));
		std::tstring tmp = DatabasePath;
		tmp += _T(".tmp");
		if (AdoWrap::CompactDatabase(DatabasePath, tmp.c_str(), sDBPassword))
		{
			DeleteFile(DatabasePath);
			MoveFile(tmp.c_str(), DatabasePath);
		}
	}

	BOOL newDb = FALSE;
	TCHAR sConnection[600];
	_sntprintf(sConnection, 600, _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;")
		_T("Jet OLEDB:Database Password=%s;")
		_T("Jet OLEDB:Encrypt Database=True;")
		,DatabasePath, 
		sDBPassword);
	if (!PathFileExists(DatabasePath))
	{
		TRACE(_T("@2 SQLManager::Init. Creating Database\r\n"));
		ADOXCatalog cat;
		if (cat.Init())
		{
			TRACE(_T("@3 SQLManager::Init. ADOXCatalog-CreateDatabase\r\n"));
			if (!cat.CreateDatabase(sConnection))
			{
				TRACE(_T("@0 SQLManager::Init. ADOXCatalog-CreateDatabase FAILED. Aborting\r\n"));
				return FALSE;
			}
		}
		else
		{
			TRACE(_T("@0 SQLManager::Init. ADOXCatalog failed to initialize. Aborting\r\n"));
			return FALSE;
		}
		newDb = TRUE;
	}

	TRACE(_T("@3 SQLManager::Init. Connecting to Database\r\n"));
	m_pDB = new ADODatabase;
	if (!m_pDB->Open(sConnection))
	{
		TRACE(_T("@0 SQLManager::Init. DB Connection Failed. Aborting.\r\n"));
		delete m_pDB;
		m_pDB = NULL;
		return FALSE;
	}
	TRACE(_T("@3 SQLManager::Init. DB Connection Succeeded\r\n"));
	INT latestDBVersion = 6;
	INT currentDBVersion = 0;
	if (newDb)
	{
		//===Creating Tables=============================================

		//======DBInfo======
		TRACE(_T("@3 SQLManager::Init. Creating DBInfo Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE DBInfo (version int NOT NULL)"));
		TCHAR bf[500];
		_sntprintf(bf, 500, _T("INSERT INTO DBInfo VALUES (%d)"), latestDBVersion);
		m_pDB->Execute(bf);
		currentDBVersion = latestDBVersion; 

		
		//======Genres======
		TRACE(_T("@3 SQLManager::Init. Creating Genres Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE Genres ( ")
						_T("ID COUNTER PRIMARY KEY,")
						_T("Name varchar(100) WITH COMP NOT NULL)")
						);
		m_pDB->Execute(_T("CREATE INDEX GenresName ON Genres (Name)"));
		m_pDB->Execute(_T("INSERT INTO Genres (Name) VALUES ('[Unknown]')"));

		//======Artists======
		TRACE(_T("@3 SQLManager::Init. Creating Artists Table\r\n"));
		m_pDB->Execute(_T("CREATE TABLE Artists ( ")
			_T("ID COUNTER PRIMARY KEY,")
			_T("Name varchar(100) WITH COMP NOT NULL,")
			_T("GenreID int NOT NULL,")
			_T("Rating smallint NOT NULL)")
			);
		m_pDB->Execute(	_T("CREATE INDEX ArtistsName ON Artists (Name)"));
		m_pDB->Execute(	_T("INSERT INTO Artists (Name, GenreID, Rating) VALUES ('[Unknown]', 1, 0)"));
		m_pDB->Execute(	_T("INSERT INTO Artists (Name, GenreID, Rating) VALUES ('[Various]', 1, 0)"));

		//======Albums======
		TRACE(_T("@3 SQLManager::Init. Creating Albums Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE Albums ( ")
						_T("ID COUNTER PRIMARY KEY,")
						_T("Name varchar(100) WITH COMP NOT NULL,")
						_T("artID int NOT NULL,")
						_T("[Year] smallint NOT NULL,")
						_T("Rating smallint NOT NULL)")
						);
		m_pDB->Execute(_T("CREATE INDEX AlbumsName ON Albums(Name)"));

		//======Collections======
		TRACE(_T("@3 SQLManager::Init. Creating Collections Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE Collections ( ")
						_T("ID COUNTER PRIMARY KEY,")
						_T("Name varchar(255) WITH COMP NOT NULL,")
						_T("PathName varchar(255) WITH COMP NOT NULL,")
						_T("Serial int NOT NULL,")				//Media Serial Number
						_T("Type smallint NOT NULL,")			//CollectionTypesEnum CTYPE_LocalFolder,CTYPE_Media...
						//_T("Status smallint NOT NULL,")			//CollectionStatusEnum CSTAT_Ready, CSTAT_Pending...
						_T("DateAdded date NOT NULL,")
						_T("DateUpdated date)")
						);
		m_pDB->Execute(	_T("CREATE INDEX Name ON Collections (Name)"));

		//======Tracks======
		TRACE(_T("@3 SQLManager::Init. Creating Tracks Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE Tracks ( ")
						_T("ID COUNTER PRIMARY KEY,")
						_T("Name varchar(100) WITH COMP NOT NULL,")
						_T("Type smallint NOT NULL,")	//TrackTypesEnum TTYPE_mp3,TTYPE_mp2...
						_T("DateAdded date NOT NULL,")
						_T("albID int NOT NULL,")
						_T("artID int NOT NULL,")
						_T("CollectionID int NOT NULL,")
						_T("genreID int NOT NULL,")
						_T("Path varchar(255) WITH COMP NOT NULL,")
						_T("Bitrate smallint,")			//In Kbps
						_T("Duration int,")				//In seconds
						_T("[Size] int,")
						_T("TrackNo smallint,")
						_T("[Year] smallint NOT NULL,")
						_T("Rating smallint NOT NULL)")
						);
		m_pDB->Execute(	_T("CREATE INDEX TracksName ON Tracks (Name)"));
		m_pDB->Execute(	_T("CREATE INDEX TracksPath ON Tracks (Path)"));

		//======HistArtists======
		TRACE(_T("@3 SQLManager::Init. Creating HistArtists Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE HistArtists ( ")
						_T("ID COUNTER PRIMARY KEY,")
						_T("Name varchar(100) WITH COMP NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX HistArtistsName ON HistArtists (Name)"));
		m_pDB->Execute(	_T("INSERT INTO HistArtists (Name) VALUES ('[Unknown]')"));

		//======HistTracks======
		TRACE(_T("@3 SQLManager::Init. Creating HistTracks Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE HistTracks ( ")
						_T("ID COUNTER PRIMARY KEY,")
						_T("Name varchar(100) WITH COMP NOT NULL,")
						_T("artID int NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX HistTracksName ON HistTracks (Name)"));

		//======HistLog======
		TRACE(_T("@3 SQLManager::Init. Creating HistLog Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE HistLog ( ")
						_T("ID COUNTER PRIMARY KEY,")
						_T("actID smallint NOT NULL,")
						_T("DateAdded date NOT NULL,")
						_T("trackID int NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX HistLogTrackID ON HistLog (trackID)"));

		//======TracksInfo======
		TRACE(_T("@3 SQLManager::Init. Creating TracksInfo Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE TracksInfo ( ")
			_T("ID COUNTER PRIMARY KEY,")
			_T("trackID int NOT NULL,")
			_T("Name LONGTEXT NOT NULL,")
			_T("DateAdded date NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX TracksInfoTrackID ON TracksInfo (trackID)"));
		//======AlbumsInfo======
		TRACE(_T("@3 SQLManager::Init. Creating AlbumsInfo Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE AlbumsInfo ( ")
			_T("ID COUNTER PRIMARY KEY,")
			_T("albID int NOT NULL,")
			_T("Name LONGTEXT NOT NULL,")
			_T("DateAdded date NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX AlbumsInfoAlbID ON AlbumsInfo (albID)"));
		//======ArtistsInfo======
		TRACE(_T("@3 SQLManager::Init. Creating ArtistsInfo Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE ArtistsInfo ( ")
			_T("ID COUNTER PRIMARY KEY,")
			_T("artID int NOT NULL,")
			_T("Name LONGTEXT NOT NULL,")
			_T("DateAdded date NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX ArtistsInfoArtID ON ArtistsInfo (artID)"));
		//======LocalPic======
		TRACE(_T("@3 SQLManager::Init. Creating LocalPic Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE LocalPic ( ")
			_T("parID int NOT NULL,")
			_T("infoType smallint NOT NULL,")
			_T("Path varchar(255) NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE UNIQUE INDEX LocalPicID ON LocalPic (parID, infoType)"));

		//======InfoRequests======
		TRACE(_T("@3 SQLManager::Init. Creating InfoRequests Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE InfoRequests ( ")
			_T("requestTimeStamp int NOT NULL,")
			_T("hash int NOT NULL)"));
		m_pDB->Execute(	_T("CREATE UNIQUE INDEX hashInfoRequests ON InfoRequests (parID)"));


	}
	else
	{
		ADORecordset rs(m_pDB);
		if (rs.Open(_T("SELECT version FROM DBInfo")) && !rs.IsEOF())
			currentDBVersion = rs.GetINTFieldValue(_T("version"));
	}

	if (currentDBVersion > latestDBVersion)
	{
		::MessageBox(0, 
			_T("Database (music.mdb) is not compatible with this version of Jaangle.\r\n")
			_T("Delete it or get the latest program version."), 
			_T("Database Error"), 
			MB_OK);
		return FALSE;
	}


	//Start some Initializations/correction in the database
	//m_pDB->Execute(_T("Update Collections Set Status = 1 WHERE Status <> 1"));

	BOOL bDBNeedsUpgrade = (currentDBVersion != latestDBVersion);
	if (bDBNeedsUpgrade)
		CopyFile(_T("music.mdb"), _T("music.mdb.bkp"), TRUE);

	if (currentDBVersion == 0)
	{
		TRACE(_T("@1 SQLManager::Init. Applying 0-->1 DBPatch\r\n"));
		//Alter Tables for 0.92e
		m_pDB->Execute(_T("ALTER TABLE Tracks ALTER COLUMN Name TEXT(100)"));
		m_pDB->Execute(_T("ALTER TABLE Artists ALTER COLUMN Name TEXT(100)"));
		m_pDB->Execute(_T("ALTER TABLE Albums ALTER COLUMN Name TEXT(100)"));
		m_pDB->Execute(_T("ALTER TABLE Genres ALTER COLUMN Name TEXT(100)"));
		m_pDB->Execute(_T("ALTER TABLE HistTracks ALTER COLUMN Name TEXT(100)"));
		//m_pDB->Execute(_T("ALTER TABLE HistArtists ALTER COLUMN Name TEXT(100)"));
		m_pDB->Execute(_T("CREATE TABLE DBInfo (version int NOT NULL)"));
		currentDBVersion = 1;
	}
	if (currentDBVersion == 1)
	{
		TRACE(_T("@1 SQLManager::Init. Applying 1-->2 DBPatch\r\n"));
		//Alter Tables for 0.92g. I have forgotten to change the script that
		//was creating HistArtistsTable. The name len was 70 and not 100
		m_pDB->Execute(_T("ALTER TABLE HistArtists ALTER COLUMN Name TEXT(100)"));
		currentDBVersion = 2;
	}
	if (currentDBVersion == 2)
	{
		TRACE(_T("@1 SQLManager::Init. Applying 2-->3 DBPatch START\r\n"));
		INT records = m_pDB->Execute(_T("UPDATE Albums Set Rating = 0 WHERE Rating=76"));
		records += m_pDB->Execute(_T("UPDATE Artists Set Rating = 0 WHERE Rating=76"));
		records += m_pDB->Execute(_T("UPDATE Tracks Set Rating = 0 WHERE Rating=76"));
		records += m_pDB->Execute(_T("UPDATE DBInfo SET version=3"));
		TRACE(_T("@1 SQLManager::Init. Applying 2-->3. Updated Records: %d\r\n"), records);
		currentDBVersion = 3;

	}
	if (currentDBVersion == 3)//Upgrade from 3 (0.94) ->4 (0.95 beta)
	{
		TRACE(_T("@1 SQLManager::Init. Applying 3-->4 DBPatch START\r\n"));
		//===CREATE NEW TABLES
		//======TracksInfo======
		TRACE(_T("@3 SQLManager::Init. Creating TracksInfo Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE TracksInfo ( ")
			_T("ID COUNTER PRIMARY KEY,")
			_T("trackID int NOT NULL,")
			_T("Name LONGTEXT NOT NULL,")
			_T("DateAdded date NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX TracksInfoTrackID ON TracksInfo (trackID)"));
		//======AlbumsInfo======
		TRACE(_T("@3 SQLManager::Init. Creating AlbumsInfo Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE AlbumsInfo ( ")
			_T("ID COUNTER PRIMARY KEY,")
			_T("albID int NOT NULL,")
			_T("Name LONGTEXT NOT NULL,")
			_T("DateAdded date NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX AlbumsInfoAlbID ON AlbumsInfo (albID)"));
		//======ArtistsInfo======
		TRACE(_T("@3 SQLManager::Init. Creating ArtistsInfo Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE ArtistsInfo ( ")
			_T("ID COUNTER PRIMARY KEY,")
			_T("artID int NOT NULL,")
			_T("Name LONGTEXT NOT NULL,")
			_T("DateAdded date NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE INDEX ArtistsInfoArtID ON ArtistsInfo (artID)"));
		//===TRANFER INFO
		ADORecordset rs(m_pDB);
		if (rs.Open(_T("SELECT artists.ID, info.name, info.dateAdded FROM artists INNER JOIN info on artists.infoID = info.ID WHERE artists.infoID > 0")))
		{
			ADORecordset rsInsert(m_pDB);
			if (rsInsert.Open(_T("artistsinfo"), adOpenKeyset, adLockOptimistic, adCmdTable))
			{
				std::tstring name;
				SYSTEMTIME dateAdded;
				while (!rs.IsEOF())
				{
					UINT ID = rs.GetUINTFieldValue(0);
					rs.GetStringFieldValue(1, name);
					dateAdded = rs.GetSYSTEMTIMEFieldValue(2);

					rsInsert.AddNew();
					rsInsert.SetVariantFieldValue(_T("artID"), ID);
					rsInsert.SetVariantFieldValue(_T("type"), IIT_ArtistBio);
					rsInsert.SetVariantFieldValue(_T("name"), name.c_str());
					rsInsert.SetSystemTimeFieldValue(_T("dateAdded"), dateAdded);
					if (!rsInsert.Update())
						return FALSE;
					rs.MoveNext();
				}
			}
			else
				return FALSE;
		}

		if (rs.Open(_T("SELECT albums.ID, info.name, info.dateAdded FROM albums INNER JOIN info on albums.infoID = info.ID WHERE albums.infoID > 0")))
		{
			ADORecordset rsInsert(m_pDB);
			if (rsInsert.Open(_T("albumsinfo"), adOpenKeyset, adLockOptimistic, adCmdTable))
			{
				std::tstring name;
				SYSTEMTIME dateAdded;
				while (!rs.IsEOF())
				{
					UINT ID = rs.GetUINTFieldValue(0);
					rs.GetStringFieldValue(1, name);
					dateAdded = rs.GetSYSTEMTIMEFieldValue(2);

					rsInsert.AddNew();
					rsInsert.SetVariantFieldValue(_T("albID"), ID);
					rsInsert.SetVariantFieldValue(_T("type"), IIT_AlbumReview);
					rsInsert.SetVariantFieldValue(_T("name"), name.c_str());
					rsInsert.SetSystemTimeFieldValue(_T("dateAdded"), dateAdded);
					if (!rsInsert.Update())
						return FALSE;
					rs.MoveNext();
				}
			}
			else
				return FALSE;
		}

		if (rs.Open(_T("SELECT tracks.ID, info.name, info.dateAdded FROM tracks INNER JOIN info on tracks.comID = info.ID WHERE tracks.comID > 0")))
		{
			ADORecordset rsInsert(m_pDB);
			if (rsInsert.Open(_T("tracksinfo"), adOpenKeyset, adLockOptimistic, adCmdTable))
			{
				std::tstring name;
				SYSTEMTIME dateAdded;
				while (!rs.IsEOF())
				{
					UINT ID = rs.GetUINTFieldValue(0);
					rs.GetStringFieldValue(1, name);
					dateAdded = rs.GetSYSTEMTIMEFieldValue(2);

					rsInsert.AddNew();
					rsInsert.SetVariantFieldValue(_T("trackID"), ID);
					rsInsert.SetVariantFieldValue(_T("type"), IIT_TrackComment);
					rsInsert.SetVariantFieldValue(_T("name"), name.c_str());
					rsInsert.SetSystemTimeFieldValue(_T("dateAdded"), dateAdded);
					if (!rsInsert.Update())
						return FALSE;
					rs.MoveNext();
				}
			}
			else
				return FALSE;
		}
		if (rs.Open(_T("SELECT tracks.ID, info.name, info.dateAdded FROM tracks INNER JOIN info on tracks.lyrID = info.ID WHERE tracks.lyrID > 0")))
		{
			ADORecordset rsInsert(m_pDB);
			if (rsInsert.Open(_T("tracksinfo"), adOpenKeyset, adLockOptimistic, adCmdTable))
			{
				std::tstring name;
				SYSTEMTIME dateAdded;
				while (!rs.IsEOF())
				{
					UINT ID = rs.GetUINTFieldValue(0);
					rs.GetStringFieldValue(1, name);
					dateAdded = rs.GetSYSTEMTIMEFieldValue(2);

					rsInsert.AddNew();
					rsInsert.SetVariantFieldValue(_T("trackID"), ID);
					rsInsert.SetVariantFieldValue(_T("type"), IIT_TrackLyrics);
					rsInsert.SetVariantFieldValue(_T("name"), name.c_str());
					rsInsert.SetSystemTimeFieldValue(_T("dateAdded"), dateAdded);
					if (!rsInsert.Update())
						return FALSE;
					rs.MoveNext();
				}
			}
			else
				return FALSE;
		}
		if (rs.Open(_T("SELECT tracks.ID, info.name, info.dateAdded FROM tracks INNER JOIN info on tracks.perID = info.ID WHERE tracks.perID > 0")))
		{
			ADORecordset rsInsert(m_pDB);
			if (rsInsert.Open(_T("tracksinfo"), adOpenKeyset, adLockOptimistic, adCmdTable))
			{
				std::tstring name;
				SYSTEMTIME dateAdded;
				while (!rs.IsEOF())
				{
					UINT ID = rs.GetUINTFieldValue(0);
					rs.GetStringFieldValue(1, name);
					dateAdded = rs.GetSYSTEMTIMEFieldValue(2);

					rsInsert.AddNew();
					rsInsert.SetVariantFieldValue(_T("trackID"), ID);
					rsInsert.SetVariantFieldValue(_T("type"), IIT_TrackPersonal);
					rsInsert.SetVariantFieldValue(_T("name"), name.c_str());
					rsInsert.SetSystemTimeFieldValue(_T("dateAdded"), dateAdded);
					if (!rsInsert.Update())
						return FALSE;
					rs.MoveNext();
				}
			}
			else
				return FALSE;
		}
		rs.Close();

		//DROP oldData
		m_pDB->Execute(_T("ALTER TABLE artists DROP COLUMN infoID"));
		m_pDB->Execute(_T("ALTER TABLE albums DROP COLUMN infoID"));
		m_pDB->Execute(_T("ALTER TABLE tracks DROP COLUMN comID"));
		m_pDB->Execute(_T("ALTER TABLE tracks DROP COLUMN lyrID"));
		m_pDB->Execute(_T("ALTER TABLE tracks DROP COLUMN perID"));
		m_pDB->Execute(_T("ALTER TABLE tracks DROP COLUMN SyncState"));
		m_pDB->Execute(_T("ALTER TABLE tracks DROP COLUMN WriteAbleState"));
		m_pDB->Execute(_T("DROP TABLE Info"));
		m_pDB->Execute(_T("ALTER TABLE collections DROP COLUMN status"));
		currentDBVersion = 4;

	}

	if (currentDBVersion == 4)//Upgrade from 4 (0.95 beta) ->5 (0.95 internal - 1)
	{
		//======Cached Failed Info Requests======
		TRACE(_T("@3 SQLManager::Init. Creating InfoRecordRequest Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE InfoRecordRequest ( ")
			_T("parID int NOT NULL,")
			_T("infoType smallint NOT NULL,")
			_T("result smallint NOT NULL,")
			_T("requestTimeStamp int NOT NULL)"));
		m_pDB->Execute(	_T("CREATE UNIQUE INDEX InfoRecordRequestID ON InfoRecordRequest (parID,infoType)"));
		//======LocalPic======
		TRACE(_T("@3 SQLManager::Init. Creating LocalPic Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE LocalPic ( ")
			_T("parID int NOT NULL,")
			_T("infoType smallint NOT NULL,")
			_T("Path varchar(255) NOT NULL,")
			_T("Type smallint NOT NULL)"));
		m_pDB->Execute(	_T("CREATE UNIQUE INDEX LocalPicID ON LocalPic (parID, infoType)"));


		currentDBVersion = 5;
	}

	if (currentDBVersion == 5)//Upgrade from 5 (0.95d) ->5 (0.95e)
	{
		//======Cached Failed Info Requests======

		TRACE(_T("@3 SQLManager::Init. Creating InfoRequests Table\r\n"));
		m_pDB->Execute(	_T("CREATE TABLE InfoRequests ( ")
			_T("requestTimeStamp int NOT NULL,")
			_T("hash int NOT NULL)"));
		m_pDB->Execute(	_T("CREATE UNIQUE INDEX hashInfoRequests ON InfoRequests (hash)"));
		m_pDB->Execute(_T("DROP TABLE InfoRecordRequest"));
		currentDBVersion = 6;
	}


	//--- INSERT NEW PATCHES HERE
	//--- SET THE latestDBVersion



	if (bDBNeedsUpgrade)
	{
		CreateFile(_T("cdb.ins"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		TRACE(_T("@1 SQLManager::Init. Update DB Version\r\n"));
		TCHAR bf[500];
		_sntprintf(bf, 500, _T("UPDATE DBInfo SET version = %d"), latestDBVersion);
		m_pDB->Execute(bf);
		TRACE(_T("@1 SQLManager::Init. Upgrade Finished\r\n"));
	}

	return TRUE;
}





//=============================================
//====================Public Utilities
//=============================================

INT SQLManager::AddNewTrackFromTagInfo(UINT collectionID, LPCTSTR location, TagInfo& info)
{
	TRACEST(_T("SQLManager::AddNewTrackFromTagInfo"));
	//ASSERT(info.validFields != TagInfo_None);
	ASSERT(collectionID != 0 && location != NULL);
	if (collectionID == 0)
	{
		TRACE(_T("@1 SQLManager::AddNewTrackFromTagInfo collectionID: 0\r\n"));
		return 0;
	}
	if (location == 0)
	{
		TRACE(_T("@1 SQLManager::AddNewTrackFromTagInfo location: 0\r\n"));
		return 0;
	}
	TrackRecord track;
	track.colID = collectionID;
	track.location = location;
	track.trackType = GetTrackType(location);
	::GetLocalTime(&track.dateAdded);
	if (info.IsValid(TagInfo_Bitrate))
		track.bitrate = info.Bitrate;
	if (info.IsValid(TagInfo_Length))
		track.duration = info.Length;
	if (info.IsValid(TagInfo_Title))
		track.name = info.Title;
	if (info.IsValid(TagInfo_Rating))
		track.rating = info.Rating;
	if (info.IsValid(TagInfo_Size))
		track.size = info.Size;
	if (info.IsValid(TagInfo_TrackNo))
		track.trackNo = info.TrackNo;
	if (info.IsValid(TagInfo_Year))
		track.year = info.Year;
	if (track.year < 1000)
		track.year = 0;

	//======Genres
	trim(info.Genre);
	LPCTSTR genreName = _T("");
	if (info.IsValid(TagInfo_Genre))
		genreName = info.Genre.c_str();
	GenreRecord genre;
	if (!GetGenreRecordUnique(genreName, genre))//This Genre Exists in Database
	{
		genre.name = genreName;
		if (!AddNewGenreRecord(genre))
			return 0;
	}
	track.genID = genre.ID;
	//=======Artists
	trim(info.Artist);
	LPCTSTR artistName = _T("");
	if (info.IsValid(TagInfo_Artist))
		artistName = info.Artist.c_str();
	ArtistRecord artist;
	if (!GetArtistRecordUnique(artistName, artist))//This Artist Exists in Database
	{
		artist.name = artistName;
		artist.genID = track.genID;
		if (!AddNewArtistRecord(artist))
			return 0;
	}
	track.artID = artist.ID;
	//=======Albums
	trim(info.Album);
	LPCTSTR albumName = _T("");
	if (info.IsValid(TagInfo_Album))
		albumName = info.Album.c_str();
	AlbumRecord album;
	if (!GetAlbumRecordUnique(track.artID, albumName, album))//This album Exists in Database
	{
		album.artID = track.artID;
		album.year = track.year;
		album.name = albumName;
		if (!AddNewAlbumRecord(album))
			return 0;
	}
	track.albID = album.ID;

	if (!AddNewTrackRecord(track))
		return 0;

	//=======Comment
	if (info.IsValid(TagInfo_Comment))
	{
		trim(info.Comment);
		if (!info.Comment.empty())
		{
			if (!AddNewTrackInfo(track.ID, IIT_TrackComment, info.Comment.c_str()))
				TRACE(_T("@1 SQLManager::AddNewTrackFromTagInfo Add Comments failed\r\n"));
		}
	}
	//=======Lyrics
	if (info.IsValid(TagInfo_Lyrics))
	{
		trim(info.Lyrics);
		if (!info.Lyrics.empty())
		{
			if (!AddNewTrackInfo(track.ID, IIT_TrackLyrics, info.Lyrics.c_str()))
				TRACE(_T("@1 SQLManager::AddNewTrackFromTagInfo Add Lyrics failed\r\n"));
		}
	}
	return track.ID;
}

BOOL SQLManager::UpdateTrackFromTagInfo(UINT ID, UINT collectionID, LPCTSTR location, TagInfo& info)
{
	ASSERT(ID != 0);
	ASSERT(collectionID != 0);
	ASSERT(location != NULL);
	TrackRecord track;
	if (!GetTrackRecord(ID, track))
		return FALSE;

	track.colID = collectionID;
	track.location = location;
	track.trackType = GetTrackType(location);
	//dateAdded stays the same
	if (info.IsValid(TagInfo_Bitrate))
		track.bitrate = info.Bitrate;
	if (info.IsValid(TagInfo_Length))
		track.duration = info.Length;
	if (info.IsValid(TagInfo_Title))
		track.name = info.Title;
	if (info.IsValid(TagInfo_Rating))
		track.rating = info.Rating;
	if (info.IsValid(TagInfo_Size))
		track.size = info.Size;
	if (info.IsValid(TagInfo_TrackNo))
		track.trackNo = info.TrackNo;
	if (info.IsValid(TagInfo_Year))
		track.year = info.Year;

	//======Genres
	if (info.IsValid(TagInfo_Genre))
	{
		trim(info.Genre);
		GenreRecord genre;
		if (!GetGenreRecordUnique(info.Genre.c_str(), genre))//This Genre Exists in Database
		{
			genre.name = info.Genre;
			if (!AddNewGenreRecord(genre))
				return FALSE;
		}
		track.genID = genre.ID;
	}
	//=======Artists
	if (info.IsValid(TagInfo_Artist))
	{
		trim(info.Artist);
		ArtistRecord artist;
		if (!GetArtistRecordUnique(info.Artist.c_str(), artist))//This Artist Exists in Database
		{
			artist.name = info.Artist;
			artist.genID = track.genID;
			if (!AddNewArtistRecord(artist))
				return FALSE;
		}
		track.artID = artist.ID;
		//=== Test the case that. We are changing the artist name "Doors"->"Nirvana"
		//		but not the Album Name.
		if (!info.IsValid(TagInfo_Album))
		{
			AlbumRecord existing;
			if (!GetAlbumRecord(track.albID, existing))
			{
				TRACE(_T("@1 SQLManager::UpdateTrackFromTagInfo. DB Inconsistent albID not found\r\n"));
				return 0;
			}
			if (existing.artID != artist.ID)
			{
				//=== We have changed Artist but not Album Name
				AlbumRecord album;
				if (GetAlbumRecordUnique(track.artID, existing.name.c_str(), album))
				{
					//=== An album name with the same name exists under the new artist
					//=== We just assign the album.ID to the track.albID 
				}
				else
				{
					//=== There is no Album with such name in this new artist
					//=== We are creating a new one and assigning the new albID 
					album.artID = track.artID;
					album.year = existing.year;
					album.name = existing.name.c_str();
					if (!AddNewAlbumRecord(album))
						return FALSE;
				}
				track.albID = album.ID;
			}
			else
			{
				//=== We haven' t change artist
			}
		}
	}
	//=======Albums
	if (info.IsValid(TagInfo_Album))
	{
		trim(info.Album);
		AlbumRecord album;
		if (!GetAlbumRecordUnique(track.artID, info.Album.c_str(), album))//This album Exists in Database
		{
			album.artID = track.artID;
			album.year = info.Year;
			album.name = info.Album;
			if (!AddNewAlbumRecord(album))
				return FALSE;
		}
		track.albID = album.ID;
	}

	//=======Comment
	if (info.IsValid(TagInfo_Comment))
	{
		trim(info.Comment);
		AdjustTrackInfo(track.ID, IIT_TrackComment, info.Comment.c_str());
	}
	//=======Lyrics
	if (info.IsValid(TagInfo_Lyrics))
	{
		trim(info.Lyrics);
		AdjustTrackInfo(track.ID, IIT_TrackLyrics, info.Lyrics.c_str());
	}
	return UpdateTrackRecord(track);
}


//==============================================
//=================== SQL Strings / GetField functions
//==============================================

const TCHAR* const TrackSQL = _T("SELECT Tracks.ID,[Tracks.Name],[Tracks.Type],Tracks.DateAdded,")
_T("Tracks.albID,Tracks.artID,Tracks.CollectionID,Tracks.GenreID,Tracks.Path,")
_T("Tracks.Bitrate,Tracks.Duration,[Tracks.Size],Tracks.TrackNo,")
_T("Tracks.Year,Tracks.Rating ")
_T("FROM Tracks ");

void SQLManager::GetTrackRecordFields(TrackRecord& rec, ADORecordset& rs, short& fieldID)
{
	rec.ID = rs.GetUINTFieldValue(fieldID++);
	rs.GetStringFieldValue(fieldID++, rec.name);
	rec.trackType = (TrackTypesEnum)rs.GetINTFieldValue(fieldID++);
	rec.dateAdded = rs.GetSYSTEMTIMEFieldValue(fieldID++);
	
	rec.albID = rs.GetINTFieldValue(fieldID++);
	rec.artID = rs.GetINTFieldValue(fieldID++);
	rec.colID = rs.GetINTFieldValue(fieldID++);
	rec.genID = rs.GetINTFieldValue(fieldID++);
	rs.GetStringFieldValue(fieldID++, rec.location);
	
	rec.bitrate = rs.GetINTFieldValue(fieldID++);
	rec.duration = rs.GetINTFieldValue(fieldID++);
	rec.size = rs.GetINTFieldValue(fieldID++);
	rec.trackNo = rs.GetINTFieldValue(fieldID++);
	
	rec.year = rs.GetINTFieldValue(fieldID++);
	rec.rating = rs.GetINTFieldValue(fieldID++);

	ASSERT(rec.IsValid());
}

const TCHAR* const AlbumSQL = _T("SELECT ")
_T("ID, Name, artID, Year, Rating ")
_T("FROM Albums ");

void SQLManager::GetAlbumRecordFields(AlbumRecord& album, ADORecordset& rs, short& startIdx)
{
	album.ID = rs.GetUINTFieldValue(startIdx++);
	rs.GetStringFieldValue(startIdx++, album.name);
	album.artID = rs.GetUINTFieldValue(startIdx++);
	album.year = rs.GetINTFieldValue(startIdx++);
	album.rating = rs.GetSHORTFieldValue(startIdx++);
	ASSERT(album.IsValid());
}

const TCHAR* const ArtistSQL = _T("SELECT ")
_T("ID, Name, GenreID, Rating ")
_T("FROM Artists ");

void SQLManager::GetArtistRecordFields(ArtistRecord& artist, ADORecordset& rs, short& startIdx)
{
	artist.ID = rs.GetUINTFieldValue(startIdx++);
	rs.GetStringFieldValue(startIdx++, artist.name);
	artist.genID = rs.GetUINTFieldValue(startIdx++);
	artist.rating = rs.GetSHORTFieldValue(startIdx++);
	ASSERT(artist.IsValid());
}

const TCHAR* const GenreSQL = _T("SELECT ")
_T("Genres.ID, Genres.Name ")
_T(", -1 ")
_T("FROM Genres ");

const TCHAR* const CountGenreSQL = _T("SELECT ")
_T("first(Genres.ID), first(Genres.Name)")
_T(", count(*) ")
_T("FROM Tracks INNER JOIN Genres ON Tracks.genreID = Genres.ID ");

void SQLManager::GetGenreRecordFields(GenreRecord& genre, ADORecordset& rs, short& startIdx)
{
	genre.ID = rs.GetUINTFieldValue(startIdx++);
	rs.GetStringFieldValue(startIdx++, genre.name);
	ASSERT(genre.IsValid());
}

const TCHAR* const CollectionSQL = _T("SELECT ")
_T("Collections.ID, Collections.Name, Collections.PathName ")
_T(", Collections.Serial, Collections.Type ")
_T(", Collections.DateAdded, Collections.DateUpdated ")
_T(", 0, 0, 0 ")
_T("FROM Collections ");

const TCHAR* const CountCollectionSQL = _T("SELECT ")
_T("first(Collections.ID), first(Collections.Name), first(Collections.PathName) ")
_T(", first(Collections.Serial), first(Collections.Type) ")
_T(", first(Collections.DateAdded), first(Collections.DateUpdated) ")
_T(", count(Tracks.ID), Sum([Tracks.Size]), Sum(Tracks.Duration) ")
_T("FROM Collections LEFT JOIN Tracks ON Collections.ID = Tracks.CollectionID ");



void SQLManager::GetCollectionRecordFields(CollectionRecord& rec, ADORecordset& rs, short& startIdx)
{
	rec.ID = rs.GetINTFieldValue(startIdx++);
	rs.GetStringFieldValue(startIdx++, rec.name);
	rs.GetStringFieldValue(startIdx++, rec.location);
	rec.serial = rs.GetINTFieldValue(startIdx++);
	rec.type = (CollectionTypesEnum) rs.GetINTFieldValue(startIdx++);
	//rec.status = (CollectionStatusEnum)rs.GetINTFieldValue(startIdx++);
	rec.dateAdded = rs.GetSYSTEMTIMEFieldValue(startIdx++);
	rec.dateUpdated = rs.GetSYSTEMTIMEFieldValue(startIdx++);
	ASSERT(rec.IsValid());
}

BOOL SQLManager::SetCollectionRecordFields(CollectionRecord& collection, ADORecordset& rs)
{
	INT ret = rs.SetVariantFieldValue(_T("name"), collection.name.c_str()) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("pathname"), collection.location.c_str()) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("serial"), collection.serial) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("type"), collection.type) ? 1 : 0;
	//ret += rs.SetVariantFieldValue(_T("status"), collection.status) ? 1 : 0;
	ret += rs.SetSystemTimeFieldValue(_T("dateAdded"), collection.dateAdded) ? 1 : 0;
	ret += rs.SetSystemTimeFieldValue(_T("dateUpdated"), collection.dateUpdated) ? 1 : 0;
	ASSERT(ret == 6);
	return ret == 6;
}


//const TCHAR* const InfoSQL = _T("SELECT ")
//_T("ID, Name, DateAdded ")
//_T("FROM Info ");

//void SQLManager::GetInfoRecordFields(InfoRecord& rec, ADORecordset& rs, short& fieldID)
//{
//	rec.ID = rs.GetUINTFieldValue(fieldID++);
//	rs.GetStringFieldValue(fieldID++, rec.name);
//	rec.dateAdded = rs.GetSYSTEMTIMEFieldValue(fieldID++);
//	ASSERT(rec.IsValid());
//}

//BOOL SQLManager::SetInfoRecordFields(InfoRecord& rec, ADORecordset& rs)
//{
//	INT ret = rs.SetVariantFieldValue(_T("name"), rec.name.c_str()) ? 1 : 0;
//	ret += rs.SetSystemTimeFieldValue(_T("dateAdded"), rec.dateAdded) ? 1 : 0;
//	ASSERT(ret == 2);
//	return ret == 2;
//}

const TCHAR* const HistArtistSQL = _T("SELECT ")
_T("ID, Name ")
_T("FROM HistArtists ");

void SQLManager::GetHistArtistRecordFields(HistArtistRecord& rec, ADORecordset& rs, short& startIdx)
{
	rec.ID = rs.GetUINTFieldValue(startIdx++);
	rs.GetStringFieldValue(startIdx++, rec.name);
	ASSERT(rec.IsValid());
}

const TCHAR* const HistLogSQL = _T("SELECT ")
_T("ID, actID, DateAdded, trackID ")
_T("FROM HistLog");

void SQLManager::GetHistLogRecordFields(HistLogRecord& rec, ADORecordset& rs, short& startIdx)
{
	rec.ID = rs.GetUINTFieldValue(startIdx++);
	rec.actID = (HistoryLogActionsEnum) rs.GetINTFieldValue(startIdx++);
	rec.dateAdded = rs.GetSYSTEMTIMEFieldValue(startIdx++);
	rec.trackID = rs.GetUINTFieldValue(startIdx++);
	ASSERT(rec.IsValid());
}

const TCHAR* const HistTrackSQL = _T("SELECT ")
_T("ID, Name, artID ")
_T("FROM HistTracks ");

void SQLManager::GetHistTrackRecordFields(HistTrackRecord& rec, ADORecordset& rs, short& fieldID)
{
	rec.ID = rs.GetUINTFieldValue(fieldID++);
	rs.GetStringFieldValue(fieldID++, rec.name);
	rec.artID = rs.GetUINTFieldValue(fieldID++);
	ASSERT(rec.IsValid());
}





//=============================================
//====================Retrieve Collections By SQL
//=============================================
BOOL SQLManager::GetTrackRecordCollectionBySQL(TrackRecordCollection& col, LPCTSTR sql, INT limitResults)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	INT countResults = 0;
	while (!rs.IsEOF())
	{
		TrackRecord* pRec = new TrackRecord;
		short startIdx = 0;
		GetTrackRecordFields(*pRec, rs, startIdx);
		col.push_back(TrackRecordSP(pRec));
		countResults++;
		if (countResults == limitResults)
			return TRUE;
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetGenreRecordCollectionBySQL(GenreRecordCollection& col, LPCTSTR sql)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	while (!rs.IsEOF())
	{
		GenreRecord* pRec = new GenreRecord;
		short startIdx = 0;
		GetGenreRecordFields(*pRec, rs, startIdx);
		pRec->trackCount = rs.GetINTFieldValue(startIdx++);
		col.push_back(GenreRecordSP(pRec));
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetCollectionRecordCollectionBySQL(CollectionRecordCollection& col, LPCTSTR sql)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	while (!rs.IsEOF())
	{
		CollectionRecord* pRec = new CollectionRecord;
		short startIdx = 0;
		GetCollectionRecordFields(*pRec, rs, startIdx);
		pRec->trackCount = rs.GetUINTFieldValue(startIdx++);
		pRec->sumSize = (UINT)rs.GetDOUBLEFieldValue(startIdx++);
		pRec->sumDuration = (UINT)rs.GetDOUBLEFieldValue(startIdx++);
		col.push_back(CollectionRecordSP(pRec));
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetYearRecordCollectionBySQL(YearRecordCollection& col, LPCTSTR sql)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	while (!rs.IsEOF())
	{
		YearRecord* rec = new YearRecord;
		short startIdx = 0;
		rec->year = rs.GetINTFieldValue(startIdx++);
		rec->trackCount = rs.GetINTFieldValue(startIdx++);
		col.push_back(YearRecordSP(rec));
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetMonthAddedRecordCollectionBySQL(MonthAddedRecordCollection& col, LPCTSTR sql)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	while (!rs.IsEOF())
	{
		MonthAddedRecord* rec = new MonthAddedRecord;
		short startIdx = 0;
		rec->month = rs.GetINTFieldValue(startIdx++);
		rec->trackCount = rs.GetINTFieldValue(startIdx++);
		col.push_back(MonthAddedRecordSP(rec));
		rs.MoveNext();
	}
	return TRUE;
}


BOOL SQLManager::GetFullTrackRecordCollectionBySQL(FullTrackRecordCollection& col, LPCTSTR sql, INT limitResults)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	INT countResults = 0;
	while (!rs.IsEOF())
	{
		FullTrackRecordSP rec(new FullTrackRecord);
		short fieldID = 0;
		GetTrackRecordFields(rec->track, rs, fieldID);
		GetAlbumRecordFields(rec->album, rs, fieldID);
		GetArtistRecordFields(rec->artist, rs, fieldID);
		GetGenreRecordFields(rec->genre, rs, fieldID);
		GetCollectionRecordFields(rec->collection, rs, fieldID);
		ASSERT(rec->IsValid());
		col.push_back(rec);
		countResults++;
		if (countResults == limitResults)
			return TRUE;
		rs.MoveNext();
	}
	if (col.size() == 0)
		TRACE(_T("@1 SQLManager::GetFullTrackRecordCollectionBySQL. returning empty collection\r\n"));
	return TRUE;
}

BOOL SQLManager::GetFullAlbumRecordCollectionBySQL(FullAlbumRecordCollection& col, LPCTSTR sql)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	while (!rs.IsEOF())
	{
		FullAlbumRecord* rec = new FullAlbumRecord;
		short fieldID = 0;
		GetAlbumRecordFields(rec->album, rs, fieldID);
		GetArtistRecordFields(rec->artist, rs, fieldID);
		rec->album.trackCount = rs.GetINTFieldValue(fieldID++);

		col.push_back(FullAlbumRecordSP(rec));
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetFullArtistRecordCollectionBySQL(FullArtistRecordCollection& col, LPCTSTR sql)
{
	ASSERT(sql != NULL);
	ASSERT(sql[0] != 0);
	ADORecordset rs(m_pDB);
	if (!rs.Open(sql))
		return FALSE;
	while (!rs.IsEOF())
	{
		//TCHAR bf[cMaxStringLen];//MAX_STRING_LEN
		FullArtistRecord* rec = new FullArtistRecord;
		short fieldID = 0;
		GetArtistRecordFields(rec->artist, rs, fieldID);
		GetGenreRecordFields(rec->genre, rs, fieldID);
		rec->artist.trackCount = rs.GetINTFieldValue(fieldID++);

		col.push_back(FullArtistRecordSP(rec));
		rs.MoveNext();
	}
	return TRUE;
}


//=============================================
//====================Retrieve Records By ID
//=============================================

//===============Get-Xxx-Record: Gets a record from ID
//REQUIRE:	ID!=0
//RETURN:	FALSE if xxx does not exists || failure
//			TRUE on success.

BOOL SQLManager::GetPicRecord(PicRecord& rec)
{
	ASSERT(rec.parID > 0);
	if (rec.parID == 0)
		return FALSE;
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("SELECT path,type FROM localpic WHERE parID=%d AND infoType=%d"), rec.parID, rec.infoType);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short fieldID = 0;
		rs.GetStringFieldValue(fieldID++, rec.path);
		rec.pictureType = (PictureTypeEnum)rs.GetINTFieldValue(fieldID++);
		ASSERT(rec.IsValid());
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::AddOrUpdatePicRecord(PicRecord& rec)
{
	ASSERT(rec.parID != NULL);
	if (rec.parID == 0)
		return FALSE;
	TCHAR sql[200];
	_sntprintf(sql, 200, _T("SELECT * FROM LocalPic WHERE parID=%d AND infoType=%d"), rec.parID, rec.infoType);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
	{
		rs.Edit();
	}
	else
	{
		if (rs.Open(_T("LocalPic"), adOpenKeyset, adLockOptimistic, adCmdTable))
		{
			rs.AddNew();
			rs.SetVariantFieldValue(_T("parID"), rec.parID);
			rs.SetVariantFieldValue(_T("infoType"), rec.infoType);
		}
		else
			return FALSE;
	}
	rs.SetVariantFieldValue(_T("path"), rec.path.c_str());
	rs.SetVariantFieldValue(_T("type"), rec.pictureType);
	return rs.Update();

}

BOOL SQLManager::DeletePicRecord(InfoItemTypeEnum iit, UINT parID)
{
	ASSERT(iit >= IIT_Unknown && iit < IIT_Last);
	ASSERT(parID >= 0);
	ASSERT(m_pDB != NULL);
	TCHAR sql[200];
	_sntprintf(sql, 200, _T("DELETE FROM LocalPic WHERE parID=%d AND infoType=%d"), parID, iit);
	return m_pDB->Execute(sql) != -1;
}


//BOOL SQLManager::GetInfoRecordRequestRecord(InfoRecordRequest& rec)
//{
//	ASSERT(rec.parID > 0);
//	if (rec.parID == 0)
//		return FALSE;
//	TCHAR sql[1000];
//	_sntprintf(sql, 1000, _T("SELECT result, requestTimeStamp FROM InfoRecordRequest WHERE parID=%d AND infoType=%d"), 
//		rec.parID, 
//		rec.infoType);
//	ADORecordset rs(m_pDB);
//	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
//	{
//		short fieldID = 0;
//		rec.result = (InfoRecordRequestResultEnum)rs.GetINTFieldValue(fieldID++);
//		rec.requestTimeStamp = rs.GetUINTFieldValue(fieldID++);
//		ASSERT(rec.IsValid());
//		return TRUE;
//	}
//	return FALSE;
//}

BOOL SQLManager::AddInfoRequest(UINT timeStamp, UINT hash)
{
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("INSERT INTO InfoRequests (requestTimeStamp,hash) VALUES (%u,%u)"), 
		timeStamp, 
		hash
		);
	return m_pDB->Execute(sql) == -1;
}
BOOL SQLManager::DeleteInfoRequestsOlderThan(UINT timeStampLimit)
{
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("DELETE FROM InfoRequests WHERE requestTimeStamp<%u"), timeStampLimit);
	return m_pDB->Execute(sql) != - 1;
}

BOOL SQLManager::GetInfoRequests(std::set<UINT>& requestSet)
{
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("SELECT hash FROM InfoRequests"), adOpenForwardOnly, adLockReadOnly, adCmdText))
	{
		while (!rs.IsEOF())
		{
			requestSet.insert(rs.GetUINTFieldValue(0));
			rs.MoveNext();
		}
	}
	return TRUE;
}



BOOL SQLManager::GetTrackRecord(UINT ID, TrackRecord& rec)
{
	ASSERT(ID > 0);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), TrackSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetTrackRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}
BOOL SQLManager::GetAlbumRecord(UINT ID, AlbumRecord& rec)
{
	ASSERT(ID > 0);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), AlbumSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetAlbumRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetArtistRecord(UINT ID, ArtistRecord& rec)
{
	ASSERT(ID > 0);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), ArtistSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetArtistRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetGenreRecord(UINT ID, GenreRecord& rec)
{
	ASSERT(ID > 0);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), GenreSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetGenreRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}
//BOOL SQLManager::GetInfoRecord(UINT ID, InfoRecord& rec)
//{
//	ASSERT(ID > 0);
//	TCHAR sql[1000];
//	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), InfoSQL, ID);
//	ADORecordset rs(m_pDB);
//	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
//	{
//		short startIdx = 0;
//		GetInfoRecordFields(rec, rs, startIdx);
//		return TRUE;
//	}
//	return FALSE;
//}
BOOL SQLManager::GetCollectionRecord(UINT ID, CollectionRecord& rec)
{
	ASSERT(ID > 0);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), CollectionSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetCollectionRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}
BOOL SQLManager::GetHistTrackRecord(UINT ID, HistTrackRecord& rec)
{
	ASSERT(ID > 0);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), HistTrackSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetHistTrackRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetHistArtistRecord(UINT ID, HistArtistRecord& rec)
{
	ASSERT(ID > 0);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), HistArtistSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetHistArtistRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetHistLogRecord(UINT ID, HistLogRecord& rec)
{
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE ID=%d"), HistLogSQL, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetHistLogRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}

//=============================================
//====================Retrieve Records By Unique Key
//=============================================

//===============Get-Xxx-RecordUnique: Gets a record from ID
//REQUIRE:	ID!=0
//RETURN:	FALSE if xxx does not exists || failure
//			TRUE on success.
BOOL SQLManager::GetTrackRecordUnique(UINT colID, LPCTSTR location, TrackRecord& rec)
{
	ASSERT(colID > 0);
	ASSERT(location != NULL);
	if (location == NULL)
		return FALSE;
	std::tstring fname(location);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, 
		_T("%s WHERE collectionID=%d AND path='%s' "), 
		TrackSQL, 
		colID,
		fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetTrackRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetAlbumRecordUnique(UINT artID, LPCTSTR name, AlbumRecord& album)
{
	ASSERT(name != NULL);
#ifdef USE_CACHED_QUERIES
	if (sAlbumCache.ID != 0 && sAlbumCache.artID == artID && sAlbumCache.name == name)
	{
		album = sAlbumCache;
		return TRUE;
	}
#endif // USE_CACHED_QUERIES
	ADORecordset rs(m_pDB);
	std::tstring fname(name);
	if (fname.empty())
		fname = TS_UnknownString;
	else
		replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, 
		_T("%s WHERE name='%s' AND artID=%d"), 
		AlbumSQL,
		fname.c_str(), 
		artID);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetAlbumRecordFields(album, rs, startIdx);
#ifdef USE_CACHED_QUERIES
		sAlbumCache = album;
#endif // USE_CACHED_QUERIES
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetArtistRecordUnique(LPCTSTR name, ArtistRecord& artist)
{
	ASSERT(name != NULL);
	if (name[0] == 0)
	{
		artist.ID = 1;
		artist.name = name;
		return TRUE;
	}
#ifdef USE_CACHED_QUERIES
	if (sArtistCache.ID != 0 && sArtistCache.name == name)
	{
		artist = sArtistCache;
		return TRUE;
	}
#endif // USE_CACHED_QUERIES
	std::tstring fname(name);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, 
		_T("%s WHERE name='%s' "), 
		ArtistSQL, 
		fname.c_str());

	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetArtistRecordFields(artist, rs, startIdx);
#ifdef USE_CACHED_QUERIES
		sArtistCache = artist;
#endif // USE_CACHED_QUERIES
		return TRUE;
	}
	return FALSE;
}



BOOL SQLManager::GetGenreRecordUnique(LPCTSTR name, GenreRecord& genre)
{
	ASSERT(name != NULL);
	if (name[0] == 0)
	{
		genre.ID = 1;
		genre.name = TS_UnknownString;
		return TRUE;
	}
#ifdef USE_CACHED_QUERIES
	if (sGenreCache.ID != 0 && sGenreCache.name == name)
	{
		genre = sGenreCache;
		return TRUE;
	}
#endif // USE_CACHED_QUERIES
	std::tstring fname(name);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE name='%s'"), GenreSQL, fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetGenreRecordFields(genre, rs, startIdx);
#ifdef USE_CACHED_QUERIES
		sGenreCache = genre;
#endif // USE_CACHED_QUERIES
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetCollectionRecordUnique(UINT serial, LPCTSTR location, CollectionRecord& rec)
{
	ASSERT(location != NULL);
	std::tstring fname(location);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, 
		_T("%s WHERE serial=%d AND pathName='%s'"), 
		CollectionSQL, 
		serial,
		fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetCollectionRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::GetHistTrackRecordUnique(UINT artID, LPCTSTR name, HistTrackRecord& rec)
{
	ASSERT(artID != 0 && name != NULL);
	if (artID == 0 || name == NULL)
		return FALSE;
	std::tstring fname(name);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, 
		_T("%s WHERE artID=%d AND name='%s'"), 
		HistTrackSQL, 
		artID,
		fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetHistTrackRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}
BOOL SQLManager::GetHistArtistRecordUnique(LPCTSTR name, HistArtistRecord& rec)
{
	ASSERT(name != NULL);
	if (name == NULL)
		return FALSE;
	std::tstring fname(name);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, 
		_T("%s WHERE name='%s'"), 
		HistArtistSQL, 
		fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql) && !rs.IsEOF())
	{
		short startIdx = 0;
		GetHistArtistRecordFields(rec, rs, startIdx);
		return TRUE;
	}
	return FALSE;
}



//===============Get-Xxx-UD: Gets the ID
//REQUIRE:	...
//RETURN:	0 if xxx does not exists || failure
//			Database ID on success.
UINT SQLManager::GetTrackID(UINT colID, LPCTSTR location)
{
	ASSERT(colID > 0);
	ASSERT(location != NULL);
	std::tstring fname(location);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE collectionID=%d AND path='%s'"), TrackSQL, colID, fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
		return rs.GetUINTFieldValue(_T("ID"));
	return 0;
}
UINT SQLManager::GetAlbumID(UINT artID, LPCTSTR name)
{
	ASSERT(name != NULL);
	std::tstring fname(name);
	if (fname.empty())
		fname = TS_UnknownString;
	else
		replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("SELECT * FROM albums WHERE name='%s' AND artID=%d"), fname.c_str(), artID);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
		return rs.GetUINTFieldValue(_T("ID"));
	return 0;
}
UINT SQLManager::GetArtistID(LPCTSTR name)
{
	ASSERT(name != NULL);
	if (name[0] == 0)
		return 1;
	std::tstring fname(name);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE name='%s' "), ArtistSQL, fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
		return rs.GetUINTFieldValue(_T("ID"));
	return 0;
}
UINT SQLManager::GetGenreID(LPCTSTR name)
{
	ASSERT(name != NULL);
	if (name[0] == 0)
		return 1;
	std::tstring fname(name);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE name='%s'"), GenreSQL, fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
		return rs.GetUINTFieldValue(_T("ID"));
	return 0;

}
UINT SQLManager::GetCollectionID(UINT serial, LPCTSTR location)
{
	ASSERT(location != NULL);
	std::tstring fname(location);
	replace(fname, _T("'"), _T("''"));
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("%s WHERE serial=%d AND pathName='%s'"), 
		CollectionSQL, serial, fname.c_str());
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
		return rs.GetUINTFieldValue(_T("ID"));
	return 0;

}
//UINT SQLManager::GetHistTrackID(UINT artID, LPCTSTR name)
//{
//
//}
//UINT SQLManager::GetHistArtistID(LPCTSTR name)
//{
//
//}





//=============================================
//====================AddNew Records
//=============================================

//===============AddNew-Xxx-Record: Adds a new Xxx record
//REQUIRE:	xxx with ID==0 && name != "" && foreign keys valid
//RETURN:	FALSE if xxx exists || failure
//			TRUE on success. Modifies ID with the new ID
BOOL SQLManager::AddNewTrackRecord(TrackRecord& rec)
{
	ASSERT(rec.ID == 0);
	ASSERT(!rec.location.empty());
	ASSERT(rec.artID != 0);
	ASSERT(rec.albID != 0);
	ASSERT(rec.genID != 0);
	ASSERT(rec.colID != 0);
	ASSERT(rec.trackType != TTYPE_Unsupported);
	if (rec.ID != 0 || rec.location.empty() || rec.artID == 0 || rec.albID == 0 ||
		rec.genID == 0 || rec.colID == 0 || rec.trackType == TTYPE_Unsupported)
		return FALSE;
	if (rec.name.empty())
		rec.name = TS_UnknownString;
#ifdef USE_INTEGRITY_CHECKS
	TrackRecord existing;
	if (GetTrackRecordUnique(rec.colID, rec.location.c_str(), existing))
		return FALSE;
	if (!ArtistRecordExists(rec.artID))			return FALSE;
	if (!AlbumRecordExists(rec.albID))			return FALSE;
	if (!GenreRecordExists(rec.genID))			return FALSE;
	if (!CollectionRecordExists(rec.colID))		return FALSE;
#endif // USE_INTEGRITY_CHECKS
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("tracks"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		SetTrackRecordFields(rec, rs);
		if (rs.Update())
		{
			m_changes.tracks.additions++;
			rec.ID = rs.GetUINTFieldValue("ID");
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SQLManager::AddNewAlbumRecord(AlbumRecord& rec)
{
	ASSERT(rec.ID == 0);
	ASSERT(rec.artID != 0);
	if (rec.name.empty())
		rec.name = TS_UnknownString;
#ifdef USE_CACHED_QUERIES
	sAlbumCache.ID = 0;
#endif // USE_CACHED_QUERIES

	if (rec.ID != 0 || rec.artID == 0)
		return FALSE;
#ifdef USE_INTEGRITY_CHECKS
	AlbumRecord existing;
	if (GetAlbumRecordUnique(rec.artID, rec.name.c_str(), existing))
		return FALSE;
	if (!ArtistRecordExists(rec.artID))			return FALSE;
#endif // USE_INTEGRITY_CHECKS
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("albums"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		SetAlbumRecordFields(rec, rs);
		if (rs.Update())
		{
			m_changes.albums.additions++;
			rec.ID = rs.GetUINTFieldValue("ID");
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SQLManager::AddNewArtistRecord(ArtistRecord& rec)
{
	ASSERT(rec.ID == 0);
	ASSERT(!rec.name.empty());
	ASSERT(rec.genID != 0);
	if (rec.ID != 0 || rec.genID == 0 || rec.name.empty())
		return FALSE;
#ifdef USE_CACHED_QUERIES
	sArtistCache.ID = 0;
#endif // USE_CACHED_QUERIES

#ifdef USE_INTEGRITY_CHECKS
	ArtistRecord existing;
	if (GetArtistRecordUnique(rec.name.c_str(), existing))
		return FALSE;
	if (!GenreRecordExists(rec.genID))			return FALSE;
#endif // USE_INTEGRITY_CHECKS
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("artists"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		SetArtistRecordFields(rec, rs);
		if (rs.Update())
		{
			m_changes.artists.additions++;
			rec.ID = rs.GetUINTFieldValue(_T("ID"));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SQLManager::AddNewGenreRecord(GenreRecord& rec)
{
	ASSERT(rec.ID == 0);
	ASSERT(!rec.name.empty());
	if (rec.ID != 0 || rec.name.empty())
		return FALSE;
#ifdef USE_CACHED_QUERIES
	sGenreCache.ID = 0;
#endif // USE_CACHED_QUERIES

#ifdef USE_INTEGRITY_CHECKS
	GenreRecord existing;
	if (GetGenreRecordUnique(rec.name.c_str(), existing))
		return FALSE;
#endif // USE_INTEGRITY_CHECKS
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("genres"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		SetGenreRecordFields(rec, rs);
		if (rs.Update())
		{
			m_changes.genres.additions++;
			rec.ID = rs.GetUINTFieldValue("ID");
			return TRUE;
		}
	}
	return FALSE;
}

//BOOL SQLManager::AddNewInfoRecord(InfoRecord& rec)
//{
//	ASSERT(rec.ID == 0);
//	ASSERT(!rec.name.empty());
//	if (rec.ID != 0 || rec.name.empty())
//		return FALSE;
//	ADORecordset rs(m_pDB);
//	if (rs.Open(_T("info"), adOpenKeyset, adLockOptimistic, adCmdTable))
//	{
//		rs.AddNew();
//		SetInfoRecordFields(rec, rs);
//		if (rs.Update())
//		{
//			m_changes.info.additions++;
//			rec.ID = rs.GetUINTFieldValue("ID");
//			return TRUE;
//		}
//	}
//	return FALSE;
//}

BOOL SQLManager::AddNewCollectionRecord(CollectionRecord& rec)
{
	ASSERT(rec.ID == 0);
	ASSERT(!rec.location.empty());	
	ASSERT(rec.type > CTYPE_Unknown && rec.type < CTYPE_Last);
	//ASSERT(rec.status >= CSTAT_Unknown && rec.status < CSTAT_Last);
	if(rec.ID) return FALSE;
	if(rec.location.empty()) return FALSE;
	if(!(rec.type > CTYPE_Unknown && rec.type < CTYPE_Last)) return FALSE;
	//if (rec.status == CSTAT_Unknown)	rec.status = CSTAT_Ready;
	//if(!(rec.status > CSTAT_Unknown && rec.status < CSTAT_Last)) return FALSE;
	if (rec.name.empty())
		rec.name = rec.location;
#ifdef USE_INTEGRITY_CHECKS
	CollectionRecord existing;
	if (GetCollectionRecordUnique(rec.serial, rec.location.c_str(), existing))
		return FALSE;
#endif // USE_INTEGRITY_CHECKS
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("collections"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		if (rec.name.empty())
			rec.name = rec.location;
		::GetLocalTime(&rec.dateAdded);
		::GetLocalTime(&rec.dateUpdated);
		SetCollectionRecordFields(rec, rs);
		if (rs.Update())
		{
			m_changes.collections.additions++;
			rec.ID = rs.GetUINTFieldValue("ID");
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SQLManager::AddNewHistArtistRecord(HistArtistRecord& rec)
{
	ASSERT(rec.ID == 0 && !rec.name.empty());
	if(rec.ID != 0 || rec.name.empty()) 
		return FALSE;
#ifdef USE_INTEGRITY_CHECKS
	HistArtistRecord existing;
	if (GetHistArtistRecordUnique(rec.name.c_str(), existing))
		return FALSE;
#endif // USE_INTEGRITY_CHECKS
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("histartists"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		INT ret = rs.SetVariantFieldValue(_T("name"), rec.name.c_str()) ? 1 : 0;
		ASSERT(ret == 1);
		if (rs.Update())
		{
			rec.ID = rs.GetUINTFieldValue("ID");
			return TRUE;
		}
	}
	return FALSE;
}



BOOL SQLManager::AddNewHistTrackRecord(HistTrackRecord& rec)
{
	ASSERT(rec.ID == 0 && !rec.name.empty() && rec.artID != 0);
	if(rec.ID != 0 || rec.name.empty() || rec.artID == 0) 
		return FALSE;
#ifdef USE_INTEGRITY_CHECKS
	HistTrackRecord existing;
	if (GetHistTrackRecordUnique(rec.artID, rec.name.c_str(), existing))
		return FALSE;
#endif // USE_INTEGRITY_CHECKS
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("histtracks"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		INT ret = rs.SetVariantFieldValue(_T("name"), rec.name.c_str()) ? 1 : 0;
		ret += rs.SetVariantFieldValue(_T("artID"), rec.artID) ? 1 : 0;
		ASSERT(ret == 2);
		if (rs.Update())
		{
			rec.ID = rs.GetUINTFieldValue("ID");
			return TRUE;
		}
	}
	return FALSE;
}
BOOL SQLManager::AddNewHistLogRecord(HistLogRecord& rec)
{
	ASSERT(rec.ID == 0 && rec.actID > HLA_Unknown && rec.actID < HLA_Last && rec.trackID);
	if(!(rec.ID == 0 && rec.actID > HLA_Unknown && rec.actID < HLA_Last && rec.trackID)) 
		return FALSE;
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("histlog"), adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		INT ret = rs.SetVariantFieldValue(_T("actID"), rec.actID) ? 1 : 0;
		ret += rs.SetVariantFieldValue(_T("trackID"), rec.trackID) ? 1 : 0;
		ret += rs.SetSystemTimeFieldValue(_T("dateAdded"), rec.dateAdded) ? 1 : 0;
		ASSERT(ret == 3);
		if (rs.Update())
		{
			rec.ID = rs.GetUINTFieldValue("ID");
			return TRUE;
		}
	}
	return FALSE;

}



//===============Update-Xxx-Record: Updates a Xxx record
//REQUIRE:	xxx with ID!=0 && name != "" && foreign keys valid
//RETURN:	FALSE on failure
//			TRUE on success.
BOOL SQLManager::UpdateTrackRecord(TrackRecord& rec)
{
	ASSERT(rec.ID > 0);//0:Invalid
	ASSERT(!rec.location.empty());
	ASSERT(rec.artID != 0);
	ASSERT(rec.albID != 0);
	ASSERT(rec.genID != 0);
	ASSERT(rec.colID != 0);
	ASSERT(rec.trackType > TTYPE_Unsupported && rec.trackType < TTYPE_Last);
	if (rec.ID == 0 || rec.location.empty() || rec.artID == 0 || rec.albID == 0 ||
		rec.genID == 0 || rec.colID == 0 || rec.trackType == TTYPE_Unsupported)
		return FALSE;
	if (rec.name.empty())
		rec.name = TS_UnknownString;
#ifdef USE_INTEGRITY_CHECKS
	TrackRecord existingINT;
	if (GetTrackRecordUnique(rec.colID, rec.location.c_str(), existingINT))
	{
		if (existingINT.ID != rec.ID)
			return FALSE;
	}
	if (!ArtistRecordExists(rec.artID))			return FALSE;
	if (!AlbumRecordExists(rec.albID))			return FALSE;
	if (!GenreRecordExists(rec.genID))			return FALSE;
	if (!CollectionRecordExists(rec.colID))		return FALSE;
#endif // USE_INTEGRITY_CHECKS

	TrackRecord existing;
	GetTrackRecordUnique(rec.colID, rec.location.c_str(), existing);
	if (existing.ID == 0 || existing.ID == rec.ID)
	{
		ADORecordset rs(m_pDB);
		TCHAR sql[200];
		_sntprintf(sql, 200, _T("SELECT * FROM tracks WHERE ID=%d"), rec.ID);
		if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
		{
			rs.Edit();
			SetTrackRecordFields(rec, rs);
			m_changes.tracks.updates++;
			return rs.Update();
		}
	}
	return FALSE;

}
BOOL SQLManager::UpdateAlbumRecord(AlbumRecord& rec, BOOL bForce)
{
	ASSERT(rec.ID != 0);
	ASSERT(rec.artID != 0);
	ASSERT(rec.IsValid());
	if (!(rec.artID != 0))
		return FALSE;
	sAlbumCache.ID = 0;
	if (rec.name.empty())
		rec.name = TS_UnknownString;
	//We must check for name duplication
	AlbumRecord existing;
	GetAlbumRecordUnique(rec.artID, rec.name.c_str(), existing);
	if (existing.ID == 0 || existing.ID == rec.ID)
	{
		TRACE(_T("@4 SQLManager::UpdateAlbumRecord. Updating. Existing: %d\r\n"), existing.ID);
		ADORecordset rs(m_pDB);
		TCHAR sql[200];
		_sntprintf(sql, 200, _T("SELECT * FROM albums WHERE ID=%d"), rec.ID);
		if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
		{
			rs.Edit();
			SetAlbumRecordFields(rec, rs);
			m_changes.albums.updates++;
			return rs.Update();
		}
	}
	else if (bForce)
	{
		TRACE(_T("@4 SQLManager::UpdateAlbumRecord. There is already a record that has the same name\r\n"));
		//There is already a record that has the same name as this one.
		//FORCE WILL Keep the integrity of the database by
		//	1. Update all Track Records to the existing ID
		TCHAR sql[500];
		_sntprintf(sql, 500, _T("UPDATE tracks SET albID=%d WHERE albID=%d"), existing.ID, rec.ID);
		m_pDB->Execute(sql);
		//	2. Delete all the info related to this record
		DeleteAllAlbumInfo(rec.ID);
		//	3. Delete This record
		m_changes.albums.removals++;
		_sntprintf(sql, 500, _T("DELETE FROM albums WHERE ID=%d"), rec.ID);
		m_pDB->Execute(sql);
		rec = existing;
		return TRUE;
	}
	TRACE(_T("@1 SQLManager::UpdateAlbumRecord. Not forced. Do nothing\r\n"));
	return FALSE;

}
BOOL SQLManager::UpdateArtistRecord(ArtistRecord& rec, BOOL bForce)
{
	ASSERT(rec.ID > 2);
	ASSERT(rec.genID != 0);
	ASSERT(rec.IsValid());
	//We must check for name duplication
	if (!(rec.ID > 2 && rec.genID != 0 && rec.IsValid()))
		return FALSE;
	sArtistCache.ID = 0;
	ArtistRecord existing;
	GetArtistRecordUnique(rec.name.c_str(), existing);
	if (existing.ID == 0 || existing.ID == rec.ID)
	{
		TRACE(_T("@4 SQLManager::UpdateArtistRecord. Updating. Existing: %d\r\n"), existing.ID);
		ADORecordset rs(m_pDB);
		TCHAR sql[200];
		_sntprintf(sql, 200, _T("SELECT * FROM artists WHERE ID=%d"), rec.ID);
		if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
		{
			rs.Edit();
			SetArtistRecordFields(rec, rs);
			m_changes.artists.updates++;
			return rs.Update();
		}
	}
	else if (bForce)
	{
		TRACE(_T("@4 SQLManager::UpdateArtistRecord. There is already a record that has the same name\r\n"));

		//There is already a record that has the same name as this one.
		//FORCE WILL Keep the integrity of the database by
		//	1. Update all Track Records to the existing ID
		TCHAR sql[500];
		_sntprintf(sql, 500, _T("UPDATE tracks SET artID=%d WHERE artID=%d"), existing.ID, rec.ID);
		m_changes.tracks.updates += m_pDB->Execute(sql);
		//	2. Update all Album Records the same way
		//There may be ALBUM DUPLICATION ********** BUGGGGGGGGGGGG
		ADORecordset rs(m_pDB);
		_sntprintf(sql, 500, _T("SELECT * FROM albums WHERE artID=%d"), rec.ID);
		if (rs.Open(sql))
		{
			AlbumRecord al;
			while (!rs.IsEOF())
			{
				short idx = 0;
				GetAlbumRecordFields(al, rs, idx);
				al.artID = existing.ID;
				UpdateAlbumRecord(al, TRUE);
				rs.MoveNext();
			}
		}
		//_sntprintf(sql, 500, _T("UPDATE albums SET artID=%d WHERE artID=%d"), existing.ID, rec.ID);
		//m_changes.albums.updates += m_pDB->Execute(sql);
		//	3. Delete all the info related to this record
		DeleteAllArtistInfo(rec.ID);
		//	4. Delete This record
		_sntprintf(sql, 500, _T("DELETE FROM artists WHERE ID=%d"), rec.ID);
		m_changes.artists.removals += m_pDB->Execute(sql);
		rec = existing;
		return TRUE;
	}
	TRACE(_T("@1 SQLManager::UpdateArtistRecord. Not forced. Do nothing\r\n"));
	return FALSE;
}
BOOL SQLManager::UpdateGenreRecord(GenreRecord& rec, BOOL bForce)
{
	ASSERT(rec.ID != 0);
	ASSERT(rec.name.empty() == FALSE);
	sGenreCache.ID = 0;
	//We must check for name duplication
	GenreRecord existing;
	GetGenreRecordUnique(rec.name.c_str(), existing);
	if (existing.ID == 0 || existing.ID == rec.ID)
	{
		ADORecordset rs(m_pDB);
		TCHAR sql[200];
		_sntprintf(sql, 200, _T("SELECT * FROM genres WHERE ID=%d"), rec.ID);
		if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
		{
			rs.Edit();
			SetGenreRecordFields(rec, rs);
			m_changes.genres.updates++;
			return rs.Update();
		}
	}
	else if (bForce)
	{
		//There is already a record that has the same name as this one.
		//FORCE WILL Keep the integrity of the database by
		//	1. Update all Track Records to the existing ID
		TCHAR sql[500];
		_sntprintf(sql, 500, _T("UPDATE tracks SET genID=%d WHERE genID=%d"), existing.ID, rec.ID);
		m_pDB->Execute(sql);
		//	2. Update all Artists Records the same way
		_sntprintf(sql, 500, _T("UPDATE artists SET genID=%d WHERE genID=%d"), existing.ID, rec.ID);
		m_pDB->Execute(sql);
		//	3. Delete This record
		_sntprintf(sql, 500, _T("DELETE FROM genres WHERE ID=%d"), rec.ID);
		m_changes.genres.removals++;
		m_pDB->Execute(sql);
		rec = existing;
		return TRUE;
	}

	return FALSE;

}
//BOOL SQLManager::UpdateInfoRecord(InfoRecord& rec)
//{
//	ASSERT(rec.ID != 0);
//	ASSERT(rec.name.empty() == FALSE);
//	ADORecordset rs(m_pDB);
//	TCHAR sql[200];
//	_sntprintf(sql, 200, _T("SELECT * FROM info WHERE ID=%d"), rec.ID);
//	if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
//	{
//		rs.Edit();
//		SetInfoRecordFields(rec, rs);
//		m_changes.info.updates++;
//		return rs.Update();
//	}
//	return FALSE;
//}

//BOOL SQLManager::AdjustInfoRecord(InfoRecord& rec)
//{
//	if (rec.ID == 0)
//	{
//		if (rec.name.empty())
//			return TRUE;
//		else
//			return AddNewInfoRecord(rec);
//	}
//	else
//	{
//		if (rec.name.empty())
//		{
//			UINT ID = rec.ID;
//			rec.ID = 0;
//			return DeleteInfoRecord(ID);
//		}
//		else
//			return UpdateInfoRecord(rec);
//	}
//
//	return FALSE;
//}



BOOL SQLManager::UpdateCollectionRecord(CollectionRecord& rec)
{
	ASSERT(rec.ID != 0);
	ASSERT(rec.location.empty() == FALSE);
	if (rec.name.empty())
		rec.name = rec.location;
	//ASSERT(rec.status > CSTAT_Unknown && rec.status < CSTAT_Last);
	ASSERT(rec.type > CTYPE_Unknown && rec.type < CTYPE_Last);
	//We must check for name duplication
	CollectionRecord existing;
	GetCollectionRecordUnique(rec.serial, rec.location.c_str(), existing);
	if (existing.ID == 0 || existing.ID == rec.ID)
	{
		ADORecordset rs(m_pDB);
		TCHAR sql[200];
		_sntprintf(sql, 200, _T("SELECT * FROM collections WHERE ID=%d"), rec.ID);
		if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
		{
			m_changes.collections.updates++;
			rs.Edit();
			SetCollectionRecordFields(rec, rs);
			return rs.Update();
		}
	}
	return FALSE;
}

BOOL SQLManager::UpdateHistTrackRecord(HistTrackRecord& rec)
{
	ASSERT(0);
	return FALSE;
}
BOOL SQLManager::UpdateHistLogRecord(HistLogRecord& rec)
{
	ASSERT(0);
	return FALSE;
}
BOOL SQLManager::UpdateHistArtistRecord(HistArtistRecord& rec)
{
	ASSERT(0);
	return FALSE;
}


//===============Xxx-RecordExists: Test if a record exist
//REQUIRE:	ID!=0
//RETURN:	FALSE if xxx does not exists || failure
//			TRUE on success.
BOOL SQLManager::RecordExists(LPCTSTR sql, UINT ID)
{
	ASSERT(ID > 0);
	TCHAR q[1000];
	_sntprintf(q, 1000, _T("%s WHERE ID=%d"), sql, ID);
	ADORecordset rs(m_pDB);
	if (rs.Open(q, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
		return TRUE;
	TRACE(_T("@1 SQLManager::RecordExists. Not existent '%s'\r\n"), q);
	return FALSE;
}
BOOL SQLManager::TrackRecordExists(UINT ID)
{
	return RecordExists(TrackSQL, ID);
}
BOOL SQLManager::AlbumRecordExists(UINT ID)
{
	return RecordExists(AlbumSQL, ID);
}
BOOL SQLManager::ArtistRecordExists(UINT ID)
{
	return RecordExists(ArtistSQL, ID);
}
BOOL SQLManager::GenreRecordExists(UINT ID)
{
	return RecordExists(GenreSQL, ID);
}
//BOOL SQLManager::InfoRecordExists(UINT ID)
//{
//	return RecordExists(InfoSQL, ID);
//}
BOOL SQLManager::CollectionRecordExists(UINT ID)
{
	return RecordExists(CollectionSQL, ID);
}
BOOL SQLManager::HistTrackRecordExists(UINT ID)
{
	return RecordExists(HistTrackSQL, ID);
}
BOOL SQLManager::HistArtistRecordExists(UINT ID)
{
	return RecordExists(HistArtistSQL, ID);
}
BOOL SQLManager::HistLogRecordExists(UINT ID)
{
	return RecordExists(HistLogSQL, ID);
}






BOOL SQLManager::UpdateFullArtistRecord(FullArtistRecord& artist, BOOL bForce)
{
	//======Genres
	GenreRecord genre;
	if (!GetGenreRecordUnique(artist.genre.name.c_str(), genre))//This Genre Exists in Database
	{
		genre.name = artist.genre.name.c_str();
		if (!AddNewGenreRecord(genre))
			return FALSE;
	}
	artist.artist.genID = artist.genre.ID = genre.ID;
	//=======Artists
	return UpdateArtistRecord(artist.artist, bForce);
}

BOOL SQLManager::UpdateFullAlbumRecord(FullAlbumRecord& album, BOOL bForce)
{
	//======Artist
	ArtistRecord artist;
	if (!GetArtistRecordUnique(album.artist.name.c_str(), artist))//This Genre Exists in Database
	{
		artist.name = album.artist.name.c_str();
		if (!AddNewArtistRecord(artist))
			return FALSE;
	}
	album.album.artID = album.artist.ID = artist.ID;
	//=======Album
	return UpdateAlbumRecord(album.album, bForce);
}

BOOL SQLManager::UpdateFullTrackRecord(FullTrackRecord& track)
{
	ASSERT(track.track.ID != 0);
	ASSERT(track.track.colID == track.collection.ID);
	ASSERT(track.track.colID != 0);
	ASSERT(!track.track.location.empty());
	//======Genres
	GenreRecord genre;
	if (!GetGenreRecordUnique(track.genre.name.c_str(), genre))//This Genre Exists in Database
	{
		genre.name = track.genre.name.c_str();
		if (!AddNewGenreRecord(genre))
			return FALSE;
	}
	track.track.genID = track.genre.ID = genre.ID;
	//======Artist
	ArtistRecord artist;
	if (!GetArtistRecordUnique(track.artist.name.c_str(), artist))//This Genre Exists in Database
	{
		artist.name = track.artist.name.c_str();
		artist.genID = genre.ID;
		if (!AddNewArtistRecord(artist))
			return FALSE;
	}
	track.track.artID = track.artist.ID = artist.ID;

	//======Album
	AlbumRecord album;
	if (!GetAlbumRecordUnique(artist.ID, track.album.name.c_str(), album))//This Genre Exists in Database
	{
		album.name = track.album.name.c_str();
		album.artID = artist.ID;
		if (!AddNewAlbumRecord(album))
			return FALSE;
	}
	track.track.albID = track.album.ID = album.ID;
	return UpdateTrackRecord(track.track);
}


//BOOL SQLManager::GetInfo(LPCTSTR fldName, LPCTSTR tblName, 
//						 InfoItemTypeEnum iit, UINT ID, InfoRecord& rec)
//{
//	ASSERT(ID != 0);
//	ASSERT(fldName != NULL && tblName != NULL);
//	TCHAR sql[1000];
//	_sntprintf(sql, 1000, _T("SELECT %s FROM %s WHERE ID=%d"), fldName, tblName, ID);
//	ADORecordset rs(m_pDB);
//	if (rs.Open(sql) && !rs.IsEOF())
//	{
//		UINT comID = rs.GetUINTFieldValue(0);
//		if (comID > 0)
//			return GetInfoRecord(comID, rec);
//	}
//	return FALSE;
//
//}
//
//BOOL SQLManager::SetInfo(LPCTSTR fldName, LPCTSTR tblName, 
//						 InfoItemTypeEnum iit, UINT ID, LPCTSTR info)
//{
//	ASSERT(ID != 0);
//	ASSERT(fldName != NULL && tblName != NULL);
//	TCHAR sql[1000];
//	_sntprintf(sql, 1000, _T("SELECT %s FROM %s WHERE ID=%d"), fldName, tblName, ID);
//	ADORecordset rs(m_pDB);
//	if (rs.Open(sql) && !rs.IsEOF())
//	{
//		UINT infoID = rs.GetUINTFieldValue(0);
//		if (infoID > 0)
//		{
//			if (info[0] != 0)
//			{
//				InfoRecord ir;
//				ir.ID = infoID;
//				ir.name = info;
//				GetLocalTime(&ir.dateAdded);
//				return UpdateInfoRecord(ir);
//			}
//			else
//				DeleteInfoRecord(infoID);
//			return TRUE;
//		}
//		else
//		{
//			if (info[0] != 0)
//			{
//				InfoRecord ir;
//				ir.name = info;
//				GetLocalTime(&ir.dateAdded);
//				if (AddNewInfoRecord(ir))
//				{
//					TCHAR sql[1000];
//					_sntprintf(sql, 1000, _T("UPDATE %s SET %s=%d WHERE ID=%d"),
//						tblName, fldName, ir.ID, ID);
//					return m_pDB->Execute(sql);
//				}
//			}
//		}
//	}
//	return FALSE;
//}
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================
//==============================================================











//==============================================================
//		TRACKS TABLE
//==============================================================



//BOOL SQLManager::TrackRecordCollectionByCollectionID(TrackRecordCollection& col, UINT collectionID)
//{
//	ASSERT(collectionID > 0);
//	if (collectionID == 0)
//		return FALSE;
//	std::tstring sql(TrackSQL);
//	TCHAR bf[200];
//	_sntprintf(bf, 200, _T(" WHERE Tracks.CollectionID=%d"), collectionID);
//	sql += bf;
//	return TrackRecordCollectionBySQL(col, sql.c_str());
//}



BOOL SQLManager::GetTrackRecordCollectionByTracksFilter(TrackRecordCollection& col, TracksFilter& Filter)
{
	CString Where(GetWhereSQL(Filter));
	CString SQL;
	if (!Where.IsEmpty())
		SQL.Format(_T("%s WHERE %s"), TrackSQL, &((LPCTSTR)Where)[4]);
	else
		SQL = TrackSQL;
	return GetTrackRecordCollectionBySQL(col, SQL);
}


const TCHAR* const FullTrackSQL = _T("SELECT ")
					_T("Tracks.ID,[Tracks.Name],[Tracks.Type],Tracks.DateAdded,")
					_T("Tracks.albID,Tracks.artID,Tracks.CollectionID,Tracks.GenreID,Tracks.Path,")
					_T("Tracks.Bitrate,Tracks.Duration,[Tracks.Size],Tracks.TrackNo,")
					_T("Tracks.Year,Tracks.Rating,")
					_T("Albums.ID,Albums.Name,Albums.artID,Albums.Year,Albums.Rating,")
					_T("Artists.ID,Artists.Name,Artists.GenreID,Artists.Rating,")
					_T("Genres.ID,Genres.Name,") 
					_T("Collections.ID,Collections.Name,Collections.PathName,Collections.Serial,")
					_T("Collections.Type,Collections.DateAdded,Collections.DateUpdated ")
					_T("FROM ((((Tracks INNER JOIN Albums ON Tracks.albID=Albums.ID) ")
					_T("INNER JOIN Genres ON Tracks.GenreID=Genres.ID) ")
					_T("INNER JOIN Artists ON Tracks.artID=Artists.ID) ")
					_T("INNER JOIN Collections ON Tracks.collectionID=Collections.ID) ");



BOOL SQLManager::GetFullTrackRecordByID(FullTrackRecordSP& rec, UINT id)
{
	//TRACEST(_T("SQLManager::GetFullTrackRecordByID"));
	TracksFilter tf;
	tf.TrackID.match = NUMM_Exact;
	tf.TrackID.val = id;
	FullTrackRecordCollection col;
	if (GetFullTrackRecordCollectionByTracksFilter(col, tf))
	{
		if (col.size() == 1)
		{
			rec = col[0];
			return TRUE;
		}
	}
	return FALSE;
}



BOOL SQLManager::GetFullTrackRecordByLocation(FullTrackRecordSP& rec, LPCTSTR Location, UINT CollectionID)
{
	//TRACEST(_T("SQLManager::GetFullTrackRecordByLocation"));
	ASSERT(Location != NULL);
	if (Location != NULL)
	{
		TracksFilter tf;
		tf.Location.match = TXTM_Exact;
		tf.Location.val = Location;
		if (CollectionID != 0)
		{
			tf.CollectionID.match = NUMM_Exact;
			tf.CollectionID.val = CollectionID;
		}
		FullTrackRecordCollection col;
		if (GetFullTrackRecordCollectionByTracksFilter(col, tf))
		{
			if (col.size() > 0)
			{
				rec = col[0];
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL SQLManager::GetFullTrackRecordCollectionByLocation(FullTrackRecordCollection& col, LPCTSTR Location)
{
	TRACEST(_T("SQLManager::GetFullTrackRecordCollectionByLocation"));
	ASSERT(Location != NULL);
	if (Location != NULL)
	{
		TracksFilter tf;
		tf.Location.match = TXTM_Exact;
		tf.Location.val = Location;
		if (GetFullTrackRecordCollectionByTracksFilter(col, tf))
			return TRUE;
	}
	return FALSE;
}


BOOL SQLManager::GetFullTrackRecordCollectionByTracksFilter(FullTrackRecordCollection& col, TracksFilter& Filter, INT limitResults)
{
	//TRACEST(_T("SQLManager::GetFullTrackRecordCollectionByTracksFilter"));
	CString Where(GetWhereSQL(Filter));
	if (Filter.MinNumTracks.match == NUMM_Disabled)
	{
		if (Filter.ArtistID.match == NUMM_Disabled)
			AddSQLCountParameter(Where, Filter.ArtistTrackCount, _T("Tracks.artID"));
		if (Filter.AlbumID.match == NUMM_Disabled)
			AddSQLCountParameter(Where, Filter.AlbumTrackCount, _T("Tracks.albID"));
		if (Filter.GenreID.match == NUMM_Disabled)
			AddSQLCountParameter(Where, Filter.GenreTrackCount, _T("Tracks.genreID"));
		if (Filter.CollectionID.match == NUMM_Disabled)
			AddSQLCountParameter(Where, Filter.CollectionTrackCount, _T("Tracks.collectionID"));
		if (Filter.Year.match == NUMM_Disabled)
			AddSQLCountParameter(Where, Filter.YearTrackCount, _T("Tracks.Year"));
		if (Filter.MonthAdded.match == NUMM_Disabled)
			AddSQLCountParameter(Where, Filter.MonthAddedTrackCount, _T("(Month(Tracks.DateAdded) + Year(Tracks.DateAdded) * 12 - 1)"));

	}
	CString SQL;
	if (!Where.IsEmpty())
		SQL.Format(_T("%s WHERE %s"), FullTrackSQL, &((LPCTSTR)Where)[4]);
	else
		SQL = FullTrackSQL;
	return GetFullTrackRecordCollectionBySQL(col, SQL, limitResults);
}

BOOL SQLManager::GetFullTrackRecordCollectionBySimpleSearch(FullTrackRecordCollection& col, LPCTSTR searchStr, INT limitResults)
{
	TRACEST(_T("SQLManager::GetFullTrackRecordCollectionBySimpleSearch"), searchStr);
	ASSERT(searchStr != NULL);
	if (searchStr[0] == 0)
		return TRUE;//Does not touch the collection (Leave it empty usually - except if the user wanted to append)
	std::tstring searchF(searchStr);
	replace(searchF, _T("'"), _T("''"));

	std::tstring SQL(FullTrackSQL);
	SQL.reserve(1000 + 4 * searchF.size());
	SQL += _T(" WHERE (Tracks.Name LIKE '%");
	SQL += searchF;
	SQL += _T("%') OR (Artists.Name LIKE '%");
	SQL += searchF;
	SQL += _T("%') OR (Albums.Name LIKE '%");
	SQL += searchF;
	SQL += _T("%') OR (Tracks.Path LIKE '%");
	SQL += searchF;
	SQL += _T("%')");

	//TCHAR SQL[2000];
	//_sntprintf(SQL, 2000, 
	//	_T("%s WHERE (Tracks.Name LIKE '%%%s%%' OR Artists.Name LIKE '%%%s%%' OR Albums.Name LIKE '%%%s%%' OR Tracks.Path LIKE '%%%s%%')"), 
	//	FullTrackSQL, searchF.c_str(), searchF.c_str(), searchF.c_str(), searchF.c_str());
	return GetFullTrackRecordCollectionBySQL(col, SQL.c_str(), limitResults);
}

void SQLManager::AddSmartSearchCondition(std::tstring& SQL, LPCTSTR condition)
{
	SQL += _T(" (Tracks.Name + Artists.Name + Albums.Name + Tracks.Path) LIKE '%");
	SQL += condition;
	SQL += _T("%' ");
}

BOOL SQLManager::GetFullTrackRecordCollectionBySmartSearch(FullTrackRecordCollection& col, LPCTSTR searchStr, INT limitResults)
{
	TRACEST(_T("SQLManager::GetFullTrackRecordCollectionBySmartSearch"), searchStr);
	ASSERT(searchStr != NULL);
	if (searchStr[0] == 0)
		return TRUE;//Does not touch the collection (Leave it empty usually - except if the user wanted to append)
	std::tstring searchF(searchStr);
	replace(searchF, _T("'"), _T("''"));
	trim(searchF);

	std::tstring SQL(FullTrackSQL);
	SQL += _T(" WHERE ");
	LPCTSTR posStart = searchF.c_str();
	LPCTSTR posEnd = _tcschr(posStart, _T(' '));
	while (TRUE)
	{
		if (posEnd == NULL)
		{
			AddSmartSearchCondition(SQL, posStart);
			break;
		}
		const INT cbfLen = 100;
		TCHAR bf[cbfLen];
		_tcsncpy(bf, posStart, posEnd - posStart);
		bf[posEnd - posStart] = 0; 
		AddSmartSearchCondition(SQL, bf);
		//=== Find the next
		while (*posEnd == _T(' '))
			posEnd++;
		posStart = posEnd;
		posEnd = _tcschr(posStart, _T(' '));
		SQL += _T(" AND ");
	}


	//TCHAR SQL[2000];
	//_sntprintf(SQL, 2000, 
	//	_T("%s WHERE (Tracks.Name LIKE '%%%s%%' OR Artists.Name LIKE '%%%s%%' OR Albums.Name LIKE '%%%s%%' OR Tracks.Path LIKE '%%%s%%')"), 
	//	FullTrackSQL, searchF.c_str(), searchF.c_str(), searchF.c_str(), searchF.c_str());
	return GetFullTrackRecordCollectionBySQL(col, SQL.c_str(), limitResults);
}





BOOL SQLManager::GetFullTrackRecordCollectionByInfoLikeness(FullTrackRecordCollection& col, InfoItemTypeEnum iit, LPCTSTR searchString)
{
	ASSERT(searchString != NULL);
	if (searchString[0] == 0)	return FALSE;
	TCHAR FixedString[500];
	FixStringField(searchString, FixedString, 500);
	CString SQL;
	SQL.Format(_T("%s INNER JOIN tracksinfo ON TracksInfo.trackid=Tracks.ID ")
		_T("WHERE tracksinfo.Name LIKE '%%%s%%' AND tracksinfo.type=%d"),
		FullTrackSQL, FixedString, iit);
	return GetFullTrackRecordCollectionBySQL(col, SQL);
}

//==============================================================
//		ARTISTS TABLE
//==============================================================

const TCHAR* const FullArtistSQL = _T("SELECT ")
_T("Artists.ID, Artists.Name, Artists.GenreID, Artists.Rating ")
_T(",Genres.ID,Genres.Name ")
_T(", -1 ")
_T("FROM Artists INNER JOIN Genres ON Artists.GenreID=Genres.ID ");

const TCHAR* const CountFullArtistSQL = _T("SELECT ")
_T("first(Artists.ID), first(Artists.Name), first(Artists.GenreID), first(Artists.Rating) ")
_T(",first(Genres.ID),first(Genres.Name),")
_T("count(*) ")
_T("FROM Tracks INNER JOIN (Artists INNER JOIN Genres ")
_T("ON Artists.genreID = Genres.ID) ON Tracks.artID = Artists.ID ");

BOOL SQLManager::GetFullArtistRecordByID(FullArtistRecordSP& rec, UINT id)
{
	ASSERT(id > 0);
	FullArtistRecordCollection col;
	std::tstring SQL = FullArtistSQL;
	TCHAR bf[100];
	_sntprintf(bf, 100, _T(" WHERE Artists.ID=%d"), id);
	SQL += bf;
	if (GetFullArtistRecordCollectionBySQL(col, SQL.c_str()))
	{
		ASSERT(col.size() == 1);
		if (col.size() == 1)
		{
			rec = col[0];
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SQLManager::GetFullArtistRecordCollectionByTracksFilter(FullArtistRecordCollection& col, TracksFilter& Filter)
{
	std::tstring SQL;
	SQL = CountFullArtistSQL;
	
	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	SQL += _T(" GROUP BY Tracks.artID ");
	SQL += GetHavingSQL(Filter.MinNumTracks);

	return GetFullArtistRecordCollectionBySQL(col, SQL.c_str());
}


//==============================================================
//		ALBUMS TABLE
//==============================================================


const TCHAR* const FullAlbumSQL = _T("SELECT ")
_T("Albums.ID, Albums.Name, Albums.artID, Albums.Year, Albums.Rating ")
_T(", Artists.ID, Artists.Name, Artists.GenreID, Artists.Rating ")
_T(", -1 ")
_T("FROM Albums INNER JOIN Artists ON Albums.artID=Artists.ID ");

const TCHAR* const CountFullAlbumSQL = _T("SELECT ")
_T("first(Albums.ID), first(Albums.Name), first(Albums.artID), first(Albums.Year), first(Albums.Rating) ")
_T(", first(Artists.ID), first(Artists.Name), first(Artists.GenreID), first(Artists.Rating)")
_T(", count(*) ")
_T("FROM Tracks INNER JOIN (Albums INNER JOIN Artists ")
_T("ON Albums.artID = Artists.ID) ON Tracks.albID = Albums.ID ");


const TCHAR* const AlternativeFullAlbumSQL = _T("SELECT ")
_T("first(Albums.ID), first(Albums.Name), first(Albums.artID), first(Albums.Year), first(Albums.Rating) ")
_T(", count(distinct(Tracks.artID), '', 1, 0, 0")
_T(", count(*) ")
_T("FROM Tracks INNER JOIN Albums ")
_T("ON Tracks.albID = Albums.ID ");

BOOL SQLManager::GetFullAlbumRecordByID(FullAlbumRecordSP& rec, UINT id)
{
	ASSERT(id > 0);
	if (id == 0)
		return FALSE;
	FullAlbumRecordCollection col;
	std::tstring SQL = FullAlbumSQL;
	TCHAR bf[100];
	_sntprintf(bf, 100, _T(" WHERE Albums.ID=%d"), id);
	SQL += bf;
	if (GetFullAlbumRecordCollectionBySQL(col, SQL.c_str()))
	{
		ASSERT(col.size() == 1);
		if (col.size() == 1)
		{
			rec = col[0];
			return TRUE;
		}
	}
	return FALSE;
}

BOOL SQLManager::GetFullAlbumRecordCollectionByTracksFilter(FullAlbumRecordCollection& col, TracksFilter& Filter)
{
	std::tstring SQL = CountFullAlbumSQL;
	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	SQL += _T(" GROUP BY Tracks.albID ");
	SQL += GetHavingSQL(Filter.MinNumTracks);
	return GetFullAlbumRecordCollectionBySQL(col, SQL.c_str());
}

BOOL SQLManager::GetFullAlbumGPNRecordCollectionByTracksFilter(FullAlbumRecordCollection& col, TracksFilter& Filter)
{
	std::tstring SQL = CountFullAlbumSQL;
	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	SQL += _T(" GROUP BY Albums.name ");
	SQL += GetHavingSQL(Filter.MinNumTracks);
	return GetFullAlbumRecordCollectionBySQL(col, SQL.c_str());
}


BOOL SQLManager::GetFullAlbumRecordCollectionByTracksFilterForCollections(FullAlbumRecordCollection& col, TracksFilter& Filter)
{
	std::tstring SQL = _T("SELECT ")
		_T("first(Albums.ID), first(Albums.Name), first(Albums.artID), first(Albums.Year), first(Albums.Rating), 0, '', 0, 0, 0, count(*) ")
		_T("FROM Tracks INNER JOIN Albums ")
		_T("ON Tracks.albID = Albums.ID ")
		_T("WHERE Left(Albums.name,1)<>'[' ");
	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
		SQL += Where;
	SQL += _T("GROUP BY (Albums.name) HAVING MAX(Tracks.artID) <> MIN(Tracks.artID) ");
	return GetFullAlbumRecordCollectionBySQL(col, SQL.c_str());
}



//SELECT
//first(Albums.ID), first(Albums.Name), first(Albums.artID), first(Albums.Year), first(Albums.InfoID), first(Albums.Rating), count(Tracks.artID), 'aaaa', 1, 0, 0, count(*)
//FROM Tracks INNER JOIN Albums 
//ON Tracks.albID = Albums.ID 
//WHERE Left(Albums.name,1)<>'['
//GROUP BY (Albums.name)
//HAVING MAX(Tracks.artID) <> MIN(Tracks.artID)


//==============================================================
//		GENRES TABLE
//==============================================================








BOOL SQLManager::GetGenreRecordCollectionByTracksFilter(GenreRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks)
{
	std::tstring SQL;
	if (bCountTracks)
		SQL = CountGenreSQL;
	else
		SQL = GenreSQL;

	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	if (bCountTracks)
	{
		SQL += _T(" GROUP BY Tracks.genreID ");
		SQL += GetHavingSQL(Filter.MinNumTracks);
	}
	return GetGenreRecordCollectionBySQL(col, SQL.c_str());
}

//==============================================================
//		COLLECTIONS TABLE
//==============================================================





BOOL SQLManager::GetCollectionRecordCollectionByTracksFilter(CollectionRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks)
{
	std::tstring SQL;
	if (bCountTracks)
		SQL = CountCollectionSQL;
	else
		SQL = CollectionSQL;

	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	if (bCountTracks)
	{
		SQL += _T(" GROUP BY Collections.ID ");
		SQL += GetHavingSQL(Filter.MinNumTracks);
	}

	return GetCollectionRecordCollectionBySQL(col, SQL.c_str());
}

BOOL SQLManager::GetCollectionRecordCollectionByTypeID(CollectionRecordCollection& col, CollectionTypesEnum type)
{
	std::tstring SQL;
	SQL = CollectionSQL;
	TCHAR bf[200];
	_sntprintf(bf, 200, _T(" WHERE type=%d"), type);
	SQL += bf;
	return GetCollectionRecordCollectionBySQL(col, SQL.c_str());
	
}

//==============================================================
//		YEARS
//==============================================================

const TCHAR* const CountYearSQL = _T("SELECT ")
_T("first(Tracks.Year) ")
_T(", count(*) ")
_T("FROM Tracks ");





BOOL SQLManager::GetYearRecordCollectionByTracksFilter(YearRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks)
{
	std::tstring SQL;
	ASSERT(bCountTracks);
	SQL = CountYearSQL;

	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	if (bCountTracks)
	{
		SQL += _T(" GROUP BY Tracks.year ");
		SQL += GetHavingSQL(Filter.MinNumTracks);
	}


	return GetYearRecordCollectionBySQL(col, SQL.c_str());
}

//==============================================================
//		MONTH ADDED
//==============================================================

const TCHAR* const CountMonthAddedSQL = _T("SELECT ")
_T("Month(Tracks.DateAdded) + Year(Tracks.DateAdded) * 12 - 1 ")
_T(", count(*) ")
_T("FROM Tracks ");



BOOL SQLManager::GetMonthAddedRecordCollectionByTracksFilter(MonthAddedRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks)
{
	std::tstring SQL;
	ASSERT(bCountTracks);
	SQL = CountMonthAddedSQL;

	CString Where(GetWhereSQL(Filter));
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	if (bCountTracks)
	{
		SQL += _T(" GROUP BY Month(Tracks.DateAdded),Year(Tracks.DateAdded) ");
		SQL += GetHavingSQL(Filter.MinNumTracks);
	}


	return GetMonthAddedRecordCollectionBySQL(col, SQL.c_str());
}

//BOOL SQLManager::GetDirectoryRecordCollectionByTracksFilter(DirectoryRecordCollection& col, TracksFilter& Filter, BOOL bCountTracks)
//{
//	std::tstring SQL;
//	ASSERT(bCountTracks);
//	SQL = CountMonthAddedSQL;
//
//	CString Where(GetWhereSQL(Filter));
//	if (!Where.IsEmpty())
//	{
//		SQL += _T(" WHERE ");
//		SQL += &((LPCTSTR)Where)[4];
//	}
//	if (bCountTracks)
//	{
//		SQL += _T(" GROUP BY Month(Tracks.DateAdded),Year(Tracks.DateAdded) ");
//		SQL += GetHavingSQL(Filter.MinNumTracks);
//	}
//
//
//	return GetMonthAddedRecordCollectionBySQL(col, SQL.c_str());
//}





//==============================================================
//		PRIVATE UTILITIES
//==============================================================

CString SQLManager::GetWhereSQL(TracksFilter& Filter)
{
	CString ret;
	AddSQLNumParameter(ret, Filter.TrackID,			_T("Tracks.ID"));
	AddSQLNumParameter(ret, Filter.ArtistID,		_T("Tracks.artID"));
	AddSQLNumParameter(ret, Filter.AlbumID,			_T("Tracks.albID"));

	AddSQLNumParameter(ret, Filter.GenreID,			_T("Tracks.GenreID"));
	AddSQLNumParameter(ret, Filter.CollectionID,	_T("Tracks.CollectionID"));

	AddSQLStrParameter(ret, Filter.Title,			_T("Tracks.Name"));
	AddSQLStrParameter(ret, Filter.Album,			_T("Albums.Name"));
	AddSQLStrParameter(ret, Filter.Artist,			_T("Artists.Name"));
	AddSQLStrParameter(ret, Filter.Location,		_T("Tracks.Path"));
	AddSQLStrParameter(ret, Filter.Genre,			_T("Genres.Name"));

	AddSQLNumParameter(ret, Filter.Year,			_T("Tracks.year"));
	if (Filter.Rating.match != NUMM_Disabled)
	{	
		AddSQLNumParameter(ret, Filter.Rating,		_T("Tracks.Rating"));
		NumericMatch rat2;
		rat2.match = NUMM_Over;
		rat2.val = 1;
		AddSQLNumParameter(ret, rat2,_T("Tracks.Rating"));
	}
	AddSQLNumParameter(ret, Filter.TrackNo,			_T("Tracks.TrackNo"));
	AddSQLNumParameter(ret, Filter.Size,			_T("Tracks.Size"));
	AddSQLNumParameter(ret, Filter.Duration,		_T("Tracks.duration"));
	AddSQLNumParameter(ret, Filter.Bitrate,			_T("Tracks.bitrate"));
	AddSQLNumParameter(ret, Filter.TrackType,		_T("Tracks.type"));

	AddSQLNumParameter(ret, Filter.MonthAdded,		_T("(Month(Tracks.DateAdded) + 12 * Year(Tracks.DateAdded) - 1)"));


	AddSQLDateParameter(ret, Filter.DateAdded,		_T("Tracks.DateAdded"));
	return ret;

}

CString SQLManager::GetHavingSQL(NumericMatch nm)
{
	if (nm.match != NUMM_Disabled)
	{
		ASSERT(nm.val > 0);
		TCHAR bf[400];
		_sntprintf(bf, 400,
			_T(" HAVING count(*) %s %d"),
			nm.match == NUMM_Over ? _T(">") : (nm.match == NUMM_Under ? _T("<=") : _T("=")),
			nm.val);
		return CString(bf);
	}
	return CString();
}

void SQLManager::AddSQLCountParameter(CString& ret, NumericMatch nm, LPCTSTR fieldName)
{
	if (nm.match == NUMM_Disabled || nm.val == 0)
		return;
	LPCTSTR oper = _T("=");
	if (nm.match == NUMM_Over)
		oper = _T(">");
	else
		oper = _T("<=");
	TCHAR bf[400];
	_sntprintf(bf, 400,
		_T(" AND %s IN ")
		_T("(Select %s from Tracks group by %s having count(%s) %s %d)"),
		fieldName,
		fieldName,
		fieldName,
		fieldName,
		oper,
		nm.val);
	ret += bf;
}

void SQLManager::AddSQLNumParameter(CString& ret, NumericMatch nm, LPCTSTR str)
{
	if (nm.match == NUMM_Disabled)
		return;
	TCHAR* op;
	switch (nm.match)
	{
	case NUMM_Exact:	op = _T("=");	break;
	case NUMM_Over:		op = _T(">=");	break;
	case NUMM_Under:	op = _T("<");	break;
	case NUMM_Diff:		op = _T("<>");	break;
	default: 
		TRACE(_T("@1 SQLManager::AddSQLNumParameter Unknown operator\r\n"));
		ASSERT(FALSE);
		return;
	}
	//if (!ret.IsEmpty())
	ret += _T(" AND ");
	ret += str;
	ret += op;
	TCHAR bf[20];
	_itot(nm.val, bf, 10);
	ret += bf;
	//curLen += _sntprintf(&bf[curLen], TotalLen - curLen, _T(" AND %s %s %d"), str, op, nm.val);
}

void SQLManager::AddSQLStrParameter(CString& ret, TextMatch tm, LPCTSTR str)
{
	if (tm.match == TXTM_Disabled)
		return;
	ASSERT(!tm.val.empty());
	if (!tm.val.empty())
	{
		TCHAR FixedField[500];
		//DbManager::FixStringField(tm.val, FixedField, 300);
		FixStringField(tm.val.c_str(), FixedField, 500);
		TCHAR bf[1000];
		switch (tm.match)
		{
		case TXTM_Exact:	
			_sntprintf(bf, 1000, _T(" AND %s = '%s'"), str, FixedField);
			break;
		case TXTM_Like:
			_sntprintf(bf, 1000, _T(" AND %s Like '%%%s%%'"), str, FixedField);
			break;
		default: 
			ASSERT(FALSE);
			return;
		}
		ret += bf;
		//curLen += _sntprintf(&bf[curLen], TotalLen - curLen, _T(" AND %s %s '%s'"), str, op, FixedField);
	}
}

void SQLManager::AddSQLDateParameter(CString& ret, DateMatch dm, LPCTSTR str)
{
	if (dm.match == DATM_Disabled)
		return;
	TCHAR bf[100];
	GetDateFormat(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), DATE_SHORTDATE, &dm.val, NULL, bf, 100);
	//if (!ret.IsEmpty())
	ret += _T(" AND ");
	ret += str;
	ret += dm.match == DATM_After ? _T(">") : _T("<");
	ret += _T("#");
	ret += bf;
	ret += _T("#");
}


UINT SQLManager::FixStringField(LPCTSTR source, LPTSTR dest, UINT destLen)
{
	ASSERT(source != NULL);
	ASSERT(dest!=NULL);
	ASSERT(destLen > _tcslen(source));
	UINT i = 0;
	UINT additions = 0;
	while (source[i] != 0)
	{
		if (source[i] == 39)//Duplicate 39 "'"
		{
			dest[additions + i] = 39;
			additions++;
		}
		if (additions + i >= destLen - 1)//Do not overwrite
			break;
		dest[additions + i] = source[i];
		i++;
	}
	ASSERT(destLen > additions + i + 10);//Test for error boundaries
	dest[min(additions + i, destLen - 1)] = 0;
	return additions;
}

//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================
//========================================================================

//===================================COLLECTIONS


//BOOL SQLManager::GetCollectionRecordUnique(UINT serial, LPCTSTR location, CollectionRecord& collection)
//{
//	ASSERT(location != NULL);
//	if (location[0] == 0)
//	{
//		collection.ID = 1;
//		collection.name = location;
//	}
//	ADORecordset rs(m_pDB);
//	std::tstring fname(location);
//	replace2(fname, _T("'"), _T("''"));
//	TCHAR sql[500];
//	_sntprintf(sql, 500, _T("SELECT * FROM collections WHERE pathName='%s' AND serial=%d"), fname.c_str(), serial);
//	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
//	{
//		GetCollectionRecordFields(collection, rs, 0);
//		return TRUE;
//	}
//	return FALSE;
//}





//===================================GENRES




BOOL SQLManager::SetGenreRecordFields(GenreRecord& genre, ADORecordset& rs)
{
	//[DO NOT EDIT] rs.SetVariantFieldValue(_T("ID"), genre.ID); 
	INT ret = rs.SetVariantFieldValue(_T("name"), genre.name.c_str()) ? 1 : 0;
	ASSERT(ret == 1);
	return ret == 1;
}
//===================================GENRES END

//===================================ARTISTS





BOOL SQLManager::SetArtistRecordFields(ArtistRecord& artist, ADORecordset& rs)
{
	INT ret = rs.SetVariantFieldValue(_T("name"), artist.name.c_str()) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("genreID"), artist.genID) ? 1 : 0;//Foreign Key
	ret += rs.SetVariantFieldValue(_T("rating"), artist.rating) ? 1 : 0;
	ASSERT(ret == 3);
	return ret == 3;
}
//===================================ARTISTS END

//===================================ALBUMS




BOOL SQLManager::SetAlbumRecordFields(AlbumRecord& album, ADORecordset& rs)
{
	INT ret = 0;
	ret += rs.SetVariantFieldValue(_T("name"), album.name.empty() ? TS_UnknownString : album.name.c_str()) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("artID"), album.artID) ? 1 : 0;//Foreign Key
	ret += rs.SetVariantFieldValue(_T("Year"), album.year) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("rating"), album.rating) ? 1 : 0;
	ASSERT(ret == 4);
	return ret == 4;
}

//===================================ALBUMS END




//Needs album.ID to be != 0
//Needs artID-infoID to be Valid!!!!
//UNIQUE KEY = ID, Name


//==============SetFields
BOOL SQLManager::SetTrackRecordFields(TrackRecord& track, ADORecordset& rs)
{
	//[DO NOT EDIT] rs.SetVariantFieldValue(_T("ID"), track.ID); 
	INT ret = rs.SetVariantFieldValue(_T("name"), track.name.c_str()) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("type"), track.trackType) ? 1 : 0;
	ret += rs.SetSystemTimeFieldValue(_T("dateAdded"), track.dateAdded) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("albID"), track.albID) ? 1 : 0;//Foreign Key
	ret += rs.SetVariantFieldValue(_T("artID"), track.artID) ? 1 : 0;//Foreign Key
	ret += rs.SetVariantFieldValue(_T("CollectionID"), track.colID) ? 1 : 0;//Foreign Key
	ret += rs.SetVariantFieldValue(_T("genreID"), track.genID) ? 1 : 0;//Foreign Key
	ret += rs.SetVariantFieldValue(_T("path"), track.location.c_str()) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("bitrate"), track.bitrate) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("duration"), track.duration) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("size"), track.size) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("trackno"), track.trackNo) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("year"), track.year) ? 1 : 0;
	ret += rs.SetVariantFieldValue(_T("rating"), track.rating) ? 1 : 0;
	ASSERT(ret == 14);
	return ret == 14;
}


BOOL SQLManager::DeleteTrackRecord(UINT ID)
{
	ASSERT(ID > 0);
	DeleteAllTrackInfo(ID);
	TCHAR sql[100];
	_sntprintf(sql, 100, _T("DELETE FROM Tracks WHERE id=%d"), ID);
	m_changes.tracks.removals++;
	return m_pDB->Execute(sql) != -1;
}

BOOL SQLManager::GetVirtualTrackCollectionRecord(CollectionRecord& cr)
{
	ASSERT(0);return FALSE;//TODO
}

BOOL SQLManager::GetFirstVal(LPCTSTR sql, INT& val)
{
	ADORecordset rs(m_pDB);
	if (rs.Open(sql) && !rs.IsEOF())
	{
		val = rs.GetINTFieldValue(0);
		return TRUE;
	}
	return FALSE;
}

BOOL SQLManager::DeleteAlbumRecord(UINT ID)
{
	ASSERT(ID > 0);
#ifdef USE_INTEGRITY_CHECKS
	TRACE(_T("@4 SQLManager::DeleteAlbumRecord. Integrity Test\r\n"));
	INT count = 0;
	if (!GetFirstVal(_T("Select count(*) FROM Tracks WHERE albID=%d"), count) || count == 0)
		return FALSE;
	TRACE(_T("@4 SQLManager::DeleteAlbumRecord. Integrity Test = OK\r\n"));
#endif // USE_INTEGRITY_CHECKS
	DeleteAllAlbumInfo(ID);
	TCHAR sql[100];
	_sntprintf(sql, 100, _T("DELETE FROM Albums WHERE id=%d"), ID);
	m_changes.albums.removals++;
	return m_pDB->Execute(sql) != -1;
}


BOOL SQLManager::DeleteArtistRecord(UINT ID)
{
	ASSERT(ID > 0);
#ifdef USE_INTEGRITY_CHECKS
	TRACE(_T("@4 SQLManager::DeleteArtistRecord. Integrity Test\r\n"));
	INT count = 0;
	if (!GetFirstVal(_T("Select count(*) FROM Tracks WHERE artID=%d"), count) || count != 0)
		return FALSE;
	if (!GetFirstVal(_T("Select count(*) FROM Albums WHERE artID=%d"), count) || count != 0)
		return FALSE;
	TRACE(_T("@4 SQLManager::DeleteArtistRecord. Integrity Test = OK\r\n"));
#endif // USE_INTEGRITY_CHECKS
	DeleteAllArtistInfo(ID);
	TCHAR sql[100];
	_sntprintf(sql, 100, _T("DELETE FROM Artists WHERE id=%d"), ID);
	m_changes.artists.removals++;
	return m_pDB->Execute(sql) != -1;
}

BOOL SQLManager::DeleteGenreRecord(UINT ID)
{
	ASSERT(ID > 0);
#ifdef USE_INTEGRITY_CHECKS
	TRACE(_T("@4 SQLManager::DeleteGenreRecord. Integrity Test\r\n"));
	INT count = 0;
	if (!GetFirstVal(_T("Select count(*) FROM Tracks WHERE genID=%d"), count) || count != 0)
		return FALSE;
	if (!GetFirstVal(_T("Select count(*) FROM Artists WHERE genID=%d"), count) || count != 0)
		return FALSE;
	TRACE(_T("@4 SQLManager::DeleteGenreRecord. Integrity Test = OK\r\n"));
#endif // USE_INTEGRITY_CHECKS
	TCHAR sql[100];
	_sntprintf(sql, 100, _T("DELETE FROM Genres WHERE id=%d"), ID);
	m_changes.genres.removals++;
	return m_pDB->Execute(sql) != -1;
}

BOOL SQLManager::DeleteCollectionRecord(UINT ID, BOOL bForce)
{
	ASSERT(ID > 0);
	TCHAR sql[100];
	if (bForce)
	{
		_sntprintf(sql, 100, _T("DELETE FROM Tracks WHERE collectionid=%d"), ID);
		if (m_pDB->Execute(sql) == -1)
			return FALSE;
	}
#ifdef USE_INTEGRITY_CHECKS
	else
	{
		return FALSE;
	}
#endif // USE_INTEGRITY_CHECKS
	_sntprintf(sql, 100, _T("DELETE FROM Collections WHERE ID=%d"), ID);
	if (m_pDB->Execute(sql) == -1)
		return FALSE;
	m_changes.collections.removals++;
	return TRUE;
}

//BOOL SQLManager::DeleteInfoRecord(UINT ID)
//{
//	ASSERT(ID > 0);
//	TCHAR sql[100];
//	_sntprintf(sql, 100, _T("DELETE FROM Info WHERE id=%d"), ID);
//	if (m_pDB->Execute(sql))
//	{
//		m_changes.info.removals++;
//		return TRUE;
//	}
//	return FALSE;
//}

BOOL SQLManager::DeleteHistLogRecord(UINT ID)
{
	//CAUTION The track Record must be handled properly
	ASSERT(ID > 0);
	TCHAR sql[100];
	_sntprintf(sql, 100, _T("DELETE FROM HistLog WHERE id=%d"), ID);
	return m_pDB->Execute(sql) != -1;
}







UINT SQLManager::GetTrackCount(TracksFilter& tf)
{
	CString Where = GetWhereSQL(tf);
	CString SQL = _T("SELECT count(*) FROM tracks");
	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE");
		SQL +=  &((LPCTSTR)Where)[4];
	}
	ADORecordset rs(m_pDB);
	if (rs.Open(SQL) && !rs.IsEOF())
		return rs.GetUINTFieldValue((short)0);
	return 0;
}
BOOL SQLManager::GetTrackIDList(TracksFilter& tf, std::vector<UINT>& idList)
{
	CString Where = GetWhereSQL(tf);
	CString SQL;
	if (tf.Album.match == TXTM_Disabled)
		SQL = _T("SELECT ID FROM tracks");
	else
	{
		//This have been Changed because of AlbumByName case...
		SQL = _T("SELECT tracks.ID FROM tracks INNER JOIN albums ON tracks.albID = albums.ID");
	}

	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE");
		SQL +=  &((LPCTSTR)Where)[4];
	}
	ADORecordset rs(m_pDB);
	if (rs.Open(SQL))
	{
		short idx = 0;
		while (!rs.IsEOF())
		{
			idList.push_back(rs.GetUINTFieldValue(idx));
			rs.MoveNext();
		}
		return TRUE;
	}
	return FALSE;
}

void SQLManager::GetSQLDateForQueries(SYSTEMTIME& input, LPTSTR bf, UINT bfLen)
{
	ASSERT(bf != NULL && bfLen > 10);
	TCHAR dateStr[100];
	GetDateFormat(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), DATE_SHORTDATE, &input, NULL, dateStr, 100);
	_sntprintf(bf, bfLen, _T("#%s#"), dateStr);
	bf[bfLen - 1] = 0;
}

BOOL SQLManager::LogTrackInHistory(LPCTSTR artist, LPCTSTR track, HistoryLogActionsEnum actID, SYSTEMTIME& localTime)
{
	ASSERT(actID > HLA_Unknown && actID < HLA_Last);
	ASSERT(artist!= NULL && track != NULL);
	if (!(artist!= NULL && track != NULL && actID > HLA_Unknown && actID < HLA_Last))
		return FALSE;
	if (artist == _T(""))
		artist = TS_UnknownString;
	if (track == _T(""))
		track = TS_UnknownString;
	HistArtistRecord art;
	if (!GetHistArtistRecordUnique(artist, art))
	{
		art.name = artist;
		AddNewHistArtistRecord(art);
	}
	ASSERT(art.IsValid());
	HistTrackRecord tra;
	if (!GetHistTrackRecordUnique(art.ID, track, tra))
	{
		tra.artID = art.ID;
		tra.name = track;
		AddNewHistTrackRecord(tra);
	}
	ASSERT(tra.IsValid());

	HistLogRecord log;
	log.actID = actID;
	log.dateAdded = localTime;
	log.trackID = tra.ID;
	return AddNewHistLogRecord(log);
}


BOOL SQLManager::GetHistoryLogStats(SQLManager::HistoryLogStats& stats)
{
	ADORecordset rs(m_pDB);
	if (rs.Open(_T("SELECT count(*),actID,MIN(DateAdded),MAX(DateADDED) FROM HistLog GROUP BY actID")))
	{
		while (!rs.IsEOF())
		{
			HistoryLogActionsEnum actID = (HistoryLogActionsEnum) rs.GetUINTFieldValue(1);
			if (actID == HLA_Played)
			{
				stats.plays = rs.GetUINTFieldValue(0);
				stats.firstTime = rs.GetSYSTEMTIMEFieldValue(2);
				stats.lastTime = rs.GetSYSTEMTIMEFieldValue(3);
			}
			else if (actID == HLA_Clicked)
				stats.actions = rs.GetUINTFieldValue(0);
			else
				ASSERT(0);
			rs.MoveNext();
		}
		return TRUE;
	}
	return FALSE;
}


BOOL SQLManager::GetHistoryTrackStats(LPCTSTR Artist, LPCTSTR Track, SYSTEMTIME* startDate, SYSTEMTIME* endDate,//IN
								 SQLManager::HistoryTrackStats& stats)//OUT
{
	ASSERT(Artist!=NULL);
	ASSERT(Track!=NULL);
	if (Artist == NULL || Track == NULL)
		return FALSE;
#ifdef _DEBUG
	if (startDate && endDate)
	{
		FILETIME ftStart;
		FILETIME ftEnd;
		SystemTimeToFileTime(startDate, &ftStart);
		SystemTimeToFileTime(endDate, &ftEnd);
		ASSERT(CompareFileTime(&ftStart, &ftEnd) == -1);
	}
#endif
	std::tstring fartist(Artist);
	replace(fartist, _T("'"), _T("''"));
	std::tstring ftrack(Track);
	replace(ftrack, _T("'"), _T("''"));

	CString SQL;
	SQL.Format(_T("SELECT count(*), MIN(histLog.DateAdded), MAX(histLog.DateAdded) ")
		_T("FROM (histLog INNER JOIN HistTracks ON histlog.trackID = HistTracks.ID) ")
		_T("INNER JOIN HistArtists ON HistTracks.artID = HistArtists.ID ")
		_T("WHERE HistArtists.Name = '%s' AND HistTracks.Name = '%s' AND histLog.actID = 1"),
		fartist.c_str(), ftrack.c_str());
	if (startDate != NULL)
	{
		SQL += _T(" AND HistLog.DateAdded >= ");
		TCHAR dateStr[100];
		GetSQLDateForQueries(*startDate, dateStr, 100);
		SQL += dateStr;
	}
	if (endDate != NULL)
	{
		SQL += _T(" AND HistLog.DateAdded < ");
		TCHAR dateStr[100];
		GetSQLDateForQueries(*endDate, dateStr, 100);
		SQL += dateStr;
	}
	ADORecordset rs(m_pDB);
	if (rs.Open(SQL) && !rs.IsEOF())
	{
		stats.hits = rs.GetUINTFieldValue(0);
		if (stats.hits > 0)
		{
			stats.firstTime = rs.GetSYSTEMTIMEFieldValue(1);
			stats.lastTime = rs.GetSYSTEMTIMEFieldValue(2);
			SQL = _T("SELECT count(*) as Hits FROM HistLog WHERE actid=1 ");
			if (startDate)
			{
				SQL += _T(" AND HistLog.DateAdded >= ");
				TCHAR dateStr[100];
				GetSQLDateForQueries(*startDate, dateStr, 100);
				SQL += dateStr;
			}
			if (endDate)
			{
				SQL += _T(" AND HistLog.DateAdded < ");
				TCHAR dateStr[100];
				GetSQLDateForQueries(*endDate, dateStr, 100);
				SQL += dateStr;
			}
			SQL += _T(" GROUP BY trackID ORDER BY count(*) DESC");
			ADORecordset rsRank(m_pDB);
			if (rsRank.Open(SQL))
			{
				UINT rank = 0;
				while (!rsRank.IsEOF())
				{
					stats.rank++;
					if (stats.hits == rsRank.GetUINTFieldValue(0))
						break;
					rsRank.MoveNext();
				}
			}
		}
	}
	return TRUE;
}



BOOL SQLManager::GetFullLogRecordCollection(FullHistLogRecordCollection& col,
											SYSTEMTIME* startDate/* = NULL*/,
											SYSTEMTIME* endDate /*= NULL*/,
											TCHAR* ArtistFilter /*= NULL*/,
											TCHAR* TrackFilter /*= NULL*/,
											HistoryLogActionsEnum actID /*= -1*/,
											UINT limitResults /*= 0*/)
{
	std::tstring SQL(_T("SELECT ")
		_T("HistLog.ID, HistLog.actID, HistLog.DateAdded, HistLog.trackID, ")
		_T("HistTracks.ID, HistTracks.Name, HistTracks.artID, ")
		_T("HistArtists.ID, HistArtists.Name ")
		_T("FROM (HistArtists INNER JOIN HistTracks ON HistTracks.artID = HistArtists.ID) ")
		_T("INNER JOIN HistLog ON HistLog.trackID = Histtracks.ID "));

	CString Where;
	DateMatch dm;
	if (startDate != NULL)
	{
		dm.match = DATM_After;
		dm.val = *startDate;
		AddSQLDateParameter(Where, dm, _T("HistLog.DateAdded"));
	}
	if (endDate != NULL)
	{
		dm.match = DATM_Before;
		dm.val = *endDate;
		AddSQLDateParameter(Where, dm, _T("HistLog.DateAdded"));
	}
	TextMatch tm;
	if (ArtistFilter != NULL)
	{
		tm.match = TXTM_Like;
		tm.val = ArtistFilter;
		AddSQLStrParameter(Where, tm, _T("HistArtists.Name"));
	}
	if (TrackFilter != NULL)
	{
		tm.match = TXTM_Like;
		tm.val = TrackFilter;
		AddSQLStrParameter(Where, tm, _T("HistTracks.Name"));
	}
	if (actID > HLA_Unknown && actID < HLA_Last)
	{
		NumericMatch nm;
		nm.match = NUMM_Exact;
		nm.val = actID;
		AddSQLNumParameter(Where, nm, _T("HistLog.actID"));
	}

	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}

	ADORecordset rs(m_pDB);
	if (!rs.Open(SQL.c_str()))
		return FALSE;
	INT countResults = 0;
	while (!rs.IsEOF())
	{
		FullHistLogRecord* pRec = new FullHistLogRecord;
		short startIdx = 0;
		GetHistLogRecordFields(pRec->log, rs, startIdx);
		GetHistTrackRecordFields(pRec->track, rs, startIdx);
		GetHistArtistRecordFields(pRec->artist, rs, startIdx);
		col.push_back(FullHistLogRecordSP(pRec));
		countResults++;
		if (countResults == limitResults)
			return TRUE;
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetFullHistTrackRecordCollection(FullHistTrackRecordCollection& col,
											SYSTEMTIME* startDate/* = NULL*/,
											SYSTEMTIME* endDate /*= NULL*/,
											TCHAR* ArtistFilter /*= NULL*/,
											TCHAR* TrackFilter /*= NULL*/,
											HistoryLogActionsEnum actID /*= -1*/,
											UINT limitResults /*= 0*/)
{
	std::tstring SQL(_T("SELECT ")
		_T("max(HistTracks.ID), max(HistTracks.Name), max(HistTracks.artID), ")
		_T("max(HistArtists.ID), max(HistArtists.Name), ")
		_T("count(*) as Hits, min(HistLog.DateAdded) as FirstTime, max(HistLog.DateAdded) as LastTime ")
		_T("FROM (HistArtists INNER JOIN HistTracks ON HistTracks.artID = HistArtists.ID) ")
		_T("INNER JOIN HistLog ON HistLog.trackID = Histtracks.ID "));

	CString Where;
	DateMatch dm;
	if (startDate != NULL)
	{
		dm.match = DATM_After;
		dm.val = *startDate;
		AddSQLDateParameter(Where, dm, _T("HistLog.DateAdded"));
	}
	if (endDate != NULL)
	{
		dm.match = DATM_Before;
		dm.val = *endDate;
		AddSQLDateParameter(Where, dm, _T("HistLog.DateAdded"));
	}
	TextMatch tm;
	if (ArtistFilter != NULL)
	{
		tm.match = TXTM_Like;
		tm.val = ArtistFilter;
		AddSQLStrParameter(Where, tm, _T("HistArtists.Name"));
	}
	if (TrackFilter != NULL)
	{
		tm.match = TXTM_Like;
		tm.val = TrackFilter;
		AddSQLStrParameter(Where, tm, _T("HistTracks.Name"));
	}
	if (actID > HLA_Unknown && actID < HLA_Last)
	{
		NumericMatch nm;
		nm.match = NUMM_Exact;
		nm.val = actID;
		AddSQLNumParameter(Where, nm, _T("HistLog.actID"));
	}

	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}
	SQL += _T(" GROUP BY HistLog.TrackID");

	ADORecordset rs(m_pDB);
	if (!rs.Open(SQL.c_str()))
		return FALSE;
	INT countResults = 0;
	while (!rs.IsEOF())
	{
		FullHistTrackRecord* pRec = new FullHistTrackRecord;
		short startIdx = 0;
		GetHistTrackRecordFields(pRec->track, rs, startIdx);
		GetHistArtistRecordFields(pRec->artist, rs, startIdx);
		pRec->hits = rs.GetUINTFieldValue(startIdx++);
		pRec->firstTime = rs.GetSYSTEMTIMEFieldValue(startIdx++);
		pRec->lastTime = rs.GetSYSTEMTIMEFieldValue(startIdx++);
		col.push_back(FullHistTrackRecordSP(pRec));
		countResults++;
		if (countResults == limitResults)
			return TRUE;
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetFullHistArtistRecordCollection(FullHistArtistRecordCollection& col,
									   SYSTEMTIME* startDate/* = NULL*/,
									   SYSTEMTIME* endDate /*= NULL*/,
									   TCHAR* ArtistFilter /*= NULL*/,
									   TCHAR* TrackFilter /*= NULL*/,
									   HistoryLogActionsEnum actID /*= 0*/,
									   UINT limitResults /*= 0*/)
{
	std::tstring SQL(_T("SELECT ")
		_T("max(HistArtists.ID), max(HistArtists.Name), ")
		_T("count(*) as Hits, min(HistLog.DateAdded) as FirstTime, max(HistLog.DateAdded) as LastTime ")
		_T("FROM (HistArtists INNER JOIN HistTracks ON HistTracks.artID = HistArtists.ID) ")
		_T("INNER JOIN HistLog ON HistLog.trackID = Histtracks.ID "));

	CString Where;
	DateMatch dm;
	if (startDate != NULL)
	{
		dm.match = DATM_After;
		dm.val = *startDate;
		AddSQLDateParameter(Where, dm, _T("HistLog.DateAdded"));
	}
	if (endDate != NULL)
	{
		dm.match = DATM_Before;
		dm.val = *endDate;
		AddSQLDateParameter(Where, dm, _T("HistLog.DateAdded"));
	}
	TextMatch tm;
	if (ArtistFilter != NULL)
	{
		tm.match = TXTM_Like;
		tm.val = ArtistFilter;
		AddSQLStrParameter(Where, tm, _T("HistArtists.Name"));
	}
	if (TrackFilter != NULL)
	{
		tm.match = TXTM_Like;
		tm.val = TrackFilter;
		AddSQLStrParameter(Where, tm, _T("HistTracks.Name"));
	}
	if (actID > HLA_Unknown && actID < HLA_Last)
	{
		NumericMatch nm;
		nm.match = NUMM_Exact;
		nm.val = actID;
		AddSQLNumParameter(Where, nm, _T("HistLog.actID"));
	}

	if (!Where.IsEmpty())
	{
		SQL += _T(" WHERE ");
		SQL += &((LPCTSTR)Where)[4];
	}

	SQL += _T(" GROUP BY HistTracks.artID");


	ADORecordset rs(m_pDB);
	if (!rs.Open(SQL.c_str()))
		return FALSE;
	INT countResults = 0;
	while (!rs.IsEOF())
	{
		FullHistArtistRecord* pRec = new FullHistArtistRecord;
		short startIdx = 0;
		GetHistArtistRecordFields(pRec->artist, rs, startIdx);
		pRec->hits = rs.GetUINTFieldValue(startIdx++);
		pRec->firstTime = rs.GetSYSTEMTIMEFieldValue(startIdx++);
		pRec->lastTime = rs.GetSYSTEMTIMEFieldValue(startIdx++);
		col.push_back(FullHistArtistRecordSP(pRec));
		countResults++;
		if (countResults == limitResults)
			return TRUE;
		rs.MoveNext();
	}
	return TRUE;
}


BOOL SQLManager::GetHistTrackRankCollection(std::vector<Rank>& col)
{
	std::tstring SQL(_T("SELECT hits, count(hits) ")
		_T("FROM (SELECT count(*) as Hits FROM HistLog WHERE actID = 1 GROUP BY TrackID) ")
		_T("GROUP BY hits ORDER by hits DESC"));
	ADORecordset rs(m_pDB);
	if (!rs.Open(SQL.c_str()))
		return FALSE;
	INT countResults = 0;
	while (!rs.IsEOF())
	{
		short startIdx = 0;
		Rank r;
		r.Hits = rs.GetUINTFieldValue(startIdx++);
		r.HitsCount = rs.GetUINTFieldValue(startIdx++);
		col.push_back(r);
		rs.MoveNext();
	}
	return TRUE;
}

BOOL SQLManager::GetHistArtistRankCollection(std::vector<Rank>& col)
{
	std::tstring SQL(_T("SELECT hits, count(hits) ")
		_T("FROM (SELECT count(*) as Hits FROM HistLog INNER JOIN HistTracks ON histLog.trackID=HistTracks.ID WHERE HistLog.actID = 1 GROUP BY HistTracks.artID) ")
		_T("GROUP BY hits ORDER by hits DESC"));
	ADORecordset rs(m_pDB);
	if (!rs.Open(SQL.c_str()))
		return FALSE;
	INT countResults = 0;
	while (!rs.IsEOF())
	{
		short startIdx = 0;
		Rank r;
		r.Hits = rs.GetUINTFieldValue(startIdx++);
		r.HitsCount = rs.GetUINTFieldValue(startIdx++);
		col.push_back(r);
		rs.MoveNext();
	}
	return TRUE;
}

//BOOL SQLManager::GetRandomTrackIDList(std::vector<UINT>& idList, 
//		UINT numItems, RandomTrackModeEnum rtm, UINT rtmHelperID, UINT avoidID, UINT minRating)
//{
//	ASSERT(rtm >= RTM_All && rtm < RTM_Last);
//	ASSERT((rtm==RTM_All && rtmHelperID==0) || (rtm!=RTM_All && rtmHelperID!=0));
//	TCHAR sql[1000];
//	if (rtm != RTM_Newest)
//	{
//		LPCTSTR fieldName = _T("0");
//		switch (rtm)
//		{
//		case RTM_Artist:
//			fieldName = _T("artID");
//			break;
//		case RTM_Album:
//			fieldName = _T("albID");
//			break;
//		case RTM_Genre:
//			fieldName = _T("genreID");
//			break;
//		case RTM_Year:
//			fieldName = _T("year");
//			break;
//		case RTM_Collection:
//			fieldName = _T("collectionID");
//			break;
//		case RTM_All:
//			break;
//		default:
//			ASSERT(0);
//		}
//		_sntprintf(sql, 1000, _T("SELECT Tracks.ID From Tracks INNER JOIN Collections ON Tracks.collectionID = Collections.ID ")
//			_T("WHERE Tracks.ID<>%d AND Tracks.Rating >= %d AND (Collections.type=1 OR Collections.type=3) AND %s=%d"), avoidID, minRating, fieldName, rtmHelperID);
//	}
//	else
//	{
//		_sntprintf(sql, 1000, _T("SELECT TOP %d Tracks.ID From Tracks INNER JOIN Collections ON Tracks.collectionID = Collections.ID ")
//			_T("WHERE Tracks.ID<>%d AND Tracks.Rating >= %d AND (Collections.type=1 OR Collections.type=3) ORDER BY Tracks.DateAdded DESC"), rtmHelperID, avoidID, minRating);
//	}
//
//
//	ADORecordset rs(m_pDB);
//	if (rs.Open(sql) && !rs.IsEOF())
//	{
//		UINT numTracks = 0;
//		ULONG ID = 0;
//		UINT count = rs.GetRecordCount();
//		BOOL bAlreadyInResults = FALSE;
//		ULONG pos = 0;
//		ULONG startPos = 0;
//		while(numTracks < numItems)
//		{
//			if (!bAlreadyInResults)
//			{
//				pos = (rand() * count) / RAND_MAX + 1;
//			}
//			else
//			{
//				if (pos == startPos)
//					break;//There are no more results (we ve done the circle)
//				if (startPos == 0)
//					startPos = pos;
//				pos++;
//				if (pos == count + 1)
//					pos = 1;
//			}
//			//rs.SetAbsolutePosition(pos);
//			//rs->GetFieldValue(0, ID);
//			//std::vector<ULONG>::const_iterator it = IDs.begin();
//			//bAlreadyInResults = FALSE;
//			//for (; it!= IDs.end(); it++)
//			//{
//			//	if (*it == ID)
//			//	{
//			//		bAlreadyInResults = TRUE;
//			//		break;
//			//	}
//			//}
//			//if (!bAlreadyInResults)
//			//{
//			//	startPos = 0;//Reset the flag
//			//	IDs.push_back(ID);
//			//	numTracks++;
//			//}
//		}
//		return count;
//	}
//	return 0;
//
//}
//
//

BOOL SQLManager::EvaluateTrackInfo(InfoItemTypeEnum iit)
{
	switch (iit)
	{
	case IIT_TrackLyrics:
	case IIT_TrackComment:
	case IIT_TrackPersonal:
	case IIT_TrackTablatures:
		return TRUE;
	}
	ASSERT(0);
	return FALSE;
}
BOOL SQLManager::EvaluateAlbumInfo(InfoItemTypeEnum iit)
{
	switch (iit)
	{
	case IIT_AlbumReview:
		return TRUE;
	}
	ASSERT(0);
	return FALSE;
}
BOOL SQLManager::EvaluateArtistInfo(InfoItemTypeEnum iit)
{
	switch (iit)
	{
	case IIT_ArtistBio:
		return TRUE;
	}
	ASSERT(0);
	return FALSE;
}

//-----------------------------------------
//-----------GetInfoID
//-----------------------------------------

//UINT SQLManager::GetInfoID(LPCTSTR tblName, LPCTSTR fldName, UINT ID, InfoItemTypeEnum iit)
//{
//	ASSERT(tblName != NULL && ID != NULL);
//	TCHAR bf[1000];
//	_sntprintf(bf, 1000, 
//		_T("SELECT infoID FROM %s WHERE %s=%d AND type=%d"), tblName, fldName, ID, iit);
//	ADORecordset rs(m_pDB);
//	if (!rs.Open(bf))
//		return 0;
//	INT countResults = 0;
//	if (!rs.IsEOF())
//		return rs.GetUINTFieldValue(0);
//	return 0;
//}
//
//UINT SQLManager::GetTrackInfoID(UINT trackID, InfoItemTypeEnum iit)
//{
//	ASSERT(trackID != 0);
//	if (!EvaluateTrackInfo(iit))
//		return 0;
//	return GetInfoID(_T("tracksinfo"), _T("trackID"), trackID, iit);
//}
//
//UINT SQLManager::GetAlbumInfoID(UINT albID, InfoItemTypeEnum iit)
//{
//	ASSERT(albID != 0);
//	if (!EvaluateAlbumInfo(iit))
//		return 0;
//	return GetInfoID(_T("albumsinfo"), _T("albID"), albID, iit);
//}
//
//UINT SQLManager::GetArtistInfoID(UINT artID, InfoItemTypeEnum iit)
//{
//	ASSERT(artID != 0);
//	if (!EvaluateArtistInfo(iit))
//		return 0;
//	return GetInfoID(_T("artistsinfo"), _T("artID"), artID, iit);
//}

//-----------------------------------------
//-----------AddNewInfo
//-----------------------------------------

BOOL SQLManager::AddNewInfo(LPCTSTR tblName, LPCTSTR fldName, UINT ID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(tblName != NULL && fldName != NULL && ID > 0 && info != NULL);
	if (info[0] == 0)
		return FALSE;
	ADORecordset rs(m_pDB);
	if (rs.Open(tblName, adOpenKeyset, adLockOptimistic, adCmdTable))
	{
		rs.AddNew();
		rs.SetVariantFieldValue(fldName, ID);
		rs.SetVariantFieldValue(_T("type"), iit);
		rs.SetVariantFieldValue(_T("name"), info);
		SYSTEMTIME st;
		GetLocalTime(&st);
		rs.SetSystemTimeFieldValue(_T("dateAdded"), st);
		return rs.Update();
	}
	return FALSE;
}
BOOL SQLManager::AddNewTrackInfo(UINT trackID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(trackID != 0);
	ASSERT(info != NULL);
	if (!EvaluateTrackInfo(iit))
		return FALSE;
	return AddNewInfo(_T("tracksinfo"), _T("trackid"), trackID, iit, info);
}
BOOL SQLManager::AddNewAlbumInfo(UINT albID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(albID != 0);
	ASSERT(info != NULL);
	if (!EvaluateAlbumInfo(iit))
		return FALSE;
	return AddNewInfo(_T("albumsinfo"), _T("albid"), albID, iit, info);
}
BOOL SQLManager::AddNewArtistInfo(UINT artID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(artID != 0);
	ASSERT(info != NULL);
	if (!EvaluateArtistInfo(iit))
		return FALSE;
	return AddNewInfo(_T("artistsinfo"), _T("artid"), artID, iit, info);
}

//-----------------------------------------
//-----------UpdateInfo
//-----------------------------------------
BOOL SQLManager::UpdateTrackInfo(UINT trackID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(trackID != 0);
	ASSERT(info != NULL);
	if (!EvaluateTrackInfo(iit))
		return FALSE;
	ADORecordset rs(m_pDB);
	TCHAR sql[200];
	_sntprintf(sql, 200, _T("SELECT * FROM tracksinfo WHERE trackID=%d AND type=%d"), trackID, iit);
	if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
	{
		rs.Edit();
		INT ret = rs.SetVariantFieldValue(_T("name"), info) ? 1 : 0;
		ASSERT(ret == 1);
		m_changes.info.updates++;
		return rs.Update();
	}
	return FALSE;
}
BOOL SQLManager::UpdateAlbumInfo(UINT albID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(albID != 0);
	ASSERT(info != NULL);
	if (!EvaluateAlbumInfo(iit))
		return FALSE;
	ADORecordset rs(m_pDB);
	TCHAR sql[200];
	_sntprintf(sql, 200, _T("SELECT * FROM albumsinfo WHERE albID=%d AND type=%d"), albID, iit);
	if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
	{
		rs.Edit();
		INT ret = rs.SetVariantFieldValue(_T("name"), info) ? 1 : 0;
		ASSERT(ret == 1);
		m_changes.info.updates++;
		return rs.Update();
	}
	return FALSE;
}
BOOL SQLManager::UpdateArtistInfo(UINT artID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(artID != 0);
	ASSERT(info != NULL);
	if (!EvaluateArtistInfo(iit))
		return FALSE;
	ADORecordset rs(m_pDB);
	TCHAR sql[200];
	_sntprintf(sql, 200, _T("SELECT * FROM artistsinfo WHERE artID=%d AND type=%d"), artID, iit);
	if (rs.Open(sql, adOpenDynamic, adLockPessimistic, adCmdText) && !rs.IsEOF())
	{
		rs.Edit();
		INT ret = rs.SetVariantFieldValue(_T("name"), info) ? 1 : 0;
		ASSERT(ret == 1);
		m_changes.info.updates++;
		return rs.Update();
	}
	return FALSE;
}

//-----------------------------------------
//-----------DeleteInfo
//-----------------------------------------
BOOL SQLManager::DeleteTrackInfo(UINT trackID, InfoItemTypeEnum iit)
{
	ASSERT(trackID != 0);
	if (!EvaluateTrackInfo(iit))
		return FALSE;
	TCHAR sql[500];
	_sntprintf(sql, 500, _T("DELETE FROM tracksinfo WHERE trackID=%d AND type=%d"), trackID, iit);
	return m_pDB->Execute(sql) != -1;
}

BOOL SQLManager::DeleteAlbumInfo(UINT albID, InfoItemTypeEnum iit)
{
	ASSERT(albID != 0);
	if (!EvaluateAlbumInfo(iit))
		return FALSE;
	TCHAR sql[500];
	_sntprintf(sql, 500, _T("DELETE FROM albumsinfo WHERE albID=%d AND type=%d"), albID, iit);
	return m_pDB->Execute(sql) != -1;
}
BOOL SQLManager::DeleteArtistInfo(UINT artID, InfoItemTypeEnum iit)
{
	ASSERT(artID != 0);
	if (!EvaluateArtistInfo(iit))
		return FALSE;
	TCHAR sql[500];
	_sntprintf(sql, 500, _T("DELETE FROM artistsinfo WHERE artID=%d AND type=%d"), artID, iit);
	return m_pDB->Execute(sql) != -1;
}

BOOL SQLManager::DeleteAllTrackInfo(UINT trackID)
{
	return DeleteTrackInfo(trackID, IIT_TrackComment) &&
		DeleteTrackInfo(trackID, IIT_TrackLyrics) &&
		DeleteTrackInfo(trackID, IIT_TrackPersonal) &&
		DeleteTrackInfo(trackID, IIT_TrackTablatures);
}
BOOL SQLManager::DeleteAllAlbumInfo(UINT albID)
{
	return DeleteAlbumInfo(albID, IIT_AlbumReview);
}
BOOL SQLManager::DeleteAllArtistInfo(UINT artID)
{
	return DeleteArtistInfo(artID, IIT_ArtistBio);
}

BOOL SQLManager::AdjustTrackInfo(UINT trackID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(trackID != 0);
	ASSERT(info != NULL);
	if (!EvaluateTrackInfo(iit))
		return FALSE;
	InfoRecord ir;
	if (GetTrackInfoRecord(trackID, iit, ir))
	{
		if (info[0] == NULL)
			return DeleteTrackInfo(trackID, iit);
		return UpdateTrackInfo(trackID, iit, info);
	}
	return AddNewTrackInfo(trackID, iit, info);
}
BOOL SQLManager::AdjustAlbumInfo(UINT albID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(albID != 0);
	if (!EvaluateAlbumInfo(iit))
		return FALSE;
	InfoRecord ir;
	if (GetAlbumInfoRecord(albID, iit, ir))
	{
		if (info[0] == NULL)
			return DeleteAlbumInfo(albID, iit);
		return UpdateAlbumInfo(albID, iit, info);
	}
	return AddNewAlbumInfo(albID, iit, info);
}
BOOL SQLManager::AdjustArtistInfo(UINT artID, InfoItemTypeEnum iit, LPCTSTR info)
{
	ASSERT(artID != 0);
	if (!EvaluateArtistInfo(iit))
		return FALSE;
	InfoRecord ir;
	if (GetArtistInfoRecord(artID, iit, ir))
	{
		if (info[0] == NULL)
			return DeleteArtistInfo(artID, iit);
		return UpdateArtistInfo(artID, iit, info);
	}
	return AddNewArtistInfo(artID, iit, info);
}

BOOL SQLManager::GetTrackInfoRecord(UINT trackID, InfoItemTypeEnum iit, InfoRecord& ir)
{
	ASSERT(trackID != 0);
	if (!EvaluateTrackInfo(iit))
		return FALSE;
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("SELECT ID, name, dateadded FROM TracksInfo WHERE trackID=%d AND Type=%d"), trackID, iit);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		ir.ID = rs.GetUINTFieldValue(0);
		rs.GetStringFieldValue(1, ir.name);
		ir.dateAdded = rs.GetSYSTEMTIMEFieldValue(2);
		return TRUE;
	}
	return FALSE;
}
BOOL SQLManager::GetAlbumInfoRecord(UINT albID, InfoItemTypeEnum iit, InfoRecord& ir)
{
	ASSERT(albID != 0);
	if (!EvaluateAlbumInfo(iit))
		return FALSE;
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("SELECT ID, name, dateadded FROM AlbumsInfo WHERE albID=%d AND Type=%d"), albID, iit);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		ir.ID = rs.GetUINTFieldValue(0);
		rs.GetStringFieldValue(1, ir.name);
		ir.dateAdded = rs.GetSYSTEMTIMEFieldValue(2);
		return TRUE;
	}
	return FALSE;
}
BOOL SQLManager::GetArtistInfoRecord(UINT artID, InfoItemTypeEnum iit, InfoRecord& ir)
{
	ASSERT(artID != 0);
	if (!EvaluateArtistInfo(iit))
		return FALSE;
	TCHAR sql[1000];
	_sntprintf(sql, 1000, _T("SELECT ID, name, dateadded FROM artistsInfo WHERE artID=%d AND Type=%d"), artID, iit);
	ADORecordset rs(m_pDB);
	if (rs.Open(sql, adOpenForwardOnly, adLockReadOnly, adCmdText) && !rs.IsEOF())
	{
		ir.ID = rs.GetUINTFieldValue(0);
		rs.GetStringFieldValue(1, ir.name);
		ir.dateAdded = rs.GetSYSTEMTIMEFieldValue(2);
		return TRUE;
	}
	return FALSE;
}

UINT SQLManager::GetTrackInfoFlags(UINT trackID)
{
	TCHAR bf[1000];
	_sntprintf(bf, 200, _T("SELECT type FROM tracksinfo WHERE trackID=%d"), trackID);
	UINT flags = TRF_NoFlags;
	ADORecordset rs(m_pDB);
	if (!rs.Open(bf))
		return flags;
	while (!rs.IsEOF())
	{
		switch (rs.GetUINTFieldValue(0))
		{
		case IIT_TrackComment:
			flags |= TRF_HasComments;
			break;
		case IIT_TrackLyrics:
			flags |= TRF_HasLyrics;
			break;
		case IIT_TrackPersonal:
			flags |= TRF_HasPersonal;
			break;
		case IIT_TrackTablatures:
			flags |= TRF_HasTablatures;
			break;
		}
		rs.MoveNext();
	}
	return flags;

}


bool UDgreater(SQLManager::ValuedID& elem1, SQLManager::ValuedID& elem2)
{
	return elem1.value > elem2.value;
}

BOOL SQLManager::GetSimilarArtists(std::vector<ValuedID>& col, UINT artistID, INT maxTagsToExamine, INT maxArtistsPerGenreToExamine)
{
	ASSERT(artistID != 0);
	ADORecordset rs(m_pDB);
	TCHAR sql[1000];
	_sntprintf(sql, 1000, 
		_T("SELECT genreID,count(*) ") 
		_T("FROM tracks ")
		_T("WHERE tracks.artID=%d AND tracks.genreID>1 ")
		_T("GROUP BY tracks.genreID ")
		_T("ORDER BY count(*) DESC"),
		artistID
		);

	if (!rs.Open(sql))
		return FALSE;
	INT tagSearch = 0;
	ADORecordset rsArt(m_pDB);
	ADORecordset rsArtTrackCount(m_pDB);
	while (!rs.IsEOF())
	{
		tagSearch++;
		if (tagSearch > maxTagsToExamine)//===LIMIT THE TAG SEARCHING
			break;
		UINT genID = rs.GetUINTFieldValue(0);
		UINT traCount = rs.GetUINTFieldValue(1);
		_sntprintf(sql, 1000, 
			_T("SELECT artID,count(*) ") 
			_T("FROM tracks ")
			_T("WHERE genreID=%d AND artID<>%d ")
			_T("GROUP BY artID ")
			_T("ORDER BY count(*) DESC"),
			genID,
			artistID
			);
		if (rsArt.Open(sql))
		{
			INT artSearch = 0;
			const INT maxArtSearch = 5;
			while (!rsArt.IsEOF())
			{
				artSearch++;
				if (artSearch > maxArtistsPerGenreToExamine)//===LIMIT THE TAG SEARCHING
					break;
				UINT artID = rsArt.GetUINTFieldValue(0);
				UINT count = rsArt.GetUINTFieldValue(1);
				std::vector<ValuedID>::iterator it = col.begin();
				BOOL bFound = FALSE;

				_sntprintf(sql, 1000, 
					_T("SELECT count(*) FROM tracks WHERE artID=%d"),
					artID
					);
				INT artistTrackCount = count;
				if (rsArtTrackCount.Open(sql) && !rsArtTrackCount.IsEOF())
					artistTrackCount = rsArtTrackCount.GetUINTFieldValue(0);




				DOUBLE value = (traCount * count) / (DOUBLE)artistTrackCount;
				for (; it != col.end(); it++)
				{
					if (it->ID == artID)
					{
						it->value += value;
						bFound = TRUE;
						it->appearanceCount++;
						break;
					}
				}
				if (bFound == FALSE)
					col.push_back(ValuedID(artID, value));

				rsArt.MoveNext();
			}
		}
		rs.MoveNext();
	}
	std::sort(col.begin(), col.end(), UDgreater);
	return TRUE;
}








