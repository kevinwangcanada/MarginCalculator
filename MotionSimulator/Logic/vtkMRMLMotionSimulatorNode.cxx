/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Kevin Wang, Radiation Medicine Program, 
  University Health Network and was supported by 
  Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

// MRML Motion Simulator includes
#include "vtkMRMLMotionSimulatorNode.h"

// SlicerRT includes
#include "MarginCalculatorCommon.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLMotionSimulatorDoubleArrayNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
std::string vtkMRMLMotionSimulatorNode::InputDoseVolumeReferenceRole = std::string("inputDoseVolume") + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLMotionSimulatorNode::InputContourReferenceRole = std::string("inputContour") + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLMotionSimulatorNode::OutputDoubleArrayReferenceRole = std::string("outputDoubleArray") + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMotionSimulatorNode);

//----------------------------------------------------------------------------
vtkMRMLMotionSimulatorNode::vtkMRMLMotionSimulatorNode()
{
  this->NumberOfSimulation = 1;
  this->NumberOfFraction = 1;

  this->XSysSD = 1;
  this->YSysSD = 1;
  this->ZSysSD = 1;

  this->XRdmSD = 1;
  this->YRdmSD = 1;
  this->ZRdmSD = 1;

  this->HideFromEditors = false;
}

//----------------------------------------------------------------------------
vtkMRMLMotionSimulatorNode::~vtkMRMLMotionSimulatorNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);

  of << indent << " NumberOfSimulation=\"" << (this->NumberOfSimulation) << "\"";

  of << indent << " NumberOfFraction=\"" << (this->NumberOfFraction) << "\"";

  of << indent << " XSysSD=\"" << (this->XSysSD) << "\"";

  of << indent << " YSysSD=\"" << (this->YSysSD) << "\"";

  of << indent << " ZSysSD=\"" << (this->ZSysSD) << "\"";

  of << indent << " XRdmSD=\"" << (this->XRdmSD) << "\"";

  of << indent << " YRdmSD=\"" << (this->YRdmSD) << "\"";

  of << indent << " ZRdmSD=\"" << (this->ZRdmSD) << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "NumberOfSimulation")) 
      {
      this->NumberOfSimulation = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "NumberOfFraction")) 
      {
      this->NumberOfFraction = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "XSysSD")) 
      {
      this->XSysSD = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "YSysSD")) 
      {
      this->YSysSD = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "ZSysSD")) 
      {
      this->ZSysSD = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "XRdmSD")) 
      {
      this->XRdmSD = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "YRdmSD")) 
      {
      this->YRdmSD = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "ZRdmSD")) 
      {
      this->ZRdmSD = 
        (strcmp(attValue,"true") ? false : true);
      }
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLMotionSimulatorNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  this->DisableModifiedEventOn();

  vtkMRMLMotionSimulatorNode *node = (vtkMRMLMotionSimulatorNode *)anode;

  this->NumberOfSimulation = node->GetNumberOfSimulation();
  this->NumberOfFraction = node->GetNumberOfFraction();

  this->XSysSD = node->GetXSysSD();
  this->YSysSD = node->GetYSysSD();
  this->ZSysSD = node->GetZSysSD();

  this->XRdmSD = node->GetXRdmSD();
  this->YRdmSD = node->GetYRdmSD();
  this->ZRdmSD = node->GetZRdmSD();

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "NumberOfSimulation:   " << (this->NumberOfSimulation) << "\n";
  os << indent << "NumberOfFraction:   " << (this->NumberOfFraction) << "\n";

  os << indent << "XSysSD:   " << (this->XSysSD) << "\n";
  os << indent << "YSysSD:   " << (this->YSysSD) << "\n";
  os << indent << "ZSysSD:   " << (this->ZSysSD) << "\n";

  os << indent << "XRdmSD:   " << (this->XRdmSD) << "\n";
  os << indent << "YRdmSD:   " << (this->YRdmSD) << "\n";
  os << indent << "ZRdmSD:   " << (this->ZRdmSD) << "\n";
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLMotionSimulatorNode::GetInputDoseVolumeNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLMotionSimulatorNode::InputDoseVolumeReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::SetAndObserveInputDoseVolumeNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(vtkMRMLMotionSimulatorNode::InputDoseVolumeReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLMotionSimulatorNode::GetInputContourNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLMotionSimulatorNode::InputContourReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::SetAndObserveInputContourNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(vtkMRMLMotionSimulatorNode::InputContourReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLMotionSimulatorDoubleArrayNode* vtkMRMLMotionSimulatorNode::GetOutputDoubleArrayNode()
{
  return vtkMRMLMotionSimulatorDoubleArrayNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLMotionSimulatorNode::OutputDoubleArrayReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::SetAndObserveOutputDoubleArrayNode(vtkMRMLMotionSimulatorDoubleArrayNode* node)
{
  this->SetNodeReferenceID(vtkMRMLMotionSimulatorNode::OutputDoubleArrayReferenceRole.c_str(), node->GetID());
}

