#ifndef _FREEDB_H_
#define _FREEDB_H_

#include "afxinet.h"

#define MAX_IDENT_LEN 200



//MSF Helper Class --------------------------------------------------------------

class MSF
{
public:
	MSF(INT min = 0, INT sec = 0, INT Frame = 0);
	MSF operator+(const MSF& other);
	MSF& operator+=(const MSF& other);
	void operator=(const MSF &other);

	INT m_min;
	INT m_sec;
	INT m_frame;
	UINT toFrame();
	UINT toSec();

private:
	static INT msf2int(const MSF& msf);
	static MSF int2msf(INT frames);
};

struct AudioCDTrackInfo
{
	std::tstring title;
	MSF offset;
	MSF length;
};

struct AudioCDInfo
{
	AudioCDInfo():discID(0),year(0),totalLength(0){}
	UINT discID;
	std::tstring title;
	std::tstring genre;
	UINT year;
	UINT totalLength;
	std::vector<AudioCDTrackInfo> tracks;
};

//FreeDB Results struct
struct FreeDBResult
{
	std::tstring title;
	std::tstring genre;
	std::tstring discID;
};

//FreeDB Main Class-------------------------------------------------------
class FreeDB
{
public:
	FreeDB();
	~FreeDB();

	void Init(LPCTSTR clientname, LPCTSTR clientversion, HINTERNET hNet);

	//Info
	enum QueryResult
	{
		QR_NotFound,
		QR_FoundExactMatch,			//200
		QR_FoundExactMatches,		//210
		QR_FoundInexactMatches,		//211
		QR_Last
	};
	QueryResult Query(std::vector<FreeDBResult>& results, AudioCDInfo& CDInfo, LPCTSTR freeDbServer = NULL);
	BOOL Read(FreeDBResult result, AudioCDInfo& CDInfo, LPCTSTR freeDbServer = NULL);

	//Servers
	BOOL RetrieveFreeDbServers();
	UINT GetServerCount();
	LPCTSTR GetServer(UINT idx);
	//Utilities
	static BOOL ReadDisc(LPCTSTR path, AudioCDInfo& cdInfo);
	static BOOL GetFirstPart(std::tstring& buffer, LPCTSTR title);
	static BOOL GetSecondPart(std::tstring& buffer, LPCTSTR title);

#ifdef _UNITTESTING
	static BOOL UnitTest();
#endif


private:
	void NormalizeString(LPTSTR str);
	UINT GetPageErrorCode(LPCSTR page);
private:
	HINTERNET m_hNet;
	TCHAR m_ident[MAX_IDENT_LEN];
	std::vector<std::tstring> m_serverList;
};


#endif