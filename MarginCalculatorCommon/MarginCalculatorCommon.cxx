#include "MarginCalculatorCommon.h"

// MRML includes
#include <vtkMRMLTransformNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLColorTableNode.h>

//----------------------------------------------------------------------------
// Constant strings
//----------------------------------------------------------------------------

// SlicerRT constants
const char* MarginCalculatorCommon::SLICERRT_EXTENSION_NAME = "SlicerRT";
const std::string MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX = "Ref";

// Patient hierarchy constants (the python scripts use the raw strings, those need to be updated when these are changed!)
const char* MarginCalculatorCommon::PATIENTHIERARCHY_NODE_TYPE_ATTRIBUTE_NAME = "HierarchyType";
const char* MarginCalculatorCommon::PATIENTHIERARCHY_NODE_TYPE_ATTRIBUTE_VALUE = "PatientHierarchy"; // Identifier
const char* MarginCalculatorCommon::PATIENTHIERARCHY_DICOMLEVEL_ATTRIBUTE_NAME = "DicomLevel";
const char* MarginCalculatorCommon::PATIENTHIERARCHY_DICOMUID_ATTRIBUTE_NAME = "DicomUid";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX = "PatientHierarchy.";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_NODE_NAME_POSTFIX = "_PatientHierarchy";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_EXCLUDE_FROM_POTENTIAL_NODES_LIST_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "ExcludeFromPotentialNodesList"; // Identifier
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_PATIENT_NAME_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "PatientName";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_PATIENT_ID_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "PatientId";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_PATIENT_SEX_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "PatientSex";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_PATIENT_BIRTH_DATE_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "PatientBirthDate";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_STUDY_DATE_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "StudyDate";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_STUDY_TIME_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "StudyTime";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_SERIES_MODALITY_ATTRIBUTE_NAME = MarginCalculatorCommon::PATIENTHIERARCHY_ATTRIBUTE_PREFIX + "SeriesModality";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_NEW_NODE_NAME_PREFIX = "NewPatientHierarchyNode_";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_NEW_STRUCTURE_SET_NAME = "NewStructureSet";
const std::string MarginCalculatorCommon::PATIENTHIERARCHY_DUMMY_ANATOMICAL_VOLUME_NODE_NAME_PREFIX = "DummyAnatomicalVolume_";

// Contour (vtkMRMLContourNode) constants
const std::string MarginCalculatorCommon::CONTOUR_RIBBON_MODEL_NODE_NAME_POSTFIX = "_RibbonModel";
const std::string MarginCalculatorCommon::CONTOUR_INDEXED_LABELMAP_NODE_NAME_POSTFIX = "_IndexedLabelmap";
const std::string MarginCalculatorCommon::CONTOUR_CLOSED_SURFACE_MODEL_NODE_NAME_POSTFIX = "_ClosedSurfaceModel";

const double MarginCalculatorCommon::DEFAULT_RASTERIZATION_OVERSAMPLING_FACTOR = 2.0;
const double MarginCalculatorCommon::DEFAULT_DECIMATION_TARGET_REDUCTION_FACTOR = 0.0;

const char* MarginCalculatorCommon::CONTOUR_ORPHAN_CONTOURS_COLOR_TABLE_NODE_NAME = "OrphanContours_ColorTable";
const char* MarginCalculatorCommon::COLOR_NAME_BACKGROUND = "Background";
const char* MarginCalculatorCommon::COLOR_NAME_INVALID = "Invalid";
const char* MarginCalculatorCommon::COLOR_NAME_REMOVED = "Removed";
const int MarginCalculatorCommon::COLOR_INDEX_BACKGROUND = 0;
const int MarginCalculatorCommon::COLOR_INDEX_INVALID = 1;
const double MarginCalculatorCommon::COLOR_VALUE_INVALID[4] = {0.5, 0.5, 0.5, 1.0};

// DicomRtImport constants
const std::string MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX = "DicomRtImport.";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_DOSE_VOLUME_IDENTIFIER_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "DoseVolume"; // Identifier
const std::string MarginCalculatorCommon::DICOMRTIMPORT_DOSE_UNIT_NAME_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "DoseUnitName";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_DOSE_UNIT_VALUE_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "DoseUnitValue";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_SOURCE_AXIS_DISTANCE_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "SourceAxisDistance";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_GANTRY_ANGLE_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "GantryAngle";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_COUCH_ANGLE_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "CouchAngle";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_COLLIMATOR_ANGLE_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "CollimatorAngle";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_BEAM_JAW_POSITIONS_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "JawPositions";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_BEAM_NUMBER_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "BeamNumber";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_SOP_INSTANCE_UID_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "SopInstanceUid";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_ROI_REFERENCED_SERIES_UID_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "RoiReferencedSeriesUid"; // DICOM connection
const std::string MarginCalculatorCommon::DICOMRTIMPORT_CONTOUR_HIERARCHY_IDENTIFIER_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "ContourHierarchy"; // Identifier
const std::string MarginCalculatorCommon::DICOMRTIMPORT_STRUCTURE_NAME_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "StructureName";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_RTIMAGE_IDENTIFIER_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "RtImage"; // Identifier
const std::string MarginCalculatorCommon::DICOMRTIMPORT_RTIMAGE_REFERENCED_PLAN_SOP_INSTANCE_UID_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "RtImageReferencedPlanUid"; // DICOM connection
const std::string MarginCalculatorCommon::DICOMRTIMPORT_RTIMAGE_SID_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "RtImageSid";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_RTIMAGE_POSITION_ATTRIBUTE_NAME = MarginCalculatorCommon::DICOMRTIMPORT_ATTRIBUTE_PREFIX + "RtImagePosition";

const std::string MarginCalculatorCommon::DICOMRTIMPORT_COLOR_TABLE_NODE_NAME_POSTFIX = "_ColorTable";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_ROOT_MODEL_HIERARCHY_NODE_NAME_POSTFIX = "_AllStructures";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_CONTOUR_NODE_NAME_POSTFIX = "_Contour";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_ROOT_CONTOUR_HIERARCHY_NODE_NAME_POSTFIX = "_AllContours";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_ISOCENTER_HIERARCHY_NODE_NAME_POSTFIX = "_Isocenters";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_NO_NAME = "No name";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_NO_DESCRIPTION = "No description";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_SOURCE_HIERARCHY_NODE_NAME_POSTFIX = "_Sources";
const std::string MarginCalculatorCommon::DICOMRTIMPORT_BEAMMODEL_HIERARCHY_NODE_NAME_POSTFIX = "_BeamModels";

const char* MarginCalculatorCommon::DICOMRTIMPORT_DEFAULT_DOSE_COLOR_TABLE_NAME = "Dose_ColorTable";

// DoseVolumeHistogram constants
const std::string MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX = "DoseVolumeHistogram.";
const std::string MarginCalculatorCommon::DVH_DVH_IDENTIFIER_ATTRIBUTE_NAME = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "DVH"; // Identifier
const std::string MarginCalculatorCommon::DVH_DOSE_VOLUME_NODE_REFERENCE_ROLE = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "doseVolume" + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX; // Reference
const std::string MarginCalculatorCommon::DVH_CREATED_DVH_NODE_REFERENCE_ROLE = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "createdDvhArray" + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX; // Reference
const std::string MarginCalculatorCommon::DVH_STRUCTURE_CONTOUR_NODE_REFERENCE_ROLE = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "structureContour" + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX; // Reference
const std::string MarginCalculatorCommon::DVH_DOSE_VOLUME_OVERSAMPLING_FACTOR_ATTRIBUTE_NAME = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "DoseVolumeOversamplingFactor";
const std::string MarginCalculatorCommon::DVH_STRUCTURE_NAME_ATTRIBUTE_NAME = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "StructureName";
const std::string MarginCalculatorCommon::DVH_STRUCTURE_COLOR_ATTRIBUTE_NAME = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "StructureColor";
const std::string MarginCalculatorCommon::DVH_STRUCTURE_PLOT_NAME_ATTRIBUTE_NAME = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "StructurePlotName";
const std::string MarginCalculatorCommon::DVH_STRUCTURE_PLOT_LINE_STYLE_ATTRIBUTE_NAME = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "StructurePlotLineStyle";
const std::string MarginCalculatorCommon::DVH_METRIC_ATTRIBUTE_NAME_PREFIX = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "DvhMetric_";
const std::string MarginCalculatorCommon::DVH_METRIC_LIST_ATTRIBUTE_NAME = MarginCalculatorCommon::DVH_ATTRIBUTE_PREFIX + "DvhMetricList";

const char        MarginCalculatorCommon::DVH_METRIC_LIST_SEPARATOR_CHARACTER = '|';
const std::string MarginCalculatorCommon::DVH_METRIC_TOTAL_VOLUME_CC_ATTRIBUTE_NAME = "Volume (cc)";
const std::string MarginCalculatorCommon::DVH_METRIC_MEAN_ATTRIBUTE_NAME_PREFIX = "Mean ";
const std::string MarginCalculatorCommon::DVH_METRIC_MIN_ATTRIBUTE_NAME_PREFIX = "Min ";
const std::string MarginCalculatorCommon::DVH_METRIC_MAX_ATTRIBUTE_NAME_PREFIX = "Max ";
const std::string MarginCalculatorCommon::DVH_METRIC_DOSE_ATTRIBUTE_NAME_POSTFIX = "dose";
const std::string MarginCalculatorCommon::DVH_METRIC_INTENSITY_ATTRIBUTE_NAME_POSTFIX = "intensity";
const std::string MarginCalculatorCommon::DVH_METRIC_V_DOSE_ATTRIBUTE_NAME_PREFIX = "V";
const std::string MarginCalculatorCommon::DVH_ARRAY_NODE_NAME_POSTFIX = "_DvhArray";
const std::string MarginCalculatorCommon::DVH_CSV_HEADER_VOLUME_FIELD_MIDDLE = " Value (% of ";
const std::string MarginCalculatorCommon::DVH_CSV_HEADER_VOLUME_FIELD_END = " cc)";

// DoseAccumulation constants
const std::string MarginCalculatorCommon::DOSEACCUMULATION_ATTRIBUTE_PREFIX = "DoseAccumulation.";
const std::string MarginCalculatorCommon::DOSEACCUMULATION_DOSE_VOLUME_NODE_NAME_ATTRIBUTE_NAME = MarginCalculatorCommon::DOSEACCUMULATION_ATTRIBUTE_PREFIX + "DoseVolumeNodeName";
const std::string MarginCalculatorCommon::DOSEACCUMULATION_OUTPUT_BASE_NAME_PREFIX = "Accumulated_";

// Isodose constants
const char* MarginCalculatorCommon::ISODOSE_DEFAULT_ISODOSE_COLOR_TABLE_FILE_NAME = "Isodose_ColorTable.ctbl";
const std::string MarginCalculatorCommon::ISODOSE_MODEL_NODE_NAME_PREFIX = "IsodoseLevel_";
const std::string MarginCalculatorCommon::ISODOSE_PARAMETER_SET_BASE_NAME_PREFIX = "IsodoseParameterSet_";
const std::string MarginCalculatorCommon::ISODOSE_ISODOSE_SURFACES_HIERARCHY_NODE_NAME_POSTFIX = "_IsodoseSurfaces";

// Dose comparison constants
const char* MarginCalculatorCommon::DOSECOMPARISON_GAMMA_VOLUME_IDENTIFIER_ATTRIBUTE_NAME = "DoseComparison.GammaVolume"; // Identifier
const char* MarginCalculatorCommon::DOSECOMPARISON_DEFAULT_GAMMA_COLOR_TABLE_FILE_NAME = "Gamma_ColorTable.ctbl";
const std::string MarginCalculatorCommon::DOSECOMPARISON_OUTPUT_BASE_NAME_PREFIX = "GammaVolume_";

// Beams constants
const std::string MarginCalculatorCommon::BEAMS_ATTRIBUTE_PREFIX = "Beams.";
const std::string MarginCalculatorCommon::BEAMS_OUTPUT_ISOCENTER_FIDUCIAL_POSTFIX = "_Isocenter";
const std::string MarginCalculatorCommon::BEAMS_OUTPUT_SOURCE_FIDUCIAL_POSTFIX = "_Source";
const std::string MarginCalculatorCommon::BEAMS_OUTPUT_BEAM_MODEL_BASE_NAME_PREFIX = "BeamModel_";
const std::string MarginCalculatorCommon::BEAMS_PARAMETER_SET_BASE_NAME_PREFIX = "BeamParameterSet_";

// PlanarImage constants
const std::string MarginCalculatorCommon::PLANARIMAGE_MODEL_NODE_NAME_PREFIX = "PlanarImageDisplayedModel_";
const std::string MarginCalculatorCommon::PLANARIMAGE_TEXTURE_NODE_NAME_PREFIX = "PlanarImageDisplayedModelTexture_";
const std::string MarginCalculatorCommon::PLANARIMAGE_PARAMETER_SET_BASE_NAME_PREFIX = "PlanarImageParameterSet_";
const std::string MarginCalculatorCommon::PLANARIMAGE_RT_IMAGE_VOLUME_REFERENCE_ROLE = "planarRtImage" + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX; // Reference
const std::string MarginCalculatorCommon::PLANARIMAGE_DISPLAYED_MODEL_REFERENCE_ROLE = "planarImageDisplayedModel" + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX; // Reference
const std::string MarginCalculatorCommon::PLANARIMAGE_TEXTURE_VOLUME_REFERENCE_ROLE = "planarImageTexture" + MarginCalculatorCommon::SLICERRT_REFERENCE_ROLE_ATTRIBUTE_NAME_POSTFIX; // Reference

//----------------------------------------------------------------------------
// Utility functions
//----------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool MarginCalculatorCommon::IsDoseVolumeNode(vtkMRMLNode* node)
{
  if (!node)
  {
    return false;
  }

  if (node->IsA("vtkMRMLScalarVolumeNode"))
  {
    const char* doseVolumeIdentifier = node->GetAttribute(MarginCalculatorCommon::DICOMRTIMPORT_DOSE_VOLUME_IDENTIFIER_ATTRIBUTE_NAME.c_str());
    if (doseVolumeIdentifier != NULL)
    {
      return true;
    }
  }

  return false;
}

