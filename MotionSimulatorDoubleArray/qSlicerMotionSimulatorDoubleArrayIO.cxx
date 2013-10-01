/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// Qt includes
#include <QFileInfo>

// SlicerQt includes
#include "qSlicerMotionSimulatorDoubleArrayIO.h"

// Logic includes
#include "vtkSlicerMotionSimulatorDoubleArrayLogic.h"

// MRML includes
#include <vtkMRMLMotionSimulatorDoubleArrayNode.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerMotionSimulatorDoubleArrayIOPrivate
{
public:
  vtkSmartPointer<vtkSlicerMotionSimulatorDoubleArrayLogic> Logic;
};

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorDoubleArrayIO::qSlicerMotionSimulatorDoubleArrayIO(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMotionSimulatorDoubleArrayIOPrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorDoubleArrayIO
::qSlicerMotionSimulatorDoubleArrayIO(vtkSlicerMotionSimulatorDoubleArrayLogic* logic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerMotionSimulatorDoubleArrayIOPrivate)
{
  this->setLogic(logic);
}

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorDoubleArrayIO::~qSlicerMotionSimulatorDoubleArrayIO()
{
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorDoubleArrayIO::setLogic(vtkSlicerMotionSimulatorDoubleArrayLogic* logic)
{
  Q_D(qSlicerMotionSimulatorDoubleArrayIO);
  d->Logic = logic;
}

//-----------------------------------------------------------------------------
vtkSlicerMotionSimulatorDoubleArrayLogic* qSlicerMotionSimulatorDoubleArrayIO::logic()const
{
  Q_D(const qSlicerMotionSimulatorDoubleArrayIO);
  return d->Logic.GetPointer();
}

//-----------------------------------------------------------------------------
QString qSlicerMotionSimulatorDoubleArrayIO::description()const
{
  return "Double Array List";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerMotionSimulatorDoubleArrayIO::fileType()const
{
  return QString("MotionSimulatorDoubleArrayFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerMotionSimulatorDoubleArrayIO::extensions()const
{
  return QStringList()
    << "Double Array (*.csv)"
    ;
}

//-----------------------------------------------------------------------------
bool qSlicerMotionSimulatorDoubleArrayIO::load(const IOProperties& properties)
{
  Q_D(qSlicerMotionSimulatorDoubleArrayIO);
  Q_ASSERT(properties.contains("fileName"));
  QString fileName = properties["fileName"].toString();

  QString name = QFileInfo(fileName).baseName();
  if (properties.contains("name"))
    {
    name = properties["name"].toString();
    }
  Q_ASSERT(d->Logic);
  vtkMRMLMotionSimulatorDoubleArrayNode* node = d->Logic->AddDoubleArray(
    fileName.toLatin1(),
    name.toLatin1());
  if (node)
    {
    this->setLoadedNodes(QStringList(QString(node->GetID())));
    }
  else
    {
    this->setLoadedNodes(QStringList());
    }
  return node != 0;
}
