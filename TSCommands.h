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
#ifndef TSCommands_h
#define TSCommands_h

//Use WM_COMMAND with TSCommand as the wParam
enum TSCommand
{
	EXTERNALCOMMAND_First = 0x8800,
	EXTERNALCOMMAND_Previous,		//Returns 0 on success
	EXTERNALCOMMAND_Play,			//Returns 0 on success
	EXTERNALCOMMAND_Pause,			//Returns 0 on success
	EXTERNALCOMMAND_Stop,			//Returns 0 on success
	EXTERNALCOMMAND_Next,			//Returns 0 on success
	EXTERNALCOMMAND_ShowHidePlayer,	//lparam=1 to show. 0 to hide. Returns 0 on success
	EXTERNALCOMMAND_GetVolume,		//Return vol 0-100. Returns 0-100 on success
	EXTERNALCOMMAND_SetVolume,		//lParam is the new value(0-100) retunrns 0 on success
	EXTERNALCOMMAND_SetMute,		//lparam=1 for mute ON. 0  Returns 0 on success
	EXTERNALCOMMAND_GetMute,		//Returns 1 on mute ON. 0 on Mute off
	EXTERNALCOMMAND_GetMediaLength,	//returns the number of seconds
	EXTERNALCOMMAND_GetMediaPos,	//returns the number of seconds from the beginning
	EXTERNALCOMMAND_SetMediaPos,	//Sets the number of seconds from the beginning
	EXTERNALCOMMAND_Last
};

enum TSGetTextBuffers
{
	GTB_First = 300,
	GTB_Title,
	GTB_Artist,
	GTB_Last
};

LPCTSTR const mainWndClass = _T("JaangleWndClass");

LRESULT TSCSendCommand(TSCommand com, LPARAM lParam = 0)
{
	if (com > EXTERNALCOMMAND_First && com < EXTERNALCOMMAND_Last)
	{
		HWND tscHWND = ::FindWindow(mainWndClass, NULL);
		if (tscHWND != 0)
			return ::SendMessage(tscHWND, WM_COMMAND, com, lParam);
	}
	return -1;
}

BOOL TSCGetText(LPTSTR bf, TSGetTextBuffers bufLen)
{
	if (bufLen > GTB_First && bufLen < GTB_Last)
	{
		HWND tscHWND = ::FindWindow(mainWndClass, NULL);
		if (tscHWND != 0)
			return ::SendMessage(tscHWND, WM_GETTEXT, (WPARAM)bufLen, (LPARAM)bf) > 0;
	}
	return FALSE;
}



#endif
