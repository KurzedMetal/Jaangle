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
#ifndef _BaseDirector_h_
#define _BaseDirector_h_

#include "Director.h"
#include <vector>
#include <list>
class BaseDirector: public Director
{
public:
	BaseDirector();
	virtual ~BaseDirector();
public:
	virtual void RegisterActor(Actor* pActor);
	virtual void UnRegisterActor(Actor* pActor);
	virtual void SendMessage(ActorMessage& message);
	virtual void PostMessage(ActorMessage& message, BOOL bAllowGrouping = FALSE);
	virtual void HeartBeat();
private:
	std::vector<Actor*> m_pActors;
	std::list<ActorMessage> m_PostedMessages;
	DWORD m_MainThread;
	CCriticalSection m_cs;
	
};

#endif

