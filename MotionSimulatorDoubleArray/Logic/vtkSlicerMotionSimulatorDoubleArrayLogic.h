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

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __vtkSlicerMotionSimulatorDoubleArrayLogic_h
#define __vtkSlicerMotionSimulatorDoubleArrayLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
class vtkMRMLMotionSimulatorDoubleArrayNode;

// DoubleArrays includes
#include "vtkSlicerMotionSimulatorDoubleArrayModuleLogicExport.h"

/// \ingroup Slicer_QtModules_ExtensionTemplate
/// \brief Slicer logic class for double array manipulation
/// This class manages the logic associated with reading, saving,
/// and changing propertied of the double array nodes
class VTK_SLICER_MOTIONSIMULATORDOUBLEARRAY_MODULE_LOGIC_EXPORT vtkSlicerMotionSimulatorDoubleArrayLogic
  : public vtkSlicerModuleLogic
{
public:

  static vtkSlicerMotionSimulatorDoubleArrayLogic *New();
  vtkTypeMacro(vtkSlicerMotionSimulatorDoubleArrayLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();

  vtkMRMLMotionSimulatorDoubleArrayNode* AddDoubleArray(const char* fileName,
                                         const char* name = 0);
protected:
  vtkSlicerMotionSimulatorDoubleArrayLogic();
  virtual ~vtkSlicerMotionSimulatorDoubleArrayLogic();

private:
  vtkSlicerMotionSimulatorDoubleArrayLogic(const vtkSlicerMotionSimulatorDoubleArrayLogic&); // Not implemented
  void operator=(const vtkSlicerMotionSimulatorDoubleArrayLogic&);               // Not implemented
};

#endif
