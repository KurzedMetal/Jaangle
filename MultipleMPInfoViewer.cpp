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
#include "MultipleMPInfoViewer.h"

BOOL MultipleMPInfoViewer::AddViewer(MPInfoViewer* viewer)
{
	for (std::vector<MPInfoViewer*>::iterator i = m_Viewers.begin(); i < m_Viewers.end(); i++)
	{
		if (viewer == *i)
		{
			ASSERT(FALSE);//Already In
			return FALSE;
		}
	}
	m_Viewers.push_back(viewer);
	return TRUE;

}
void MultipleMPInfoViewer::RemoveViewer(MPInfoViewer* viewer)
{
	for (std::vector<MPInfoViewer*>::iterator i = m_Viewers.begin(); i < m_Viewers.end(); i++)
		if (viewer == *i)
		{
			m_Viewers.erase(i);
			break;
		}
}

void MultipleMPInfoViewer::SetTitle(LPCTSTR Title)
{
	for (std::vector<MPInfoViewer*>::iterator i = m_Viewers.begin(); i < m_Viewers.end(); i++)
		(*i)->SetTitle(Title);
}
void MultipleMPInfoViewer::SetArtist(LPCTSTR Artist)
{
	for (std::vector<MPInfoViewer*>::iterator i = m_Viewers.begin(); i < m_Viewers.end(); i++)
		(*i)->SetArtist(Artist);
}
void MultipleMPInfoViewer::SetTotalSec(DOUBLE totalSec)
{
	for (std::vector<MPInfoViewer*>::iterator i = m_Viewers.begin(); i < m_Viewers.end(); i++)
		(*i)->SetTotalSec(totalSec);
}
void MultipleMPInfoViewer::SetCurSec(DOUBLE curSec)
{
	for (std::vector<MPInfoViewer*>::iterator i = m_Viewers.begin(); i < m_Viewers.end(); i++)
		(*i)->SetCurSec(curSec);
}
