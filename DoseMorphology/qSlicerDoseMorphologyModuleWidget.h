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

#ifndef __qSlicerDoseMorphologyModuleWidget_h
#define __qSlicerDoseMorphologyModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerDoseMorphologyModuleExport.h"

class qSlicerDoseMorphologyModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_DOSEMORPHOLOGY_EXPORT qSlicerDoseMorphologyModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerDoseMorphologyModuleWidget(QWidget *parent=0);
  virtual ~qSlicerDoseMorphologyModuleWidget();

  virtual void enter();

public slots:
  /// Set the current MRML scene to the widget
  virtual void setMRMLScene(vtkMRMLScene*);

  /// Process loaded scene
  void onSceneImportedEvent();

  /// Set current parameter node
  void setDoseMorphologyNode(vtkMRMLNode*);

  /// Update widget GUI from parameter node
  void updateWidgetFromMRML();

protected slots:

  ///
  void currentDoseVolumeNodeChanged(vtkMRMLNode* node);

  ///
  void secondaryDoseVolumeNodeChanged(vtkMRMLNode* node);

  ///
  void outputDoseVolumeNodeChanged(vtkMRMLNode* node);

  ///
  void radioButtonExpandByScalingClicked();

  ///
  void radioButtonExpandByDilationClicked();

  ///
  void lineEditXSizeChanged(const QString & text);

  ///
  void lineEditYSizeChanged(const QString & text);

  ///
  void lineEditZSizeChanged(const QString & text);

  ///
  void applyClicked();

  ///
  void onLogicModified();

protected:
  QScopedPointer<qSlicerDoseMorphologyModuleWidgetPrivate> d_ptr;
  
  ///
  virtual void setup();

  // Generates a new isodose level name
  void onEnter();

  ///
  void updateButtonsState();

private:
  Q_DECLARE_PRIVATE(qSlicerDoseMorphologyModuleWidget);
  Q_DISABLE_COPY(qSlicerDoseMorphologyModuleWidget);
};

#endif
