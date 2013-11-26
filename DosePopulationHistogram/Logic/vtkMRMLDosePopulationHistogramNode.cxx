/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kevin Wang, Radiation Medicine Program, 
  University Health Network and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provIDed by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in MarginCalculator Oncology (OCAIRO).

==============================================================================*/

// vtkMRMLDosePopulationHistogram includes
#include "vtkMRMLDosePopulationHistogramNode.h"

// SlicerRT includes
#include "SlicerRtCommon.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLDoubleArrayNode.h>
#include <vtkMRMLChartNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLMotionSimulatorDoubleArrayNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
std::string vtkMRMLDosePopulationHistogramNode::DoseVolumeReferenceRole = std::string("doseVolume") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLDosePopulationHistogramNode::DoubleArrayReferenceRole = std::string("doubleArray") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLDosePopulationHistogramNode::ContourReferenceRole = std::string("contour") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLDosePopulationHistogramNode::ChartReferenceRole = std::string("chart") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLDosePopulationHistogramNode::OutputDoubleArrayReferenceRole = std::string("outputDoubleArray") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLDosePopulationHistogramNode);

//----------------------------------------------------------------------------
vtkMRMLDosePopulationHistogramNode::vtkMRMLDosePopulationHistogramNode()
{
  this->UseDoseOption = ART_DPH_USEDMIN;
  this->DPHDoubleArrayNodeIDs.clear();
  this->ShowHideAll = 0;
  this->ShowInChartCheckStates.clear();

  this->HideFromEditors = false;
}

//----------------------------------------------------------------------------
vtkMRMLDosePopulationHistogramNode::~vtkMRMLDosePopulationHistogramNode()
{
  this->DPHDoubleArrayNodeIDs.clear();
  this->ShowInChartCheckStates.clear();
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);

  of << indent << " UseDoseOption=\"" << this->UseDoseOption << "\"";

  of << indent << " DPHDoubleArrayNodeIDs=\"";
  for (std::vector<std::string>::iterator it = this->DPHDoubleArrayNodeIDs.begin(); it != this->DPHDoubleArrayNodeIDs.end(); ++it)
    {
    of << (*it) << "|";
    }
  of << "\"";

  {
    std::stringstream ss;
    ss << this->ShowHideAll;
    of << indent << " ShowHIDeAll=\"" << ss.str() << "\"";
  }

  of << indent << " ShowInChartCheckStates=\"";
  for (std::vector<bool>::iterator it = this->ShowInChartCheckStates.begin(); it != this->ShowInChartCheckStates.end(); ++it)
    {
    of << ((*it) ? "true" : "false") << "|";
    }
  of << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "UseDoseOption")) 
      {
      this->UseDoseOption = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "DPHDoubleArrayNodeIDs")) 
      {
      std::stringstream ss;
      ss << attValue;
      std::string valueStr = ss.str();
      std::string separatorCharacter("|");

      this->DPHDoubleArrayNodeIDs.clear();
      size_t separatorPosition = valueStr.find( separatorCharacter );
      while (separatorPosition != std::string::npos)
        {
        this->DPHDoubleArrayNodeIDs.push_back( valueStr.substr(0, separatorPosition) );
        valueStr = valueStr.substr( separatorPosition+1 );
        separatorPosition = valueStr.find( separatorCharacter );
        }
      if (! valueStr.empty() )
        {
        this->DPHDoubleArrayNodeIDs.push_back( valueStr );
        }
      }
    else if (!strcmp(attName, "ShowHIDeAll")) 
      {
      std::stringstream ss;
      ss << attValue;
      int intAttValue;
      ss >> intAttValue;
      this->ShowHideAll = intAttValue;
      }
    else if (!strcmp(attName, "ShowInChartCheckStates")) 
      {
      std::stringstream ss;
      ss << attValue;
      std::string valueStr = ss.str();
      std::string separatorCharacter("|");

      this->ShowInChartCheckStates.clear();
      size_t separatorPosition = valueStr.find( separatorCharacter );
      while (separatorPosition != std::string::npos)
        {
        this->ShowInChartCheckStates.push_back(
          (valueStr.substr(0, separatorPosition).compare("true") ? false : true));
        valueStr = valueStr.substr( separatorPosition+1 );
        separatorPosition = valueStr.find( separatorCharacter );
        }
      if (! valueStr.empty() )
        {
        this->ShowInChartCheckStates.push_back(
          (valueStr.compare("true") ? false : true));
        }
      }
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLDosePopulationHistogramNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  this->DisableModifiedEventOn();

  vtkMRMLDosePopulationHistogramNode *node = (vtkMRMLDosePopulationHistogramNode *) anode;

  this->UseDoseOption = node->UseDoseOption;

  this->DPHDoubleArrayNodeIDs = node->DPHDoubleArrayNodeIDs;
  this->ShowHideAll = node->ShowHideAll;
  this->ShowInChartCheckStates = node->ShowInChartCheckStates;

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  {
    os << indent << "DPHDoubleArrayNodeIDs:   ";
    for (std::vector<std::string>::iterator it = this->DPHDoubleArrayNodeIDs.begin(); it != this->DPHDoubleArrayNodeIDs.end(); ++it)
      {
      os << (*it) << "|";
      }
    os << "\n";
  }

  os << indent << "ShowHideAll:   " << this->ShowHideAll << "\n";

  {
    os << indent << "ShowInChartCheckStates:   ";
    for (std::vector<bool>::iterator it = this->ShowInChartCheckStates.begin(); it != this->ShowInChartCheckStates.end(); ++it)
      {
      os << indent << ((*it) ? "true" : "false") << "|";
      }
    os << "\n";
  }
}

//----------------------------------------------------------------------------
vtkMRMLMotionSimulatorDoubleArrayNode* vtkMRMLDosePopulationHistogramNode::GetDoubleArrayNode()
{
  return vtkMRMLMotionSimulatorDoubleArrayNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDosePopulationHistogramNode::DoubleArrayReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveDoubleArrayNode(vtkMRMLMotionSimulatorDoubleArrayNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDosePopulationHistogramNode::DoubleArrayReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLDosePopulationHistogramNode::GetDoseVolumeNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDosePopulationHistogramNode::DoseVolumeReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveDoseVolumeNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDosePopulationHistogramNode::DoseVolumeReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLDosePopulationHistogramNode::GetContourNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDosePopulationHistogramNode::ContourReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveContourNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDosePopulationHistogramNode::ContourReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLChartNode* vtkMRMLDosePopulationHistogramNode::GetChartNode()
{
  return vtkMRMLChartNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDosePopulationHistogramNode::ChartReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveChartNode(vtkMRMLChartNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDosePopulationHistogramNode::ChartReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLDoubleArrayNode* vtkMRMLDosePopulationHistogramNode::GetOutputDoubleArrayNode()
{
  return vtkMRMLDoubleArrayNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDosePopulationHistogramNode::OutputDoubleArrayReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveOutputDoubleArrayNode(vtkMRMLDoubleArrayNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDosePopulationHistogramNode::OutputDoubleArrayReferenceRole.c_str(), node->GetID());
}


