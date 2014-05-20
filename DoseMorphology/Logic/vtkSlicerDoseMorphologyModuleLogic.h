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
  and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

// .NAME vtkSlicerDoseMorphologyModuleLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerDoseMorphologyModuleLogic_h
#define __vtkSlicerDoseMorphologyModuleLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerDoseMorphologyModuleLogicExport.h"

class vtkMRMLScalarVolumeNode;
class vtkMRMLDoseMorphologyNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_DOSEMORPHOLOGY_MODULE_LOGIC_EXPORT vtkSlicerDoseMorphologyModuleLogic :
  public vtkSlicerModuleLogic
{
public:

  static vtkSlicerDoseMorphologyModuleLogic *New();
  vtkTypeMacro(vtkSlicerDoseMorphologyModuleLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  ///
  void SetAndObserveDoseMorphologyNode(vtkMRMLDoseMorphologyNode* node);

  ///
  vtkGetObjectMacro(DoseMorphologyNode, vtkMRMLDoseMorphologyNode);

  ///
  bool VolumeContainsDose();

  ///
  void GetVolumeNodeCenterOfMass(vtkMRMLScalarVolumeNode* volumeNode, int* centerOfMass);

  ///
  int MorphDose();

protected:
  vtkSlicerDoseMorphologyModuleLogic();
  virtual ~vtkSlicerDoseMorphologyModuleLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
  virtual void OnMRMLSceneEndImport();
  virtual void OnMRMLSceneEndClose();

  /// Parameter set MRML node
  vtkMRMLDoseMorphologyNode* DoseMorphologyNode;

private:

  vtkSlicerDoseMorphologyModuleLogic(const vtkSlicerDoseMorphologyModuleLogic&); // Not implemented
  void operator=(const vtkSlicerDoseMorphologyModuleLogic&);               // Not implemented
};

#endif
