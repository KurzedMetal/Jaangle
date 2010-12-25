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
#include "CtrlColorsMediaSliderAdapter.h"
#include "MediaSlider.h"

CtrlColorsMediaSliderAdapter::CtrlColorsMediaSliderAdapter():
m_pMediaSlider(NULL)
{

}

void CtrlColorsMediaSliderAdapter::ConfigAdapter(CMediaSlider* pMediaSlider)
{
	m_pMediaSlider = pMediaSlider;
}

void CtrlColorsMediaSliderAdapter::SetColor(UINT idx, COLORREF value)
{
	if (m_pMediaSlider != NULL && idx < CMediaSlider::COL_Last)
		m_pMediaSlider->SetColor((CMediaSlider::COLORS)idx, value);
	else
		ASSERT(0);
}
COLORREF CtrlColorsMediaSliderAdapter::GetColor(UINT idx)
{
	if (m_pMediaSlider != NULL && idx < CMediaSlider::COL_Last)
		return m_pMediaSlider->GetColor((CMediaSlider::COLORS)idx);
	return -1;
}

LPCTSTR CtrlColorsMediaSliderAdapterColNames[] = {
	_T("Bk"),
	_T("Border"),
	_T("FirstPart"),
	_T("SecondPart"),
	_T("CursorBorder")
};

LPCTSTR CtrlColorsMediaSliderAdapter::GetColorName(UINT idx)
{
	if (idx < CMediaSlider::COL_Last)
		return CtrlColorsMediaSliderAdapterColNames[idx];
	return NULL;
}

