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
#include "LastFMAdapterTracker.h"

#include "LastFMServices.h"


void LastFMAdapterTracker::SubmitTrack(Action action, const TrackerTrack& track, UINT actionTime)
{
	ASSERT(m_pLastFM != NULL);
	if (m_pLastFM == NULL)
		return;
	if (action == ACT_Played)
	{
		LastFMSubmitTrack lfmtrack;
		if (track.artist != NULL)
			lfmtrack.artist = track.artist;
		if (track.album != NULL)
			lfmtrack.album = track.album;
		if (track.title != NULL)
			lfmtrack.track = track.title;
		lfmtrack.length = track.length;
		lfmtrack.unixTimeStamp = actionTime;
		m_pLastFM->Submit(lfmtrack);
	}
}
