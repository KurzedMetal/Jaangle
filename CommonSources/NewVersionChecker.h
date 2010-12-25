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
#pragma once
#include "NotificationTarget.h"
//class IUrlDownloader;

struct FileVersion
{
	WORD Major;
	WORD Minor;
	WORD Build;
	WORD SubBuild;
	FileVersion():
	Major(0),Minor(0),Build(0),SubBuild(0)	{}
	FileVersion(WORD major, WORD minor=0, WORD build=0, WORD subBuild=0):
	Major(major),Minor(minor),Build(build),SubBuild(subBuild)	{}
	BOOL operator==(const FileVersion& rgt)
	{
		return Major==rgt.Major && Minor==rgt.Minor && Build==rgt.Build && SubBuild==rgt.SubBuild;
	}
	BOOL operator>(const FileVersion& rgt) const
	{
		if (Major!=rgt.Major)
			return Major > rgt.Major;
		if (Minor!=rgt.Minor)
			return Minor > rgt.Minor;
		if (Build!=rgt.Build)
			return Build > rgt.Build;
		return SubBuild > rgt.SubBuild;
	}
	//Should be more than 30 bytes
	void toString(LPTSTR bf, UINT bfLen)
	{
		_sntprintf(bf, bfLen, _T("%d.%d.%d.%d"), Major, Minor, Build, SubBuild);
	}
private:
};
BOOL GetFileVersion(FileVersion& fv, HMODULE module = 0);

class NewVersionChecker
{
public:

	enum Error
	{
		ERR_NotReady,
		ERR_OK,
		ERR_Failed,
		ERR_ConnectionFailed,
		ERR_Last
	};

	struct CheckResult
	{
		CheckResult():error(ERR_NotReady)	{}
		Error error;
		FileVersion stableVersion;
		FileVersion betaVersion;
		std::tstring downloadURL;
		std::tstring infoURL;
	};
	//CheckUpdates----------------
	//URL2Chack: The URL page (HTML - php etc) that will be used for checking. 
	//	The returned page must be in a certain XML format
	//asyncHWND:	HWND to send the MSG_MODULEUPDATER message (wParam=0). 
	//				The window must query the GetCheckResult function to get the results
	//				If this value is 0 then the function is synchronous
	//				T
	static CheckResult CheckForNewVersion(HINTERNET hNet, LPCTSTR URL2Check, LPCTSTR progData = NULL);
private:
	static std::tstring XMLGetText(LPCTSTR xml, LPCTSTR tag);
	static void String2FileVersion(FileVersion& fv, LPCTSTR str);
};
