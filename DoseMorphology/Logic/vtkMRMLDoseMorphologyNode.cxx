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
  University Health Network and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

// MRMLDoseAccumulation includes
#include "vtkMRMLDoseMorphologyNode.h"

// SlicerRT includes
#include "SlicerRtCommon.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
std::string vtkMRMLDoseMorphologyNode::ReferenceDoseVolumeReferenceRole = std::string("referenceDoseVolume") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLDoseMorphologyNode::InputDoseVolumeReferenceRole = std::string("inputDoseVolume") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;
std::string vtkMRMLDoseMorphologyNode::OutputDoseVolumeReferenceRole = std::string("outputDoseVolume") + SlicerRtCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX;

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLDoseMorphologyNode);

//----------------------------------------------------------------------------
vtkMRMLDoseMorphologyNode::vtkMRMLDoseMorphologyNode()
{
  this->Operation = SLICERRT_EXPAND_BY_SCALING;
  this->XSize = 1;
  this->YSize = 1;
  this->ZSize = 1;

  this->HideFromEditors = false;
}

//----------------------------------------------------------------------------
vtkMRMLDoseMorphologyNode::~vtkMRMLDoseMorphologyNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);

  of << indent << " Operation=\"" << (this->Operation) << "\"";

  of << indent << " XSize=\"" << (this->XSize) << "\"";

  of << indent << " YSize=\"" << (this->YSize) << "\"";

  of << indent << " ZSize=\"" << (this->ZSize) << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "Operation")) 
      {
      this->Operation = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "XSize")) 
      {
      this->XSize = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "YSize")) 
      {
      this->YSize = 
        (strcmp(attValue,"true") ? false : true);
      }
    else if (!strcmp(attName, "ZSize")) 
      {
      this->ZSize = 
        (strcmp(attValue,"true") ? false : true);
      }
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLDoseMorphologyNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  this->DisableModifiedEventOn();

  vtkMRMLDoseMorphologyNode *node = (vtkMRMLDoseMorphologyNode *)anode;

  this->Operation = node->Operation;
  this->XSize = node->XSize;
  this->YSize = node->YSize;
  this->ZSize = node->ZSize;

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "Operation:   " << (this->Operation) << "\n";
  os << indent << "XSize:   " << (this->XSize) << "\n";
  os << indent << "YSize:   " << (this->YSize) << "\n";
  os << indent << "ZSize:   " << (this->ZSize) << "\n";
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLDoseMorphologyNode::GetReferenceDoseVolumeNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDoseMorphologyNode::ReferenceDoseVolumeReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::SetAndObserveReferenceDoseVolumeNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDoseMorphologyNode::ReferenceDoseVolumeReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLDoseMorphologyNode::GetInputDoseVolumeNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDoseMorphologyNode::InputDoseVolumeReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::SetAndObserveInputDoseVolumeNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDoseMorphologyNode::InputDoseVolumeReferenceRole.c_str(), node->GetID());
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkMRMLDoseMorphologyNode::GetOutputDoseVolumeNode()
{
  return vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetNodeReference(vtkMRMLDoseMorphologyNode::OutputDoseVolumeReferenceRole.c_str()) );
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::SetAndObserveOutputDoseVolumeNode(vtkMRMLScalarVolumeNode* node)
{
  this->SetNodeReferenceID(vtkMRMLDoseMorphologyNode::OutputDoseVolumeReferenceRole.c_str(), node->GetID());
}

