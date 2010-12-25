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
#ifndef _TagInfo_h_
#define _TagInfo_h_

#define	TagInfo_None		0

#define	TagInfo_Title		0x1
#define	TagInfo_Album		0x2
#define	TagInfo_Artist		0x4
#define	TagInfo_Genre		0x8
#define	TagInfo_Comment		0x10
#define	TagInfo_Lyrics		0x20
#define	TagInfo_Year		0x40
#define	TagInfo_TrackNo		0x80
#define	TagInfo_Rating		0x100
#define	TagInfo_Bitrate		0x200
#define	TagInfo_Size		0x400
#define	TagInfo_Length		0x800
#define	TagInfo_Picture		0x1000
#define	TagInfo_AlbumArtist	0x2000
#define	TagInfo_Composer	0x4000

#define	TagInfo_All			0x8000 - 1

#define TagInfo_Tags		TagInfo_Title|TagInfo_Album|TagInfo_Artist|TagInfo_Genre|TagInfo_Comment|TagInfo_Lyrics|TagInfo_Personal|TagInfo_Year|TagInfo_TrackNo|TagInfo_Rating
#define TagInfo_Format		TagInfo_Bitrate|TagInfo_Size|TagInfo_Length
//|TagInfo_FormatInfo



enum TagPictureTypeEnum
{
	TPT_None,
	TPT_Jpeg,
	TPT_Png,
	TPT_Last
};


struct TagPicture
{
	TagPicture():type(TPT_None){}
	std::vector<BYTE> data;
	TagPictureTypeEnum type;
};


struct TagInfo
{
	TagInfo():
	  Year(0),
		  TrackNo(0),
		  Rating(0),
		  Size(0),
		  Length(0),
		  Bitrate(0),
		validFields(0)
	  {}
	  std::tstring Title;
	  std::tstring Album;
	  std::tstring Artist;
	  std::tstring AlbumArtist;
	  std::tstring Composer;
	  std::tstring Genre;
	  std::tstring Comment;
	  std::tstring Lyrics;
	  INT Year;
	  INT TrackNo;
	  INT Rating;					//1-255, 0 = Unknown
	  INT Bitrate;					//KBps
	  UINT Size;					//Kb
	  UINT Length;					//secs
	  TagPicture Picture;
	  //std::tstring FormatInfo;
	  UINT validFields;
	  BOOL IsValid(UINT fieldFlag) {return (validFields & fieldFlag) == fieldFlag;}
};

#endif
