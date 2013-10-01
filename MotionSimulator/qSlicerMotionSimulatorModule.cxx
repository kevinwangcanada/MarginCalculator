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
#include <vtkSlicerMotionSimulatorModuleLogic.h>

// ExtensionTemplate includes
#include "qSlicerMotionSimulatorModule.h"
#include "qSlicerMotionSimulatorModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerMotionSimulatorModule, qSlicerMotionSimulatorModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMotionSimulatorModulePrivate
{
public:
  qSlicerMotionSimulatorModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMotionSimulatorModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorModulePrivate::qSlicerMotionSimulatorModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerMotionSimulatorModule methods

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorModule::qSlicerMotionSimulatorModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMotionSimulatorModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorModule::~qSlicerMotionSimulatorModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerMotionSimulatorModule::helpText()const
{
  return "This loadable modules performs simualtion study of patient motion.";
}

//-----------------------------------------------------------------------------
QString qSlicerMotionSimulatorModule::acknowledgementText()const
{
  return "This work is part of SparKit project, funded by Cancer Care Ontario (CCO)'s ACRU program and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kevin Wang (RMP, PMH)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerMotionSimulatorModule::icon()const
{
  return QIcon(":/Icons/MotionSimulator.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorModule::categories() const
{
  return QStringList() << "Margin Calculator";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerMotionSimulatorModule::createWidgetRepresentation()
{
  return new qSlicerMotionSimulatorModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerMotionSimulatorModule::createLogic()
{
  return vtkSlicerMotionSimulatorModuleLogic::New();
}
