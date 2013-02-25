
#ifndef _CRC_32_H_EA6C0EE0_BC30_11d5_B625_A58C4DF45B22_INCLUDED
#define _CRC_32_H_EA6C0EE0_BC30_11d5_B625_A58C4DF45B22_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WINDOWS_
#include <windows.h>
#endif // _WINDOWS_

#define WM_CRC_THREAD_DONE WM_APP + 0x2DB1

class CRC32
{
    //typedef struct tag_CRCStruct
    //{
    //    CRC32 *pCRC_32;
    //    TCHAR FileName[_MAX_PATH];
    //    LPBYTE pByte;
    //    UINT size;
    //    HWND hWnd;
    //    HANDLE Thread;
    //} CRCStruct, *LPCRCStruct;

public:
    CRC32();
    DWORD CalcCRC(LPVOID buffer, UINT size);

private:
    void Calculate (const LPBYTE buffer, UINT size, ULONG &crc);
    ULONG Reflect(ULONG ref, char ch);
    ULONG Table[256];
};

#endif // _CRC_32_H_EA6C0EE0_BC30_11d5_B625_A58C4DF45B22_INCLUDED

/////////////////////////////////////////////////////////////////////////////
//
//  End of CRC_32.h
//
/////////////////////////////////////////////////////////////////////////////
