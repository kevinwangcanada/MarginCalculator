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
  and Ontario Consortium for Adaptive Interventions in MotionSimulatorculator Oncology (OCAIRO).

==============================================================================*/

// DosePopulationHistogram includes
#include "vtkSlicerDosePopulationHistogramModuleLogic.h"
#include "vtkMRMLDosePopulationHistogramNode.h"

// SlicerRT includes
#include "SlicerRtCommon.h"

#include "vtkMRMLMotionSimulatorDoubleArrayNode.h"

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLChartNode.h>
#include <vtkMRMLLayoutNode.h>
#include <vtkMRMLChartViewNode.h>
#include <vtkMRMLDoubleArrayNode.h>
#include <vtkMRMLTransformNode.h>
#include <vtkMRMLModelDisplayNode.h>
//#include <vtkMRMLContourNode.h>
#include <vtkMRMLColorTableNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkTransform.h>
#include <vtkImageAccumulate.h>
#include <vtkImageThreshold.h>
#include <vtkImageToImageStencil.h>
#include <vtkImageStencilData.h>
#include <vtkDoubleArray.h>
#include <vtkStringArray.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageResample.h>
#include <vtkImageData.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <cassert>
#include <set>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerDosePopulationHistogramModuleLogic);

//----------------------------------------------------------------------------
vtkSlicerDosePopulationHistogramModuleLogic::vtkSlicerDosePopulationHistogramModuleLogic()
{
  this->DosePopulationHistogramNode = NULL;
  this->StartValue = 0.1;
  this->StepSize = 0.2;
}

//----------------------------------------------------------------------------
vtkSlicerDosePopulationHistogramModuleLogic::~vtkSlicerDosePopulationHistogramModuleLogic()
{
  if (this->GetMRMLScene() && this->DosePopulationHistogramNode)
  {
    for (std::vector<std::string>::iterator it = this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->begin();
      it != this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->end(); ++it)
    {
      vtkMRMLDoubleArrayNode* dvhNode = vtkMRMLDoubleArrayNode::SafeDownCast(
        this->GetMRMLScene()->GetNodeByID(it->c_str()));
      dvhNode->Delete();
    }    
  }

  vtkSetAndObserveMRMLNodeMacro(this->DosePopulationHistogramNode, NULL);
}

//----------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::SetAndObserveDosePopulationHistogramNode(vtkMRMLDosePopulationHistogramNode *node)
{
  vtkSetAndObserveMRMLNodeMacro(this->DosePopulationHistogramNode, node);
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndImportEvent);
  events->InsertNextValue(vtkMRMLScene::EndCloseEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEvents(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::RegisterNodes()
{
  vtkMRMLScene* scene = this->GetMRMLScene(); 
  if (!scene)
  {
    return;
  }
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLDosePopulationHistogramNode>::New());
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::RefreshDPHDoubleArrayNodesFromScene()
{
  if (!this->GetMRMLScene() || !this->DosePopulationHistogramNode)
  {
    vtkErrorMacro("RefreshDPHDoubleArrayNodesFromScene: Invalid MRML scene or parameter set node!");
    return;
  }
  this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->clear();

  if (this->GetMRMLScene() == NULL || this->GetMRMLScene()->GetNumberOfNodesByClass("vtkMRMLDoubleArrayNode") < 1)
  {
    return;
  }

  this->GetMRMLScene()->InitTraversal();
  vtkMRMLNode *node = this->GetMRMLScene()->GetNextNodeByClass("vtkMRMLDoubleArrayNode");
  while (node != NULL)
  {
    vtkMRMLDoubleArrayNode* doubleArrayNode = vtkMRMLDoubleArrayNode::SafeDownCast(node);
    if (doubleArrayNode)
    {
      const char* type = doubleArrayNode->GetAttribute(SlicerRtCommon::DVH_DVH_IDENTIFIER_ATTRIBUTE_NAME.c_str());
      if (type)
      {
        this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->push_back(doubleArrayNode->GetID());
      }
    }

    node = this->GetMRMLScene()->GetNextNodeByClass("vtkMRMLDoubleArrayNode");
  }
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);

  this->RefreshDPHDoubleArrayNodesFromScene();

  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::OnMRMLSceneNodeAdded(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene() || !this->DosePopulationHistogramNode)
  {
    return;
  }

  // if the scene is still updating, jump out
  if (this->GetMRMLScene()->IsBatchProcessing())
  {
    return;
  }

  if (node->IsA("vtkMRMLDoubleArrayNode"))
  {
    vtkMRMLDoubleArrayNode* doubleArrayNode = vtkMRMLDoubleArrayNode::SafeDownCast(node);
    if (doubleArrayNode)
    {
      const char* type = doubleArrayNode->GetAttribute(SlicerRtCommon::DVH_DVH_IDENTIFIER_ATTRIBUTE_NAME.c_str());
      if (type)
      {
        this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->push_back(doubleArrayNode->GetID());
      }
    }
  }

  if (node->IsA("vtkMRMLMotionSimulatorDoubleArrayNode") || node->IsA("vtkMRMLChartNode") 
      || node->IsA("vtkMRMLDosePopulationHistogramNode"))
  {
    this->Modified();
  }
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::OnMRMLSceneNodeRemoved(vtkMRMLNode* node)
{
  if (!node || !this->GetMRMLScene() || !this->DosePopulationHistogramNode)
  {
    return;
  }

  // if the scene is still updating, jump out
  if (this->GetMRMLScene()->IsBatchProcessing())
  {
    return;
  }

  if (node->IsA("vtkMRMLDoubleArrayNode"))
  {
    const char* removedNodeId = vtkMRMLDoubleArrayNode::SafeDownCast(node)->GetID();
    for (std::vector<std::string>::iterator it = this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->begin();
      it != this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->end(); ++it)
    {
      if (!it->compare(removedNodeId))
      {
        this->DosePopulationHistogramNode->GetDPHDoubleArrayNodeIDs()->erase(it);
        break;
      }
    }
  }

  if (node->IsA("vtkMRMLMotionSimulatorDoubleArrayNode") || node->IsA("vtkMRMLChartNode") || node->IsA("vtkMRMLDosePopulationHistogramNode"))
  {
    this->Modified();
  }
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::OnMRMLSceneEndImport()
{
  // If we have a parameter node select it
  vtkMRMLDosePopulationHistogramNode *paramNode = NULL;
  vtkMRMLNode *node = this->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLDosePopulationHistogramNode");
  if (node)
  {
    paramNode = vtkMRMLDosePopulationHistogramNode::SafeDownCast(node);
    vtkSetAndObserveMRMLNodeMacro(this->DosePopulationHistogramNode, paramNode);
  }
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::OnMRMLSceneEndClose()
{
  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::GetStencilForContour(vtkMRMLScalarVolumeNode* volumeNode,
                                                         //vtkMRMLContourNode* structureContourNode, 
                                                         vtkMRMLScalarVolumeNode* structureContourNode, 
                                                         vtkImageData* resampledDoseVolume, 
                                                         vtkImageStencilData* structureStencil )
{
  if ( !this->GetMRMLScene() || !this->DosePopulationHistogramNode )
  {
    return;
  }

  // Get indexed labelmap representation (convert if does not exist yet)
  //vtkMRMLScalarVolumeNode* indexedLabelmapNode = structureContourNode->GetIndexedLabelmapVolumeNode();
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
void vtkSlicerDosePopulationHistogramModuleLogic::ComputeDPH()
{
  if ( !this->GetMRMLScene() || !this->DosePopulationHistogramNode )
  {
    return;
  }

  vtkMRMLMotionSimulatorDoubleArrayNode* doubleArrayNode = vtkMRMLMotionSimulatorDoubleArrayNode::SafeDownCast(
    this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetDoubleArrayNodeID()));
  vtkMRMLScalarVolumeNode* doseVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetDoseVolumeNodeID()));
  //vtkMRMLContourNode* contourNode = vtkMRMLContourNode::SafeDownCast(
  //  this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetContourNodeID()));
  vtkMRMLScalarVolumeNode* contourNode = vtkMRMLScalarVolumeNode::SafeDownCast(
    this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetContourNodeID()));
  // Make sure inputs are initialized
  if (!doseVolumeNode || !contourNode || !doubleArrayNode)
  {
    vtkErrorMacro("DosePopulationHistogram: inputs are not initialized!")
    return ;
  }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> structureStenciledDoseVolumeNode
    = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

  // Compute statistics
  vtkSmartPointer<vtkImageData> resampledDoseVolume = vtkSmartPointer<vtkImageData>::New();
  vtkSmartPointer<vtkImageStencilData> structureStencil = vtkSmartPointer<vtkImageStencilData>::New();
  this->GetStencilForContour(doseVolumeNode, contourNode, resampledDoseVolume, structureStencil);

  // Get maximum dose from dose volume
  vtkNew<vtkImageAccumulate> doseStat;
  doseStat->SetInput(doseVolumeNode->GetImageData());
  doseStat->Update();
  double maxDose = doseStat->GetMax()[0];
  double startValue = 0.1, stepSize = 0.2;
  int numSamples = (int)ceil( (maxDose-startValue)/stepSize ) + 1;

  // Get min dose from dose volume
  vtkSmartPointer<vtkImageAccumulate> structureStat = vtkSmartPointer<vtkImageAccumulate>::New();
  structureStat->SetInput(resampledDoseVolume);
  structureStat->SetStencil(structureStencil);
  structureStat->SetComponentExtent(0,numSamples-1,0,0,0,0);
  structureStat->SetComponentOrigin(startValue,0,0);
  structureStat->SetComponentSpacing(stepSize,1,1);
  structureStat->Update();
  double minDose = structureStat->GetMin()[0];

  // Get D98 dose from dose volume
  double D98Dose = 0.0;
  vtkImageData* statArray = structureStat->GetOutput();
  unsigned long totalVoxels = structureStat->GetVoxelCount();
  double vlast = 0.0;
  double dlast = 0.0;
  unsigned long voxelBelowDose = 0;
  for (int sampleIndex=0; sampleIndex<numSamples; ++sampleIndex)
  {
    unsigned long voxelsInBin = statArray->GetScalarComponentAsDouble(sampleIndex,0,0,0);
    if ( (1.0-(double)voxelBelowDose/(double)totalVoxels)*100.0 < 98 && vlast >= 98)
    {
      D98Dose = (startValue + sampleIndex * stepSize + dlast)/2;
    }
    vlast = (1.0-(double)voxelBelowDose/(double)totalVoxels)*100.0;
    voxelBelowDose += voxelsInBin;
    dlast = startValue + sampleIndex * stepSize;
  }

  //// Get maximum dose 
  vtkDoubleArray* planArray = doubleArrayNode->GetArray();
  int numberTotal = planArray->GetNumberOfTuples();
  maxDose = 0.0;
  int UseDoseNormalization = this->DosePopulationHistogramNode->GetUseDoseOption();
  double doseNormalizationFactor = 1.0;
  for (int i=0; i<numberTotal; i++)
  {
    // check Dmin
    if (UseDoseNormalization == ART_DPH_USEDMIN)
    {
      if (maxDose < planArray->GetComponent(i, 3))
      {
        maxDose = planArray->GetComponent(i, 3);
      }
    }
    else if (UseDoseNormalization == ART_DPH_USED98)
    {
      if (maxDose < planArray->GetComponent(i, 4))
      {
        maxDose = planArray->GetComponent(i, 4);
      }
    }
  }

  // Compute statistics
  std::vector<int> DPHBins;
  int numberBins = 100;
  DPHBins.resize(numberBins);
  if (UseDoseNormalization == ART_DPH_USEDMIN)
  {
    if (minDose > 0.01)
    {
      this->StepSize = 1.0;
      doseNormalizationFactor = 100.0/minDose;
    }
    for (int i=0; i<numberTotal; i++)
    {
      // fill bins
      int binIndex = (int)(planArray->GetComponent(i, 3)*doseNormalizationFactor/this->StepSize);
      if (binIndex+1>numberBins)
      {
        numberBins = binIndex+1;
        DPHBins.resize(numberBins);
      }
      DPHBins[binIndex]++;
    }
  }
  if (UseDoseNormalization == ART_DPH_USED98)
  {
    if (D98Dose > 0.01)
    {
      this->StepSize = 1.0;
      doseNormalizationFactor = 100.0/D98Dose;
    }
    for (int i=0; i<numberTotal; i++)
    {
      // fill bins
      int binIndex = (int)(planArray->GetComponent(i, 4)*doseNormalizationFactor/this->StepSize);
      if (binIndex+1>numberBins)
      {
        numberBins = binIndex+1;
        DPHBins.resize(numberBins);
      }
      DPHBins[binIndex]++;
    }
  }
  
  // Create node and fill statistics
  vtkMRMLDoubleArrayNode* arrayNode = vtkMRMLDoubleArrayNode::SafeDownCast(
    this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetOutputDoubleArrayNodeID()));

  if (!doubleArrayNode) 
  {
    arrayNode = (vtkMRMLDoubleArrayNode*)( this->GetMRMLScene()->CreateNodeByClass("vtkMRMLDoubleArrayNode") );
  }
  std::string DPHArrayNodeName = std::string(doubleArrayNode->GetName()) + SlicerRtCommon::DVH_ARRAY_NODE_NAME_POSTFIX;
  DPHArrayNodeName = this->GetMRMLScene()->GenerateUniqueName(DPHArrayNodeName);
  arrayNode->SetName(DPHArrayNodeName.c_str());
  //arrayNode->HideFromEditorsOff();

  arrayNode->SetAttribute(SlicerRtCommon::DVH_DVH_IDENTIFIER_ATTRIBUTE_NAME.c_str(), "1");

  vtkDoubleArray* doubleArray = arrayNode->GetArray();
  bool insertPointAtOrigin=true;
  doubleArray->SetNumberOfTuples(numberBins + (insertPointAtOrigin?1:0));

  int outputArrayIndex=0;

  if (insertPointAtOrigin)
  {
    // Add first fixed point at (0.0, 100%)
    doubleArray->SetComponent(outputArrayIndex, 0, 0.0);
    doubleArray->SetComponent(outputArrayIndex, 1, 100.0);
    doubleArray->SetComponent(outputArrayIndex, 2, 0);
    ++outputArrayIndex;
  }

  unsigned long numberBelowDose = 0;
  for (int sampleIndex=0; sampleIndex<numberBins; ++sampleIndex)
  {
    unsigned long numberInBin = DPHBins[sampleIndex];
    doubleArray->SetComponent( outputArrayIndex, 0, this->StartValue + sampleIndex * this->StepSize );
    doubleArray->SetComponent( outputArrayIndex, 1, (1.0-(double)numberBelowDose/(double)numberTotal)*100.0 );
    doubleArray->SetComponent( outputArrayIndex, 2, 0 );
    ++outputArrayIndex;
    numberBelowDose += numberInBin;
  }

  this->GetMRMLScene()->AddNode(arrayNode);

  this->Modified();
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::AddDPHToSelectedChart(const char* planName, const char* DPHArrayNodeID)
{
  if (!this->GetMRMLScene() || !this->DosePopulationHistogramNode)
  {
    return;
  }

  vtkMRMLChartNode* chartNode = vtkMRMLChartNode::SafeDownCast(
    this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetChartNodeID()));
  vtkMRMLMotionSimulatorDoubleArrayNode* doubleArrayNode = vtkMRMLMotionSimulatorDoubleArrayNode::SafeDownCast(
    this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetDoubleArrayNodeID()));

  if (!chartNode || !doubleArrayNode)
  {
    return;
  }

  vtkMRMLChartViewNode* chartViewNode = this->GetChartViewNode();
  if (chartViewNode == NULL)
  {
    vtkErrorMacro("Error: unable to get chart view node!");
    return;
  }

  chartNode->SetProperty("default", "title", "Dose Population Histogram");
  chartNode->SetProperty("default", "xAxisLabel", "Dose (%)");
  chartNode->SetProperty("default", "yAxisLabel", "Population [%]");
  chartNode->SetProperty("default", "type", "Line");
  chartNode->SetProperty("default", "xAxisPad", "0");
  chartNode->SetProperty("default", "yAxisPad", "0");

  vtkMRMLDoubleArrayNode* DPHArrayNode = vtkMRMLDoubleArrayNode::SafeDownCast( this->GetMRMLScene()->GetNodeByID(DPHArrayNodeID) );
  if (DPHArrayNode == NULL)
  {
    vtkErrorMacro("Error: unable to get double array node!");
    return;
  }

  // Add array to chart
  chartNode->AddArray( planName, DPHArrayNodeID );

  // Set plot color and line style
  const char* color = DPHArrayNode->GetAttribute(SlicerRtCommon::DVH_STRUCTURE_COLOR_ATTRIBUTE_NAME.c_str());
  chartNode->SetProperty(planName, "color", color);
  const char* lineStyle = DPHArrayNode->GetAttribute(SlicerRtCommon::DVH_STRUCTURE_PLOT_LINE_STYLE_ATTRIBUTE_NAME.c_str());
  chartNode->SetProperty(planName, "linePattern", lineStyle);

  // Add chart to chart view
  chartViewNode->SetChartNodeID( chartNode->GetID() );
}

//---------------------------------------------------------------------------
void vtkSlicerDosePopulationHistogramModuleLogic::RemoveDPHFromSelectedChart(const char* planName)
{
  if (!this->GetMRMLScene() || !this->DosePopulationHistogramNode)
  {
    return;
  }

  vtkMRMLChartNode* chartNode = vtkMRMLChartNode::SafeDownCast(
    this->GetMRMLScene()->GetNodeByID(this->DosePopulationHistogramNode->GetChartNodeID()));

  if (!chartNode)
  {
    return;
  }

  vtkMRMLChartViewNode* chartViewNode = this->GetChartViewNode();
  if (chartViewNode == NULL)
  {
    vtkErrorMacro("Error: unable to get chart view node!");
    return;
  }

  chartViewNode->SetChartNodeID( chartNode->GetID() );
  chartNode->RemoveArray(planName);
}

//---------------------------------------------------------------------------
vtkMRMLChartViewNode* vtkSlicerDosePopulationHistogramModuleLogic::GetChartViewNode()
{
  vtkSmartPointer<vtkCollection> layoutNodes
    = vtkSmartPointer<vtkCollection>::Take( this->GetMRMLScene()->GetNodesByClass("vtkMRMLLayoutNode") );
  layoutNodes->InitTraversal();
  vtkObject* layoutNodeVtkObject = layoutNodes->GetNextItemAsObject();
  vtkMRMLLayoutNode* layoutNode = vtkMRMLLayoutNode::SafeDownCast(layoutNodeVtkObject);
  if (layoutNode == NULL)
  {
    vtkErrorMacro("Error: unable to get layout node!");
    return NULL;
  }
  layoutNode->SetViewArrangement( vtkMRMLLayoutNode::SlicerLayoutConventionalQuantitativeView );
  
  vtkSmartPointer<vtkCollection> chartViewNodes
    = vtkSmartPointer<vtkCollection>::Take( this->GetMRMLScene()->GetNodesByClass("vtkMRMLChartViewNode") );
  chartViewNodes->InitTraversal();
  vtkMRMLChartViewNode* chartViewNode = vtkMRMLChartViewNode::SafeDownCast( chartViewNodes->GetNextItemAsObject() );
  if (chartViewNode == NULL)
  {
    vtkErrorMacro("Error: unable to get chart view node!");
    return NULL;
  }

  return chartViewNode;
}

