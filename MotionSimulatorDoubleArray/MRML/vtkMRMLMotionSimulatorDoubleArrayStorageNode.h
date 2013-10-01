/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLMotionSimulatorDoubleArrayStorageNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.8 $

=========================================================================auto=*/

#ifndef __vtkMRMLMotionSimulatorDoubleArrayStorageNode_h
#define __vtkMRMLMotionSimulatorDoubleArrayStorageNode_h

#include "vtkMRMLStorageNode.h"

// DoubleArrays includes
#include "vtkSlicerMotionSimulatorDoubleArrayModuleMRMLExport.h"

/// \brief MRML node for representing a volume storage
///
/// vtkMRMLMotionSimulatorDoubleArrayStorageNode nodes describe the fiducial storage
/// node that allows to read/write point data from/to file.
class VTK_SLICER_MOTIONSIMULATORDOUBLEARRAY_MODULE_MRML_EXPORT vtkMRMLMotionSimulatorDoubleArrayStorageNode : public vtkMRMLStorageNode
{
public:
  static vtkMRMLMotionSimulatorDoubleArrayStorageNode *New();
  vtkTypeMacro(vtkMRMLMotionSimulatorDoubleArrayStorageNode,vtkMRMLStorageNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLNode* CreateNodeInstance();

  // Description:
  // Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName()  {return "MotionSimulatorDoubleArrayStorage";};

  // Description:
  // Return a default file extension for writting
  virtual const char* GetDefaultWriteFileExtension();

  // Return true if the node can be read in
  virtual bool CanReadInReferenceNode(vtkMRMLNode *refNode);

protected:
  vtkMRMLMotionSimulatorDoubleArrayStorageNode();
  ~vtkMRMLMotionSimulatorDoubleArrayStorageNode();
  vtkMRMLMotionSimulatorDoubleArrayStorageNode(const vtkMRMLMotionSimulatorDoubleArrayStorageNode&);
  void operator=(const vtkMRMLMotionSimulatorDoubleArrayStorageNode&);

  // Initialize all the supported write file types
  virtual void InitializeSupportedReadFileTypes();

  // Initialize all the supported write file types
  virtual void InitializeSupportedWriteFileTypes();

  /// Read data and set it in the referenced node
  virtual int ReadDataInternal(vtkMRMLNode *refNode);

  /// Write data from a  referenced node
  virtual int WriteDataInternal(vtkMRMLNode *refNode);

};

#endif



