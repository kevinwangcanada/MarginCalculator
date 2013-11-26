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
  University Health Network and was supported by 
  Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

#ifndef __qSlicerMotionSimulatorModuleWidget_h
#define __qSlicerMotionSimulatorModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerMotionSimulatorModuleExport.h"

class qSlicerMotionSimulatorModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_MOTIONSIMULATOR_EXPORT qSlicerMotionSimulatorModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerMotionSimulatorModuleWidget(QWidget *parent=0);
  virtual ~qSlicerMotionSimulatorModuleWidget();

  virtual void enter();

public slots:
  /// Set the current MRML scene to the widget
  virtual void setMRMLScene(vtkMRMLScene*);

  /// Process loaded scene
  void onSceneImportedEvent();

  /// Update widget GUI from parameter node
  void updateWidgetFromMRML();

  /// Set current parameter node
  void setMotionSimulatorNode(vtkMRMLNode *node);

protected slots:

  void doseVolumeNodeChanged(vtkMRMLNode* node);

  void structureSetNodeChanged(vtkMRMLNode* node);

  void outputArrayNodeChanged(vtkMRMLNode* node);

  void lineEditXSysSDChanged(const QString & text);

  void lineEditYSysSDChanged(const QString & text);

  void lineEditZSysSDChanged(const QString & text);

  void lineEditXRdmSDChanged(const QString & text);

  void lineEditYRdmSDChanged(const QString & text);

  void lineEditZRdmSDChanged(const QString & text);

  void lineEditNumberSimulationChanged(const QString & text);
  
  void lineEditNumberOfFractionsChanged(const QString & text);

  ///
  void onLogicModified();

  void runSimulationClicked();

protected:
  QScopedPointer<qSlicerMotionSimulatorModuleWidgetPrivate> d_ptr;
  
  virtual void setup();
  
  void updateButtonsState();

  // Generates a new isodose level name
  void onEnter();

private:
  Q_DECLARE_PRIVATE(qSlicerMotionSimulatorModuleWidget);
  Q_DISABLE_COPY(qSlicerMotionSimulatorModuleWidget);
};

#endif
