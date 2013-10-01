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

  This file was originally developed by Kevin Wang, MarginCalculator Medicine Program, 
  University Health Network and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in MarginCalculator Oncology (OCAIRO).

==============================================================================*/

#ifndef __qSlicerDosePopulationHistogramModuleWidget_h
#define __qSlicerDosePopulationHistogramModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerDosePopulationHistogramModuleExport.h"

class qSlicerDosePopulationHistogramModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_DOSEPOPULATIONHISTOGRAM_EXPORT qSlicerDosePopulationHistogramModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerDosePopulationHistogramModuleWidget(QWidget *parent=0);
  virtual ~qSlicerDosePopulationHistogramModuleWidget();

  virtual void enter();

public slots:
  /// Set the current MRML scene to the widget
  virtual void setMRMLScene(vtkMRMLScene*);

  /// Process loaded scene
  void onSceneImportedEvent();

  /// Set current parameter node
  void setDosePopulationHistogramNode(vtkMRMLNode *node);

  /// Update widget GUI from parameter node
  void updateWidgetFromMRML();

protected slots:
  void doubleArrayNodeChanged(vtkMRMLNode*);
  void doseVolumeNodeChanged(vtkMRMLNode*);
  void contourNodeChanged(vtkMRMLNode*);
  void chartNodeChanged(vtkMRMLNode*);
  void radioButtonUseDminClicked();
  void radioButtonUseD98Clicked();

  void computeDPH();
  void showInChartCheckStateChanged(int aState);
  void showHideAllCheckedStateChanged(int aState);

  void onLogicModified();

protected:
  QScopedPointer<qSlicerDosePopulationHistogramModuleWidgetPrivate> d_ptr;
  
  virtual void setup();
  void onEnter();

protected:
  /// Updates button states
  void updateButtonsState();

  /// Updates state of show/hide chart checkboxes according to the currently selected chart
  void updateChartCheckboxesState();

  /// Refresh DPH statistics table
  void refreshDPHTable();

private:
  Q_DECLARE_PRIVATE(qSlicerDosePopulationHistogramModuleWidget);
  Q_DISABLE_COPY(qSlicerDosePopulationHistogramModuleWidget);
};

#endif
