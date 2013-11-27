import os
import unittest
import string
from __main__ import vtk, qt, ctk, slicer

#
# Dose volume convolution module
#

class DoseVolumeConvolution:
  def __init__(self, parent):
    parent.title = "Dose Volume Convolution"
    parent.categories = ["Margin Calculator"]
    parent.dependencies = []
    parent.contributors = ["Kevin Wang (Princess Margaret Cancer Centre)"]
    parent.helpText = string.Template("""
    Use this module to convolve a dose distribution volume. 
""").substitute({ 'a':parent.slicerWikiUrl, 'b':slicer.app.majorVersion, 'c':slicer.app.minorVersion })
    parent.acknowledgementText = """
    Supported by SparKit and the Slicer Community. See http://www.slicerrt.org for details.
    """
    self.parent = parent

#
# Widget
#

class DoseVolumeConvolutionWidget:

  def __init__(self, parent=None):
    self.parent = parent
    self.logic = None

  def setup(self):

    frame = qt.QFrame()
    layout = qt.QFormLayout()
    frame.setLayout( layout )
    self.parent.layout().addWidget( frame )

    # Dose Volume selector
    self.inputDoseVolumeSelectorLabel = qt.QLabel()
    self.inputDoseVolumeSelectorLabel.setText( "Input Dose Volume: " )
    self.inputDoseVolumeSelector = slicer.qMRMLNodeComboBox()
    self.inputDoseVolumeSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.inputDoseVolumeSelector.noneEnabled = False
    self.inputDoseVolumeSelector.addEnabled = False
    self.inputDoseVolumeSelector.removeEnabled = False
    self.inputDoseVolumeSelector.selectNodeUponCreation = True
    self.inputDoseVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.inputDoseVolumeSelector.setToolTip( "Set the input dose volume" )
    layout.addRow(self.inputDoseVolumeSelectorLabel, self.inputDoseVolumeSelector)

    # Dose Volume selector
    self.outputDoseVolumeSelectorLabel = qt.QLabel()
    self.outputDoseVolumeSelectorLabel.setText( "Output Dose Volume: " )
    self.outputDoseVolumeSelector = slicer.qMRMLNodeComboBox()
    self.outputDoseVolumeSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.outputDoseVolumeSelector.noneEnabled = False
    self.outputDoseVolumeSelector.addEnabled = True
    self.outputDoseVolumeSelector.removeEnabled = False
    self.outputDoseVolumeSelector.selectNodeUponCreation = True
    self.outputDoseVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.outputDoseVolumeSelector.setToolTip( "Set the output dose volume" )
    layout.addRow(self.outputDoseVolumeSelectorLabel, self.outputDoseVolumeSelector)

    # kernel SD value editor
    self.kernelSDValueLabel = qt.QLabel('Kernel SD value:')
    self.kernelSDValueEdit = qt.QDoubleSpinBox()
    self.kernelSDValueEdit.value = 0
    layout.addRow(self.kernelSDValueLabel, self.kernelSDValueEdit)

    # Apply button
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Convolve Dose"
    layout.addWidget(self.applyButton)
    self.UpdateApplyButtonState()

    # connections
    self.applyButton.connect('clicked()', self.onApply)
    self.inputDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onDoseVolumeSelect)
    self.outputDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onDoseVolumeSelect)

  def UpdateApplyButtonState(self):
    if not self.inputDoseVolumeSelector.currentNode() or not self.outputDoseVolumeSelector.currentNode() :
      self.applyButton.enabled = False
    else:
      self.applyButton.enabled = True

  def onDoseVolumeSelect(self, node):
    self.UpdateApplyButtonState()

  def onApply(self):
    self.applyButton.text = "Working..."
    self.applyButton.repaint()
    slicer.app.processEvents()
    self.logic = DoseVolumeConvolutionLogic()
    self.logic.run(self.inputDoseVolumeSelector.currentNode(), self.outputDoseVolumeSelector.currentNode(), self.kernelSDValueEdit.value)
    self.applyButton.text = "Apply"

#
# Logic
#

class DoseVolumeConvolutionLogic:
  """Implement the logic to fill the ROI.
  Nodes are passed in as arguments.
  Results are stored as 'statistics' instance variable.
  """
  def __init__(self):
    pass

  def run(self, inputVolumeNode, outputVolumeNode, kernelSDValue):

    inputImageData=inputVolumeNode.GetImageData()

    # Apply image convolution to input imagedata
    imageConvolve = vtk.vtkImageConvolve()
    imageConvolve.SetInput(inputImageData)
    kernel = [3, 3, 3, 
              3, 3, 3, 
              3, 3, 3,
              3, 3, 3,
              3, 5, 3,
              3, 3, 3,
              3, 3, 3, 
              3, 3, 3,
              3, 3, 3]
    imageConvolve.SetKernel3x3x3(kernel)
    imageConvolve.Update()

    # Update the volume with convolutionl operation result
    outputVolumeNode.CopyOrientation(inputVolumeNode)
    outputImageData=vtk.vtkImageData()
    outputImageData = imageConvolve.GetOutput()
    outputVolumeNode.SetAndObserveImageData(outputImageData)