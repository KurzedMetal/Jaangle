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
#ifndef _YouTubePlayerEngine_h_
#define _YouTubePlayerEngine_h_

#include "MediaPlayerEngine.h"
#include "SimpleBrowser.h"
#include <string>
#include <vector>

class YouTubePlayerEngine: public MediaPlayerEngine, private SimpleBrowserNotifier
{
public:
	YouTubePlayerEngine();
	virtual ~YouTubePlayerEngine();
	virtual void SetEventTarget(EventTarget* evTarg)					{m_pEventTarget = evTarg;}
	virtual EventTarget* GetEventTarget()								{return m_pEventTarget;}


	virtual void SetConfig(MediaEngineConfigOption meco, INT value)		{}
	virtual INT GetConfig(MediaEngineConfigOption meco)					{return -1;}
	virtual BOOL Supports(MediaEngineConfigOption meco)					{return FALSE;}


	virtual BOOL Open(LPCTSTR str);
	virtual void Close();

	virtual BOOL Start();
	virtual BOOL Pause();
	virtual BOOL Stop();

	virtual PlayState GetPlayState();

	virtual INT GetMediaInfo(MediaEngineOptionalIntInfoEnum meoii)		{return -1;}
	virtual LPCTSTR GetLocation()			{return m_videoURL.c_str();} 
	virtual DOUBLE GetMediaLength();


	virtual DOUBLE GetMediaPos();
	virtual void SetMediaPos(DOUBLE secs);

	virtual void SetVolume(INT volume);
	virtual INT GetVolume();
	virtual void SetMute(BOOL bMute);
	virtual BOOL GetMute();

	virtual BOOL CanPlay(LPCTSTR str);
	virtual MediaEngineState GetEngineState();
	virtual MediaEngineErrorEnum GetLastError();

	//Visualization
	virtual BOOL IsVideo()							{return TRUE;}
	virtual const TCHAR* GetEngineDescription()		{return _T("YouTube");}

	virtual BOOL GetSoundData(MediaEngineSoundData mesd, void* buffer, UINT bufferLen)	{return FALSE;}

	virtual void SetVideoContainerHWND(HWND hwnd);
	virtual HWND GetVideoContainerHWND();
	virtual void SetVideoPosition(int x, int y, int cx, int cy);

	void SetYouTubeAPIURL(LPCTSTR apiURL)	{m_apiURL = apiURL;}
	LPCTSTR GetYouTubeAPIURL()				{return m_apiURL.c_str();}



private:
	virtual void OnBeforeNavigate(LPCTSTR url)								
	{
		TRACE(_T("OnBeforeNavigate: %s\r\n"), url);
	}
	virtual void OnDocumentComplete(LPCTSTR url)
	{
		TRACE(_T("OnDocumentComplete: %s\r\n"), url);
	}
	virtual void OnDownloadBegin()											
	{
		TRACE(_T("OnDownloadBegin\r\n"));
	}
	virtual void OnProgressChange(long progress,long progress_max)
	{
		TRACE(_T("OnProgressChange: %d / %d\r\n"), progress, progress_max);
	}
	virtual void OnDownloadComplete()
	{
		TRACE(_T("OnDownloadComplete\r\n"));
	}
	virtual void OnNavigateComplete(LPCTSTR url)
	{
		TRACE(_T("OnNavigateComplete: %s\r\n"), url);
	}
	virtual void OnStatusTextChange(LPCTSTR text);
	//{
	//	TRACE(_T("OnStatusTextChange: %s\r\n"), text);
	//}
	virtual void OnTitleChange(LPCTSTR text)
	{
		TRACE(_T("OnTitleChange: %s\r\n"), text);
	}
	virtual void EnableBackButton(BOOL bEnable)
	{
		TRACE(_T("EnableBackButton: %d\r\n"), bEnable);
	}
	virtual void EnableNextButton(BOOL bEnable)
	{
		TRACE(_T("EnableNextButton: %d\r\n"), bEnable);
	}


private:
	BOOL GetVideoID(LPCTSTR url, LPTSTR id, UINT idLen);
	INT GetIntValue(LPCTSTR divName);
	DOUBLE GetDoubleValue(LPCTSTR divName);
	BOOL ExecuteCommand(LPCTSTR command);
private:
	SimpleBrowser* m_pBrowser;
	HWND m_hwndParent;
	RECT m_rcVideoPosition;
	std::basic_string<TCHAR> m_apiURL;
	std::basic_string<TCHAR> m_videoURL;
	std::vector<std::basic_string<TCHAR> > m_commandsToExecute;
	INT m_volume;
	BOOL m_bMute;
	DOUBLE m_mediaLength;
	BOOL m_bNavigating;
	EventTarget* m_pEventTarget;
	BOOL m_bReachedEndSend;

};

#endif
