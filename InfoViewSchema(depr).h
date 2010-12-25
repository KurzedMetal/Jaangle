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
#ifndef _InfoViewSchema_h_
#define _InfoViewSchema_h_

#include "ViewSchema.h"
#include "InfoTableSchema.h"


class InfoViewSchema:public ViewSchema
{
public:

	InfoViewSchema()								{}
	static void GetSQL(TCHAR* sql, int len, 
		UINT InfoID);
	static void GetSQLJoined(TCHAR* sql, int len, 
		ITEM_INFO_TYPE iit, UINT itemID);

	//Artists Main Table
	virtual void GetInfo(CString& info);
	virtual INT GetType();
	virtual void GetDateAdded(SYSTEMTIME& t);
	virtual INT GetInfoID();
};

#endif
