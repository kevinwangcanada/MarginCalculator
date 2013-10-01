import os
import unittest
import string
from __main__ import vtk, qt, ctk, slicer

#
# Dose convolution module
#

class DoseVolumeComparison:
  def __init__(self, parent):
    parent.title = "Dose Volume Comparison"
    parent.categories = ["Margin Calculator"]
    parent.dependencies = []
    parent.contributors = ["Kevin Wang (Princess Margaret Hospital)"]
    parent.helpText = string.Template("""
    Use this module to compare the similarity of two dose distributions by volume. 
""").substitute({ 'a':parent.slicerWikiUrl, 'b':slicer.app.majorVersion, 'c':slicer.app.minorVersion })
    parent.acknowledgementText = """
    Supported by SparKit and the Slicer Community. See http://www.slicerrt.org for details.
    """
    self.parent = parent

#
# Widget
#

class DoseVolumeComparisonWidget:

  def __init__(self, parent=None):
    self.parent = parent
    self.logic = None

  def setup(self):

    frame = qt.QFrame()
    layout = qt.QFormLayout()
    frame.setLayout( layout )
    self.parent.layout().addWidget( frame )

    # Dose Volume selector
    self.referenceDoseVolumeSelectorLabel = qt.QLabel()
    self.referenceDoseVolumeSelectorLabel.setText( "Reference Dose Volume : " )
    self.referenceDoseVolumeSelector = slicer.qMRMLNodeComboBox()
    self.referenceDoseVolumeSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.referenceDoseVolumeSelector.noneEnabled = False
    self.referenceDoseVolumeSelector.addEnabled = False
    self.referenceDoseVolumeSelector.removeEnabled = False
    self.referenceDoseVolumeSelector.selectNodeUponCreation = True
    self.referenceDoseVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.referenceDoseVolumeSelector.setToolTip( "Set the reference dose volume" )
    layout.addRow(self.referenceDoseVolumeSelectorLabel, self.referenceDoseVolumeSelector)

    # Dose Volume selector
    self.secondaryDoseVolumeSelectorLabel = qt.QLabel()
    self.secondaryDoseVolumeSelectorLabel.setText( "Secondary Dose Volume: " )
    self.secondaryDoseVolumeSelector = slicer.qMRMLNodeComboBox()
    self.secondaryDoseVolumeSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.secondaryDoseVolumeSelector.noneEnabled = False
    self.secondaryDoseVolumeSelector.addEnabled = False
    self.secondaryDoseVolumeSelector.removeEnabled = False
    self.secondaryDoseVolumeSelector.selectNodeUponCreation = True
    self.secondaryDoseVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.secondaryDoseVolumeSelector.setToolTip( "Set the secondary dose volume" )
    layout.addRow(self.secondaryDoseVolumeSelectorLabel, self.secondaryDoseVolumeSelector)

    # kernel SD value editor
    self.volumeSimilarityValueLabel = qt.QLabel('Similarity value:')
    self.volumeSimilarityValueEdit = qt.QDoubleSpinBox()
    self.volumeSimilarityValueEdit.value = 0
    layout.addRow(self.volumeSimilarityValueLabel, self.volumeSimilarityValueEdit)

    # Apply button
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Compare Dose"
    layout.addWidget(self.applyButton)
    self.UpdateApplyButtonState()

    # connections
    self.applyButton.connect('clicked()', self.onApply)
    self.referenceDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onDoseVolumeSelect)
    self.secondaryDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onDoseVolumeSelect)

  def UpdateApplyButtonState(self):
    if not self.referenceDoseVolumeSelector.currentNode() or not self.secondaryDoseVolumeSelector.currentNode() :
      self.applyButton.enabled = False
    else:
      self.applyButton.enabled = True

  def onDoseVolumeSelect(self, node):
    self.UpdateApplyButtonState()

  def onApply(self):
    self.applyButton.text = "Working..."
    self.applyButton.repaint()
    slicer.app.processEvents()
    self.logic = DoseVolumeComparisonLogic()
    self.logic.run(self.referenceDoseVolumeSelector.currentNode(), self.secondaryDoseVolumeSelector.currentNode())
    self.volumeSimilarityValueEdit.value = self.logic.getVolumeSimilarity()
    self.volumeSimilarityValueEdit.repaint()
    self.applyButton.text = "Apply"

#
# Logic
#

class DoseVolumeComparisonLogic:
  """Implement the logic to fill the ROI.
  Nodes are passed in as arguments.
  Results are stored as 'statistics' instance variable.
  """
  def __init__(self):
    self._volumeSimilarity = 0.0

  def run(self, referenceVolumeNode, secondaryVolumeNode):

    referenceImageData=referenceVolumeNode.GetImageData()
    secondaryImageData=secondaryVolumeNode.GetImageData()

    # Apply image convolution to input imagedata
    imageMath = vtk.vtkImageMathematics()
    imageMath.SetInput1(referenceImageData)
    imageMath.SetInput2(secondaryImageData)
    imageMath.SetOperationToSubtract()
    imageMath.Update()

    imageAccumulate = vtk.vtkImageAccumulate()
    imageAccumulate.SetInput(referenceImageData)
    imageAccumulate.IgnoreZeroOn()
    imageAccumulate.Update()
    referenceVolumeVoxelCount = imageAccumulate.GetVoxelCount()

    # Update the volume with convolutionl operation result
    imageAccumulate2 = vtk.vtkImageAccumulate()
    imageAccumulate2.SetInput(imageMath.GetOutput())
    imageAccumulate2.IgnoreZeroOn()
    imageAccumulate2.Update()
    differenceVolumeVoxelCount = imageAccumulate2.GetVoxelCount()
    
    self._volumeSimilarity = (referenceVolumeVoxelCount-differenceVolumeVoxelCount)/referenceVolumeVoxelCount

  def getVolumeSimilarity(self):
    return self._volumeSimilarity