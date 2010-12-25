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

#define MSG_TRANSMITTER_QUIT 0


class MessageDetails
{
public:
	virtual ~MessageDetails()		{}
};

class Receiver
{
public:
	virtual void OnMessage(UINT msgID, LPVOID sender, MessageDetails* detail)	= 0;
#ifdef _DEBUG
	virtual LPCTSTR GetReceiverName()											= 0;
#endif
};

class Transmitter
{
public:
	virtual ~Transmitter()	{}

	virtual void RegisterReceiver(Receiver& receiver)									= 0;
	virtual void UnRegisterReceiver(Receiver& receiver)									= 0;
	//===SendMessage
	//Sends a message directly to the Registered Receivers
	//- msgID: The App-Defined Identification of the message. *value 0 is reserved
	//- sender: [Optional]The sender of the message.
	//- detail: [Optional] Details for the message. 
	//		The receiver must be able to cast the derived MessageDetain through the ID
	virtual void SendMessage(UINT msgID, 
		LPVOID sender = NULL, MessageDetails* detail = NULL)							= 0;
	//===PostMessage
	//Queues the message in a storage. The message is being send in the next Heartbeat.
	//Extra options
	//- bGroupSimilar. Similar msgIDs may be grouped in one.
	virtual void PostMessage(UINT msgID, 
		LPVOID sender = NULL, MessageDetails* detail = NULL, 
		BOOL bGroupSimilar = FALSE)														= 0;
	//===HeartBeat
	//- Posted Messages are send when HeartBeat is called
	virtual void HeartBeat()															= 0;
};
