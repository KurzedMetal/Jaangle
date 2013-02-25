#ifndef _IPane_h_
#define _IPane_h_

#include "ITSMenu.h"
class IPaneController;

struct PaneButtonInfo
{
	PaneButtonInfo():text(0), hIcon(NULL),iconSize(0)				{}
	LPCTSTR text; 
	HICON hIcon;
	UINT iconSize;
};

struct PaneInfo
{
	LPCTSTR Name;
	LPCTSTR friendlyName;
	LPCTSTR author;
	BOOL bAllowMultipleInstances;
};

class IPaneState
{
public:
	IPaneState()				{}
	virtual ~IPaneState()		{}

	virtual INT GetIntSetting(LPCTSTR settingName)							= 0;
	virtual void SetIntSetting(LPCTSTR settingName, INT value)				= 0;
	virtual LPCTSTR GetLPCTSTRSetting(LPCTSTR settingName)					= 0;
	virtual void SetLPCTSTRSetting(LPCTSTR settingName, LPCTSTR value)		= 0;

};

class IPane
{
public:
	IPane()					{}
	virtual ~IPane()		{}

	virtual BOOL Init(HWND hwndParent)										= 0;
	virtual void UnInit()													= 0;

	virtual BOOL Show()														= 0;
	virtual void Hide()														= 0;

	virtual void MovePane(INT x, INT y, INT cx, INT cy)						= 0;

	virtual LPCTSTR GetTitle(UINT rollingIdx)								= 0;
	virtual BOOL GetButtonInfo(PaneButtonInfo& bInfo, UINT idx)				= 0;
	virtual ITSMenu* GetMenu(UINT idx)										= 0;
	virtual BOOL OnButton(UINT idx)											= 0;

	virtual const PaneInfo& GetPaneInfo()									= 0;

	virtual BOOL LoadState(IPaneState& paneState)							= 0;
	virtual BOOL SaveState(IPaneState& paneState)							= 0;

};

class IPaneCreator
{
public:
	IPaneCreator()					{}
	virtual ~IPaneCreator()			{}

	virtual IPane* CreateInstance()											= 0;
	virtual const PaneInfo& GetPaneInfo()									= 0;
	virtual BOOL IsCreatable()												= 0;
};


#endif
