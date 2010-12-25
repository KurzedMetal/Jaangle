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
#include "GenericMediaFile.h"


void GenericMediaFile::Open(LPCTSTR location)
{
	//Check The FileName and set The correct mediaFile Object
	if (TRUE)
		m_Actual = &m_Mp3MF;
	m_Actual->Open(location);
}

void GenericMediaFile::Save()
{
	ASSERT(m_Actual != NULL);
	m_Actual->Save();
}

void GenericMediaFile::GetTrackName(TCHAR* TrackName, UINT len)
{
	ASSERT(TrackName != NULL);
	ASSERT(len>0);
	ASSERT(m_Actual != NULL);
	m_Actual->GetTrackName(TrackName, len);

}

void GenericMediaFile::SetTrackName(LPCTSTR TrackName)
{
	ASSERT(TrackName != NULL);
	ASSERT(m_Actual != NULL);
	m_Actual->SetTrackName(TrackName);
}

void GenericMediaFile::GetAlbumName(LPTSTR AlbumName, UINT len)
{
	ASSERT(AlbumName != NULL);
	ASSERT(len>0);
	ASSERT(m_Actual != NULL);
	m_Actual->GetAlbumName(AlbumName, len);

}
void GenericMediaFile::SetAlbumName(LPCTSTR AlbumName)
{
	ASSERT(AlbumName != NULL);
	ASSERT(m_Actual != NULL);
	m_Actual->SetAlbumName(AlbumName);

}

void GenericMediaFile::GetArtistName(LPTSTR ArtistName, UINT len)
{
	ASSERT(ArtistName != NULL);
	ASSERT(len>0);
	ASSERT(m_Actual != NULL);
	m_Actual->GetArtistName(ArtistName, len);

}
void GenericMediaFile::SetArtistName(LPCTSTR ArtistName)
{
	ASSERT(ArtistName != NULL);
	ASSERT(m_Actual != NULL);
	m_Actual->SetArtistName(ArtistName);

}

void GenericMediaFile::GetGenreName(LPTSTR GenreName, UINT len)
{
	ASSERT(GenreName != NULL);
	ASSERT(len>0);
	ASSERT(m_Actual != NULL);
	m_Actual->GetGenreName(GenreName, len);

}
void GenericMediaFile::SetGenreName(LPCTSTR GenreName)
{
	ASSERT(GenreName != NULL);
	ASSERT(m_Actual != NULL);
	m_Actual->SetGenreName(GenreName);

}

INT GenericMediaFile::GetTrackNo()
{
	ASSERT(m_Actual != NULL);
	return m_Actual->GetTrackNo();
}
void GenericMediaFile::SetTrackNo(INT TrackNo)
{
	ASSERT(m_Actual != NULL);
	m_Actual->SetTrackNo(TrackNo);

}

INT GenericMediaFile::GetYear()
{
	ASSERT(m_Actual != NULL);
	return m_Actual->GetYear();
}
void GenericMediaFile::SetYear(INT Year)
{
	ASSERT(m_Actual != NULL);
	m_Actual->SetYear(Year);

}

INT GenericMediaFile::GetRating()
{
	ASSERT(m_Actual != NULL);
	return m_Actual->GetRating();
}
void GenericMediaFile::SetRating(INT Rating)
{
	ASSERT(m_Actual != NULL);
	m_Actual->SetRating(Rating);

}

void GenericMediaFile::GetLyrics(LPTSTR Lyrics, UINT len)
{
	ASSERT(Lyrics != NULL);
	ASSERT(len>0);
	ASSERT(m_Actual != NULL);
	m_Actual->GetLyrics(Lyrics, len);

}
void GenericMediaFile::SetLyrics(LPCTSTR Lyrics)
{
	ASSERT(Lyrics != NULL);
	ASSERT(m_Actual != NULL);
	m_Actual->SetLyrics(Lyrics);

}

void GenericMediaFile::GetComment(LPTSTR Comment, UINT len)
{
	ASSERT(Comment != NULL);
	ASSERT(len>0);
	ASSERT(m_Actual != NULL);
	m_Actual->GetComment(Comment, len);

}
void GenericMediaFile::SetComment(LPCTSTR Comment)
{
	ASSERT(Comment != NULL);
	ASSERT(m_Actual != NULL);
	m_Actual->SetComment(Comment);

}

void GenericMediaFile::GetPersonal(LPTSTR Personal, UINT len)
{
	ASSERT(Personal != NULL);
	ASSERT(len>0);
	ASSERT(m_Actual != NULL);
	m_Actual->GetPersonal(Personal, len);

}
void GenericMediaFile::SetPersonal(LPCTSTR Personal)
{
	ASSERT(Personal != NULL);
	ASSERT(m_Actual != NULL);
	m_Actual->SetPersonal(Personal);

}

INT GenericMediaFile::GetBitrate()
{
	ASSERT(m_Actual != NULL);
	return m_Actual->GetBitrate();
}
INT GenericMediaFile::GetDuration()
{
	ASSERT(m_Actual != NULL);
	return m_Actual->GetDuration();
}
