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

// Qt includes
#include <QtPlugin>

// ExtensionTemplate Logic includes
#include <vtkSlicerMotionSimulatorModuleLogic.h>

// ExtensionTemplate includes
#include "qSlicerMotionSimulatorModule.h"
#include "qSlicerMotionSimulatorModuleWidget.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerMotionSimulatorModule, qSlicerMotionSimulatorModule);
#endif

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
  return "This loadable module calculates dose statistics with respect to patient motion using simulation ";
}

//-----------------------------------------------------------------------------
QString qSlicerMotionSimulatorModule::acknowledgementText()const
{
  return "This work is funded by Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kevin Wang (Princess Margaret Cancer Center)");
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
