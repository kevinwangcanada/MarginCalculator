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
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in MarginCalculator Oncology (OCAIRO).

==============================================================================*/

#ifndef __vtkMRMLDosePopulationHistogramNode_h
#define __vtkMRMLDosePopulationHistogramNode_h

// MRML includes
#include <vtkMRML.h>
#include <vtkMRMLNode.h>

// STD includes
#include <vector>
#include <set>

#include "vtkSlicerDosePopulationHistogramModuleLogicExport.h"

// Operation options.
#define ART_DPH_USEDMIN            0
#define ART_DPH_USED98             1

class VTK_SLICER_DOSEPOPULATIONHISTOGRAM_MODULE_LOGIC_EXPORT vtkMRMLDosePopulationHistogramNode 
  : public vtkMRMLNode
{
public:
  static vtkMRMLDosePopulationHistogramNode *New();
  vtkTypeMacro(vtkMRMLDosePopulationHistogramNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// Create instance of a GAD node. 
  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes from name/value pairs 
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format. 
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object 
  virtual void Copy(vtkMRMLNode *node);

  /// Get unique node XML tag name (like Volume, Model) 
  virtual const char* GetNodeTagName() {return "DosePopulationHistogram";};

public:
  /// Get dose volume node ID
  vtkGetStringMacro(DoubleArrayNodeID);

  /// Set and observe dose volume node ID
  void SetAndObserveDoubleArrayNodeID(const char* ID);

  /// Get dose volume node ID
  vtkGetStringMacro(DoseVolumeNodeID);

  /// Set and observe dose volume node ID
  void SetAndObserveDoseVolumeNodeID(const char* id);

  /// Get dose volume node ID
  vtkGetStringMacro(ContourNodeID);

  /// Set and observe dose volume node ID
  void SetAndObserveContourNodeID(const char* id);

  /// Get chart node ID
  vtkGetStringMacro(ChartNodeID);

  /// Set and observe chart node ID
  void SetAndObserveChartNodeID(const char* ID);

  /// Get chart node ID
  vtkGetStringMacro(OutputDoubleArrayNodeID);

  /// Set and observe chart node ID
  void SetAndObserveOutputDoubleArrayNodeID(const char* ID);

  /// Description:
  /// Set/Get the Operation to perform.
  vtkSetMacro(UseDoseOption,int);
  vtkGetMacro(UseDoseOption,int);
  void SetUseDoseOptionToDmin() {this->SetUseDoseOption(ART_DPH_USEDMIN);};
  void SetUseDoseOptionToD98() {this->SetUseDoseOption(ART_DPH_USED98);};

  /// Get list of all the DPH double array node IDs in the scene
  std::vector<std::string>* GetDPHDoubleArrayNodeIDs()
  {
    return &this->DPHDoubleArrayNodeIDs;
  }

  /// Get/Set Show/Hide all checkbox state
  vtkGetMacro(ShowHideAll, int);
  vtkSetMacro(ShowHideAll, int);

  /// Get show in chart check states
  std::vector<bool>* GetShowInChartCheckStates()
  {
    return &this->ShowInChartCheckStates;
  }

  /// Update the stored reference to another node in the scene 
  virtual void UpdateReferenceID(const char *oldID, const char *newID);

protected:
  /// Set dose volume node ID
  vtkSetStringMacro(DoubleArrayNodeID);

  /// Set dose volume node ID
  vtkSetStringMacro(DoseVolumeNodeID);

  /// Set dose volume node ID
  vtkSetStringMacro(ContourNodeID);

  /// Set chart node ID
  vtkSetStringMacro(ChartNodeID);

  /// Set output double array node ID
  vtkSetStringMacro(OutputDoubleArrayNodeID);

protected:
  vtkMRMLDosePopulationHistogramNode();
  ~vtkMRMLDosePopulationHistogramNode();
  vtkMRMLDosePopulationHistogramNode(const vtkMRMLDosePopulationHistogramNode&);
  void operator=(const vtkMRMLDosePopulationHistogramNode&);

protected:
  /// Selected MarginCalDoubleArray MRML node object ID
  char* DoubleArrayNodeID;

  /// Selected dose volume MRML node object ID
  char* DoseVolumeNodeID;

  /// Selected dose volume MRML node object ID
  char* ContourNodeID;

  /// Selected chart MRML node object ID
  char* ChartNodeID;

  /// Selected output double array MRML node object ID
  char* OutputDoubleArrayNodeID;

  /// State of Show isodose lines checkbox
  int UseDoseOption;

  /// List of all the DPH double array MRML node IDs that are present in the scene
  std::vector<std::string> DPHDoubleArrayNodeIDs;

  /// State of Show/Hide all checkbox
  int ShowHideAll;

  /// Vector of checkbox states for the case the user makes the show/hide all checkbox state
  /// partially checked. Then the last configuration is restored
  std::vector<bool> ShowInChartCheckStates;
};

#endif
