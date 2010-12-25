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
#include "TagLibTagger.h"
#include <fileref.h>
#include <tag.h>
#include <sstream>
#include <tfile.h>

//#define USE_PICTURES

void TagLibString2String(std::tstring& dest, TagLib::String& src)
{
	if (src.isAscii())
	{
		dest = src.toWString();
	}
	else if (src.isLatin1())
	{
		std::string latin1 = src.to8Bit(false);
		UINT bfLen = MultiByteToWideChar(CP_ACP, 0, latin1.c_str(), latin1.size(), 0, 0);
		dest.resize(bfLen + 1);
		MultiByteToWideChar(CP_ACP, 0, latin1.c_str(), latin1.size(), (LPWSTR)dest.data(), bfLen);
	}
	else
	{
		dest = src.toWString();
	}
}
void GetTag(std::tstring& dest, TagLib::String& src, INT tagFlag, INT& appendFlag)
{
	if (src.isEmpty() == false)
	{
		appendFlag |= tagFlag;
		TagLibString2String(dest, src);
	}
}



UINT TagLibTagger::Read(LPCTSTR fileName, TagInfo& info)
{
	TRACEST(_T("TagLibTagger::Read"));
	INT ret = TagInfo_None;
	ASSERT(fileName != NULL);
	try
	{
		TagLib::FileRef f(fileName, true);
		if (f.isNull())
		{
			TRACE(_T("@1 TagLibTagger::Read. FileRef Failed on: %s\r\n"), fileName);
			return TagInfo_None;
		}
		TagLib::Tag *tag = f.tag();
		if (tag == NULL)
		{
			TRACE(_T("@1 TagLibTagger::Read. Get tag failed on: %s\r\n"), fileName);
			return TagInfo_None;
		}

		TagLib::String s = tag->title();
		GetTag(info.Title, s, TagInfo_Title, ret);
		s = tag->artist();
		GetTag(info.Artist, s, TagInfo_Artist, ret);
		s = tag->albumArtist_AL();
		GetTag(info.AlbumArtist, s, TagInfo_AlbumArtist, ret);
		s = tag->composer_AL();
		GetTag(info.Composer, s, TagInfo_Composer, ret);
		s = tag->album();
		GetTag(info.Album, s, TagInfo_Album, ret);
		s = tag->comment();
		GetTag(info.Comment, s, TagInfo_Comment, ret);
		s = tag->genre();
		GetTag(info.Genre, s, TagInfo_Genre, ret);
		s = tag->lyrics_AL();
		GetTag(info.Lyrics, s, TagInfo_Lyrics, ret);
		info.Year = tag->year();
		if (info.Year > 0 && info.Year < 2100)
			ret |= TagInfo_Year;
		else
			info.Year = 0;
		info.TrackNo = tag->track();
		if (info.TrackNo > 0 && info.TrackNo < 10000)
			ret|=TagInfo_TrackNo;
		else
			info.TrackNo = 0;
		info.Rating = tag->rating_AL();
		if (info.Rating > 0 && info.Rating <= 255)
			ret|=TagInfo_Rating;
		else
			info.Rating = 0;

#ifdef USE_PICTURES
		TagLib::String mime;
		TagLib::ByteVector data;
		if (tag->picture_AL(mime, data, s))
		{
			if (mime == "png")
				info.Picture.type = TPT_Png;
			else
				info.Picture.type = TPT_Jpeg;
			info.Picture.data.assign(data.begin(), data.end());
			std::tstring fName = fileName;
			fName += _T(".jpg");
			CFile f;
			if (f.Open(fName.c_str(), CFile::modeCreate | CFile::modeWrite))
			{
				f.Write(&info.Picture.data[0], info.Picture.data.size());
				f.Close();
				ShellExecute(0, 0, fName.c_str(), 0, 0, 0);
			}
		}
#endif
		TagLib::AudioProperties *properties = f.audioProperties();
		if (properties)
		{
			info.Length = properties->length();
			if (info.Length != 0)			
				ret |= TagInfo_Length;
			info.Bitrate = properties->bitrate();
			if (info.Bitrate != 0)
				ret |= TagInfo_Bitrate;
			//if (properties->sampleRate() != 0)
			//{
			//	TCHAR bf[200];
			//	_sntprintf(bf, 200, _T("%d[%d]"), properties->sampleRate(), properties->channels());
			//	info.FormatInfo = bf;
			//	ret |= TagInfo_FormatInfo;
			//}
		}	
	}
	catch (...)
	{
		TRACE(_T("@1 TagLibTagger::Read Exception***\r\n"));
	}

	if (ret == 0)
		TRACE(_T("@1 TagLibTagger::Read Failed ON: %s\r\n"), fileName);
	return ret;
}


UINT TagLibTagger::Write(LPCTSTR fileName, TagInfo& info)
{
	TRACEST(_T("TagLibTagger::Write"), fileName);
	if (info.validFields == TagInfo_None)
	{
		TRACE(_T("@3 TagLibTagger::Write. No valid fields to tag\r\n"));
		return TagInfo_None;
	}
	try
	{
		TagLib::FileRef f(fileName, false);
		if (f.isNull())
		{
			TRACE(_T("@1 TagLibTagger::Write. FileRef Failed\r\n"));
			return TagInfo_None;
		}
		TagLib::Tag *tag = f.tag();
		if (tag == NULL)
		{
			TRACE(_T("@1 TagLibTagger::Write. Get tag failed\r\n"));
			return TagInfo_None;
		}

		BOOL bUnicode = TRUE;//=== Set it to FALSE for the non-unicode version
		if (bUnicode)
		{
			//=== Writes everything in unicode (utf8).
			//=== Advantages
			//=== 1. No problems with different languages
			//=== Disadvantages
			//=== 1. Not 100% compatible with all tag readers
			//=== 2. Id3v1 is not properly written (Text tags are empty)
			if (info.IsValid(TagInfo_Title))	tag->setTitle(info.Title);
			if (info.IsValid(TagInfo_Album))	tag->setAlbum(info.Album);
			if (info.IsValid(TagInfo_Artist))	tag->setArtist(info.Artist);
			if (info.IsValid(TagInfo_Comment))	tag->setComment(info.Comment);
			if (info.IsValid(TagInfo_Genre))	tag->setGenre(info.Genre);
			if (info.IsValid(TagInfo_Lyrics))	tag->setLyrics_AL(info.Lyrics);
			if (info.IsValid(TagInfo_AlbumArtist))	tag->setAlbumArtist_AL(info.AlbumArtist);
			if (info.IsValid(TagInfo_Composer))	tag->setComposer_AL(info.Composer);
		}
		else
		{
			INT convertionCP = CP_ACP;//=== otherwise 1252
			const INT cBfLen = 60000;
			CHAR bf[cBfLen + 1];
			INT len = WideCharToMultiByte(convertionCP, 0, info.Title.c_str(), -1, bf, cBfLen, 0, 0);
			bf[len] = 0;
			if (info.IsValid(TagInfo_Title))	tag->setTitle(bf);
			len = WideCharToMultiByte(convertionCP, 0, info.Album.c_str(), -1, bf, cBfLen, 0, 0);
			bf[len] = 0;
			if (info.IsValid(TagInfo_Album))	tag->setAlbum(bf);
			len = WideCharToMultiByte(convertionCP, 0, info.Artist.c_str(), -1, bf, cBfLen, 0, 0);
			bf[len] = 0;
			if (info.IsValid(TagInfo_Artist))	tag->setArtist(bf);
			len = WideCharToMultiByte(convertionCP, 0, info.Comment.c_str(), -1, bf, cBfLen, 0, 0);
			bf[len] = 0;
			if (info.IsValid(TagInfo_Comment))	tag->setComment(bf);
			len = WideCharToMultiByte(convertionCP, 0, info.Genre.c_str(), -1, bf, cBfLen, 0, 0);
			bf[len] = 0;
			if (info.IsValid(TagInfo_Genre))	tag->setGenre(bf);
			len = WideCharToMultiByte(convertionCP, 0, info.Lyrics.c_str(), -1, bf, cBfLen, 0, 0);
			bf[len] = 0;
			if (info.IsValid(TagInfo_Lyrics))	tag->setLyrics_AL(bf);

		}
		if (info.IsValid(TagInfo_Year))		tag->setYear(info.Year);
		if (info.IsValid(TagInfo_TrackNo))	tag->setTrack(info.TrackNo);
		if (info.IsValid(TagInfo_Rating))	tag->setRating_AL(info.Rating);

		//{
		//	UINT len = WideCharToMultiByte(CP_UTF8, 0, info.Lyrics.c_str(),-1,0,0,0,0);
		//	CHAR* utf8 = new CHAR[len + 1];
		//	utf8[len] = 0;
		//	WideCharToMultiByte(CP_UTF8, 0, info.Lyrics.c_str(),-1,utf8,len,0,0);
		//	TagLib::String lyrics(utf8, TagLib::String::UTF8);
		//	tag->setLyrics_AL(lyrics);
		//	delete utf8;
		//}
		if (!f.save())	
		{
			TRACE(_T("@1 TagLibTagger::Write. Save tag failed\r\n"));
			return TagInfo_None;
		}
		return info.validFields & (TagInfo_Title|TagInfo_Album|TagInfo_Artist|TagInfo_Year|TagInfo_Comment
			|TagInfo_TrackNo|TagInfo_Genre|TagInfo_Lyrics|TagInfo_Rating);
	}
	catch(...)
	{
		TRACE(_T("@1 TagLibTagger::Write. Exception***\r\n"));
	}
	return TagInfo_None;
}

BOOL TagLibTagger::ClearTags(LPCTSTR fileName)
{
	return FALSE;
}

