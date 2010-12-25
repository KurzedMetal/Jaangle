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

#ifndef _TracksViewSchemaUser_h_
#define _TracksViewSchemaUser_h_

//#include "TracksViewSchema.h"
//#include "ObjFactory.h"
//#include "DBEngine.h"
//
//class TracksViewSchemaBase: public TracksViewSchema
//{
//public:
//	virtual INT GetRating() {INT val = 0; GetRs()->GetFieldValue(0, val); return val;}
//	virtual void GetName(TCHAR* bf, INT len){GetRs()->GetFieldValue(1, bf, len);}
//	virtual void GetAlbumName(TCHAR* bf, INT len){GetRs()->GetFieldValue(2, bf, len);}
//	virtual void GetArtistName(TCHAR* bf, INT len){GetRs()->GetFieldValue(3, bf, len);}
//	virtual INT GetTrackNo() {INT val = 0; GetRs()->GetFieldValue(4, val); return val;}
//	virtual void GetLocation(TCHAR* bf, INT len){GetRs()->GetFieldValue(5, bf, len);}
//	virtual LPCTSTR GetLocation(){return GetRs()->GetFieldValue(5);}
//	virtual UINT GetSize() {ULONG val = 0; GetRs()->GetFieldValue(6, val); return val;}
//	virtual UINT GetDuration() {ULONG val = 0; GetRs()->GetFieldValue(7, val); return val;}
//	virtual INT GetBitRate() {INT val = 0; GetRs()->GetFieldValue(8, val); return val;}
//	virtual INT GetYear() {INT val = 0; GetRs()->GetFieldValue(9, val); return val;}
//	virtual void GetGenreName(TCHAR* bf, INT len){GetRs()->GetFieldValue(10, bf, len);}
//	virtual void GetDateAdded(SYSTEMTIME& t){GetRs()->GetFieldValue(11, t);}
//	virtual void GetDateAdded(TCHAR* bf, INT len, LPCTSTR format)
//	{
//		SYSTEMTIME t;
//		GetRs()->GetFieldValue(11, t);
//		GetDateFormat(LOCALE_SYSTEM_DEFAULT, NULL, &t, format, bf, len);
//	}
//	//DOUBLE GetDateAdded() {DOUBLE val = 0; rs->GetFieldValue(11, val); return val;}
//	virtual UINT GetTrackID() {ULONG val = 0; GetRs()->GetFieldValue(12, val); return val;}
//	virtual UINT GetArtID() {ULONG val = 0; GetRs()->GetFieldValue(13, val); return val;}
//	virtual UINT GetAlbID() {ULONG val = 0; GetRs()->GetFieldValue(14, val); return val;}
//	virtual INT GetTypeID() {INT val = 0; GetRs()->GetFieldValue(15, val); return val;}
//	virtual UINT GetLyricsID() {ULONG val = 0; GetRs()->GetFieldValue(16, val); return val;}
//	virtual UINT GetReviewID() {ULONG val = 0; GetRs()->GetFieldValue(17, val); return val;}
//	virtual UINT GetCommentID() {ULONG val = 0; GetRs()->GetFieldValue(18, val); return val;}
//protected:
//	virtual CADORecordset* GetRs() = 0;
//};


//class TracksViewSchemaUser:public TracksViewSchemaBase
//{
//public:
//	TracksViewSchemaUser(CADORecordset* RecordSet)
//	{
//		ASSERT(RecordSet!=NULL);
//		m_rs = RecordSet;
//	}
//protected:
//	virtual CADORecordset* GetRs()
//	{
//		ASSERT(m_rs != NULL);
//		return m_rs;
//	}
//	CADORecordset* m_rs;
//};
//
//class TracksViewSchemaInfo:public TracksViewSchemaBase
//{
//public:
//	TracksViewSchemaInfo(UINT TrackID)
//	{
//		ASSERT(TrackID!=NULL);
//		m_TrackID = TrackID;
//	}
//	virtual UINT GetTrackID() {return m_TrackID;};
//
//private:
//	virtual CADORecordset* GetRs()
//	{
//		if (!m_rs.IsValid())
//		{
//			TCHAR sql[2000];
//			TracksViewSchema::GetSQL(sql, 2000, NULL, TrackInfo(m_TrackID), ArtistInfo(), AlbumInfo(), GenreInfo(), YearInfo());
//			CDBEngine* eng = ObjFactory::Instance()->GetDbEngine();
//			m_rs = eng->QueryDatabase(sql);
//		}
//		return &(*m_rs);
//	}
//	CADORecordsetSP m_rs;
//	UINT m_TrackID;
//};
#endif
