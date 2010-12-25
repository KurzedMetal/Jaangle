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
#ifndef _TSSKIN_H
#define _TSSKIN_H

#include "ISkin.h"

enum SectionsEnum
{
	CSEC_Global,

	CSEC_Toolbar,
	CSEC_ToolbarButtons,
	CSEC_StatusBar,
	CSEC_PlayerBar,

	CSEC_Splitters,
	CSEC_Panes,

	CSEC_TreeCtrl,
	CSEC_TrackListTitle,
	CSEC_TrackList,
	CSEC_PlayList,
	CSEC_InfoControl,
	CSEC_WebBrowserControl,

	CSEC_Volume,
	CSEC_PlaySlider,
	
	CSEC_MiniPlayer,

	CSEC_Last
};

enum ColorValuesEnum
{
	CVAL_Bk,
	CVAL_Text,

	CVAL_OverBk,
	CVAL_OverText,
	
	CVAL_DownBk,
	CVAL_DownText,

	CVAL_AltBk,
	CVAL_AltText,

	CVAL_SelBk,
	CVAL_SelText,

	CVAL_DisabledBk,
	CVAL_DisabledText,

	CVAL_CaptionBk,
	CVAL_CaptionText,

	CVAL_CaptionOutline,
	CVAL_Outline,

	CVAL_BorderLine,

	CVAL_SubBk,
	CVAL_SubText,

	CVAL_CurText,
	CVAL_SelCurText,

	CVAL_Slider1,
	CVAL_Slider2,

	CVAL_Last
};

enum FontStylesEnum
{
	FSTY_Small,
	FSTY_SmallBold,
	FSTY_Normal,
	FSTY_NormalBold,
	FSTY_Big,
	FSTY_BigBold,
	FSTY_VeryBig,
	FSTY_Time, 
	FSTY_Length,
	FSTY_Edit,
	FSTY_Last
};

enum IntValuesEnum
{
	IPARAM_Size,
	IPARAM_Width,
	IPARAM_Height,
	IPARAM_EditHeight,
	IPARAM_MaxRelativeFont,
	IPARAM_MinRelativeFont,
	IPARAM_Last
};


typedef std::map<ColorValuesEnum, COLORREF> ColorValuesMap;
//typedef std::map<SectionsEnum, ColorValuesMap> ColorSectionsMap;

typedef std::map<IntValuesEnum, INT> IntValuesMap;
//typedef std::map<SectionsEnum, IntValuesMap> IntSectionsMap;

typedef std::map<FontStylesEnum, LOGFONT> FontStylesMap;
typedef std::map<FontStylesEnum, CFont*> FontsMap;
//typedef std::map<SectionsEnum, FontStylesMap> FontStylesSectionsMap;


class TSSkinInfo
{
public:
	TSSkinInfo()	{}
	LPCTSTR GetSkinInfo(ISkin::SkinInfoEnum infoType)
	{
		if (infoType < ISkin::SI_Last)
			return m_skinInfo[infoType].c_str();
		return NULL;
	}
	BOOL SetSkinInfo(ISkin::SkinInfoEnum infoType, LPCTSTR info)
	{
		ASSERT(info != NULL);
		ASSERT(infoType < ISkin::SI_Last);
		if (infoType < ISkin::SI_Last && info != NULL)
		{
			m_skinInfo[infoType] = info;
			return TRUE;
		}
		return FALSE;
	}
private:
	std::tstring m_skinInfo[ISkin::SI_Last];
};


class TSSkin : public ISkin
{
public:
	TSSkin();
	virtual ~TSSkin();

	virtual LPCTSTR GetSkinInfo(SkinInfoEnum infoType);

	LPCTSTR GetSkinPath();

	//=== If returns the color of the skin.
	//=== If this value is not available then the skin should calculate another value
	//=== or the value of the default skin
	COLORREF GetColor(SectionsEnum section, ColorValuesEnum color);
	CFont* GetFont(SectionsEnum section, FontStylesEnum style);
	INT GetIntParameter(SectionsEnum section, IntValuesEnum param);


	//=== Set Functions
	BOOL SetSkinInfo(SkinInfoEnum infoType, LPCTSTR info);
	BOOL SetColor(SectionsEnum section, ColorValuesEnum color, COLORREF value);
	BOOL SetFont(SectionsEnum section, FontStylesEnum style, LOGFONT& lf);
	BOOL SetIntParam(SectionsEnum section, IntValuesEnum param, INT value);

	BOOL LoadSkin(LPCTSTR skinDir);
	BOOL SaveSkin(LPCTSTR skinDir);
	void ResetSkin();


	//Configuration Options
	INT GetRelativeFontSize();
	void SetRelativeFontSize(INT size);

	static BOOL LoadSkinInfo(TSSkinInfo& skin, LPCTSTR skinDir);
	static BOOL LoadSkin(TSSkin& skin, LPCTSTR skinDir, BOOL& bIsPartial);
	static void GetSkinInfoPath(LPCTSTR skinPath, LPTSTR bf, UINT bfLen);
	static void GetSkinPreviewPath(LPCTSTR skinPath, LPTSTR bf, UINT bfLen);
	static void GetSkinFolderName(LPCTSTR skinPath, LPTSTR bf, UINT bfLen);

	void SetFontQuality(INT fontQuality);
	INT GetFontQuality()					{return m_fontQuality;}




private:
	void ClearFonts(BOOL bDeleteAlso);
	void SetLogFont(LOGFONT& lf,
		LPCTSTR faceName, LONG height, LONG width, LONG weight, BOOL bItalic, BOOL bUnderline);

	static BOOL	String2LogFont(LOGFONT& lf, LPCTSTR str);
	static LPTSTR LogFont2String(LOGFONT& lf, LPTSTR bf, UINT bfLen);
private:
	std::basic_string<TCHAR> m_loadedSkinDir;
	INT m_relativeFontSize;
	//std::tstring m_skinInfo[ISkin::SI_Last];
	ColorValuesMap m_colors[CSEC_Last];
	IntValuesMap m_intParams[CSEC_Last];
	FontStylesMap m_logFonts[CSEC_Last];
	FontsMap m_pFonts[CSEC_Last];
	INT m_fontQuality;
	TSSkinInfo m_skinInfo;
};
#endif
