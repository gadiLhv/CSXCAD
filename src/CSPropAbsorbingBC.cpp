/*
*	Copyright (C) 2023-2025 Gadi Lahav (gadi@rfwithcare.com)
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU Lesser General Public License as published
*	by the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tinyxml.h"

#include "CSPropAbsorbingBC.h"

#define _C0_ 299792458.0

CSPropAbsorbingBC::CSPropAbsorbingBC(ParameterSet* paraSet) : CSProperties(paraSet) {Type = ABSORBING_BC; Init();}
CSPropAbsorbingBC::CSPropAbsorbingBC(CSPropAbsorbingBC* prop, bool copyPrim) : CSProperties(prop, copyPrim)
{
	Type = ABSORBING_BC;
	Init();

	NormSignPositive = prop->NormSignPositive;
	PhaseVelocity.Copy(&prop->PhaseVelocity);
	AbsorbingBoundaryType = prop->AbsorbingBoundaryType;
	m_EModeFileName = prop->m_EModeFileName;
	m_HModeFileName = prop->m_HModeFileName;
	m_WaveImpedance = prop->m_WaveImpedance;
}
CSPropAbsorbingBC::CSPropAbsorbingBC(unsigned int ID, ParameterSet* paraSet) : CSProperties(ID,paraSet) {Type = ABSORBING_BC; Init();}
CSPropAbsorbingBC::~CSPropAbsorbingBC()
{
}

void CSPropAbsorbingBC::Init()
{
	NormSignPositive = true;
	PhaseVelocity.SetValue((double)_C0_);
	AbsorbingBoundaryType = CSPropAbsorbingBC::UNDEFINED;
	m_EModeFileName.clear();
	m_HModeFileName.clear();
	// Sentinel: any value < 0 means "not set by the user". MODAL absorbers
	// require the caller to provide a positive wave impedance; setup will
	// abort if this is left negative.
	m_WaveImpedance = -1.0;
}

bool CSPropAbsorbingBC::Update(std::string *ErrStr)
{
	bool bOK = true;
	int EC = PhaseVelocity.Evaluate();
	if (EC != ParameterScalar::PS_NO_ERROR) bOK = false;
	if ((EC != ParameterScalar::PS_NO_ERROR) && (ErrStr != NULL))
	{
		std::stringstream stream;
		stream << std::endl << "Error in AbsorbingBC-Property PhaseVelocity-Value";
		ErrStr->append(stream.str());
		PSErrorCode2Msg(EC,ErrStr);
	}

	return bOK & CSProperties::Update(ErrStr);
}

void CSPropAbsorbingBC::SetPhaseVelocity(double val)
{
	if (val >= 0)
		PhaseVelocity.SetValue(val);
	else
	{
		std::cerr << "CSPropAbsorbingBC::SetPhaseVelocity: Warning: Unable to set velocity smaller than zero. Setting to 0*C0" << std::endl;
		PhaseVelocity.SetValue(0);
	}

}

bool CSPropAbsorbingBC::Write2XML(TiXmlNode& root, bool parameterised, bool sparse)
{
	if (CSProperties::Write2XML(root,parameterised,sparse) == false) return false;

	TiXmlElement* prop=root.ToElement();

	if (prop==NULL) return false;

	prop->SetAttribute("NormalSignPositive",(int)NormSignPositive);
	prop->SetAttribute("AbsorbingBoundaryType",(int)AbsorbingBoundaryType);

	WriteTerm(PhaseVelocity,*prop,"PhaseVelocity",parameterised);

	if (!m_EModeFileName.empty())
		prop->SetAttribute("EModeFileName", m_EModeFileName.c_str());
	if (!m_HModeFileName.empty())
		prop->SetAttribute("HModeFileName", m_HModeFileName.c_str());
	if (m_WaveImpedance > 0.0)
		prop->SetDoubleAttribute("WaveImpedance", m_WaveImpedance);

	return true;
}

bool CSPropAbsorbingBC::ReadFromXML(TiXmlNode &root)
{
	if (CSProperties::ReadFromXML(root)==false) return false;

	TiXmlElement* prop=root.ToElement();
	if (prop==NULL) return false;

	if (prop->QueryBoolAttribute("NormalSignPositive", &NormSignPositive) != TIXML_SUCCESS)
	{
		std::cerr << "CSPropAbsorbingBC::ReadFromXML: Warning: Failed to read normal sign. Setting to true" << std::endl;
		NormSignPositive = true;
	}

	if (ReadTerm(PhaseVelocity,*prop,"PhaseVelocity")==false)
		std::cerr << "CSPropAbsorbingBC::ReadFromXML: Warning: Failed to read Phase velocity. Set to C0." << std::endl;

	int i_ABCtype;
	if (prop->QueryIntAttribute("AbsorbingBoundaryType", &i_ABCtype) != TIXML_SUCCESS) i_ABCtype = 0;
	AbsorbingBoundaryType = (ABCtype)i_ABCtype;

	if (prop->QueryStringAttribute("EModeFileName", &m_EModeFileName) != TIXML_SUCCESS)
		m_EModeFileName.clear();
	if (prop->QueryStringAttribute("HModeFileName", &m_HModeFileName) != TIXML_SUCCESS)
		m_HModeFileName.clear();
	if (prop->QueryDoubleAttribute("WaveImpedance", &m_WaveImpedance) != TIXML_SUCCESS)
		m_WaveImpedance = -1.0;

	return true;
}

void CSPropAbsorbingBC::ShowPropertyStatus(std::ostream& stream)
{
	std::string s_sign = NormSignPositive ?  "True" : "False";

	std::string s_BoundaryType;

	switch (AbsorbingBoundaryType)
	{
		case ABCtype::UNDEFINED:
			s_BoundaryType = "Undefined";
			break;
		case ABCtype::MUR_1ST:
			s_BoundaryType = "1st order Mur BC";
			break;
		case ABCtype::MUR_1ST_SA:
			s_BoundaryType = "1st order Mur BC with super-absorption";
			break;
		case ABCtype::MODAL:
			s_BoundaryType = "Modal absorber";
			break;
	}

	CSProperties::ShowPropertyStatus(stream);
	stream << " --- Absorbing BC Properties --- " << std::endl;
	stream << "  Normal Sign Positive: " << s_sign << std::endl;
	stream << "  Phase velocity: "   << PhaseVelocity.GetValue()/_C0_ << "*C0" << std::endl;
	stream << "  Absorbing boundary condition type: "   << s_BoundaryType << std::endl;
	if (!m_EModeFileName.empty())
		stream << "  E-mode file: " << m_EModeFileName << std::endl;
	if (!m_HModeFileName.empty())
		stream << "  H-mode file: " << m_HModeFileName << std::endl;
	if (m_WaveImpedance > 0.0)
		stream << "  Wave impedance: " << m_WaveImpedance << " Ohm" << std::endl;
}
