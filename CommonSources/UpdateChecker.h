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
class IUrlDownloader;
class AsyncUrlDownloader;

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
	void toString(LPTSTR bf)
	{
		_sntprintf(bf, 30, _T("%d.%d.%d.%d"), Major, Minor, Build, SubBuild);
	}
private:
};

#define MSG_UPDATECHECKER WM_APP+0x55
//Utilities
BOOL GetFileVersion(FileVersion& fv, HMODULE module = 0);

class UpdateChecker: public NotificationTarget<IUrlDownloader>
{
public:
	UpdateChecker(void);
	virtual ~UpdateChecker(void);

	enum Status
	{
		S_Idle,
		S_Working,
		S_Succeeded,
		S_Failed,
		S_ConnectionFailed,
		S_Last
	};

	enum UpdateSeverity
	{
		SEV_NoNeed,
		SEV_Maintenance,
		SEV_Optional,
		SEV_Recommended,
		SEV_Important,
		SEV_Last
	};

	struct CheckResult
	{
		UpdateSeverity severity;
		FileVersion currentVersion;
		FileVersion onlineVersion;
		std::tstring downloadURL;
		std::tstring infoURL;
		CheckResult():severity(SEV_NoNeed)	{}
	};
	//CheckUpdates----------------
	//URL2Chack: The URL page (HTML - php etc) that will be used for checking. 
	//	The returned page must be in a certain XML format
	//asyncHWND:	HWND to send the MSG_MODULEUPDATER message (wParam=0). 
	//				The window must query the GetCheckResult function to get the results
	//				If this value is 0 then the function is synchronous
	//				T
	BOOL CheckForNewVersion(LPCTSTR URL2Check, FileVersion& curVersion, LPCTSTR progData = NULL, HWND asyncHWND = 0);
	const CheckResult* GetCheckResult() const;
	Status GetStatus() const;

	virtual void Notify(IUrlDownloader* pCaller);

private:
	BOOL Parse(LPCSTR html, UINT size);
	std::tstring XMLGetText(LPCTSTR xml, LPCTSTR tag);
	void SetStatus(Status st);
	Status m_Status;
	CheckResult m_checkResult;
	AsyncUrlDownloader* m_pFD;
	HWND m_asyncHWND;

};
