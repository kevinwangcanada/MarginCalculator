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

#ifndef __vtkMRMLMotionSimulatorNode_h
#define __vtkMRMLMotionSimulatorNode_h

// MRML includes
#include <vtkMRML.h>
#include <vtkMRMLNode.h>

// STD includes
#include <vector>

#include "vtkSlicerMotionSimulatorModuleLogicExport.h"

class VTK_SLICER_MOTIONSIMULATOR_MODULE_LOGIC_EXPORT vtkMRMLMotionSimulatorNode : public vtkMRMLNode
{
public:
  static vtkMRMLMotionSimulatorNode *New();
  vtkTypeMacro(vtkMRMLMotionSimulatorNode, vtkMRMLNode);
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
  virtual const char* GetNodeTagName() {return "MotionSimulator";};

public:
  /// Get dose volume node ID
  vtkGetStringMacro(InputDoseVolumeNodeID);

  /// Set and observe dose volume node ID
  void SetAndObserveInputDoseVolumeNodeID(const char* id);

  /// Get dose volume node ID
  vtkGetStringMacro(InputContourNodeID);

  /// Set and observe dose volume node ID
  void SetAndObserveInputContourNodeID(const char* id);

  /// Get output hierarchy node ID
  vtkGetStringMacro(OutputDoubleArrayNodeID);

  /// Set and observe output hierarchy node ID
  void SetAndObserveOutputDoubleArrayNodeID(const char* id);

  /// Update the stored reference to another node in the scene 
  virtual void UpdateReferenceID(const char *oldID, const char *newID);
 
  // Description:

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(XSysSD, double);
  vtkSetMacro(XSysSD, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(YSysSD, double);
  vtkSetMacro(YSysSD, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(ZSysSD, double);
  vtkSetMacro(ZSysSD, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(XRdmSD, double);
  vtkSetMacro(XRdmSD, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(YRdmSD, double);
  vtkSetMacro(YRdmSD, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(ZRdmSD, double);
  vtkSetMacro(ZRdmSD, double);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(NumberOfSimulation, int);
  vtkSetMacro(NumberOfSimulation, int);

  /// Get/Set Save labelmaps checkbox state
  vtkGetMacro(NumberOfFraction, int);
  vtkSetMacro(NumberOfFraction, int);

protected:
  /// Set dose volume node ID
  vtkSetStringMacro(InputDoseVolumeNodeID);

  /// Set dose volume node ID
  vtkSetStringMacro(InputContourNodeID);

  /// Set output hierarchy node ID
  vtkSetStringMacro(OutputDoubleArrayNodeID);

protected:
  vtkMRMLMotionSimulatorNode();
  ~vtkMRMLMotionSimulatorNode();
  vtkMRMLMotionSimulatorNode(const vtkMRMLMotionSimulatorNode&);
  void operator=(const vtkMRMLMotionSimulatorNode&);

protected:
  /// Selected dose volume MRML node object ID
  char* InputDoseVolumeNodeID;

  /// Selected dose volume MRML node object ID
  char* InputContourNodeID;

  /// Selected dose volume MRML node object ID
  char* OutputDoubleArrayNodeID;

  /// State of Show scalarbar checkbox
  double XSysSD;
  double YSysSD;
  double ZSysSD;

  /// 
  double XRdmSD;
  double YRdmSD;
  double ZRdmSD;

  ///
  int    NumberOfSimulation;

  ///
  int    NumberOfFraction;
};

#endif
