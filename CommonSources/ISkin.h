#ifndef _ISKIN_H
#define _ISKIN_H

class ISkin
{
public:
	enum SkinInfoEnum
	{
		SI_DirName,
		SI_Name,
		SI_Author,
		SI_Version,
		SI_Email,
		SI_Description,
		SI_WebSite,
		SI_AutoUpdateURL,
		SI_Last
	};

public:
	virtual ~ISkin()		{}
	virtual LPCTSTR GetSkinInfo(SkinInfoEnum infoType)		= 0;

};

class ISkinnable
{
public:
	virtual ~ISkinnable()						{}
	virtual void ApplySkin(ISkin& skin)			= 0;
};

class ISkinManager
{
public:
	virtual ~ISkinManager()									{}
	virtual void RegisterSkinnable(ISkinnable& skinnable)	= 0;
	virtual void UnRegisterSkinnable(ISkinnable& skinnable)	= 0;
	virtual void UpdateSkinnables(ISkin& skin)				= 0;
};

#endif