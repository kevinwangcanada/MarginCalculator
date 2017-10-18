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

// ExtensionTemplate Logic includes
#include <vtkSlicerDoseMorphologyModuleLogic.h>

// ExtensionTemplate includes
#include "qSlicerDoseMorphologyModule.h"
#include "qSlicerDoseMorphologyModuleWidget.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerDoseMorphologyModule, qSlicerDoseMorphologyModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDoseMorphologyModulePrivate
{
public:
  qSlicerDoseMorphologyModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerDoseMorphologyModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerDoseMorphologyModulePrivate::qSlicerDoseMorphologyModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerDoseMorphologyModule methods

//-----------------------------------------------------------------------------
qSlicerDoseMorphologyModule::qSlicerDoseMorphologyModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerDoseMorphologyModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerDoseMorphologyModule::~qSlicerDoseMorphologyModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerDoseMorphologyModule::helpText()const
{
  QString help = 
    "The dose morphology module performs simualtion study of patient motion. "
    "For more information see <a href=\"%1/Documentation/%2.%3/Modules/DoseMorphology\">%1/Documentation/%2.%3/Modules/DoseMorphology</a><br>";
  return help.arg(this->slicerWikiUrl()).arg(Slicer_VERSION_MAJOR).arg(Slicer_VERSION_MINOR);
}

//-----------------------------------------------------------------------------
QString qSlicerDoseMorphologyModule::acknowledgementText()const
{
  return "This work is part of SparKit project, funded by Cancer Care Ontario (CCO)'s ACRU program and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).";
}

//-----------------------------------------------------------------------------
QStringList qSlicerDoseMorphologyModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kevin Wang (PMH, Toronto)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerDoseMorphologyModule::icon()const
{
  return QIcon(":/Icons/DoseMorphology.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerDoseMorphologyModule::categories() const
{
  return QStringList() << "Margin Calculator";
}

//-----------------------------------------------------------------------------
QStringList qSlicerDoseMorphologyModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerDoseMorphologyModule::createWidgetRepresentation()
{
  return new qSlicerDoseMorphologyModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerDoseMorphologyModule::createLogic()
{
  return vtkSlicerDoseMorphologyModuleLogic::New();
}
