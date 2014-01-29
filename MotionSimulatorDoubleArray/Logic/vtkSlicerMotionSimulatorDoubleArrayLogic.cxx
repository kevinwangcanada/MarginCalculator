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

// DoubleArrays Logic includes
#include "vtkSlicerMotionSimulatorDoubleArrayLogic.h"

// MRML includes
#include <vtkMRMLMotionSimulatorDoubleArrayNode.h>
#include <vtkMRMLMotionSimulatorDoubleArrayStorageNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerMotionSimulatorDoubleArrayLogic);

//----------------------------------------------------------------------------
vtkSlicerMotionSimulatorDoubleArrayLogic::vtkSlicerMotionSimulatorDoubleArrayLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerMotionSimulatorDoubleArrayLogic::~vtkSlicerMotionSimulatorDoubleArrayLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerMotionSimulatorDoubleArrayLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-----------------------------------------------------------------------------
void vtkSlicerMotionSimulatorDoubleArrayLogic::RegisterNodes()
{
  vtkMRMLScene* scene = this->GetMRMLScene(); 
  if (!scene)
  {
    return;
  }
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLMotionSimulatorDoubleArrayNode>::New());
}

//----------------------------------------------------------------------------
vtkMRMLMotionSimulatorDoubleArrayNode* vtkSlicerMotionSimulatorDoubleArrayLogic
::AddDoubleArray(const char* fileName, const char* name)
{
  if (this->GetMRMLScene() == 0 || fileName == 0)
    {
    return 0;
    }

  // Storage node
  vtkNew<vtkMRMLMotionSimulatorDoubleArrayStorageNode> doubleArrayStorageNode;
  doubleArrayStorageNode->SetFileName(fileName);
  this->GetMRMLScene()->AddNode(doubleArrayStorageNode.GetPointer());

  // Storable node
  vtkNew<vtkMRMLMotionSimulatorDoubleArrayNode> doubleArrayNode;
  this->GetMRMLScene()->AddNode(doubleArrayNode.GetPointer());

  // Read
  int res = doubleArrayStorageNode->ReadData(doubleArrayNode.GetPointer());
  if (res == 0) // failed to read
    {
    this->GetMRMLScene()->RemoveNode(doubleArrayStorageNode.GetPointer());
    this->GetMRMLScene()->RemoveNode(doubleArrayNode.GetPointer());
    return 0;
    }
  if (name)
    {
    doubleArrayNode->SetName(name);
    }
  return doubleArrayNode.GetPointer();
}
