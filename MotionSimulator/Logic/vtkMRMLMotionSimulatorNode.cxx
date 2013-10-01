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
#include "vtkMRMLMotionSimulatorNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLMotionSimulatorNode);

//----------------------------------------------------------------------------
vtkMRMLMotionSimulatorNode::vtkMRMLMotionSimulatorNode()
{
  this->InputDoseVolumeNodeID = NULL;
  this->InputContourNodeID = NULL;
  this->OutputDoubleArrayNodeID = NULL;
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
  this->SetInputDoseVolumeNodeID(NULL);
  this->SetInputContourNodeID(NULL);
  this->SetOutputDoubleArrayNodeID(NULL);
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);

  {
    std::stringstream ss;
    if ( this->InputDoseVolumeNodeID )
      {
      ss << this->InputDoseVolumeNodeID;
      of << indent << " InputDoseVolumeNodeID=\"" << ss.str() << "\"";
      }
  }

  {
    std::stringstream ss;
    if ( this->InputContourNodeID )
      {
      ss << this->InputContourNodeID;
      of << indent << " InputContourNodeID=\"" << ss.str() << "\"";
      }
  }

  {
    std::stringstream ss;
    if ( this->OutputDoubleArrayNodeID )
      {
      ss << this->OutputDoubleArrayNodeID;
      of << indent << " OutputDoubleArrayNodeID=\"" << ss.str() << "\"";
      }
  }

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

    if (!strcmp(attName, "InputDoseVolumeNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveInputDoseVolumeNodeID(ss.str().c_str());
      }
    if (!strcmp(attName, "InputContourNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveInputContourNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "OutputDoubleArrayNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveOutputDoubleArrayNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "NumberOfSimulation")) 
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

  this->SetAndObserveInputDoseVolumeNodeID(node->InputDoseVolumeNodeID);
  this->SetAndObserveInputContourNodeID(node->InputContourNodeID);
  this->SetAndObserveOutputDoubleArrayNodeID(node->OutputDoubleArrayNodeID);

  this->NumberOfSimulation = node->GetNumberOfSimulation();
  this->NumberOfFraction = node->GetNumberOfFraction();
  this->XSysSD = node->GetXSysSD();
  this->YSysSD = node->GetYSysSD();
  this->ZSysSD = node->GetZSysSD();
  this->XSysSD = node->GetXSysSD();
  this->YSysSD = node->GetYSysSD();
  this->ZSysSD = node->GetZSysSD();

  this->DisableModifiedEventOff();
  this->InvokePendingModifiedEvent();
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "InputDoseVolumeNodeID:   " << this->InputDoseVolumeNodeID << "\n";
  os << indent << "InputContourNodeID:   " << this->InputContourNodeID << "\n";
  os << indent << "OutputDoubleArrayNodeID:   " << this->OutputDoubleArrayNodeID << "\n";
  os << indent << "NumberOfSimulation:   " << (this->NumberOfSimulation) << "\n";
  os << indent << "NumberOfFraction:   " << (this->NumberOfFraction) << "\n";
  os << indent << "XSysSD:   " << (this->XSysSD) << "\n";
  os << indent << "YSysSD:   " << (this->YSysSD) << "\n";
  os << indent << "ZSysSD:   " << (this->ZSysSD) << "\n";
  os << indent << "XSysSD:   " << (this->XSysSD) << "\n";
  os << indent << "YSysSD:   " << (this->YSysSD) << "\n";
  os << indent << "ZSysSD:   " << (this->ZSysSD) << "\n";
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  if (this->InputDoseVolumeNodeID && !strcmp(oldID, this->InputDoseVolumeNodeID))
    {
    this->SetAndObserveInputDoseVolumeNodeID(newID);
    }
  if (this->InputContourNodeID && !strcmp(oldID, this->InputContourNodeID))
    {
    this->SetAndObserveInputContourNodeID(newID);
    }
  if (this->OutputDoubleArrayNodeID && !strcmp(oldID, this->OutputDoubleArrayNodeID))
    {
    this->SetAndObserveOutputDoubleArrayNodeID(newID);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::SetAndObserveInputDoseVolumeNodeID(const char* id)
{
  if (this->InputDoseVolumeNodeID != NULL)
    {
    this->Scene->RemoveReferencedNodeID(this->InputDoseVolumeNodeID, this);
    }

  this->SetInputDoseVolumeNodeID(id);

  if (id)
    {
    this->Scene->AddReferencedNodeID(this->InputDoseVolumeNodeID, this);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::SetAndObserveInputContourNodeID(const char* id)
{
  if (this->InputContourNodeID != NULL)
    {
    this->Scene->RemoveReferencedNodeID(this->InputContourNodeID, this);
    }

  this->SetInputContourNodeID(id);

  if (id)
    {
    this->Scene->AddReferencedNodeID(this->InputContourNodeID, this);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLMotionSimulatorNode::SetAndObserveOutputDoubleArrayNodeID(const char* id)
{
  if (this->OutputDoubleArrayNodeID != NULL)
    {
    this->Scene->RemoveReferencedNodeID(this->OutputDoubleArrayNodeID, this);
    }

  this->SetOutputDoubleArrayNodeID(id);

  if (id)
    {
    this->Scene->AddReferencedNodeID(this->OutputDoubleArrayNodeID, this);
    }
}
