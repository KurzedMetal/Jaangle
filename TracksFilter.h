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
#ifndef _TracksFilter_h_
#define _TracksFilter_h_

enum TextMatchType
{
	TXTM_Disabled,
	TXTM_Exact,
	TXTM_Like
};
struct TextMatch
{
	TextMatch():match(TXTM_Disabled){}
	std::tstring val;
	TextMatchType match;
};



enum NumericMatchType
{
	NUMM_Disabled = 0,
	NUMM_Exact,
	NUMM_Over,
	NUMM_Under,
	NUMM_Diff
};

struct NumericMatch
{
	NumericMatch():val(0),match(NUMM_Disabled){}
	INT val;
	NumericMatchType match;
};

enum DateMatchType
{
	DATM_Disabled = 0,
	DATM_After,
	DATM_Before
};
struct DateMatch
{
	DateMatch():match(DATM_Disabled){memset(&val, 0, sizeof(SYSTEMTIME));}
	SYSTEMTIME val;
	DateMatchType match;
};

class TracksFilter
{
public:
	TracksFilter()								{}
//	TracksFilter(TracksFilter& tf)				{*this = tf;}

	NumericMatch TrackID;
	TextMatch Title;
	NumericMatch TrackType;
	DateMatch DateAdded;
	NumericMatch MonthAdded;
	NumericMatch AlbumID;
	NumericMatch ArtistID;
	NumericMatch CollectionID;
	NumericMatch GenreID;
	TextMatch Location;
	NumericMatch Bitrate;
	NumericMatch Duration;
	NumericMatch Size;
	NumericMatch TrackNo;
	NumericMatch Year;
	NumericMatch Rating;

	NumericMatch MinNumTracks;

	NumericMatch ArtistTrackCount;
	NumericMatch AlbumTrackCount;
	NumericMatch GenreTrackCount;
	NumericMatch CollectionTrackCount;
	NumericMatch YearTrackCount;
	NumericMatch MonthAddedTrackCount;

	TextMatch Album;
	TextMatch Artist;
	TextMatch Genre;
	NumericMatch CollectionStatus;
	NumericMatch CollectionType;


	BOOL IsEmpty()
	{
		return 
			(TrackID.match == NUMM_Disabled &&
			Title.match == TXTM_Disabled &&
			TrackType.match == NUMM_Disabled &&
			DateAdded.match == DATM_Disabled &&
			MonthAdded.match == DATM_Disabled &&
			AlbumID.match == NUMM_Disabled &&
			ArtistID.match == NUMM_Disabled &&
			CollectionID.match == NUMM_Disabled &&
			GenreID.match == NUMM_Disabled &&
			Location.match == TXTM_Disabled &&
			Bitrate.match == NUMM_Disabled &&
			Duration.match == NUMM_Disabled &&
			Size.match == NUMM_Disabled &&
			TrackNo.match == NUMM_Disabled &&
			Year.match == NUMM_Disabled &&
			Rating.match == NUMM_Disabled &&

			MinNumTracks.match == NUMM_Disabled &&

			ArtistTrackCount.match == NUMM_Disabled &&
			AlbumTrackCount.match == NUMM_Disabled &&
			GenreTrackCount.match == NUMM_Disabled &&
			CollectionTrackCount.match == NUMM_Disabled &&
			YearTrackCount.match == NUMM_Disabled &&
			MonthAddedTrackCount.match == NUMM_Disabled &&

			Album.match == TXTM_Disabled &&
			Artist.match == TXTM_Disabled &&
			Genre.match == TXTM_Disabled &&
			CollectionStatus.match == NUMM_Disabled &&
			CollectionType.match == NUMM_Disabled);
	}

};



#endif
