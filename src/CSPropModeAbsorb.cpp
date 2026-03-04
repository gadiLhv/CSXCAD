/*
*	Copyright (C) 2025 Gadi Lahav (gadi@rfwithcare.com)
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tinyxml.h"
#include "CSPropModeAbsorb.h"

CSPropModeAbsorb::CSPropModeAbsorb(ParameterSet* paraSet) : CSProperties(paraSet) {Type = MODE_ABSORB; Init();}

CSPropModeAbsorb::CSPropModeAbsorb(CSPropModeAbsorb* prop, bool copyPrim) : CSProperties(prop, copyPrim)
{
	Type = MODE_ABSORB;
	Init();
	m_EModeFileName = prop->m_EModeFileName;
	m_HModeFileName = prop->m_HModeFileName;
	NormSignPositive = prop->NormSignPositive;
}

CSPropModeAbsorb::CSPropModeAbsorb(unsigned int ID, ParameterSet* paraSet) : CSProperties(ID, paraSet) {Type = MODE_ABSORB; Init();}

CSPropModeAbsorb::~CSPropModeAbsorb() {}

void CSPropModeAbsorb::Init()
{
	NormSignPositive = true;
	m_EModeFileName.clear();
	m_HModeFileName.clear();
}

bool CSPropModeAbsorb::Update(std::string *ErrStr)
{
	return CSProperties::Update(ErrStr);
}

bool CSPropModeAbsorb::Write2XML(TiXmlNode& root, bool parameterised, bool sparse)
{
	if (CSProperties::Write2XML(root, parameterised, sparse) == false) return false;
	TiXmlElement* prop = root.ToElement();
	if (prop == NULL) return false;

	prop->SetAttribute("NormalSignPositive", (int)NormSignPositive);
	if (!m_EModeFileName.empty())
		prop->SetAttribute("EModeFileName", m_EModeFileName.c_str());
	if (!m_HModeFileName.empty())
		prop->SetAttribute("HModeFileName", m_HModeFileName.c_str());

	return true;
}

bool CSPropModeAbsorb::ReadFromXML(TiXmlNode &root)
{
	if (CSProperties::ReadFromXML(root) == false) return false;
	TiXmlElement* prop = root.ToElement();
	if (prop == NULL) return false;

	if (prop->QueryBoolAttribute("NormalSignPositive", &NormSignPositive) != TIXML_SUCCESS)
	{
		std::cerr << "CSPropModeAbsorb::ReadFromXML: Warning: Failed to read normal sign. Setting to true" << std::endl;
		NormSignPositive = true;
	}

	const char* attr = NULL;
	attr = prop->Attribute("EModeFileName");
	if (attr)
		m_EModeFileName = std::string(attr);
	else
		std::cerr << "CSPropModeAbsorb::ReadFromXML: Warning: Failed to read E-mode file name." << std::endl;

	attr = prop->Attribute("HModeFileName");
	if (attr)
		m_HModeFileName = std::string(attr);
	else
		std::cerr << "CSPropModeAbsorb::ReadFromXML: Warning: Failed to read H-mode file name." << std::endl;

	return true;
}

void CSPropModeAbsorb::ShowPropertyStatus(std::ostream& stream)
{
	CSProperties::ShowPropertyStatus(stream);
	stream << " --- Mode Absorb Properties --- " << std::endl;
	stream << "  Normal Sign Positive: " << (NormSignPositive ? "true" : "false") << std::endl;
	stream << "  E-mode file: " << m_EModeFileName << std::endl;
	stream << "  H-mode file: " << m_HModeFileName << std::endl;
}
