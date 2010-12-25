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
#include "FlexDblBuffer.h"


FlexDblBuffer::~FlexDblBuffer()
{
	delete m_pB;
	delete m_pG;
}

Gdiplus::Graphics* FlexDblBuffer::GetGraphics(INT cx, INT cy)
{
	if (cx != 0 && cy != 0)
	{
		m_cx = cx;
		m_cy = cy;
	}
	ASSERT(m_cx != 0 && m_cy != 0);
	if (m_pB != NULL)
	{//===Checks if there is a need to recreate the buffer
		if ((int)m_pB->GetWidth() < m_cx || (int)m_pB->GetHeight() < m_cy)
		{
			delete m_pB;
			m_pB = NULL;
			delete m_pG;
			m_pG = NULL;
		}
	}
	if (m_pB == NULL)
	{//===This can happen at the beginning and on resize (sometimes)
		TRACE(_T("@3 FlexDblBuffer::GetGraphics. Creating New Buffer (%dx%d)\r\n"), cx, cy);
		m_pB = new Gdiplus::Bitmap(m_cx, m_cy, PixelFormat32bppARGB);
		if (m_pB == NULL)
			TRACE(_T("@1 FlexDblBuffer::GetGraphics. Creating New Bitmap (FAILED)\r\n"));
		else
		{
			m_pG = Gdiplus::Graphics::FromImage(m_pB);
			if (m_pG == NULL)
				TRACE(_T("@1 FlexDblBuffer::GetGraphics. Creating New Graphics (FAILED)\r\n"));
			else
				m_pG->SetSmoothingMode(m_smoothingMode);

		}
	}

	return m_pG;
}

void FlexDblBuffer::Render(Gdiplus::Graphics& destination, INT X, INT Y)
{
	ASSERT(m_cx > 0 && m_cy > 0);
	ASSERT(m_pB != NULL);
	if (m_cx > 0 && m_cy > 0 && m_pB != NULL)
	{
		//=== DEBUG in order to find out why in VMWare it fails to destination.DrawImage 
		//			(without getting an error)
		//			FillRectangle in contrast always succeeds
		//destination.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::Red), X, Y, m_cx,m_cy);
		//m_pG->FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::Blue), X, Y, m_cx,m_cy / 2);
		if (destination.DrawImage(m_pB, X, Y, 0, 0, m_cx, m_cy, Gdiplus::UnitPixel) != Gdiplus::Ok)
		{
			TRACE(_T("@1 FlexDblBuffer::Render x:%d y:%d (FAILED)\r\n"), X, Y);
			TRACE(_T("@1 \tcx:%d cy:%d (FAILED)\r\n"), m_cx, m_cy);
		}
	}
}

Gdiplus::Bitmap* FlexDblBuffer::GetBitmap()
{
	return m_pB;
}

void FlexDblBuffer::ConfigBuffer(Gdiplus::SmoothingMode smoothingMode)
{
	m_smoothingMode = smoothingMode;
}


