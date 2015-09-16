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

==============================================================================*/

// Qt includes
#include <QtPlugin>

// Slice includes
#include <qSlicerCoreApplication.h>
#include <qSlicerCoreIOManager.h>
#include <qSlicerNodeWriter.h>

// DoubleArrays Logic includes
#include <vtkSlicerMotionSimulatorDoubleArrayLogic.h>

// DoubleArrays includes
#include "qSlicerMotionSimulatorDoubleArrayIO.h"
#include "qSlicerMotionSimulatorDoubleArrayModule.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerMotionSimulatorDoubleArrayModule, qSlicerMotionSimulatorDoubleArrayModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMotionSimulatorDoubleArrayModulePrivate
{
public:
  qSlicerMotionSimulatorDoubleArrayModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerMotionSimulatorDoubleArrayModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorDoubleArrayModulePrivate::qSlicerMotionSimulatorDoubleArrayModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerMotionSimulatorDoubleArrayModule methods

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorDoubleArrayModule::qSlicerMotionSimulatorDoubleArrayModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMotionSimulatorDoubleArrayModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorDoubleArrayModule::~qSlicerMotionSimulatorDoubleArrayModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerMotionSimulatorDoubleArrayModule::helpText()const
{
  return "This module provides support for double array nodes";
}

//-----------------------------------------------------------------------------
QString qSlicerMotionSimulatorDoubleArrayModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorDoubleArrayModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Kevin Wang (RMP, PMH)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorDoubleArrayModule::categories() const
{
  return QStringList() << "Margin Calculator";
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorDoubleArrayModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorDoubleArrayModule::setup()
{
  this->Superclass::setup();
  vtkSlicerMotionSimulatorDoubleArrayLogic* doubleArraysLogic =
    vtkSlicerMotionSimulatorDoubleArrayLogic::SafeDownCast(this->logic());

  qSlicerCoreIOManager* ioManager =
    qSlicerCoreApplication::application()->coreIOManager();
  ioManager->registerIO(new qSlicerMotionSimulatorDoubleArrayIO(doubleArraysLogic,this));
  ioManager->registerIO(new qSlicerNodeWriter(
    "Double Array List", QString("MotionSimulatorDoubleArrayFile"),
    QStringList() << "vtkMRMLMotionSimulatorDoubleArrayNode", true, this));
}

//-----------------------------------------------------------------------------
bool qSlicerMotionSimulatorDoubleArrayModule::isHidden() const
{
  return true;
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerMotionSimulatorDoubleArrayModule::createWidgetRepresentation()
{
  return 0;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerMotionSimulatorDoubleArrayModule::createLogic()
{
  return vtkSlicerMotionSimulatorDoubleArrayLogic::New();
}
