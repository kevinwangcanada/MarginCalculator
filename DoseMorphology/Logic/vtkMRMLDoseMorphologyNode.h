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

#ifndef __vtkMRMLDoseMorphologyNode_h
#define __vtkMRMLDoseMorphologyNode_h

// MRML includes
#include <vtkMRML.h>
#include <vtkMRMLNode.h>

// STD includes
#include <vector>

#include "vtkSlicerDoseMorphologyModuleLogicExport.h"

// Operation options.
#define SLICERRT_EXPAND_BY_SCALING            0
#define SLICERRT_EXPAND_BY_DILATION           1

class vtkMRMLScalarVolumeNode;

class VTK_SLICER_DOSEMORPHOLOGY_MODULE_LOGIC_EXPORT vtkMRMLDoseMorphologyNode : public vtkMRMLNode
{
public:
  static vtkMRMLDoseMorphologyNode *New();
  vtkTypeMacro(vtkMRMLDoseMorphologyNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  static std::string ReferenceDoseVolumeReferenceRole;
  static std::string InputDoseVolumeReferenceRole;
  static std::string OutputDoseVolumeReferenceRole;

  /// Create instance of a GAD node. 
  virtual vtkMRMLNode* CreateNodeInstance();

  /// Set node attributes from name/value pairs 
  virtual void ReadXMLAttributes( const char** atts);

  /// Write this node's information to a MRML file in XML format. 
  virtual void WriteXML(ostream& of, int indent);

  /// Copy the node's attributes to this object 
  virtual void Copy(vtkMRMLNode *node);

  /// Get unique node XML tag name (like Volume, Model) 
  virtual const char* GetNodeTagName() {return "DoseMorphology";};

public:
  /// Get input dose volume node 
  vtkMRMLScalarVolumeNode* GetInputDoseVolumeNode();

  /// Set and observe input dose volume node
  void SetAndObserveInputDoseVolumeNode(vtkMRMLScalarVolumeNode* node);

  /// Get input contour labelmap node
  vtkMRMLScalarVolumeNode* GetReferenceDoseVolumeNode();

  /// Set and observe input contour labelmap node 
  void SetAndObserveReferenceDoseVolumeNode(vtkMRMLScalarVolumeNode* node);

  /// Get output double array node
  vtkMRMLScalarVolumeNode* GetOutputDoseVolumeNode();

  /// Set and observe output double array node
  void SetAndObserveOutputDoseVolumeNode(vtkMRMLScalarVolumeNode* node);

  // Description:
  // Set/Get the Operation to perform.
  vtkSetMacro(Operation,int);
  vtkGetMacro(Operation,int);
  void SetOperationToExpandByScaling() {this->SetOperation(SLICERRT_EXPAND_BY_SCALING);};
  void SetOperationToExpandByDilation() {this->SetOperation(SLICERRT_EXPAND_BY_DILATION);};

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(XSize, double);
  vtkSetMacro(XSize, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(YSize, double);
  vtkSetMacro(YSize, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(ZSize, double);
  vtkSetMacro(ZSize, double);

protected:
  vtkMRMLDoseMorphologyNode();
  ~vtkMRMLDoseMorphologyNode();
  vtkMRMLDoseMorphologyNode(const vtkMRMLDoseMorphologyNode&);
  void operator=(const vtkMRMLDoseMorphologyNode&);

protected:
  /// State of Show isodose lines checkbox
  int Operation;

  /// State of Show isodose surface checkbox
  double XSize;

  /// State of Show scalarbar checkbox
  double YSize;

  /// State of Show scalarbar checkbox
  double ZSize;
};

#endif
