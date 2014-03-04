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

// MotionSimulator Logic includes
#include "vtkSlicerMotionSimulatorModuleLogic.h"

// SlicerRT includes
#include "SlicerRtCommon.h"

// MRML includes
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVolumeDisplayNode.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLColorNode.h>
#include <vtkMRMLColorTableNode.h>
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLMotionSimulatorDoubleArrayNode.h>
//#include <vtkMRMLContourNode.h>
#include <vtkMRMLMotionSimulatorNode.h>
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
#include <vtkImageResample.h>
#include <vtkTransform.h>
#include <vtkImageToImageStencil.h>
#include <vtkImageAccumulate.h>
#include <vtkImageStencilData.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageMathematics.h>
#include <vtkDoubleArray.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>
#ifdef WIN32
#include <random>
#else
#include <tr1/random>
#endif
#include <cmath>
#include <time.h>

#define MOTION_MAX 5

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerMotionSimulatorModuleLogic);

//----------------------------------------------------------------------------
vtkSlicerMotionSimulatorModuleLogic::vtkSlicerMotionSimulatorModuleLogic()
{
  this->StartValue = 0.1;
  this->StepSize = 0.2;
  this->NumberOfSamples = 100;
  this->MotionSimulatorNode = NULL;
}

//----------------------------------------------------------------------------
vtkSlicerMotionSimulatorModuleLogic::~vtkSlicerMotionSimulatorModuleLogic()
{
  vtkSetAndObserveMRMLNodeMacro(this->MotionSimulatorNode, NULL);
}

//----------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::SetAndObserveMotionSimulatorNode(vtkMRMLMotionSimulatorNode *node)
{
  vtkSetAndObserveMRMLNodeMacro(this->MotionSimulatorNode, node);
}

//---------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
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
void vtkSlicerMotionSimulatorModuleLogic::RegisterNodes()
{
  vtkMRMLScene* scene = this->GetMRMLScene(); 
  if (!scene)
  {
    return;
  }
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLMotionSimulatorNode>::New());
}

//---------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene())
  {
    return;
  }

  // if the scene is still updating, jump out
  if (this->GetMRMLScene()->IsBatchProcessing())
  {
    return;
  }

  //if (node->IsA("vtkMRMLScalarVolumeNode") || node->IsA("vtkMRMLMotionSimulatorDoubleArrayNode") 
  //    || node->IsA("vtkMRMLContourNode") || node->IsA("vtkMRMLMotionSimulatorNode") )
  if (node->IsA("vtkMRMLScalarVolumeNode") || node->IsA("vtkMRMLMotionSimulatorDoubleArrayNode") 
      || node->IsA("vtkMRMLMotionSimulatorNode") )
  {
    this->Modified();
  }
}

//---------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene())
  {
    return;
  }

  // if the scene is still updating, jump out
  if (this->GetMRMLScene()->IsBatchProcessing())
  {
    return;
  }

  //if (node->IsA("vtkMRMLScalarVolumeNode") || node->IsA("vtkMRMLMotionSimulatorDoubleArrayNode") 
  //    || node->IsA("vtkMRMLContourNode") || node->IsA("vtkMRMLMotionSimulatorNode"))
  if (node->IsA("vtkMRMLScalarVolumeNode") || node->IsA("vtkMRMLMotionSimulatorDoubleArrayNode") 
      || node->IsA("vtkMRMLMotionSimulatorNode"))
  {
    this->Modified();
  }
}

//---------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::OnMRMLSceneEndImport()
{
  // If we have a parameter node select it
  vtkMRMLMotionSimulatorNode *paramNode = NULL;
  vtkMRMLNode *node = this->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLMotionSimulatorNode");
  if (node)
  {
    paramNode = vtkMRMLMotionSimulatorNode::SafeDownCast(node);
    vtkSetAndObserveMRMLNodeMacro(this->MotionSimulatorNode, paramNode);
  }
}

//---------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::OnMRMLSceneEndClose()
{
  this->Modified();
}

//---------------------------------------------------------------------------
bool vtkSlicerMotionSimulatorModuleLogic::DoseVolumeContainsDose()
{
  if (!this->GetMRMLScene() || !this->MotionSimulatorNode)
    {
    return false;
    }

  vtkMRMLScalarVolumeNode* doseVolumeNode = this->MotionSimulatorNode->GetInputDoseVolumeNode();

  const char* doseVolumeIdentifier = doseVolumeNode->GetAttribute(SlicerRtCommon::DICOMRTIMPORT_DOSE_VOLUME_IDENTIFIER_ATTRIBUTE_NAME.c_str());
  if (doseVolumeIdentifier != NULL)
  {
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
void vtkSlicerMotionSimulatorModuleLogic::GetStencilForContour(vtkMRMLScalarVolumeNode* volumeNode,
                                                         //vtkMRMLContourNode* structureContourNode, 
                                                         vtkMRMLScalarVolumeNode* structureContourNode, 
                                                         vtkImageData* resampledDoseVolume, 
                                                         vtkImageStencilData* structureStencil )
{
  if ( !this->GetMRMLScene() || !this->MotionSimulatorNode )
  {
    return;
  }

  // Get indexed labelmap representation (convert if does not exist yet)
  // vtkMRMLScalarVolumeNode* indexedLabelmapNode = structureContourNode->GetIndexedLabelmapVolumeNode();
  if (!structureContourNode)
  {
    vtkErrorMacro("Failed to get indexed labelmap representation from contour node '" << structureContourNode->GetName() << "' !");
    return;
  }

  vtkImageData* indexedLabelmap = structureContourNode->GetImageData();

  //double rasterizationDownsamplingFactor = structureContourNode->GetRasterizationOversamplingFactor();
  //if (rasterizationDownsamplingFactor != 1.0)
  //{
  //  vtkSmartPointer<vtkImageResample> resampler = vtkSmartPointer<vtkImageResample>::New();
  //  resampler->SetInput(volumeNode->GetImageData());
  //  resampler->SetAxisMagnificationFactor(0, rasterizationDownsamplingFactor);
  //  resampler->SetAxisMagnificationFactor(1, rasterizationDownsamplingFactor);
  //  resampler->SetAxisMagnificationFactor(2, rasterizationDownsamplingFactor);
  //  resampler->Update();

  //  resampledDoseVolume->DeepCopy(resampler->GetOutput());
  //}
  //else
  //{
  //  resampledDoseVolume->DeepCopy(volumeNode->GetImageData());
  //}
  resampledDoseVolume->DeepCopy(volumeNode->GetImageData());

  // Sanity check
  int resampledDoseDimensions[3];
  resampledDoseVolume->GetDimensions(resampledDoseDimensions);
  int indexedLabelmapDimensions[3];
  indexedLabelmap->GetDimensions(indexedLabelmapDimensions);

  if ( resampledDoseDimensions[0] != indexedLabelmapDimensions[0]
    || resampledDoseDimensions[1] != indexedLabelmapDimensions[1]
    || resampledDoseDimensions[2] != indexedLabelmapDimensions[2] )
  {
    vtkErrorMacro("Resampled dose volume has different dimensions than the indexed labelmap (probably downsampling factor has been changed)!");
    return;
  }

  // Create stencil for structure  
  vtkNew<vtkImageToImageStencil> stencil;
  stencil->SetInput(indexedLabelmap);
  stencil->ThresholdByUpper(0.5);
  stencil->Update();
  structureStencil->DeepCopy(stencil->GetOutput());
}

//---------------------------------------------------------------------------
int vtkSlicerMotionSimulatorModuleLogic::RunSimulation()
{
  vtkMRMLScalarVolumeNode* doseVolumeNode = this->MotionSimulatorNode->GetInputDoseVolumeNode();
  //vtkMRMLContourNode* contourNode = this->MotionSimulatorNode->GetInputContourNode();
  vtkMRMLScalarVolumeNode* contourNode = this->MotionSimulatorNode->GetInputContourNode();
  vtkMRMLMotionSimulatorDoubleArrayNode* outputArrayNode = this->MotionSimulatorNode->GetOutputDoubleArrayNode();
  // Make sure inputs are initialized
  if (!this->GetMRMLScene() || !this->MotionSimulatorNode || !doseVolumeNode || !contourNode || !outputArrayNode)
  {
    vtkErrorMacro("MotionSimulator: inputs and outputs are not initialized!")
    return -1;
  }

  if (this->MotionSimulatorNode->GetNumberOfSimulation() <=0 || this->MotionSimulatorNode->GetNumberOfSimulation() > 5000)
  {
    vtkErrorMacro("Unable to perform large number of simulation!");
    return -1;
  }
  //this->GetMRMLScene()->StartState(vtkMRMLScene::BatchProcessState); 

  // Get dose grid scaling and dose units
  const char* doseUnitName = doseVolumeNode->GetAttribute(SlicerRtCommon::DICOMRTIMPORT_DOSE_UNIT_NAME_ATTRIBUTE_NAME.c_str());

  // Get maximum dose from dose volume
  vtkNew<vtkImageAccumulate> doseStat;
  doseStat->SetInput(doseVolumeNode->GetImageData());
  doseStat->Update();
  double maxDose = doseStat->GetMax()[0];
  double minDose = doseStat->GetMin()[0];

  double startValue, stepSize;
  int numSamples;
  if (minDose < 0.0)
  {
    startValue = this->StartValue;
  }
  else
  {
    startValue = minDose;
  }

  stepSize = this->StepSize;
  numSamples = (int)ceil( (maxDose-startValue)/stepSize ) + 1;

  // Get dose grid scaling and dose units
  //std::string structureName(contourNode->GetStructureName());
  std::string structureName(contourNode->GetName());

  vtkSmartPointer<vtkMRMLScalarVolumeNode> structureStenciledDoseVolumeNode
    = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

  // Compute statistics
  vtkSmartPointer<vtkImageData> resampledDoseVolume = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageStencilData> structureStencil = vtkSmartPointer<vtkImageStencilData>::New();
  this->GetStencilForContour(doseVolumeNode, contourNode, resampledDoseVolume, structureStencil);

  //// Get spacing and voxel volume
  //contourNode->SetAndObserveRasterizationReferenceVolumeNodeId(doseVolumeNode->GetID());
  //vtkMRMLScalarVolumeNode* indexedLabelmapNode = contourNode->GetIndexedLabelmapVolumeNode();
  //if (!indexedLabelmapNode)
  //{
  //  vtkErrorMacro("Unable to get indexed labelmap representation from structure contour node '" << contourNode->GetName() << "'!");
  //  return 0;
  //}
  //double* indexedLabelmapSpacing = indexedLabelmapNode->GetSpacing();

  double* doseVolumeOrigin = doseVolumeNode->GetOrigin();
  double* doseVolumeSpacing = doseVolumeNode->GetSpacing();
    //indexedLabelmapVolumeNode->SetSpacing(
    //  referenceSpacing[0]/this->RasterizationOversamplingFactor,
    //  referenceSpacing[1]/this->RasterizationOversamplingFactor,
    //  referenceSpacing[2]/this->RasterizationOversamplingFactor );

  // Create node and fill statistics
  //std::string dvhArrayNodeName = structureName + SlicerRtCommon::DVH_ARRAY_NODE_NAME_POSTFIX;
  //dvhArrayNodeName = this->GetMRMLScene()->GenerateUniqueName(dvhArrayNodeName);
  //arrayNode->SetName(dvhArrayNodeName.c_str());
  //arrayNode->HideFromEditorsOff();

  //outputArrayNode->SetAttribute(SlicerRtCommon::DVH_TYPE_ATTRIBUTE_NAME.c_str(), SlicerRtCommon::DVH_TYPE_ATTRIBUTE_VALUE.c_str());
  //outputArrayNode->SetAttribute(SlicerRtCommon::DVH_DOSE_VOLUME_NODE_ID_ATTRIBUTE_NAME.c_str(), doseVolumeNode->GetID());
  outputArrayNode->SetAttribute(SlicerRtCommon::DVH_DVH_IDENTIFIER_ATTRIBUTE_NAME.c_str(), "1");
  outputArrayNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_NAME_ATTRIBUTE_NAME.c_str(), structureName.c_str());
  //outputArrayNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_CONTOUR_NODE_ID_ATTRIBUTE_NAME.c_str(), contourNode->GetID());

  vtkDoubleArray* doubleArray = outputArrayNode->GetArray();
  doubleArray->SetNumberOfTuples(this->MotionSimulatorNode->GetNumberOfSimulation());
  doubleArray->SetNumberOfComponents(5);
  int outputArrayIndex=0;

  std::tr1::mt19937 eng;  
  eng.seed(time(NULL));

  double xSysWeight = this->MotionSimulatorNode->GetXSysSD();
  double ySysWeight = this->MotionSimulatorNode->GetYSysSD();
  double zSysWeight = this->MotionSimulatorNode->GetZSysSD();
  double totalSysWeight = 1.0;
  totalSysWeight = sqrt(xSysWeight*xSysWeight + ySysWeight*ySysWeight + zSysWeight*zSysWeight);
  xSysWeight = xSysWeight/totalSysWeight;
  ySysWeight = ySysWeight/totalSysWeight;
  zSysWeight = zSysWeight/totalSysWeight;
  std::tr1::normal_distribution<double> Xdistribution(0.0, xSysWeight * this->MotionSimulatorNode->GetSystematicErrorSD());
  std::tr1::normal_distribution<double> Ydistribution(0.0, ySysWeight * this->MotionSimulatorNode->GetSystematicErrorSD());
  std::tr1::normal_distribution<double> Zdistribution(0.0, zSysWeight * this->MotionSimulatorNode->GetSystematicErrorSD());

  double xRdmWeight = this->MotionSimulatorNode->GetXRdmSD();
  double yRdmWeight = this->MotionSimulatorNode->GetYRdmSD();
  double zRdmWeight = this->MotionSimulatorNode->GetZRdmSD();
  double totalRdmWeight = 1.0;
  totalRdmWeight = sqrt(xRdmWeight*xRdmWeight + yRdmWeight*yRdmWeight + zRdmWeight*zRdmWeight);
  xRdmWeight = xRdmWeight/totalRdmWeight;
  yRdmWeight = yRdmWeight/totalRdmWeight;
  zRdmWeight = zRdmWeight/totalRdmWeight;
  std::tr1::normal_distribution<double> Xdistribution2(0.0, xRdmWeight * this->MotionSimulatorNode->GetRandomErrorSD());
  std::tr1::normal_distribution<double> Ydistribution2(0.0, yRdmWeight * this->MotionSimulatorNode->GetRandomErrorSD());
  std::tr1::normal_distribution<double> Zdistribution2(0.0, zRdmWeight * this->MotionSimulatorNode->GetRandomErrorSD());

  for (int i = 0; i<this->MotionSimulatorNode->GetNumberOfSimulation(); i++)
  { 
    vtkSmartPointer<vtkImageData> baseImageData = NULL;
    
	// Generate systematic error for all fractions, it stays the same over all fractions
    double xSys = Xdistribution(eng);
    double ySys = Ydistribution(eng);
    double zSys = Zdistribution(eng);
    double x = xSys + Xdistribution2(eng);
    double y = ySys + Ydistribution2(eng);
    double z = zSys + Zdistribution2(eng);
    x = x < MOTION_MAX ? x : MOTION_MAX;
    y = y < MOTION_MAX ? y : MOTION_MAX;
    z = z < MOTION_MAX ? z : MOTION_MAX;

    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Identity();
    transform->Translate(x, y, z);

    vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
    reslice->SetInput(resampledDoseVolume);
    reslice->SetInformationInput(resampledDoseVolume);
    reslice->SetResliceTransform(transform);
    reslice->SetInterpolationModeToLinear();
    reslice->UpdateWholeExtent();
    
    vtkSmartPointer<vtkImageData> translatedDoseVolume;
    baseImageData = reslice->GetOutput();

    if (this->MotionSimulatorNode->GetNumberOfFraction()>=2)
    {
      for (int j = 1; j<this->MotionSimulatorNode->GetNumberOfFraction(); j++)
      { // Generate new random error for each new fraction
        double x = xSys + Xdistribution2(eng);
        double y = ySys + Ydistribution2(eng);
        double z = zSys + Zdistribution2(eng);
        x = x < MOTION_MAX ? x : MOTION_MAX;
        y = y < MOTION_MAX ? y : MOTION_MAX;
        z = z < MOTION_MAX ? z : MOTION_MAX;
      
        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Identity();
        transform->Translate(x, y, z);
      
        vtkSmartPointer<vtkImageReslice> reslice = vtkSmartPointer<vtkImageReslice>::New();
        reslice->SetInput(resampledDoseVolume);
        reslice->SetInformationInput(resampledDoseVolume);
        reslice->SetResliceTransform(transform);
        reslice->UpdateWholeExtent();
        
        vtkSmartPointer<vtkImageData> translatedDoseVolume;
        //translatedDoseVolume = reslice->GetOutput();
      
        vtkSmartPointer<vtkImageMathematics> adder = vtkSmartPointer<vtkImageMathematics>::New();
        adder->SetInput1(baseImageData);
        adder->SetInput2(reslice->GetOutput());
        adder->SetOperationToAdd();
        adder->Update();
         
        baseImageData = adder->GetOutput();
      }
    
      vtkSmartPointer<vtkImageMathematics> MultiplyFilter1 = vtkSmartPointer<vtkImageMathematics>::New();
      MultiplyFilter1->SetInput(baseImageData);
      MultiplyFilter1->SetConstantK(1./this->MotionSimulatorNode->GetNumberOfFraction());
      MultiplyFilter1->SetOperationToMultiplyByK();
      MultiplyFilter1->Update();
      baseImageData = MultiplyFilter1->GetOutput();
    }
    
    vtkSmartPointer<vtkImageAccumulate> structureStat = vtkSmartPointer<vtkImageAccumulate>::New();
    structureStat->SetInput(baseImageData);
    structureStat->SetStencil(structureStencil);
    structureStat->SetComponentExtent(0,numSamples-1,0,0,0,0);
    structureStat->SetComponentOrigin(startValue,0,0);
    structureStat->SetComponentSpacing(stepSize,1,1);
    structureStat->Update();

    if (structureStat->GetVoxelCount() < 1)
    {
      vtkWarningMacro("No voxels in the structure. DVH computation aborted.");
      return 0;
    }

    vtkSmartPointer<vtkDoubleArray> dvhArray = vtkSmartPointer<vtkDoubleArray>::New();
    dvhArray->SetNumberOfComponents(2);
    dvhArray->SetNumberOfTuples(numSamples);

    vtkImageData* statArray = structureStat->GetOutput();
    unsigned long totalVoxels = structureStat->GetVoxelCount();
    //for (int sampleIndex=0; sampleIndex<numSamples; ++sampleIndex)
    //{
    //  unsigned long voxelsInBin = statArray->GetScalarComponentAsDouble(sampleIndex,0,0,0);
    //  doubleArray->SetComponent( outputArrayIndex, 0, startValue + sampleIndex * stepSize );
    //  doubleArray->SetComponent( outputArrayIndex, 1, (1.0-(double)voxelBelowDose/(double)totalVoxels)*100.0 );
    //  ++outputArrayIndex;
    //  voxelBelowDose += voxelsInBin;
    //}
    double D98 = 0.0;
    double vlast = 0.0;
    double dlast = 0.0;
    unsigned long voxelBelowDose = 0;
    for (int sampleIndex=0; sampleIndex<numSamples; ++sampleIndex)
    {
      unsigned long voxelsInBin = statArray->GetScalarComponentAsDouble(sampleIndex,0,0,0);
      if ( (1.0-(double)voxelBelowDose/(double)totalVoxels)*100.0 < 98 && vlast >= 98)
      {
        D98 = (startValue + sampleIndex * stepSize + dlast)/2;
      }
      vlast = (1.0-(double)voxelBelowDose/(double)totalVoxels)*100.0;
      voxelBelowDose += voxelsInBin;
      dlast = startValue + sampleIndex * stepSize;
    }

    double minDoseROI = structureStat->GetMin()[0];
    doubleArray->SetComponent(outputArrayIndex, 0, x);
    doubleArray->SetComponent(outputArrayIndex, 1, y);
    doubleArray->SetComponent(outputArrayIndex, 2, z);
    doubleArray->SetComponent(outputArrayIndex, 3, minDoseROI);
    doubleArray->SetComponent(outputArrayIndex, 4, D98);
    outputArrayIndex++;
  }

  return 0;
}