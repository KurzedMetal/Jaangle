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

#include "NotificationTarget.h"

class IUrlDownloader
{
public:
	enum Status
	{
		S_Idle,
		S_Working,
		S_Success,
		S_Failure,
		S_CantOpenInternet,
		S_Last
	};
	virtual ~IUrlDownloader(void)						{}

	virtual BOOL Init(LPCTSTR agent = NULL)				= 0;

	virtual BOOL DownloadURL(LPCTSTR url)				= 0;
	virtual void Abort()								= 0;

	virtual const BYTE* GetBuffer() const				= 0;
	virtual UINT GetBufferSize() const					= 0;
	virtual Status GetStatus() const					= 0;

	virtual void Clear()								= 0;

	//Utilities
	LPSTR CreateCharCopy() const;
	BOOL SaveToFile(LPCTSTR path) const;

};
