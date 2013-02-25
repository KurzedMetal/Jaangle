#ifndef _ITRANSLATION_H_
#define _ITRANSLATION_H_

class ITranslation
{
public:
	enum TranslationInfoEnum
	{
		TI_Name,
		TI_LocalizedName,
		TI_Author,
		TI_Email,
		TI_WebSite,
		TI_Description,
		TI_Version,
		TI_AutoUpdateURL,
		TI_AppVersion,
		TI_Last
	};

public:
	virtual ~ITranslation()												{}
	virtual LPCTSTR GetTranslationInfo(TranslationInfoEnum infoType)	= 0;
	virtual LPCTSTR GetString(UINT stringID)							= 0;
};

class ITranslatable
{
public:
	virtual ~ITranslatable()											{}
	virtual void ApplyTranslation(ITranslation& translation)			= 0;
};

class ITranslationManager
{
public:
	virtual ~ITranslationManager()										{}
	virtual void RegisterTranslatable(ITranslatable& translatable)		= 0;
	virtual void UnRegisterTranslatable(ITranslatable& translatable)	= 0;
	virtual void UpdateTranslatables(ITranslation& translation)			= 0;
};

#endif