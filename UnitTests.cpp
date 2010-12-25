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
#include "UnitTest.h"

#include "stlStringUtils.h"
#include "WebPageUtilities.h"
#include "InformationProviders/GoogleLyrics/GoogleLyricsProvider.h"
#include "InformationProviders/CachedImageInfoProvider/CachedImageInfoProvider.h"
#include "InformationProviders/LastFMProvider/LastFMProvider.h"
#include "InformationProviders/DirImageInfoProvider/DirImageInfoProvider.h"
#include "InformationProviders/AmazonProvider/AmazonProvider.h"
#include "MultiThreadJobQueue.h"
#include "FreeDb/FreeDb.h"
#include "SQLManager.h"
#include "XFolderDialog.h"
#include "MediaPlayers/BassEngine.h"
#include "MediaPlayers/DShowEngine.h"
#include "PrgAPI.h"
#include "ApplicationMonitor.h"
#include "FileNameParser.h"
#include "TSOnlineServices.h"
#include "SystemTimeFunctions.h"
#include "AppMonitor.h"

BOOL RunTests()
{
	//UNITTEST(TestStlStringUtils());
	//UNITTEST(TestFreeDB());
	//UNITTEST(TestWebPageUtilities());
	//UNITTEST(GoogleLyricsProvider::UnitTest());
	//UNITTEST(LastFMProvider::UnitTest());
	//UNITTEST(CachedImageInfoProvider::UnitTest());
	//UNITTEST(DirImageInfoProvider::UnitTest());
	//UNITTEST(AmazonProvider::UnitTest());
	//UNITTEST(TestMultiThreadJobQueue());
	//UNITTEST(TestSQLManager());
	//UNITTEST(TestCFolderDialog());
	//UNITTEST(BassEngine::UnitTest());
	//UNITTEST(DShowEngine::UnitTest());
	UNITTEST(AppMonitor::UnitTest());
	//UNITTEST(PrgAPI::UnitTest());
	//UNITTEST(FileNameParser::UnitTest());
	//UNITTEST(SQLManager::UnitTest());
	//UNITTEST(TSOnlineServices::UnitTest());
	//UNITTEST(SystemTimeFunctions_UnitTest());
	return TRUE;
}

void UnitTestFailed(LPCSTR message, LPCSTR file)
{
#ifdef _UNICODE
	TRACE(L"!! UnitTestFailure !!: %s (%s)\r\n", (LPCTSTR)CA2CT(message), (LPCTSTR)CA2CT(file));
#else
	TRACE("!! UnitTestFailure !!: %s (%s)\r\n", message, file);
#endif
	MessageBox(0, (LPCTSTR)CA2CT(message), _T("Failed"), 0);
}

void UnitTestSuccess(LPCSTR message, LPCSTR file)
{
#ifdef _UNICODE
	TRACE(L"!-- UnitTestSuccess --: %s (%s)\r\n", (LPCTSTR)CA2CT(message), (LPCTSTR)CA2CT(file));
#else
	TRACE("-- UnitTestSuccess --: %s (%s)\r\n", message, file);
#endif
}
