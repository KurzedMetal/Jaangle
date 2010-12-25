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
#include "TSOnlineAdapterTracker.h"
#include "TSOnlineServices.h"
#include "PrgAPI.h"


void TSOnlineAdapterTracker::SubmitTrack(Action action, const TrackerTrack& track, UINT actionTime)
{
	ASSERT(m_pTSOS != NULL);
	if (m_pTSOS != NULL && action == ACT_Played)
	{
		TSOnlineServices::TrackInfo ti;
		ti.artist = track.artist;
		ti.title = track.title;
		ti.album = track.album;
		ti.tag = track.tag;
		ti.rating = track.rating;
		ti.lyrics = track.lyrics;
		m_pTSOS->SendTrackInfo(ti, actionTime);
	}
}
