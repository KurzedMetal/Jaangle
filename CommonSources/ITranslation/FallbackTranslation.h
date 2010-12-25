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
#ifndef _FALLBACKTRANSLATION_H_
#define _FALLBACKTRANSLATION_H_

class FallbackTranslation: public ITranslation
{
public:
	FallbackTranslation():m_pTranslation(NULL),m_pDefaultTranslation(NULL)	{}
	virtual ~FallbackTranslation()											{}
	virtual LPCTSTR GetTranslationInfo(TranslationInfoEnum infoType)
	{
		if (m_pTranslation != NULL)
			return m_pTranslation->GetTranslationInfo(infoType);
		if (m_pDefaultTranslation != NULL)
			return m_pDefaultTranslation->GetTranslationInfo(infoType);
		return NULL;
	}

	virtual LPCTSTR GetString(UINT stringID)
	{
		LPCTSTR ret = NULL;
		if (m_pTranslation != NULL)
			ret = m_pTranslation->GetString(stringID);
		if (ret == NULL && m_pDefaultTranslation != NULL)
			return m_pDefaultTranslation->GetString(stringID);
		return ret;
	}

	//=== Extra Interface
	void SetTranslation(ITranslation* pTranslation)				{m_pTranslation = pTranslation;}
	void SetDefaultTranslation(ITranslation* pDefTranslation)	{m_pDefaultTranslation = pDefTranslation;}

private:
	ITranslation* m_pTranslation;
	ITranslation* m_pDefaultTranslation;
};

#endif
