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
#ifndef _VectorTransmitter_h_
#define _VectorTransmitter_h_

#include "Transmitter.h"
#include <vector>
#include <list>

//VectorTransmitter is a Thread-Safe Vector Implementation of Transmitter.
//It stores a "Main Thread" Id when the object is created
//SendMessages from other threads are posted!
class VectorTransmitter:public Transmitter
{
public:
	VectorTransmitter();
	virtual ~VectorTransmitter();

	virtual void RegisterReceiver(Receiver& actor);
	virtual void UnRegisterReceiver(Receiver& actor);
	virtual void SendMessage(UINT msgID, 
		LPVOID pSender = NULL, MessageDetails* pMsgDetail = NULL);
	virtual void PostMessage(UINT msgID, 
		LPVOID pSender = NULL, MessageDetails* pMsgDetail = NULL, 
		BOOL bGroupSimilar = FALSE);
	virtual void HeartBeat();
private:
	std::vector<Receiver*> m_pReceivers;
	struct MessageStruct
	{
		UINT msgID;
		LPVOID pSender;
		MessageDetails* pMsgDetail;
	};
	std::list<MessageStruct> m_PostedMessages;
	DWORD m_MainThread;
	CCriticalSection m_cs;
};

#endif

