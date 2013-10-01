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

// .NAME vtkSlicerDosePopulationHistogramModuleLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerDosePopulationHistogramModuleLogic_h
#define __vtkSlicerDosePopulationHistogramModuleLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// VTK includes
#include "vtkImageAccumulate.h"

// STD includes
#include <cstdlib>
#include <set>

#include "vtkSlicerDosePopulationHistogramModuleLogicExport.h"

class vtkImageData;
class vtkImageStencilData;
class vtkMRMLDoubleArrayNode;
class vtkMRMLScalarVolumeNode;
class vtkMRMLContourNode;
class vtkMRMLModelNode;
class vtkMRMLChartViewNode;
class vtkMRMLDosePopulationHistogramNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
///
/// \brief The DosePopulationHistogram module computes dose volume histogram (DVH) and metrics from a dose map and structure set(s).
///
/// The dimensions of the 3D elements (voxels) describing delineated structures are derived from the selected dose distribution volume,
/// in which the voxels have width in the transverse imaging plane as described in the DICOM image header. The image set volume is
/// defined by a grid of voxels derived from the voxel grid in the dose volume. The dose grid is oversampled by a factor currently
/// fixed to the value 2. The centre of each voxel is examined and if found to lie within a structure, is included in the volume for
/// that structure. The dose value at the centre of the cube is interpolated in 3D from the dose grid.
class VTK_SLICER_DOSEPOPULATIONHISTOGRAM_MODULE_LOGIC_EXPORT vtkSlicerDosePopulationHistogramModuleLogic :
  public vtkSlicerModuleLogic
{
public:
  static vtkSlicerDosePopulationHistogramModuleLogic *New();
  vtkTypeMacro(vtkSlicerDosePopulationHistogramModuleLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

public:
  /// Compute DVH for the selected structure set based on the selected dose volume
  void ComputeDPH();

  /// Add dose volume histogram of a structure (ROI) to the selected chart given its plot name (including table row number) and the corresponding DVH double array node ID
  void AddDPHToSelectedChart(const char* structurePlotName, const char* dvhArrayNodeId);

  /// Remove dose volume histogram of a structure from the selected chart
  void RemoveDPHFromSelectedChart(const char* dvhArrayNodeId);

  /// Refreshes DVH double array MRML node vector from the scene
  void RefreshDPHDoubleArrayNodesFromScene();

public:
  void SetAndObserveDosePopulationHistogramNode(vtkMRMLDosePopulationHistogramNode* node);
  vtkGetObjectMacro(DosePopulationHistogramNode, vtkMRMLDosePopulationHistogramNode);

  vtkGetMacro(StartValue, double);
  vtkSetMacro(StartValue, double);

  vtkGetMacro(StepSize, double);
  vtkSetMacro(StepSize, double);

protected:
  vtkSlicerDosePopulationHistogramModuleLogic();
  virtual ~vtkSlicerDosePopulationHistogramModuleLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);

  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();

  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
  virtual void OnMRMLSceneEndImport();
  virtual void OnMRMLSceneEndClose();

  ///
  void GetStencilForContour( vtkMRMLScalarVolumeNode* volumeNode,
                             vtkMRMLContourNode* structureContourNode, 
                             vtkImageData* resampledDoseVolume, 
                             vtkImageStencilData* structureStencil );

  /// Return the chart view node object from the layout
  vtkMRMLChartViewNode* GetChartViewNode();

protected:
  /// Parameter set MRML node
  vtkMRMLDosePopulationHistogramNode* DosePopulationHistogramNode;

  /// Start value for the dose axis of the DVH table
  double StartValue;

  /// Step size for the dose axis of the DVH table
  double StepSize;

private:
  vtkSlicerDosePopulationHistogramModuleLogic(const vtkSlicerDosePopulationHistogramModuleLogic&); // Not implemented
  void operator=(const vtkSlicerDosePopulationHistogramModuleLogic&);               // Not implemented
};

#endif
