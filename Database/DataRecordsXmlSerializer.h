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
#ifndef _DataRecordsXmlSerializer_h_
#define _DataRecordsXmlSerializer_h_

#include "DataRecords.h"
struct IXmlWriter;

class DataRecordsXmlSerializer
{
public:
	static BOOL Export(LPCTSTR fileName, FullTrackRecordCollection& col);
	static BOOL Export(LPCTSTR fileName, FullHistLogRecordCollection& col);
	static BOOL Export(LPCTSTR fileName, FullHistTrackRecordCollection& col);
	static BOOL Export(LPCTSTR fileName, FullHistArtistRecordCollection& col);

	static BOOL Import(LPCTSTR fileName, FullTrackRecordCollection& col);
	static BOOL Import(LPCTSTR fileName, FullHistLogRecordCollection& col);

private:
	static void AppendXMLElement(IXmlWriter& xmlWriter, TrackRecord& rec);
	static void AppendXMLElement(IXmlWriter& xmlWriter, AlbumRecord& rec);
	static void AppendXMLElement(IXmlWriter& xmlWriter, ArtistRecord& rec);
	static void AppendXMLElement(IXmlWriter& xmlWriter, GenreRecord& rec);
	static void AppendXMLElement(IXmlWriter& xmlWriter, CollectionRecord& rec);
	static void AppendXMLElement(IXmlWriter& xmlWriter, HistLogRecord& rec);
	static void AppendXMLElement(IXmlWriter& xmlWriter, HistArtistRecord& rec);
	static void AppendXMLElement(IXmlWriter& xmlWriter, HistTrackRecord& rec);


	static LPCTSTR SystemTime2String(LPTSTR buffer, SYSTEMTIME& st);

};

#endif
