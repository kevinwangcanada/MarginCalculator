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

class vtkMRMLDoubleArrayNode;
class vtkMRMLChartNode;
class vtkMRMLScalarVolumeNode;
class vtkMRMLMotionSimulatorDoubleArrayNode;

class VTK_SLICER_DOSEPOPULATIONHISTOGRAM_MODULE_LOGIC_EXPORT vtkMRMLDosePopulationHistogramNode 
  : public vtkMRMLNode
{
public:
  static vtkMRMLDosePopulationHistogramNode *New();
  vtkTypeMacro(vtkMRMLDosePopulationHistogramNode,vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  static std::string DoubleArrayReferenceRole;
  static std::string DoseVolumeReferenceRole;
  static std::string ContourReferenceRole;
  static std::string ChartReferenceRole;
  static std::string OutputDoubleArrayReferenceRole;

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
  /// Get output double array node
  vtkMRMLMotionSimulatorDoubleArrayNode* GetDoubleArrayNode();

  /// Set and observe output double array node
  void SetAndObserveDoubleArrayNode(vtkMRMLMotionSimulatorDoubleArrayNode* node);

  /// Get input dose volume node 
  vtkMRMLScalarVolumeNode* GetDoseVolumeNode();

  /// Set and observe input dose volume node
  void SetAndObserveDoseVolumeNode(vtkMRMLScalarVolumeNode* node);

  /// Get input contour labelmap node
  vtkMRMLScalarVolumeNode* GetContourNode();

  /// Set and observe input contour labelmap node 
  void SetAndObserveContourNode(vtkMRMLScalarVolumeNode* node);

  /// Get output double array node
  vtkMRMLChartNode* GetChartNode();

  /// Set and observe output double array node
  void SetAndObserveChartNode(vtkMRMLChartNode* node);

  /// Get output double array node
  vtkMRMLDoubleArrayNode* GetOutputDoubleArrayNode();

  /// Set and observe output double array node
  void SetAndObserveOutputDoubleArrayNode(vtkMRMLDoubleArrayNode* node);

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

protected:
  vtkMRMLDosePopulationHistogramNode();
  ~vtkMRMLDosePopulationHistogramNode();
  vtkMRMLDosePopulationHistogramNode(const vtkMRMLDosePopulationHistogramNode&);
  void operator=(const vtkMRMLDosePopulationHistogramNode&);

protected:
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
