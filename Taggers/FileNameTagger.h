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
#ifndef _FileNameTagger_h_
#define _FileNameTagger_h_

#include "FileTagger.h"

class FileNameTagger: public FileTagger
{
public:
	FileNameTagger()				{}
	virtual ~FileNameTagger()		{}

	//In the best case it ll return: TagInfo_Title
	virtual UINT Read(LPCTSTR fileName,	TagInfo& tagInfo);
	virtual UINT Write(LPCTSTR fileName, TagInfo& tagInfo)					{return TagInfo_None;}
	virtual BOOL ClearTags(LPCTSTR fileName)								{return FALSE;}

	virtual void SetConfig(Config conf, INT val)	{};
	virtual INT GetConfig(Config conf)				{return -1;};

	
	UINT CustomRead(LPCTSTR fileName, TagInfo& tagInfo, LPCTSTR format);
	UINT CustomWrite(LPTSTR newPathName, LPCTSTR fileName, LPCTSTR format,
		LPCTSTR artist, LPCTSTR album, INT trackNo, LPCTSTR title, INT year);

	
	static LPCTSTR GetReadPredifinedFormat(INT idx);
	static LPCTSTR GetWritePredifinedFormat(INT idx);

	static BOOL TranslatePattern(LPTSTR newName, UINT bfLen,
		LPCTSTR pattern, 
		LPCTSTR artist, LPCTSTR album, LPCTSTR trackNo, LPCTSTR title, LPCTSTR year, LPCTSTR discard);
	static BOOL TranslatePattern(LPTSTR newName, UINT bfLen,
		LPCTSTR pattern, 
		LPCTSTR artist, LPCTSTR album, INT trackNo, LPCTSTR title, INT year, LPCTSTR discard);

private:
	BOOL MakeMewPath(LPTSTR outPath, LPCTSTR newName, LPCTSTR originalPath);


	

}; 


#endif
