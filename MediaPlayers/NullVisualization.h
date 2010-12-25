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
#ifndef _NullVisualization_h_
#define _NullVisualization_h_

#include "MediaVisualization.h"

class NullVisualization: public MediaVisualization
{
public:
	NullVisualization():
	m_active(FALSE),
	m_viewer(NULL)
	{}
	virtual ~NullVisualization()								{}

	virtual void SetMediaPlayerEngine(MediaPlayerEngine* mpe);
	virtual void SetContainerHWND(HWND hwnd);
	virtual HWND GetContainerHWND();
	//virtual BOOL Init(HWND hwndViewer, MediaPlayerEngine* mpe)	{m_viewer = hwndViewer;}
	//virtual void UnInit()										{}

	virtual BOOL Start(LPCTSTR str)								{m_active = TRUE;}
	virtual void Stop()											{m_active = FALSE;}

	virtual BOOL IsActive()										= 0;
private:
	HWND m_viewer;
	BOOL m_active;

};

#endif
