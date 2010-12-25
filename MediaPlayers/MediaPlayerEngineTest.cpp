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
#include "MediaPlayerEngine.h"
#include "EventTarget.h"

#ifdef _UNITTESTING
class EventTargetDeriv:public EventTarget
{
	virtual INT OnEvent(INT ID, INT param /*= 0*/, void* sender /*= 0*/)
	{
		TRACE(_T("@D TestBassEngine. OnEvent: %d - %d\r\n"), ID, param);
		TRACE(_T("@D TestBassEngine. OnEvent Sender: %d \r\n"), (INT) sender);
		return 0;
	}

};

LPCTSTR testFName = _T("D:\\Musi1\\A Perfect Circle - Emotive\\07-a_perfect_circle--people_are_people-fnt.mp3");
//LPCTSTR testFName = _T("D:\\Musi1\\8eodwrakhs Mikhs - 01 - Kaymos.mp3");
LPCTSTR testFName2 = _T("D:\\Musi1\\Electric six - 01 Dance Commander.mp3");

//Transfers Ownership
BOOL TestGenericEngine(MediaPlayerEngine* pEng)
{
	//BassEngine* pEng = new BassEngine;
	MessageBox(NULL, pEng->GetEngineDescription(), _T("Engine Description"), MB_OK);
	UNITTEST(pEng->GetEngineState() == MES_Ready);
	for (INT i = MECO_First + 1; i < MECO_Last; i++)
	{
		if (pEng->Supports(MediaEngineConfigOption(i)))
			TRACE(_T("@D TestBassEngine. Supports: %d Value:%d\r\n"), i, pEng->GetConfig(MediaEngineConfigOption(i)));
		else
			TRACE(_T("@D TestBassEngine. Not supported: %d \r\n"), i);
	}
	EventTargetDeriv evTarg;
	pEng->SetEventTarget(&evTarg);
	UNITTEST(pEng->CanPlay(testFName));
	UNITTEST(pEng->Open(testFName));
	UNITTEST(pEng->GetPlayState() == PS_Stopped);
	pEng->Start();
	UNITTEST(pEng->GetPlayState() == PS_Playing);
	Sleep(1000);
	TRACE(_T("OptionalMediaInfo: Ch: %d\r\n"), 
		pEng->GetMediaInfo(MEOII_Channels)
		);
	TRACE(_T("OptionalMediaInfo: Freq: %d\r\n"), 
		pEng->GetMediaInfo(MEOII_Frequency)
		);
	TRACE(_T("OptionalMediaInfo: bitrate: %d\r\n"), 
		pEng->GetMediaInfo(MEOII_Bitrate)
		);
	UNITTEST(pEng->GetMediaLength() > 60.0);
	UNITTEST(_tcscmp(testFName, pEng->GetLocation()) == 0);

	UNITTEST(pEng->GetMediaPos() > .5);
	pEng->SetMediaPos(30.0);
	Sleep(1000);
	UNITTEST(pEng->GetMediaPos() > 30.0);
	pEng->Pause();
	UNITTEST(pEng->GetPlayState() == PS_Paused);
	pEng->SetMediaPos(45.0);
	UNITTEST(pEng->GetMediaPos() == 45.0);
	pEng->Start();
	Sleep(1000);
	pEng->Stop();
	UNITTEST(pEng->GetPlayState() == PS_Stopped);
	pEng->SetMediaPos(30.0);
	pEng->Start();
	Sleep(1000);
	pEng->Close();
	UNITTEST(pEng->Open(testFName));
	pEng->Start();
	pEng->SetMediaPos(30.0);
	pEng->SetVolume(30);
	UNITTEST(pEng->GetVolume() == 30);
	Sleep(1000);
	pEng->SetVolume(80);
	UNITTEST(pEng->GetVolume() == 80);
	Sleep(1000);
	pEng->SetMute(TRUE);
	UNITTEST(pEng->GetMute());
	Sleep(1000);
	pEng->SetMute(FALSE);
	UNITTEST(pEng->GetEngineState() == MES_Ready);
	UNITTEST(pEng->GetLastError() == MEE_NoError);
	UNITTEST(pEng->IsVideo() == FALSE);
	BYTE bf[500];
	TRACE(_T("GetSoundData: %s\r\n"), pEng->GetSoundData(MSD_Float, bf, 500) ? _T("OK") : _T("FAIL"));
	UNITTEST(pEng->GetVideoContainerHWND() == NULL);
	pEng->Close();
	UNITTEST(pEng->Open(testFName));
	pEng->Start();
	UNITTEST(pEng->Open(testFName2));
	pEng->Start();
	Sleep(2000);
	delete pEng;

	return TRUE;

	//virtual void SetVideoContainerHWND(HWND hwnd)						{}
	//virtual HWND GetVideoContainerHWND()									{return 0;}
	//virtual void SetVideoPosition(int x, int y, int cx, int cy)	{};

	////Extra IF;
	////void NotifyOnPosition(DOUBLE pos, BOOL bFromEnd);
	//void FadedStart(DOUBLE fadeLength);
	//void FadedStop(DOUBLE fadeLength);


}
#endif
