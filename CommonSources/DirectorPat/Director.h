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

#define MSG_DIRECTOR_CLOSING 0

class Actor;

class ActorMessage
{
public:
	ActorMessage():ID(0),
		  Param1(0),
		  Param2(0),
		  pSourceActor(NULL)			{}
	ActorMessage(UINT msgID, INT msgParam1 = 0, INT msgParam2 = 0, Actor* pmsgSourceActor = NULL):ID(msgID),
		Param1(msgParam1),
		Param2(msgParam2),
		pSourceActor(pmsgSourceActor)	{}
public:
	INT ID;
	INT Param1;
	INT Param2;
	Actor* pSourceActor;
	INT Options;
};

class Actor
{
public:
	virtual void OnNotify(ActorMessage& message)	= 0;
#ifdef _DEBUG
	virtual LPCTSTR GetActorName()					= 0;
#endif
};

class Director
{
public:
	virtual void RegisterActor(Actor* pActor)										= 0;
	virtual void UnRegisterActor(Actor* pActor)										= 0;
	virtual void SendMessage(ActorMessage& message)									= 0;
	virtual void PostMessage(ActorMessage& message, BOOL bAllowGrouping = FALSE)	= 0;
	virtual void HeartBeat()														= 0;
};

