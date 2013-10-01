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

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLDoseMorphologyNode);

//----------------------------------------------------------------------------
vtkMRMLDoseMorphologyNode::vtkMRMLDoseMorphologyNode()
{
  this->ReferenceDoseVolumeNodeID = NULL;
  this->InputDoseVolumeNodeID = NULL;
  this->OutputDoseVolumeNodeID = NULL;
  this->Operation = SLICERRT_EXPAND_BY_SCALING;
  this->XSize = 1;
  this->YSize = 1;
  this->ZSize = 1;

  this->HideFromEditors = false;
}

//----------------------------------------------------------------------------
vtkMRMLDoseMorphologyNode::~vtkMRMLDoseMorphologyNode()
{
  this->SetReferenceDoseVolumeNodeID(NULL);
  this->SetInputDoseVolumeNodeID(NULL);
  this->SetOutputDoseVolumeNodeID(NULL);
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);

  {
    std::stringstream ss;
    if ( this->ReferenceDoseVolumeNodeID )
      {
      ss << this->ReferenceDoseVolumeNodeID;
      of << indent << " ReferenceDoseVolumeNodeID=\"" << ss.str() << "\"";
      }
  }

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
    if ( this->OutputDoseVolumeNodeID )
      {
      ss << this->OutputDoseVolumeNodeID;
      of << indent << " OutputDoseVolumeNodeID=\"" << ss.str() << "\"";
      }
  }

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

    if (!strcmp(attName, "ReferenceDoseVolumeNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveReferenceDoseVolumeNodeID(ss.str().c_str());
      }
    if (!strcmp(attName, "InputDoseVolumeNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveInputDoseVolumeNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "OutputDoseVolumeNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveOutputDoseVolumeNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "Operation")) 
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

  this->SetAndObserveReferenceDoseVolumeNodeID(node->ReferenceDoseVolumeNodeID);
  this->SetAndObserveInputDoseVolumeNodeID(node->InputDoseVolumeNodeID);
  this->SetAndObserveOutputDoseVolumeNodeID(node->OutputDoseVolumeNodeID);

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

  os << indent << "ReferenceDoseVolumeNodeID:   " << this->ReferenceDoseVolumeNodeID << "\n";
  os << indent << "InputDoseVolumeNodeID:   " << this->InputDoseVolumeNodeID << "\n";
  os << indent << "OutputDoseVolumeNodeID:   " << this->OutputDoseVolumeNodeID << "\n";
  os << indent << "Operation:   " << (this->Operation) << "\n";
  os << indent << "XSize:   " << (this->XSize) << "\n";
  os << indent << "YSize:   " << (this->YSize) << "\n";
  os << indent << "ZSize:   " << (this->ZSize) << "\n";
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  if (this->ReferenceDoseVolumeNodeID && !strcmp(oldID, this->ReferenceDoseVolumeNodeID))
    {
    this->SetAndObserveReferenceDoseVolumeNodeID(newID);
    }
  if (this->InputDoseVolumeNodeID && !strcmp(oldID, this->InputDoseVolumeNodeID))
    {
    this->SetAndObserveInputDoseVolumeNodeID(newID);
    }
  if (this->OutputDoseVolumeNodeID && !strcmp(oldID, this->OutputDoseVolumeNodeID))
    {
    this->SetAndObserveOutputDoseVolumeNodeID(newID);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::SetAndObserveReferenceDoseVolumeNodeID(const char* id)
{
  if (this->ReferenceDoseVolumeNodeID != NULL)
    {
    this->Scene->RemoveReferencedNodeID(this->ReferenceDoseVolumeNodeID, this);
    }

  this->SetReferenceDoseVolumeNodeID(id);

  if (id)
    {
    this->Scene->AddReferencedNodeID(this->ReferenceDoseVolumeNodeID, this);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDoseMorphologyNode::SetAndObserveInputDoseVolumeNodeID(const char* id)
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
void vtkMRMLDoseMorphologyNode::SetAndObserveOutputDoseVolumeNodeID(const char* id)
{
  if (this->OutputDoseVolumeNodeID != NULL)
    {
    this->Scene->RemoveReferencedNodeID(this->OutputDoseVolumeNodeID, this);
    }

  this->SetOutputDoseVolumeNodeID(id);

  if (id)
    {
    this->Scene->AddReferencedNodeID(this->OutputDoseVolumeNodeID, this);
    }
}
