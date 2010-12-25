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


class ScrobSubmitter;

class ScrobWrapper
{
public:
	ScrobWrapper();
	~ScrobWrapper();

	static void ScrobCallbackSt(int  reqId, bool error, std::string msg, void* userData);

	void ScrobCallback(int reqId, bool error, std::string msg);
	void Start(LPCTSTR artist, LPCTSTR track, LPCTSTR album, LPCTSTR musicbrainzID, INT Length, LPCTSTR path);
	void Pause();
	void Resume();
	void Stop();

	enum ERR_CODES
	{
		ERR_UnInitialized = -1,
		ERR_Success,
		ERR_Last
	};
	ERR_CODES GetErrorStatus()	{return m_Err;}
private:
	ERR_CODES m_Err;
	ScrobSubmitter* m_pScrobbler;
};
