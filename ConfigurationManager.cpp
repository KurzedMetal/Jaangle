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
#include "StdAfx.h"
#include "ConfigurationManager.h"

ConfigurationManager::ConfigurationManager()
{
}


ConfigurationManager::~ConfigurationManager()
{
}


//=== ITranslationManager
void ConfigurationManager::RegisterConfigurable(IConfigurable& configurable)
{
	m_configurables.insert(&configurable);
}
void ConfigurationManager::UnRegisterConfigurable(IConfigurable& configurable)
{
	std::set<IConfigurable*>::iterator it = m_configurables.find(&configurable);
	if (it != m_configurables.end())
		m_configurables.erase(it);
	else
		ASSERT(0);
}
void ConfigurationManager::UpdateConfigurables()
{
	std::set<IConfigurable*>::iterator it = m_configurables.begin();
	for (;it != m_configurables.end(); it++)
		(*it)->UpdateConfiguration();
}

