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

  This file was originally developed by Kevin Wang, Radiation Medicine Program, 
  University Health Network and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

// ContourMorphology includes
#include "vtkSlicerDoseMorphologyModuleLogic.h"
#include "vtkMRMLDoseMorphologyNode.h"

// SlicerRt includes
#include "SlicerRtCommon.h"

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>
#include <vtkMRMLModelHierarchyNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLColorTableNode.h>
//#include <vtkMRMLContourNode.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkImageAccumulate.h>
#include <vtkImageMathematics.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

#define MIN_VOLUME_DIFFERENCE_TOLERANCE_VOXEL 100

//-----------------------------------------------------------------------------
int vtkSlicerDoseMorphologyModuleLogicTest1( int argc, char * argv[] )
{
  int argIndex = 1;

  const char *dataDirectoryPath = NULL;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-DataDirectoryPath") == 0)
    {
      dataDirectoryPath = argv[argIndex+1];
      std::cout << "Data directory path: " << dataDirectoryPath << std::endl;
      argIndex += 2;
    }
    else
    {
      dataDirectoryPath = "";
    }
  }
  else
  {
    std::cerr << "No arguments!" << std::endl;
    return EXIT_FAILURE;
  }

  const char *referenceDoseVolumeFile = NULL;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-ReferenceDoseVolumeFile") == 0)
    {
      referenceDoseVolumeFile = argv[argIndex+1];
      std::cout << "Reference dose volume file name: " << referenceDoseVolumeFile << std::endl;
      argIndex += 2;
    }
    else
    {
      referenceDoseVolumeFile = "";
    }
  }
  else
  {
    std::cerr << "No arguments!" << std::endl;
    return EXIT_FAILURE;
  }

  const char *inputDoseVolumeFile = NULL;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-InputDoseVolumeFile") == 0)
    {
      inputDoseVolumeFile = argv[argIndex+1];
      std::cout << "Input dose volume file name: " << inputDoseVolumeFile << std::endl;
      argIndex += 2;
    }
    else
    {
      inputDoseVolumeFile = "";
    }
  }
  else
  {
    std::cerr << "No arguments!" << std::endl;
    return EXIT_FAILURE;
  }

  const char *baselineDoseVolumeFile = NULL;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-BaselineDoseVolumeFile") == 0)
    {
      baselineDoseVolumeFile = argv[argIndex+1];
      std::cout << "Baseline dose volume file name: " << baselineDoseVolumeFile << std::endl;
      argIndex += 2;
    }
    else
    {
      baselineDoseVolumeFile = "";
    }
  }
  else
  {
    std::cerr << "No arguments!" << std::endl;
    return EXIT_FAILURE;
  }

  const char *temporarySceneFile = NULL;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-TemporarySceneFile") == 0)
    {
      temporarySceneFile = argv[argIndex+1];
      std::cout << "Temporary scene file name: " << temporarySceneFile << std::endl;
      argIndex += 2;
    }
    else
    {
      temporarySceneFile = "";
    }
  }
  else
  {
    std::cerr << "No arguments!" << std::endl;
    return EXIT_FAILURE;
  }

  const char *morphologicalOperation = NULL;
  unsigned int operation = 0;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-MorphologicalOperation") == 0)
    {
      morphologicalOperation = argv[argIndex+1];
      std::cout << "Morphological Operation: " << morphologicalOperation << std::endl;
      argIndex += 2;
      if (STRCASECMP(morphologicalOperation, "ExpandByScaling") == 0)
      {
        operation = 0;
      }
      else if (STRCASECMP(morphologicalOperation, "ExpandByDilation") == 0)
      {
        operation = 1;
      }
      else 
      {
        std::cerr << "Invalid Morphological Operation!" << std::endl;
        return EXIT_FAILURE;
      }
    }
    else
    {
      morphologicalOperation = "";
    }
  }

  double morphologicalParameter = 0.0;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-MorphologicalParameter") == 0)
    {
      std::stringstream ss;
      ss << argv[argIndex+1];
      double doubleValue;
      ss >> doubleValue;
      morphologicalParameter = doubleValue;
      std::cout << "Morphological Parameter: " << morphologicalParameter << std::endl;
      argIndex += 2;
    }
  }

int volumeDifferenceToleranceVoxel = 0.0;
  if (argc > argIndex+1)
  {
    if (STRCASECMP(argv[argIndex], "-VolumeDifferenceToleranceVoxel") == 0)
    {
      std::stringstream ss;
      ss << argv[argIndex+1];
      int intValue;
      ss >> intValue;
      volumeDifferenceToleranceVoxel = intValue;
      std::cout << "Volume difference Tolerance(Cc): " << volumeDifferenceToleranceVoxel << std::endl;
      argIndex += 2;
    }
  }

  // Constraint the criteria to be greater than zero
  if (volumeDifferenceToleranceVoxel == 0.0)
  {
    volumeDifferenceToleranceVoxel = MIN_VOLUME_DIFFERENCE_TOLERANCE_VOXEL;
  }

  // Create scene
  vtkSmartPointer<vtkMRMLScene> mrmlScene = vtkSmartPointer<vtkMRMLScene>::New();

  vtksys::SystemTools::RemoveFile(temporarySceneFile);
  mrmlScene->SetRootDirectory( vtksys::SystemTools::GetParentDirectory(temporarySceneFile).c_str() );
  mrmlScene->SetURL(temporarySceneFile);
  mrmlScene->Commit();

  // Create reference dose volume node
  vtkSmartPointer<vtkMRMLScalarVolumeNode> referenceDoseVolumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  mrmlScene->AddNode(referenceDoseVolumeNode);
  referenceDoseVolumeNode->SetName("Reference_Dose");

  // Load reference dose volume
  std::string referenceDoseVolumeFileName = std::string(dataDirectoryPath) + std::string(referenceDoseVolumeFile);
  if (!vtksys::SystemTools::FileExists(referenceDoseVolumeFileName.c_str()))
  {
    std::cerr << "Loading reference dose from file '" << referenceDoseVolumeFileName << "' failed - the file does not exist!" << std::endl;
  }

  vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> referenceDoseVolumeArchetypeStorageNode =
    vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New();
  mrmlScene->AddNode(referenceDoseVolumeArchetypeStorageNode);
  referenceDoseVolumeArchetypeStorageNode->SetFileName(referenceDoseVolumeFileName.c_str());

  referenceDoseVolumeNode->SetAndObserveStorageNodeID(referenceDoseVolumeArchetypeStorageNode->GetID());

  if (! referenceDoseVolumeArchetypeStorageNode->ReadData(referenceDoseVolumeNode))
  {
    mrmlScene->Commit();
    std::cerr << "Reading reference dose from file '" << referenceDoseVolumeFileName << "' failed!" << std::endl;
    return EXIT_FAILURE;
  }

  // Create input dose volume node
  vtkSmartPointer<vtkMRMLScalarVolumeNode> inputDoseVolumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  mrmlScene->AddNode(inputDoseVolumeNode);
  inputDoseVolumeNode->SetName("Input_Dose");

  // Load input dose volume
  std::string inputDoseVolumeFileName = std::string(dataDirectoryPath) + std::string(inputDoseVolumeFile);
  if (!vtksys::SystemTools::FileExists(inputDoseVolumeFileName.c_str()))
  {
    std::cerr << "Loading input dose from file '" << inputDoseVolumeFileName << "' failed - the file does not exist!" << std::endl;
  }

  vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> inputDoseVolumeArchetypeStorageNode =
    vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New();
  mrmlScene->AddNode(inputDoseVolumeArchetypeStorageNode);
  inputDoseVolumeArchetypeStorageNode->SetFileName(inputDoseVolumeFileName.c_str());

  inputDoseVolumeNode->SetAndObserveStorageNodeID(inputDoseVolumeArchetypeStorageNode->GetID());

  if (! inputDoseVolumeArchetypeStorageNode->ReadData(inputDoseVolumeNode))
  {
    mrmlScene->Commit();
    std::cerr << "Reading reference dose from file '" << inputDoseVolumeFileName << "' failed!" << std::endl;
    return EXIT_FAILURE;
  }

  // Create output dose volume node
  vtkSmartPointer<vtkMRMLScalarVolumeNode> outputDoseVolumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  mrmlScene->AddNode(outputDoseVolumeNode);
  outputDoseVolumeNode->SetName("Output_Dose");

  // Create and set up parameter set MRML node
  vtkSmartPointer<vtkMRMLDoseMorphologyNode> paramNode = vtkSmartPointer<vtkMRMLDoseMorphologyNode>::New();
  mrmlScene->AddNode(paramNode);
  paramNode->SetAndObserveReferenceDoseVolumeNodeID(referenceDoseVolumeNode->GetID());
  paramNode->SetAndObserveInputDoseVolumeNodeID(inputDoseVolumeNode->GetID());
  paramNode->SetAndObserveOutputDoseVolumeNodeID(outputDoseVolumeNode->GetID());
  paramNode->SetOperation(operation);
  paramNode->SetXSize(morphologicalParameter);
  paramNode->SetYSize(morphologicalParameter);
  paramNode->SetZSize(morphologicalParameter);

  // Create and set up logic
  vtkSmartPointer<vtkSlicerDoseMorphologyModuleLogic> doseMorphologyLogic = vtkSmartPointer<vtkSlicerDoseMorphologyModuleLogic>::New();
  doseMorphologyLogic->SetMRMLScene(mrmlScene);
  doseMorphologyLogic->SetAndObserveDoseMorphologyNode(paramNode);

  // Compute ContourMorphology
  doseMorphologyLogic->MorphDose();

  outputDoseVolumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(mrmlScene->GetNodeByID(paramNode->GetOutputDoseVolumeNodeID()));  
  if (outputDoseVolumeNode == NULL)
  {
    mrmlScene->Commit();
    std::cerr << "Invalid output dose volume node!" << std::endl;
    return EXIT_FAILURE;
  }
  
  // Create baseline labelmap node
  vtkSmartPointer<vtkMRMLScalarVolumeNode> baselineDoseVolumeNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  mrmlScene->AddNode(baselineDoseVolumeNode);
  baselineDoseVolumeNode->SetName("Baseline_dose");

  // Load baseline dose volume
  std::string baselineDoseVolumeFileName = std::string(dataDirectoryPath) + std::string(baselineDoseVolumeFile);
  if (!vtksys::SystemTools::FileExists(baselineDoseVolumeFileName.c_str()))
  {
    std::cerr << "Loading baseline dose volume from file '" << baselineDoseVolumeFileName << "' failed - the file does not exist!" << std::endl;
  }

  vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> baselineDoseVolumeArchetypeStorageNode =
    vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New();
  mrmlScene->AddNode(baselineDoseVolumeArchetypeStorageNode);
  baselineDoseVolumeArchetypeStorageNode->SetFileName(baselineDoseVolumeFileName.c_str());
  baselineDoseVolumeNode->SetAndObserveStorageNodeID(baselineDoseVolumeArchetypeStorageNode->GetID());

  if (! baselineDoseVolumeArchetypeStorageNode->ReadData(baselineDoseVolumeNode))
  {
    mrmlScene->Commit();
    std::cerr << "Reading baseline dose from file '" << baselineDoseVolumeFileName << "' failed!" << std::endl;
    return EXIT_FAILURE;
  }
  mrmlScene->Commit();

  // use vtkimagedifferenct
  vtkSmartPointer<vtkImageMathematics> difference = vtkSmartPointer<vtkImageMathematics>::New();
  difference->SetInput1(outputDoseVolumeNode->GetImageData());
  difference->SetInput2(baselineDoseVolumeNode->GetImageData());
  difference->SetOperationToSubtract();
  difference->Update();

  // use vtkimageaccumulate to do histogram
  vtkSmartPointer<vtkImageAccumulate> histogram = vtkSmartPointer<vtkImageAccumulate>::New();
  histogram->SetInput(difference->GetOutput());
  histogram->IgnoreZeroOn();
  histogram->Update();
  
  if (histogram->GetVoxelCount() > volumeDifferenceToleranceVoxel)
  {
    std::cerr << "Volume difference Tolerance(Cc) exceeds threshold!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

