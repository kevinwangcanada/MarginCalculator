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

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLDoubleArrayNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>

// STD includes
#include <sstream>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLDosePopulationHistogramNode);

//----------------------------------------------------------------------------
vtkMRMLDosePopulationHistogramNode::vtkMRMLDosePopulationHistogramNode()
{
  this->DoubleArrayNodeID = NULL;
  this->DoseVolumeNodeID = NULL;
  this->ContourNodeID = NULL;
  this->ChartNodeID = NULL;
  this->OutputDoubleArrayNodeID = NULL;
  this->UseDoseOption = ART_DPH_USEDMIN;
  this->DPHDoubleArrayNodeIDs.clear();
  this->ShowHideAll = 0;
  this->ShowInChartCheckStates.clear();

  this->HideFromEditors = false;
}

//----------------------------------------------------------------------------
vtkMRMLDosePopulationHistogramNode::~vtkMRMLDosePopulationHistogramNode()
{
  this->SetDoubleArrayNodeID(NULL);
  this->SetDoseVolumeNodeID(NULL);
  this->SetContourNodeID(NULL);
  this->SetChartNodeID(NULL);
  this->SetOutputDoubleArrayNodeID(NULL);
  this->DPHDoubleArrayNodeIDs.clear();
  this->ShowInChartCheckStates.clear();
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  // Write all MRML node attributes into output stream
  vtkIndent indent(nIndent);

  if ( this->DoubleArrayNodeID )
    {
    of << indent << " DoubleArrrayNodeID=\"" << this->DoubleArrayNodeID << "\"";
    }

  if ( this->DoseVolumeNodeID )
    {
    of << indent << " DoseVolumeNodeID=\"" << this->DoseVolumeNodeID << "\"";
    }

  if ( this->ContourNodeID )
    {
    of << indent << " ContourNodeID=\"" << this->ContourNodeID << "\"";
    }

  if ( this->ChartNodeID )
    {
    of << indent << " ChartNodeID=\"" << this->ChartNodeID << "\"";
    }

  if ( this->OutputDoubleArrayNodeID )
    {
    of << indent << " OutputDoubleArrayNodeID=\"" << this->OutputDoubleArrayNodeID << "\"";
    }

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

    if (!strcmp(attName, "DoubleArrayNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveDoubleArrayNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "DoseVolumeNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveDoseVolumeNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "ContourNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveContourNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "ChartNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveChartNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "OutputDoubleArrayNodeID")) 
      {
      std::stringstream ss;
      ss << attValue;
      this->SetAndObserveOutputDoubleArrayNodeID(ss.str().c_str());
      }
    else if (!strcmp(attName, "UseDoseOption")) 
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

  this->SetAndObserveDoubleArrayNodeID(node->DoubleArrayNodeID);
  this->SetAndObserveDoseVolumeNodeID(node->DoseVolumeNodeID);
  this->SetAndObserveContourNodeID(node->ContourNodeID);
  this->SetAndObserveChartNodeID(node->ChartNodeID);
  this->SetAndObserveOutputDoubleArrayNodeID(node->OutputDoubleArrayNodeID);

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

  os << indent << "DoubleArrayNodeID:   " << this->DoubleArrayNodeID << "\n";
  os << indent << "DoseVolumeNodeID:   " << this->DoseVolumeNodeID << "\n";
  os << indent << "ContourNodeID:   " << this->ContourNodeID << "\n";
  os << indent << "ChartNodeID:   " << this->ChartNodeID << "\n";
  os << indent << "OutputDoubleArrayNodeID:   " << this->OutputDoubleArrayNodeID << "\n";
  os << indent << "UseDoseOption:   " << (this->UseDoseOption) << "\n";

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
void vtkMRMLDosePopulationHistogramNode::SetAndObserveDoubleArrayNodeID(const char* ID)
{
  if (this->DoubleArrayNodeID && this->Scene)
  {
    this->Scene->RemoveReferencedNodeID(this->DoubleArrayNodeID, this);
  }

  this->SetDoubleArrayNodeID(ID);

  if (ID && this->Scene)
  {
    this->Scene->AddReferencedNodeID(this->DoubleArrayNodeID, this);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveDoseVolumeNodeID(const char* id)
{
  if (this->DoseVolumeNodeID != NULL)
    {
    this->Scene->RemoveReferencedNodeID(this->DoseVolumeNodeID, this);
    }

  this->SetDoseVolumeNodeID(id);

  if (id)
    {
    this->Scene->AddReferencedNodeID(this->DoseVolumeNodeID, this);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveContourNodeID(const char* id)
{
  if (this->ContourNodeID != NULL)
    {
    this->Scene->RemoveReferencedNodeID(this->ContourNodeID, this);
    }

  this->SetContourNodeID(id);

  if (id)
    {
    this->Scene->AddReferencedNodeID(this->ContourNodeID, this);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveChartNodeID(const char* ID)
{
  if (this->ChartNodeID && this->Scene)
  {
    this->Scene->RemoveReferencedNodeID(this->ChartNodeID, this);
  }

  this->SetChartNodeID(ID);

  if (ID && this->Scene)
  {
    this->Scene->AddReferencedNodeID(this->ChartNodeID, this);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::SetAndObserveOutputDoubleArrayNodeID(const char* ID)
{
  if (this->OutputDoubleArrayNodeID && this->Scene)
  {
    this->Scene->RemoveReferencedNodeID(this->OutputDoubleArrayNodeID, this);
  }

  this->SetOutputDoubleArrayNodeID(ID);

  if (ID && this->Scene)
  {
    this->Scene->AddReferencedNodeID(this->OutputDoubleArrayNodeID, this);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLDosePopulationHistogramNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  if (this->DoubleArrayNodeID && !strcmp(oldID, this->DoubleArrayNodeID))
    {
    this->SetAndObserveDoubleArrayNodeID(newID);
    }
  if (this->DoseVolumeNodeID && !strcmp(oldID, this->DoseVolumeNodeID))
    {
    this->SetAndObserveDoseVolumeNodeID(newID);
    }
  if (this->ContourNodeID && !strcmp(oldID, this->ContourNodeID))
    {
    this->SetAndObserveContourNodeID(newID);
    }
  if (this->ChartNodeID && !strcmp(oldID, this->ChartNodeID))
    {
    this->SetAndObserveChartNodeID(newID);
    }
  if (this->OutputDoubleArrayNodeID && !strcmp(oldID, this->OutputDoubleArrayNodeID))
    {
    this->SetAndObserveOutputDoubleArrayNodeID(newID);
    }
  for (std::vector<std::string>::iterator it = this->DPHDoubleArrayNodeIDs.begin(); it != this->DPHDoubleArrayNodeIDs.end(); ++it)
    {
    if (!it->compare(oldID))
      {
      (*it) = newID;
      }
    }
}
