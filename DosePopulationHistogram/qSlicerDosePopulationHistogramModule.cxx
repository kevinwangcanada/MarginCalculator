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

// Qt includes
#include <QtPlugin>
//#include <QDebug>

// Slicer includes
#include <qSlicerCoreApplication.h>
#include <qSlicerModuleManager.h>

// ExtensionTemplate Logic includes
#include <vtkSlicerDosePopulationHistogramModuleLogic.h>

// ExtensionTemplate includes
#include "qSlicerDosePopulationHistogramModule.h"
#include "qSlicerDosePopulationHistogramModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerDosePopulationHistogramModule, qSlicerDosePopulationHistogramModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDosePopulationHistogramModulePrivate
{
public:
  qSlicerDosePopulationHistogramModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerDosePopulationHistogramModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerDosePopulationHistogramModulePrivate::qSlicerDosePopulationHistogramModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerDosePopulationHistogramModule methods

//-----------------------------------------------------------------------------
qSlicerDosePopulationHistogramModule::qSlicerDosePopulationHistogramModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerDosePopulationHistogramModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerDosePopulationHistogramModule::~qSlicerDosePopulationHistogramModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerDosePopulationHistogramModule::helpText()const
{
  QString help = 
    "This module computes dose population histogram (DPH) from a margin calculator double array node. "
    "For more information see <a href=\"%1/Documentation/%2.%3/Modules/DosePopulationHistogram\">%1/Documentation/%2.%3/Modules/DosePopulationHistogram</a><br>";
  return help.arg(this->slicerWikiUrl()).arg(Slicer_VERSION_MAJOR).arg(Slicer_VERSION_MINOR);
}

//-----------------------------------------------------------------------------
QString qSlicerDosePopulationHistogramModule::acknowledgementText()const
{
  return "This work is part of SparKit project, funded by Cancer Care Ontario (CCO)'s ACRU program and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).";
}

//-----------------------------------------------------------------------------
QIcon qSlicerDosePopulationHistogramModule::icon()const
{
  return QIcon(":/Icons/DosePopulationHistogram.png");
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModule::setup()
{
  this->Superclass::setup();

  //qSlicerAbstractCoreModule* modelToLabelmapModule =
  //  qSlicerCoreApplication::application()->moduleManager()->module("ModelToLabelMap");
  //if (modelToLabelmapModule)
  //  {
  //  vtkSlicerModuleLogic* modelToLabelmapModuleLogic = 
  //    vtkSlicerModuleLogic::SafeDownCast(modelToLabelmapModule->logic());
  //  }
  //else
  //  {
  //  qWarning() << "ModelToLabelMap module is not found";
  //  }
}

//-----------------------------------------------------------------------------
QStringList qSlicerDosePopulationHistogramModule::categories() const
{
  return QStringList() << "Margin Calculator";
}

//-----------------------------------------------------------------------------
QStringList qSlicerDosePopulationHistogramModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kevin Wang (PMH, Toronto)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerDosePopulationHistogramModule::createWidgetRepresentation()
{
  return new qSlicerDosePopulationHistogramModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerDosePopulationHistogramModule::createLogic()
{
  return vtkSlicerDosePopulationHistogramModuleLogic::New();
}
