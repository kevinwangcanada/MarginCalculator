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

#ifndef __qSlicerDosePopulationHistogramModule_h
#define __qSlicerDosePopulationHistogramModule_h

// SlicerQt includes
#include "qSlicerLoadableModule.h"

#include "qSlicerDosePopulationHistogramModuleExport.h"

class qSlicerDosePopulationHistogramModulePrivate;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_DOSEPOPULATIONHISTOGRAM_EXPORT qSlicerDosePopulationHistogramModule :
  public qSlicerLoadableModule
{
  Q_OBJECT
#ifdef Slicer_HAVE_QT5
  Q_PLUGIN_METADATA(IID "org.slicer.modules.loadable.qSlicerLoadableModule/1.0");
#endif
  Q_INTERFACES(qSlicerLoadableModule);

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerDosePopulationHistogramModule(QObject *parent=0);
  virtual ~qSlicerDosePopulationHistogramModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);

  /// Help to use the module
  virtual QString helpText()const;

  /// Return acknowledgements
  virtual QString acknowledgementText()const;
  
  /// Return a custom icon for the module
  virtual QIcon icon()const;

  /// Return the category name for the module
  virtual QStringList categories() const;

  /// Return the list of contributors of the module
  virtual QStringList contributors() const;

protected:

  /// Initialize the module. Register the volumes reader/writer
  virtual void setup();

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerDosePopulationHistogramModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerDosePopulationHistogramModule);
  Q_DISABLE_COPY(qSlicerDosePopulationHistogramModule);

};

#endif
