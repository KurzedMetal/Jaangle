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
#include "ScrobWrapper.h"
#include <ScrobSubmitter.h>
#include <EncodingUtils.h>

const std::string scrobId = "TSC";

void ScrobWrapper::ScrobCallbackSt(int  reqId, bool error, std::string msg, void* userData)
{
	ScrobWrapper* me = (ScrobWrapper*) userData;
	me->ScrobCallback(reqId, error, msg);
}

void ScrobWrapper::ScrobCallback(int reqId, bool error, std::string msg)
{
	if (error)
		TRACE(_T("@1 ScrobCallback Error reqId: %d msg: %s\n"), reqId, (LPCTSTR)CA2CT(msg.c_str()));
	else
		TRACE(_T("@4 ScrobCallback Msg reqId: %d msg: %s\n"), reqId, (LPCTSTR)CA2CT(msg.c_str()));

}

ScrobWrapper::ScrobWrapper():
m_Err(ERR_UnInitialized),
m_pScrobbler(NULL)
{

}

ScrobWrapper::~ScrobWrapper()
{
	if (m_pScrobbler != NULL)
	{
		m_pScrobbler->Stop();
		m_pScrobbler->Term();
		delete m_pScrobbler;
	}
}


void ScrobWrapper::Start(LPCTSTR artist, LPCTSTR track, LPCTSTR album, LPCTSTR musicbrainzID, INT Length, LPCTSTR path)
{
	if (m_pScrobbler == NULL)
	{
		ASSERT(m_Err == ERR_UnInitialized);
		m_Err = ERR_Success;
		m_pScrobbler = new ScrobSubmitter;
		m_pScrobbler->Init(scrobId, &ScrobWrapper::ScrobCallbackSt, this);
	}
#ifdef UNICODE
	CHAR bf[MAX_PATH * 4]; 
	bf[EncodingUtils::UnicodeToUtf8(artist, (INT)_tcslen(artist), bf, MAX_PATH * 4)]=0;
	std::string ar(bf);
	bf[EncodingUtils::UnicodeToUtf8(track, (INT)_tcslen(track), bf, MAX_PATH * 4)]=0;
	std::string tr(bf);
	bf[EncodingUtils::UnicodeToUtf8(album, (INT)_tcslen(album), bf, MAX_PATH * 4)]=0;
	std::string al(bf);
	bf[EncodingUtils::UnicodeToUtf8(musicbrainzID, (INT)_tcslen(musicbrainzID), bf, MAX_PATH * 4)]=0;
	std::string mb(bf);
	bf[EncodingUtils::UnicodeToUtf8(path, (INT)_tcslen(path), bf, MAX_PATH * 4)]=0;
	std::string pa(bf);
	m_pScrobbler->Start(ar, tr, al, mb, Length, pa);
#else
	m_pScrobbler->Start(artist, track, album, musicbrainzID, Length, path);
#endif

}
void ScrobWrapper::Pause()
{
	if (m_pScrobbler != NULL)
		m_pScrobbler->Pause();
}
void ScrobWrapper::Resume()
{
	if (m_pScrobbler != NULL)
		m_pScrobbler->Resume();

}
void ScrobWrapper::Stop()
{
	if (m_pScrobbler != NULL)
		m_pScrobbler->Stop();
}
