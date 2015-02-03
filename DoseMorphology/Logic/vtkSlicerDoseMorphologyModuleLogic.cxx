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

// DoseMorpholgyModule Logic includes
#include "vtkSlicerDoseMorphologyModuleLogic.h"
#include "vtkMRMLDoseMorphologyNode.h"

// SlicerRT includes
#include "MarginCalculatorCommon.h"

// MRML includes
//#include <vtkMRMLContourNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVolumeDisplayNode.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLTransformNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkImageMarchingCubes.h>
#include <vtkImageChangeInformation.h>
#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkImageContinuousDilate3D.h>
#include <vtkImageContinuousErode3D.h>
#include <vtkImageLogic.h>
#include <vtkImageAccumulate.h>
#include <vtkImageReslice.h>
#include <vtkGeneralTransform.h>
#include <vtkTransform.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

#define THRESHOLD 0.001

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerDoseMorphologyModuleLogic);

//----------------------------------------------------------------------------
vtkSlicerDoseMorphologyModuleLogic::vtkSlicerDoseMorphologyModuleLogic()
{
  this->DoseMorphologyNode = NULL;
}

//----------------------------------------------------------------------------
vtkSlicerDoseMorphologyModuleLogic::~vtkSlicerDoseMorphologyModuleLogic()
{
  vtkSetAndObserveMRMLNodeMacro(this->DoseMorphologyNode, NULL);
}

//----------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::SetAndObserveDoseMorphologyNode(vtkMRMLDoseMorphologyNode *node)
{
  vtkSetAndObserveMRMLNodeMacro(this->DoseMorphologyNode, node);
}

//---------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndImportEvent);
  events->InsertNextValue(vtkMRMLScene::EndCloseEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::RegisterNodes()
{
  vtkMRMLScene* scene = this->GetMRMLScene(); 
  if (!scene)
  {
    return;
  }
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLDoseMorphologyNode>::New());
}

//---------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene() || !this->DoseMorphologyNode)
    {
    return;
    }

  // if the scene is still updating, jump out
  if (this->GetMRMLScene()->IsBatchProcessing())
  {
    return;
  }

  if (node->IsA("vtkMRMLScalarVolumeNode") || node->IsA("vtkMRMLDoseMorphologyNode"))
    {
    this->Modified();
    }
}

//---------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene() || !this->DoseMorphologyNode)
    {
    return;
    }

  // if the scene is still updating, jump out
  if (this->GetMRMLScene()->IsBatchProcessing())
  {
    return;
  }

  if (node->IsA("vtkMRMLScalarVolumeNode") || node->IsA("vtkMRMLDoseMorphologyNode"))
    {
    this->Modified();
    }
}

//---------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::OnMRMLSceneEndImport()
{
  // If we have a parameter node select it
  vtkMRMLDoseMorphologyNode *paramNode = NULL;
  vtkMRMLNode *node = this->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLDoseMorphologyNode");
  if (node)
    {
    paramNode = vtkMRMLDoseMorphologyNode::SafeDownCast(node);
    vtkSetAndObserveMRMLNodeMacro(this->DoseMorphologyNode, paramNode);
    }
}

//---------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::OnMRMLSceneEndClose()
{
  this->Modified();
}

//---------------------------------------------------------------------------
bool vtkSlicerDoseMorphologyModuleLogic::VolumeContainsDose()
{
  if (!this->GetMRMLScene() || !this->DoseMorphologyNode)
  {
    return false;
  }

  //vtkMRMLContourNode* contourNode = vtkMRMLContourNode::SafeDownCast(
  //  this->GetMRMLScene()->GetNodeByID(this->DoseMorphologyNode->GetContourNodeID()));

  //if (contourNode->GetActiveRepresentationType() == vtkMRMLContourNode::IndexedLabelmap)
  //{
  //  return true;
  //}

  return false;
}

//---------------------------------------------------------------------------
void vtkSlicerDoseMorphologyModuleLogic::GetVolumeNodeCenterOfMass(vtkMRMLScalarVolumeNode *volumeNode, int* centerOfMass)
{
  double sumValue = 0.0;
  double sumX = 0, sumY = 0, sumZ = 0;

  vtkSmartPointer<vtkImageData> imageData = volumeNode->GetImageData();
  int extent[6];
  int dim[3] = {0,0,0};
#if (VTK_MAJOR_VERSION <= 5)
  imageData->GetWholeExtent(extent);
#else
  imageData->GetExtent(extent);
#endif
  imageData->GetDimensions(dim);
  for (int zz = extent[4]; zz <= extent[5]; zz ++)
  {
    for (int yy = extent[2]; yy <= extent[3]; yy ++)
    {
      for (int xx = extent[0]; xx <= extent[1]; xx ++)
      {
        double voxelVal = imageData->GetScalarComponentAsDouble(xx, yy, zz, 0);
        if (voxelVal > 0)
        {
          sumValue = sumValue + voxelVal;
          sumX = sumX + xx * voxelVal;
          sumY = sumY + yy * voxelVal;
          sumZ = sumZ + zz * voxelVal;
        }
      } // for xx
    } // for yy
  } // for zz

  if (sumValue > 0)
  {
    centerOfMass[0] = sumX / sumValue;
    centerOfMass[1] = sumY / sumValue;
    centerOfMass[2] = sumZ / sumValue;
  }
  return;
}

//---------------------------------------------------------------------------
int vtkSlicerDoseMorphologyModuleLogic::MorphDose()
{
  double originX, originY, originZ;
  double spacingX, spacingY, spacingZ;
  int dimensions[3] = {0, 0, 0};
  double originX2, originY2, originZ2;
  double spacingX2, spacingY2, spacingZ2;
  int dimensions2[3] = {0, 0, 0};

  vtkSmartPointer<vtkMRMLScalarVolumeNode> referenceDoseVolumeNode = this->DoseMorphologyNode->GetReferenceDoseVolumeNode();
  vtkSmartPointer<vtkMRMLScalarVolumeNode> inputDoseVolumeNode = this->DoseMorphologyNode->GetInputDoseVolumeNode();
  vtkSmartPointer<vtkMRMLScalarVolumeNode> outputDoseVolumeNode = this->DoseMorphologyNode->GetOutputDoseVolumeNode();

  // Make sure inputs are initialized
  if (!this->GetMRMLScene() || !referenceDoseVolumeNode || !inputDoseVolumeNode || !outputDoseVolumeNode)
  {
    vtkErrorMacro("DoseMorphology: Inputs are not initialized!")
    return -1;
  }

  this->GetMRMLScene()->StartState(vtkMRMLScene::BatchProcessState); 

  int op = this->DoseMorphologyNode->GetOperation();

  referenceDoseVolumeNode->GetOrigin(originX, originY, originZ); 
  referenceDoseVolumeNode->GetSpacing(spacingX, spacingY, spacingZ);
  referenceDoseVolumeNode->GetImageData()->GetDimensions(dimensions);
  inputDoseVolumeNode->GetOrigin(originX2, originY2, originZ2); 
  inputDoseVolumeNode->GetSpacing(spacingX2, spacingY2, spacingZ2);
  inputDoseVolumeNode->GetImageData()->GetDimensions(dimensions2);

  vtkSmartPointer<vtkImageData> tempImage = NULL;
  tempImage = inputDoseVolumeNode->GetImageData();

  double dimsH[4];
  double rasCorner[4];
  double origin[3] = {0.,0.,0.};
  
  // Use image center to scale dose
  {
  int *dims = tempImage->GetDimensions();
  dimsH[0] = dims[0] - 1;
  dimsH[1] = dims[1] - 1;
  dimsH[2] = dims[2] - 1;
  dimsH[3] = 0.;
  }
  
  // Use center of mass to scale dose
  {
  int centerOfMass[3] = {0,0,0};
  this->GetVolumeNodeCenterOfMass(inputDoseVolumeNode, centerOfMass);
  dimsH[0] = centerOfMass[0];
  dimsH[1] = centerOfMass[1];
  dimsH[2] = centerOfMass[2];
  dimsH[3] = 0.;
  }

  vtkSmartPointer<vtkMatrix4x4> ijkToRAS = vtkSmartPointer<vtkMatrix4x4>::New();
  inputDoseVolumeNode->GetIJKToRASMatrix(ijkToRAS);
  ijkToRAS->MultiplyPoint(dimsH, rasCorner);

  // Use image center to scale dose
  {
  origin[0] = -0.5 * rasCorner[0];
  origin[1] = -0.5 * rasCorner[1];
  origin[2] = -0.5 * rasCorner[2];
  }

  // Use center of mass to scale dose
  {
  origin[0] = - rasCorner[0];
  origin[1] = - rasCorner[1];
  origin[2] = - rasCorner[2];
  }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> tempDoseVolumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  tempDoseVolumeNode->Copy(inputDoseVolumeNode);
  tempDoseVolumeNode->SetOrigin(origin);

  vtkSmartPointer<vtkMatrix4x4> inputIJK2RASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  tempDoseVolumeNode->GetIJKToRASMatrix(inputIJK2RASMatrix);
  vtkSmartPointer<vtkMatrix4x4> inputRAS2IJKMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  tempDoseVolumeNode->GetRASToIJKMatrix(inputRAS2IJKMatrix);

  vtkSmartPointer<vtkTransform> outputResliceTransform = vtkSmartPointer<vtkTransform>::New();
  outputResliceTransform->Identity();
  outputResliceTransform->PostMultiply();
  outputResliceTransform->SetMatrix(inputIJK2RASMatrix);

  vtkSmartPointer<vtkMRMLTransformNode> inputVolumeNodeTransformNode = tempDoseVolumeNode->GetParentTransformNode();
  vtkSmartPointer<vtkMatrix4x4> inputRAS2RASMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  if (inputVolumeNodeTransformNode!=NULL)
  {
    inputVolumeNodeTransformNode->GetMatrixTransformToWorld(inputRAS2RASMatrix);  
    outputResliceTransform->Concatenate(inputRAS2RASMatrix);
  }
  
  if (op == SLICERRT_EXPAND_BY_SCALING) 
  {
    vtkSmartPointer<vtkMatrix4x4> ScalingMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Scale(this->GetDoseMorphologyNode()->GetXSize(),
                     this->GetDoseMorphologyNode()->GetYSize(),
                     this->GetDoseMorphologyNode()->GetZSize());
    transform->GetMatrix(ScalingMatrix);
    outputResliceTransform->Concatenate(ScalingMatrix);
  }
  outputResliceTransform->Concatenate(inputRAS2IJKMatrix);
  outputResliceTransform->Inverse();

  vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
#if (VTK_MAJOR_VERSION <= 5)
  reslice->SetInput(inputDoseVolumeNode->GetImageData());
#else
  reslice->SetInputData(inputDoseVolumeNode->GetImageData());
#endif
  reslice->SetOutputOrigin(0, 0, 0);
  if (op == SLICERRT_EXPAND_BY_SCALING) 
  {
    reslice->SetOutputSpacing(1, 1, 1);
    reslice->SetOutputExtent(0, dimensions[0]-1, 0, dimensions[1]-1, 0, dimensions[2]-1);
  }
  if (op == SLICERRT_EXPAND_BY_DILATION) 
  {
    reslice->SetOutputSpacing(0.2/spacingX, 0.2/spacingY, 0.2/spacingZ);
    reslice->SetOutputExtent(0, dimensions[0]*spacingX/0.2-1, 0, dimensions[1]*spacingY/0.2-1, 0, dimensions[2]*spacingZ/0.2-1);
  }
  reslice->SetResliceTransform(outputResliceTransform);
  reslice->SetInterpolationModeToCubic();
  reslice->Update();
  tempImage = reslice->GetOutput();

  vtkSmartPointer<vtkImageData> tempImageData = NULL;
  vtkSmartPointer<vtkImageContinuousDilate3D> dilateFilter = vtkSmartPointer<vtkImageContinuousDilate3D>::New();

  //int op = this->DoseMorphologyNode->GetOperation();
  switch (op) 
  {
    case SLICERRT_EXPAND_BY_SCALING:
      //transform->Scale(this->GetDoseMorphologyNode()->GetXSize(),
      //                 this->GetDoseMorphologyNode()->GetYSize(),
      //                 this->GetDoseMorphologyNode()->GetZSize());
      //resliceFilter->SetInput(tempImage2);
      //resliceFilter->SetInformationInput(tempImage);
      //resliceFilter->SetResliceTransform(transform);
      //resliceFilter->GetOutput();
      tempImageData = tempImage;
      break;
    case SLICERRT_EXPAND_BY_DILATION:
      // temp work around for calculating the kernel size
      //double spacing[3] = {0,0,0};
      //volumeNode->GetSpacing(spacing);
      int kernelSize[3] = {1,1,1};
      kernelSize[0] = (int)(this->GetDoseMorphologyNode()->GetXSize()*2*spacingX/0.2+1);
      kernelSize[1] = (int)(this->GetDoseMorphologyNode()->GetYSize()*2*spacingY/0.2+1);
      kernelSize[2] = (int)(this->GetDoseMorphologyNode()->GetZSize()*2*spacingZ/0.2+1);
#if (VTK_MAJOR_VERSION <= 5)
      dilateFilter->SetInput(tempImage);
#else
      dilateFilter->SetInputData(tempImage);
#endif
      dilateFilter->SetKernelSize(kernelSize[0], kernelSize[1], kernelSize[2]);
      dilateFilter->Update();
      //tempImageData = dilateFilter->GetOutput();

      vtkSmartPointer<vtkImageReslice> reslice2 = vtkSmartPointer<vtkImageReslice>::New();
#if (VTK_MAJOR_VERSION <= 5)
      reslice2->SetInput(dilateFilter->GetOutput());
#else
      reslice2->SetInputData(dilateFilter->GetOutput());
#endif
      reslice2->SetOutputOrigin(0, 0, 0);
      reslice2->SetOutputSpacing(1, 1, 1);
      reslice2->SetOutputExtent(0, dimensions[0]-1, 0, dimensions[1]-1, 0, dimensions[2]-1);
      reslice2->SetInterpolationModeToCubic();
      reslice2->Update();
      tempImageData = reslice2->GetOutput();
      break;
  }

  // to do .... 
  outputDoseVolumeNode->CopyOrientation( referenceDoseVolumeNode );
  outputDoseVolumeNode->SetAttribute(MarginCalculatorCommon::DICOMRTIMPORT_DOSE_UNIT_NAME_ATTRIBUTE_NAME.c_str(), inputDoseVolumeNode->GetAttribute(MarginCalculatorCommon::DICOMRTIMPORT_DOSE_UNIT_NAME_ATTRIBUTE_NAME.c_str()));
  outputDoseVolumeNode->SetAttribute(MarginCalculatorCommon::DICOMRTIMPORT_DOSE_UNIT_VALUE_ATTRIBUTE_NAME.c_str(), inputDoseVolumeNode->GetAttribute(MarginCalculatorCommon::DICOMRTIMPORT_DOSE_UNIT_VALUE_ATTRIBUTE_NAME.c_str()));

  outputDoseVolumeNode->SetAndObserveImageData( tempImageData );
  outputDoseVolumeNode->SetAttribute(MarginCalculatorCommon::DICOMRTIMPORT_DOSE_VOLUME_IDENTIFIER_ATTRIBUTE_NAME.c_str(), "1");
  outputDoseVolumeNode->HideFromEditorsOff();

  // Create display node
  vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> outputDoseVolumeDisplayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();
  outputDoseVolumeDisplayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(this->GetMRMLScene()->AddNode(outputDoseVolumeDisplayNode));
  //outputDoseVolumeNodeName.append("Display");
  //doseVolumeDisplayNode->SetName(outputDoseVolumeNodeName.c_str());
  outputDoseVolumeNode->SetAndObserveDisplayNodeID( outputDoseVolumeDisplayNode->GetID() );

  // Set default colormap to rainbow
  outputDoseVolumeDisplayNode->SetAndObserveColorNodeID("vtkMRMLColorTableNodeRainbow");

  // Select as active volume
  if (this->GetApplicationLogic()!=NULL)
  {
    if (this->GetApplicationLogic()->GetSelectionNode()!=NULL)
    {
      this->GetApplicationLogic()->GetSelectionNode()->SetReferenceActiveVolumeID(outputDoseVolumeNode->GetID());
      this->GetApplicationLogic()->PropagateVolumeSelection();
    }
  }
  outputDoseVolumeDisplayNode->SetVisibility(1);

  this->GetMRMLScene()->EndState(vtkMRMLScene::BatchProcessState); 

  return 0;
}

