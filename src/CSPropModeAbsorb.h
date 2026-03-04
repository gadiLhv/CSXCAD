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

#pragma once

#include "CSProperties.h"

//! Property for mode-matched waveguide port absorption.
/*!
  Stores E-field and H-field mode distribution file paths and the
  normal direction sign.  Used together with the mode-absorb operator
  and engine extensions in openEMS.
*/
class CSXCAD_EXPORT CSPropModeAbsorb : public CSProperties
{
public:
	CSPropModeAbsorb(ParameterSet* paraSet);
	CSPropModeAbsorb(CSPropModeAbsorb* prop, bool copyPrim = false);
	CSPropModeAbsorb(unsigned int ID, ParameterSet* paraSet);
	virtual ~CSPropModeAbsorb();

	virtual void Init();

	virtual const std::string GetTypeXMLString() const {return std::string("ModeAbsorb");}

	void SetEModeFileName(const std::string& fn) {m_EModeFileName = fn;}
	std::string GetEModeFileName() const {return m_EModeFileName;}

	void SetHModeFileName(const std::string& fn) {m_HModeFileName = fn;}
	std::string GetHModeFileName() const {return m_HModeFileName;}

	void SetNormalSignPositive(bool val) {NormSignPositive = val;}
	bool GetNormalSignPositive() const {return NormSignPositive;}

	virtual bool Update(std::string *ErrStr = NULL);
	virtual bool Write2XML(TiXmlNode& root, bool parameterised = true, bool sparse = false);
	virtual bool ReadFromXML(TiXmlNode &root);
	virtual void ShowPropertyStatus(std::ostream& stream);

protected:
	std::string m_EModeFileName;
	std::string m_HModeFileName;
	bool        NormSignPositive;
};
