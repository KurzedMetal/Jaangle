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
#include "stdafx.h"
#include "TSSkin.h"
#include "AppSettings/IniAppSettings.h"
#include "hdcUtils.h"


LPCTSTR const cColorsIni = _T("colors.ini");
LPCTSTR const cPreviewImage = _T("_preview.png");
LPCTSTR const cInfoSection = _T("SkinDetails");

LPCTSTR const cSectionNames[] =
{
	_T("Global"),

	_T("Toolbar"),
	_T("ToolbarButtons"),
	_T("StatusBar"),
	_T("PlayerBar"),

	_T("Splitters"),
	_T("Panes"),

	_T("TreeCtrl"),
	_T("TrackListTitle"),
	_T("TrackList"),
	_T("PlayList"),
	_T("InfoControl"),
	_T("WebBrowserControl"),

	_T("Volume"),
	_T("PlaySlider"),

	_T("MiniPlayer"),
	
	NULL
};


LPCTSTR const cColorNames[] = 
{
	_T("Bk"),
	_T("Text"),

	_T("OverBk"),
	_T("OverText"),

	_T("DownBk"),
	_T("DownText"),

	_T("AltBk"),
	_T("AltText"),

	_T("SelBk"),
	_T("SelText"),

	_T("DisabledBk"),
	_T("DisabledText"),

	_T("CaptionBk"),
	_T("CaptionText"),

	_T("CaptionOutline"),
	_T("Outline"),

	_T("BorderLine"),

	_T("SubBk"),
	_T("SubText"),

	_T("CurText"),
	_T("SelCurText"),

	_T("Slider1"),
	_T("Slider2"),

	NULL
};


LPCTSTR const cIntNames[] = 
{
	_T("Int_Size"),
	_T("Int_Width"),
	_T("Int_Height"),
	_T("Int_EditHeight"),
	_T("Int_MaxRelativeFont"),
	_T("Int_MinRelativeFont"),
	NULL
};

LPCTSTR const cFontStyleNames[] = 
{
	_T("Font_Small"),
	_T("Font_SmallBold"),
	_T("Font_Normal"),
	_T("Font_NormalBold"),
	_T("Font_Big"),
	_T("Font_BigBold"),
	_T("Font_VeryBig"),
	_T("Font_Time"),
	_T("Font_Length"),
	_T("Font_Edit"),
	NULL
};


BOOL String2COLORREF(LPCTSTR str, COLORREF& col)
{
	//---Version 1
	//return (COLORREF)_tcstoul(str, NULL, 16);
	//Version 2
	UINT len = _tcslen(str);
	LPCTSTR strStart = str;
	if ((len == 7 && str[0]=='#') || len == 6)
	{
		if (len == 7)
			strStart++;
	}
	else
	{
		TRACE(_T("@1String2COLORREF Invalid Color Value: '%s'\r\n"), str);
		return FALSE;
	}
	TCHAR hex[3];
	hex[2] = 0;
	hex[0]=strStart[0];
	hex[1]=strStart[1];
	UINT red = _tcstoul(hex, NULL, 16);
	hex[0]=strStart[2];
	hex[1]=strStart[3];
	UINT green = _tcstoul(hex, NULL, 16);
	hex[0]=strStart[4];
	hex[1]=strStart[5];
	UINT blue = _tcstoul(hex, NULL, 16);
	col = COLORREF(RGB(red,green,blue));
	return TRUE;
}

COLORREF S2C(LPCTSTR str)
{
	COLORREF clr = RGB(0,0,0);
	String2COLORREF(str, clr);
	return clr;
}

LPTSTR COLORREF2String(COLORREF col, LPTSTR str, UINT strLen)
{
	ASSERT(str!=NULL);
	ASSERT(strLen >= 10);
	//---Version 1
	//_ultot(col, str, 16);
	//---Version 2
	_sntprintf(str, strLen, _T("#%0.2X%0.2X%0.2X"), GetRValue(col), GetGValue(col), GetBValue(col));
	return str;
}

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------

TSSkin::TSSkin():
m_relativeFontSize(0),
m_fontQuality(CLEARTYPE_QUALITY)
{
	ResetSkin();
}

TSSkin::~TSSkin()
{
	ClearFonts(TRUE);
}

COLORREF TSSkin::GetColor(SectionsEnum section, ColorValuesEnum color)
{
	ASSERT(section < CSEC_Last);
	if (section < CSEC_Last)
	{
		ColorValuesMap::iterator colit = m_colors[section].find(color);
		if (colit != m_colors[section].end())
			return colit->second;
		else
		{
			TRACE(_T("TSSkin::GetColor Color Not Found in Category\r\n"));
			ASSERT(0);
		}
	}
	TRACE(_T("TSSkin::GetColor Color Category Not Found\r\n"));
	return RGB(0,0,0);
}

void TSSkin::ClearFonts(BOOL bDeleteAlso)
{
	for (INT sec = 0; sec < CSEC_Last; sec++)
	{
		FontsMap::iterator it = m_pFonts[sec].begin();
		for (;it !=m_pFonts[sec].end() ;it++)
		{
			if (it->second != NULL)
			{
				it->second->DeleteObject();
				if (bDeleteAlso)
				{
					delete it->second;
					it->second = NULL;
				}
			}
		}
	}
}

void TSSkin::SetFontQuality(INT fontQuality)
{
	if (fontQuality != m_fontQuality)
	{
		ClearFonts(FALSE);
		m_fontQuality = fontQuality;
	}
}




void TSSkin::ResetSkin()
{
	m_loadedSkinDir.clear();
	//=== Fonts
	ClearFonts(FALSE);
	LPCTSTR cCommonFont = _T("Tahoma");
	UINT cCommonFontSize = 14;

	SetLogFont(m_logFonts[CSEC_Global][FSTY_Normal], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_Toolbar][FSTY_Normal], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_PlayerBar][FSTY_Normal], cCommonFont, cCommonFontSize + 12, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_PlayerBar][FSTY_NormalBold], cCommonFont, cCommonFontSize + 12, 0, FW_BOLD,0,0);
	SetLogFont(m_logFonts[CSEC_PlayerBar][FSTY_Time], cCommonFont, cCommonFontSize + 12, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_PlayerBar][FSTY_Length], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_StatusBar][FSTY_Normal], cCommonFont, cCommonFontSize - 2, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_Panes][FSTY_Normal], cCommonFont, cCommonFontSize - 2, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_TrackListTitle][FSTY_Normal], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_TrackListTitle][FSTY_NormalBold], cCommonFont, cCommonFontSize, 0, FW_BOLD, 0, 0);
	SetLogFont(m_logFonts[CSEC_TrackListTitle][FSTY_Edit], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_TrackList][FSTY_Normal], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_TreeCtrl][FSTY_Normal], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_TreeCtrl][FSTY_NormalBold], cCommonFont, cCommonFontSize, 0, FW_BOLD, 0, 0);
	SetLogFont(m_logFonts[CSEC_TreeCtrl][FSTY_Big], cCommonFont, cCommonFontSize + 4, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_TreeCtrl][FSTY_BigBold], cCommonFont, cCommonFontSize + 4, 0, FW_BOLD, 0, 0);
	SetLogFont(m_logFonts[CSEC_TreeCtrl][FSTY_VeryBig], cCommonFont, cCommonFontSize + 8, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_InfoControl][FSTY_Normal], cCommonFont, cCommonFontSize - 2, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_InfoControl][FSTY_Edit], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);

	SetLogFont(m_logFonts[CSEC_PlayList][FSTY_Normal], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_PlayList][FSTY_NormalBold], cCommonFont, cCommonFontSize, 0, FW_BOLD, 0, 0);

	SetLogFont(m_logFonts[CSEC_MiniPlayer][FSTY_Normal], cCommonFont, cCommonFontSize, 0, FW_NORMAL, 0, 0);
	SetLogFont(m_logFonts[CSEC_MiniPlayer][FSTY_NormalBold], cCommonFont, cCommonFontSize, 0, FW_BOLD, 0, 0);


	m_intParams[CSEC_Global][IPARAM_MaxRelativeFont] = 8;
	m_intParams[CSEC_Global][IPARAM_MinRelativeFont] = 6;

	m_intParams[CSEC_Toolbar][IPARAM_Height] = 26;
	
	m_intParams[CSEC_PlayerBar][IPARAM_Height] = 52;
	
	m_intParams[CSEC_StatusBar][IPARAM_Height] = 18;
	
	m_intParams[CSEC_Panes][IPARAM_Height] = 20;
	
	m_intParams[CSEC_Splitters][IPARAM_Size] = 6;
	
	m_intParams[CSEC_TrackListTitle][IPARAM_Height] = 28;
	m_intParams[CSEC_TrackListTitle][IPARAM_EditHeight] = 20;

	m_intParams[CSEC_PlayList][IPARAM_Height] = 18;

	m_intParams[CSEC_MiniPlayer][IPARAM_Height] = 25;
	//m_intParams[CSEC_MiniPlayer][IPARAM_Width] = 500;


	//=== Colors
	//COLORREF cCommonBkColor = S2C(_T("#DDDDDD"));
	//COLORREF cCommonTextColor = S2C(_T("#353535"));
	//COLORREF cCommonPaneBkColor = S2C(_T("#EEEEEE"));

	m_colors[CSEC_Global][CVAL_BorderLine] = S2C(_T("#BBBBBB"));

	//=== Toolbar
	m_colors[CSEC_Toolbar][CVAL_Bk] = S2C(_T("#DDDDDD"));
	m_colors[CSEC_Toolbar][CVAL_Text] = S2C(_T("#353535"));
	m_colors[CSEC_ToolbarButtons][CVAL_Bk] = S2C(_T("#DDDDDD"));
	m_colors[CSEC_ToolbarButtons][CVAL_Text] = S2C(_T("#353535"));
	m_colors[CSEC_ToolbarButtons][CVAL_OverBk] = S2C(_T("#DDDDDD"));
	m_colors[CSEC_ToolbarButtons][CVAL_OverText] = S2C(_T("#353535"));
	m_colors[CSEC_ToolbarButtons][CVAL_DownBk] = S2C(_T("#DDDDDD"));
	m_colors[CSEC_ToolbarButtons][CVAL_DownText] = S2C(_T("#353535"));

	//=== StatusBar
	m_colors[CSEC_StatusBar][CVAL_Bk] = S2C(_T("#DDDDDD"));
	m_colors[CSEC_StatusBar][CVAL_Text] = S2C(_T("#353535"));

	//=== PlayerBar
	m_colors[CSEC_PlayerBar][CVAL_Bk] = S2C(_T("#DDDDDD"));
	m_colors[CSEC_PlayerBar][CVAL_Text] = S2C(_T("#353535"));
	m_colors[CSEC_PlayerBar][CVAL_Slider1] = S2C(_T("#CECECE"));
	m_colors[CSEC_PlayerBar][CVAL_Slider2] = S2C(_T("#CECECE"));

	//=== Panes
	m_colors[CSEC_Panes][CVAL_Bk] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_Panes][CVAL_Text] = S2C(_T("#4B4B4B"));
	m_colors[CSEC_Panes][CVAL_CaptionBk] = S2C(_T("#A8BDAE"));
	m_colors[CSEC_Panes][CVAL_CaptionText] = S2C(_T("#4B4B4B"));
	m_colors[CSEC_Panes][CVAL_CaptionOutline] = S2C(_T("#EEEEEE"));

	//=== Splitters
	m_colors[CSEC_Splitters][CVAL_Bk] = S2C(_T("#769880"));
	m_colors[CSEC_Splitters][CVAL_DisabledBk] = S2C(_T("#769880"));
	m_colors[CSEC_Splitters][CVAL_OverBk] = S2C(_T("#618F75"));

	//=== TreeCtrl
	m_colors[CSEC_TreeCtrl][CVAL_Text] = S2C(_T("#353535"));
	m_colors[CSEC_TreeCtrl][CVAL_Bk] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_TreeCtrl][CVAL_SubBk] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_TreeCtrl][CVAL_SubText] = S2C(_T("#929292"));
	m_colors[CSEC_TreeCtrl][CVAL_SelBk] = S2C(_T("#97B09E"));
	m_colors[CSEC_TreeCtrl][CVAL_SelText] = S2C(_T("#353535"));

	//=== ListCtrl
	m_colors[CSEC_TrackList][CVAL_Bk] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_TrackList][CVAL_Text] = S2C(_T("#000000"));
	m_colors[CSEC_TrackList][CVAL_AltBk] = S2C(_T("#E2E2E2"));
	m_colors[CSEC_TrackList][CVAL_AltText] = S2C(_T("#000000"));
	m_colors[CSEC_TrackList][CVAL_SelBk] = S2C(_T("#97B09E"));
	m_colors[CSEC_TrackList][CVAL_SelText] = S2C(_T("#EEEEEE"));

	//=== ListCtrlTitle
	m_colors[CSEC_TrackListTitle][CVAL_Bk] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_TrackListTitle][CVAL_Text] = S2C(_T("#808080"));

	//=== PlayListCtrl
	m_colors[CSEC_PlayList][CVAL_Bk] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_PlayList][CVAL_Text] = S2C(_T("#808080"));
	m_colors[CSEC_PlayList][CVAL_SelText] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_PlayList][CVAL_SelBk] = S2C(_T("#608069"));
	m_colors[CSEC_PlayList][CVAL_CurText] = S2C(_T("#384B3E"));
	m_colors[CSEC_PlayList][CVAL_SelCurText] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_PlayList][CVAL_DisabledText] = S2C(_T("#888888"));

	//=== InfoCtrl
	m_colors[CSEC_InfoControl][CVAL_Bk] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_InfoControl][CVAL_Text] = S2C(_T("#131116"));
	m_colors[CSEC_InfoControl][CVAL_AltBk] = S2C(_T("#FDFDFD"));
	m_colors[CSEC_InfoControl][CVAL_AltText] = S2C(_T("#EEEEEE"));
	m_colors[CSEC_InfoControl][CVAL_Outline] = S2C(_T("#888888"));

	//=== MiniPlayer
	m_colors[CSEC_MiniPlayer][CVAL_Bk] = S2C(_T("#9DB7A6"));
	m_colors[CSEC_MiniPlayer][CVAL_Text] = S2C(_T("#384B3E"));

}

void TSSkin::SetLogFont(LOGFONT& lf,
		LPCTSTR faceName, LONG height, LONG width, LONG weight, BOOL bItalic, BOOL bUnderline)
{
	lf.lfHeight = height;
	lf.lfWidth = width;
	lf.lfEscapement = 0; 
	lf.lfOrientation = 0;
	lf.lfWeight = weight;
	lf.lfItalic = bItalic;
	lf.lfUnderline = bUnderline;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = m_fontQuality;//ANTIALIASED_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy(lf.lfFaceName, faceName);

}


LPCTSTR TSSkin::GetSkinInfo(SkinInfoEnum infoType)
{
	return m_skinInfo.GetSkinInfo(infoType);// [infoType].c_str();
}

CFont* TSSkin::GetFont(SectionsEnum section, FontStylesEnum style)
{
	ASSERT(section < CSEC_Last);
	if (!(section < CSEC_Last))
		return NULL;
	if (m_pFonts[section][style] == NULL)
		m_pFonts[section][style] = new CFont;
	if(m_pFonts[section][style]->GetSafeHandle() == NULL)
	{
		m_logFonts[section][style].lfQuality = m_fontQuality;
		if (section != CSEC_MiniPlayer)
		{
			m_logFonts[section][style].lfHeight += m_relativeFontSize;
			m_pFonts[section][style]->CreateFontIndirect(&m_logFonts[section][style]);
			m_logFonts[section][style].lfHeight -= m_relativeFontSize;
		}
		else
			m_pFonts[section][style]->CreateFontIndirect(&m_logFonts[section][style]);
	}
	return m_pFonts[section][style];
}
INT TSSkin::GetIntParameter(SectionsEnum section, IntValuesEnum param)
{
	ASSERT(section < CSEC_Last);
	if (!(section < CSEC_Last))
		return 0;
	return m_intParams[section][param];
}


//=== Set Functions
BOOL TSSkin::SetSkinInfo(SkinInfoEnum infoType, LPCTSTR info)
{
	ASSERT(info != NULL);
	ASSERT(infoType < SI_Last);
	if (infoType < SI_Last && info != NULL)
	{
		m_skinInfo.SetSkinInfo(infoType, info);
		//m_skinInfo[infoType] = info;
		return TRUE;
	}
	return FALSE;

}

BOOL TSSkin::SetColor(SectionsEnum section, ColorValuesEnum color, COLORREF value)
{
	ASSERT(section < CSEC_Last);
	if (!(section < CSEC_Last))
		return FALSE;
	ColorValuesMap::iterator colit = m_colors[section].find(color);
	if (colit == m_colors[section].end())
		return FALSE;
	colit->second = value;
	return TRUE;

}

BOOL TSSkin::SetFont(SectionsEnum section, FontStylesEnum style, LOGFONT& logFont)
{
	ASSERT(section < CSEC_Last);
	if (!(section < CSEC_Last))
		return FALSE;
	ASSERT(0);		//=== TODO
	return FALSE;
}
BOOL TSSkin::SetIntParam(SectionsEnum section, IntValuesEnum param, INT value)
{
	ASSERT(section < CSEC_Last);
	if (!(section < CSEC_Last))
		return FALSE;
	ASSERT(0);		//=== TODO
	return FALSE;
}

//Configuration Options
INT TSSkin::GetRelativeFontSize()
{
	return m_relativeFontSize;
}
void TSSkin::SetRelativeFontSize(INT size)
{
	if (size > m_intParams[CSEC_Global][IPARAM_MaxRelativeFont])
		size = m_intParams[CSEC_Global][IPARAM_MaxRelativeFont];
	else if (size < -m_intParams[CSEC_Global][IPARAM_MinRelativeFont])
		size = - m_intParams[CSEC_Global][IPARAM_MinRelativeFont];
	if (size != m_relativeFontSize)
	{
		m_relativeFontSize = size;
		ClearFonts(FALSE);
	}
}

LPCTSTR TSSkin::GetSkinPath()
{
	return m_loadedSkinDir.c_str();
}

void TSSkin::GetSkinInfoPath(LPCTSTR skinPath, LPTSTR bf, UINT bfLen)
{
	ASSERT(skinPath != NULL);
	ASSERT(bf != NULL);
	BOOL bAddSlash = skinPath[_tcslen(skinPath) - 1] != '\\';
	_sntprintf(bf, bfLen, _T("%s%s%s"), 
		skinPath, 
		bAddSlash ? _T("\\") : _T(""),
		cColorsIni);
}


void TSSkin::GetSkinPreviewPath(LPCTSTR skinPath, LPTSTR bf, UINT bfLen)
{
	ASSERT(skinPath != NULL);
	ASSERT(bf != NULL);
	BOOL bAddSlash = skinPath[_tcslen(skinPath) - 1] != '\\';
	_sntprintf(bf, bfLen, _T("%s%s%s"), 
		skinPath, 
		bAddSlash ? _T("\\") : _T(""),
		cPreviewImage);
}



void TSSkin::GetSkinFolderName(LPCTSTR skinPath, LPTSTR bf, UINT bfLen)
{
	ASSERT(skinPath != NULL);
	ASSERT(bf != NULL);
	if (skinPath[_tcslen(skinPath) - 1] == '\\')
	{
		LPCTSTR slashPos = &skinPath[_tcslen(skinPath) - 2];
		while (slashPos > skinPath)
		{
			if (*slashPos == '\\')
			{
				INT copyLen = _tcslen(skinPath) - (slashPos - skinPath);
				_tcsncpy(bf, &slashPos[1], copyLen);	
				bf[copyLen] = 0;
			}
		}

	}
	else
	{
		LPCTSTR slashPos = _tcsrchr(skinPath, '\\'); 
		if (slashPos != NULL)
			_tcsncpy(bf, &slashPos[1], bfLen);
	}
}

BOOL TSSkin::LoadSkin(LPCTSTR skinDir)
{
	ASSERT(skinDir != NULL);
	ResetSkin();
	//=============== END
	//return TRUE;

	BOOL bIsPartial = FALSE;
	if (LoadSkin(*this, skinDir, bIsPartial))
	{
		if (bIsPartial)
			SaveSkin(skinDir);
		ClearFonts(FALSE);
		m_loadedSkinDir = skinDir;
		return TRUE;
	}
	return FALSE;
}

BOOL TSSkin::LoadSkinInfo(TSSkinInfo& skin, LPCTSTR skinDir)
{
	TCHAR iniFile[MAX_PATH];
	GetSkinInfoPath(skinDir, iniFile, MAX_PATH);
	IniAppSettings appSettings;
	if (!appSettings.Load(iniFile))
		return FALSE;

	TCHAR bf[MAX_PATH];
	GetSkinFolderName(skinDir, bf, MAX_PATH);
	skin.SetSkinInfo(SI_Name, bf);


	std::tstring tmpBuffer;
	if (appSettings.Read(cInfoSection, _T("Name"), tmpBuffer, _T("")))
	{
		if (_tcscmp(tmpBuffer.c_str(), skin.GetSkinInfo(SI_Name)) != 0)
		{
			tmpBuffer += _T(" (");
			tmpBuffer += skin.GetSkinInfo(SI_Name);
			tmpBuffer += _T(")");
			skin.SetSkinInfo(SI_Name, tmpBuffer.c_str());
		}
	}
	if (appSettings.Read(cInfoSection, _T("Author"), tmpBuffer, _T("")))
		skin.SetSkinInfo(SI_Author, tmpBuffer.c_str());
	if (appSettings.Read(cInfoSection, _T("Version"), tmpBuffer, _T("")))
		skin.SetSkinInfo(SI_Version, tmpBuffer.c_str());
	if (appSettings.Read(cInfoSection, _T("Email"), tmpBuffer, _T("")))
		skin.SetSkinInfo(SI_Email, tmpBuffer.c_str());
	if (appSettings.Read(cInfoSection, _T("Description"), tmpBuffer, _T("")))
		skin.SetSkinInfo(SI_Description, tmpBuffer.c_str());
	if (appSettings.Read(cInfoSection, _T("Website"), tmpBuffer, _T("")))
		skin.SetSkinInfo(SI_WebSite, tmpBuffer.c_str());
	if (appSettings.Read(cInfoSection, _T("AutoUpdateURL"), tmpBuffer, _T("")))
		skin.SetSkinInfo(SI_AutoUpdateURL, tmpBuffer.c_str());

	return TRUE;
}

BOOL TSSkin::LoadSkin(TSSkin& skin, LPCTSTR skinDir, BOOL& bIsPartial)
{
	bIsPartial = FALSE;
	ASSERT(skinDir != NULL);
	TCHAR iniFile[MAX_PATH];
	GetSkinInfoPath(skinDir, iniFile, MAX_PATH);
	IniAppSettings appSettings;
	if (!appSettings.Load(iniFile))
		return FALSE;

	LoadSkinInfo(skin.m_skinInfo, skinDir);
	
	//=== Read Info
	std::tstring tmpBuffer;
	//if (appSettings.Read(cInfoSection, _T("Name"), tmpBuffer, _T("")))
	//{
	//	if (_tcscmp(tmpBuffer.c_str(), skin.GetSkinInfo(SI_Name)) != 0)
	//	{
	//		tmpBuffer += _T(" (");
	//		tmpBuffer += skin.GetSkinInfo(SI_Name);
	//		tmpBuffer += _T(")");
	//		skin.SetSkinInfo(SI_Name, tmpBuffer.c_str());
	//	}
	//}
	//if (appSettings.Read(cInfoSection, _T("Author"), tmpBuffer, _T("")))
	//	skin.SetSkinInfo(SI_Author, tmpBuffer.c_str());
	//if (appSettings.Read(cInfoSection, _T("Version"), tmpBuffer, _T("")))
	//	skin.SetSkinInfo(SI_Version, tmpBuffer.c_str());
	//if (appSettings.Read(cInfoSection, _T("Email"), tmpBuffer, _T("")))
	//	skin.SetSkinInfo(SI_Email, tmpBuffer.c_str());
	//if (appSettings.Read(cInfoSection, _T("Description"), tmpBuffer, _T("")))
	//	skin.SetSkinInfo(SI_Description, tmpBuffer.c_str());
	//if (appSettings.Read(cInfoSection, _T("Website"), tmpBuffer, _T("")))
	//	skin.SetSkinInfo(SI_WebSite, tmpBuffer.c_str());
	//if (appSettings.Read(cInfoSection, _T("AutoUpdateURL"), tmpBuffer, _T("")))
	//	skin.SetSkinInfo(SI_AutoUpdateURL, tmpBuffer.c_str());



	//if (bInfoOnly == TRUE)
	//	return TRUE;

	//=== Get Some Default colors for partial written skins
	COLORREF cBkColor = RGB(255,255,255);
	COLORREF cTextColor = RGB(0,0,0);
	COLORREF cSelBkColor = RGB(100,100,200);
	COLORREF cBarBkColor = cBkColor;
	COLORREF cBarTextColor = cTextColor;
	if (appSettings.Read(cSectionNames[CSEC_StatusBar], cColorNames[CVAL_Bk], tmpBuffer, _T("")))
		String2COLORREF(tmpBuffer.c_str(), cBarBkColor);
	if (appSettings.Read(cSectionNames[CSEC_StatusBar], cColorNames[CVAL_Text], tmpBuffer, _T("")))
		String2COLORREF(tmpBuffer.c_str(), cBarTextColor);

	for (INT sec = CSEC_Global; sec < CSEC_Last; sec++)
	{
		//=== Load IntParams
		IntValuesMap::iterator intit = skin.m_intParams[sec].begin();
		for (;intit != skin.m_intParams[sec].end(); intit++)
		{
			if (appSettings.Read(cSectionNames[sec], cIntNames[intit->first], tmpBuffer, _T("")))
				intit->second = _ttol(tmpBuffer.c_str());
			else
				bIsPartial = TRUE;
		}

		//=== Load Fonts
		FontStylesMap::iterator lfit = skin.m_logFonts[sec].begin();
		for (;lfit != skin.m_logFonts[sec].end(); lfit++)
		{
			if (appSettings.Read(cSectionNames[sec], cFontStyleNames[lfit->first], tmpBuffer, _T("")))
			{
				LOGFONT lf;
				if (String2LogFont(lf, tmpBuffer.c_str()))
				{
					lf.lfQuality = skin.m_fontQuality;
					lfit->second = lf;
				}
			}
			else
				bIsPartial = TRUE;
		}

		//=== Load Colors
		ColorValuesMap::iterator valit = skin.m_colors[sec].begin();
		for (;valit != skin.m_colors[sec].end(); valit++)
		{
			if (appSettings.Read(cSectionNames[sec], cColorNames[valit->first],	tmpBuffer, _T("")))
			{
				COLORREF clr;
				if (String2COLORREF(tmpBuffer.c_str(), clr) == TRUE)
					valit->second = clr;
				else
					TRACE(_T("@1 SkinManager::LoadSkin Cannot decode value: '%s'\r\n"), tmpBuffer.c_str());
			}
			else
			{
				bIsPartial = TRUE;
				//=== Partial Skins
				ColorValuesEnum col = valit->first;
				switch (col)
				{
				case CVAL_Bk:
					valit->second = cBarBkColor;
					//=== Compatibility
					if (sec == CSEC_TreeCtrl)
					{
						if (appSettings.Read(_T("TreeCtrl"), _T("TextBk1"),	tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					else if (sec == CSEC_Splitters)
					{
						if (appSettings.Read(_T("Splitters"), _T("Normal"),	tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_OverBk:
					valit->second = cBarBkColor;
					//=== Compatibility
					if (sec == CSEC_Splitters)
					{
						if (appSettings.Read(_T("Splitters"), _T("Tracking"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_DownBk:
					valit->second = cBarBkColor;
					break;
				case CVAL_Text:
					valit->second = cBarTextColor;
					break;
				case CVAL_OverText:
					valit->second = cBarTextColor;
					break;
				case CVAL_DownText:
					valit->second = cBarTextColor;
					break;
				case CVAL_DisabledBk:
					valit->second = GetSysColor(COLOR_GRAYTEXT);
					//=== Compatibility
					if (sec == CSEC_Splitters)
					{
						if (appSettings.Read(_T("Splitters"), _T("Disabled"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_Slider1:
					//=== Compatibility
					if (appSettings.Read(_T("Volume"), _T("BkAlt"),	tmpBuffer, _T("")))
						String2COLORREF(tmpBuffer.c_str(), valit->second);
					break;
				case CVAL_Slider2:
					//=== Compatibility
					if (appSettings.Read(_T("Volume"),_T("Bk"), tmpBuffer, _T("")))
						String2COLORREF(tmpBuffer.c_str(), valit->second);
					break;
				case CVAL_AltBk:
					valit->second = cBarBkColor;
					//=== Compatibility
					if (sec == CSEC_InfoControl)
					{
						if (appSettings.Read(_T("InfoControl"), _T("BkText"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					else if (sec == CSEC_TrackList)
					{
						if (appSettings.Read(_T("TrackList"), _T("TextAltBk"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_AltText:
					valit->second = cBarTextColor;
					//=== Compatibility
					if (sec == CSEC_InfoControl)
					{
						if (appSettings.Read(_T("InfoControl"), _T("TextTitle"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					else if (sec == CSEC_TrackList)
					{					
						if (appSettings.Read(_T("TrackList"), _T("TextAlt"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_SelBk:
					valit->second = cSelBkColor;
					//=== Compatibility
					if (sec == CSEC_TreeCtrl)
					{
						if (appSettings.Read(_T("TreeCtrl"), _T("TextSelBk1"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					else if (sec == CSEC_TrackList)
					{
						if (appSettings.Read(_T("TrackList"), _T("TextSelBk"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					else if (sec == CSEC_PlayList)
					{					
						if (appSettings.Read(_T("PlayList"), _T("TextSelBk"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_SelText:
					valit->second = cBarTextColor;
					//=== Compatibility
					if (sec == CSEC_TreeCtrl)
					{
						if (appSettings.Read(_T("TreeCtrl"), _T("TextSel"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					else if (sec == CSEC_TrackList)
					{					
						if (appSettings.Read(_T("TrackList"), _T("TextSel"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					else if (sec == CSEC_PlayList)
					{					
						if (appSettings.Read(_T("PlayList"), _T("TextSel"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_BorderLine:
				case CVAL_CaptionOutline:
				case CVAL_Outline:
				case CVAL_DisabledText:
					valit->second = RGB(128,128,128);
					break;
				case CVAL_SubBk:
					valit->second = cBarBkColor;
					//=== Compatibility
					if (appSettings.Read(_T("TreeCtrl"), _T("TextSubBk1"), tmpBuffer, _T("")))
						String2COLORREF(tmpBuffer.c_str(), valit->second);
					break;
				case CVAL_SubText:
					valit->second = cBarTextColor;
					//=== Compatibility
					if (appSettings.Read(_T("TreeCtrl"), _T("TextSub"), tmpBuffer, _T("")))
						String2COLORREF(tmpBuffer.c_str(), valit->second);
					break;
				case CVAL_CurText:
					valit->second = cBarTextColor;
					//=== Compatibility
					if (sec == CSEC_PlayList)
					{
						if (appSettings.Read(_T("PlayList"), _T("TextCur"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				case CVAL_SelCurText:
					valit->second = cBarTextColor;
					//=== Compatibility
					if (sec == CSEC_PlayList)
					{
						if (appSettings.Read(_T("PlayList"), _T("TextSelCur"), tmpBuffer, _T("")))
							String2COLORREF(tmpBuffer.c_str(), valit->second);
					}
					break;
				default:
					valit->second = RGB(255,0,0);
					TRACE(_T("@2 SkinManager::LoadSkin Value Not Set: '%s-%s'\r\n"), cSectionNames[sec], cColorNames[valit->first]);
					break;
				}
			}
		}
	}
	return TRUE;
}



BOOL TSSkin::SaveSkin(LPCTSTR skinFolderName)
{
	TCHAR colorsIniPath[MAX_PATH];
	_sntprintf(colorsIniPath, MAX_PATH, _T("%s%s%s"),
		skinFolderName,
		skinFolderName[_tcslen(skinFolderName) - 1] == '\\' ? _T("") : _T("\\"),
		cColorsIni
		);
	HANDLE f = CreateFile(colorsIniPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	BOOL bRet = FALSE;
	if (f != NULL)
	{
		CloseHandle(f);
		IniAppSettings appSettings;
		if (appSettings.Load(colorsIniPath))
		{
			//=== Write Info
			appSettings.Write(cInfoSection, _T("Name"), GetSkinInfo(SI_Name));
			appSettings.Write(cInfoSection, _T("Author"), GetSkinInfo(SI_Author));
			appSettings.Write(cInfoSection, _T("Version"), GetSkinInfo(SI_Version));
			appSettings.Write(cInfoSection, _T("Email"), GetSkinInfo(SI_Email));
			appSettings.Write(cInfoSection, _T("Description"), GetSkinInfo(SI_Description));
			appSettings.Write(cInfoSection, _T("Website"), GetSkinInfo(SI_WebSite));
			appSettings.Write(cInfoSection, _T("AutoUpdateURL"), GetSkinInfo(SI_AutoUpdateURL));

			TCHAR bf[100];
			for (INT sec = CSEC_Global; sec < CSEC_Last; sec++)
			{
				//=== Write Colors
				ColorValuesMap::iterator valit = m_colors[sec].begin();
				for (;valit != m_colors[sec].end(); valit++)
				{
					appSettings.Write(
						cSectionNames[sec], 
						cColorNames[valit->first], 
						COLORREF2String(GetColor((SectionsEnum) sec, valit->first), bf, 100)
						);
				}
				//=== Write IntParams
				IntValuesMap::iterator intit = m_intParams[sec].begin();
				for (;intit != m_intParams[sec].end(); intit++)
				{
					appSettings.Write(
						cSectionNames[sec], 
						cIntNames[intit->first], 
						_ltot(GetIntParameter((SectionsEnum) sec, intit->first), bf, 10)
						);
				}
				//=== Write Fonts
				FontStylesMap::iterator lfit = m_logFonts[sec].begin();
				for (;lfit != m_logFonts[sec].end(); lfit++)
				{
					appSettings.Write(
						cSectionNames[sec], 
						cFontStyleNames[lfit->first], 
						LogFont2String(m_logFonts[sec][lfit->first], bf, 100)
						);
				}
			}
			appSettings.Save();
			//ShellExecute(NULL, NULL, colorsIniPath, 0,0,0);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL TSSkin::String2LogFont(LOGFONT& lf, LPCTSTR str)
{
	//=== Font format: lfFaceName;lfHeight;lfWidth;lfWeight;lfItalic
	INT tokNum = 0;
	LPCTSTR startPos = str;
	LPCTSTR endPos = NULL;//_tcschr(str, ';');
	memset(&lf, 0, sizeof(LOGFONT));
	while (startPos != NULL)
	{
		endPos = _tcschr(startPos, ';');
		switch (tokNum)
		{
		case 0://=== lfFaceName
			if (endPos == NULL)
				return FALSE;
			_tcsncpy(lf.lfFaceName, startPos, endPos - startPos);
			lf.lfFaceName[endPos - startPos] = 0;
			break;
		case 1://=== lfHeight
			lf.lfHeight = _ttol(startPos);
			break;
		case 2://=== lfWidth
			lf.lfWidth = _ttol(startPos);
			break;
		case 3://=== lfWeight
			lf.lfWeight = _ttol(startPos);
			break;
		case 4://=== lfItalic
			lf.lfItalic = _ttol(startPos) > 0 ? 1 : 0;
			break;
		case 5://=== lfUnderline
			lf.lfUnderline = _ttol(startPos) > 0 ? 1 : 0;
			break;
		}
		if (endPos == NULL)
		{
			if (tokNum == 5)
				return TRUE;
			return FALSE;
		}
		startPos = endPos + 1;
		tokNum++;

	}
	return FALSE;
}
LPTSTR TSSkin::LogFont2String(LOGFONT& lf, LPTSTR bf, UINT bfLen)
{
	//=== Font format: lfFaceName;lfHeight;lfWidth;lfWeight;lfItalic
	_sntprintf(bf, bfLen, 
		_T("%s;%d;%d;%d;%d;%d"), 
		lf.lfFaceName, 
		lf.lfHeight,
		lf.lfWidth,
		lf.lfWeight,
		lf.lfItalic,
		lf.lfUnderline);
	return bf;
}
