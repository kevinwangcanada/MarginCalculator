import os
import unittest
import string
from __main__ import vtk, qt, ctk, slicer

#
# Margin caluclator module
#

class MarginCalculator:
  def __init__(self, parent):
    parent.title = "Margin Calculator"
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

class MarginCalculatorWidget:

  def __init__(self, parent=None):
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
    self.logic = None
    self.layout = self.parent.layout()
    if not parent:
      self.setup()
      self.parent.show()

  def setup(self):
    # Instantiate and connect widgets ...

    #
    # Reload and Test area
    #
    reloadCollapsibleButton = ctk.ctkCollapsibleButton()
    reloadCollapsibleButton.text = "Reload && Test"
    self.layout.addWidget(reloadCollapsibleButton)
    reloadFormLayout = qt.QFormLayout(reloadCollapsibleButton)

    # reload button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadButton = qt.QPushButton("Reload")
    self.reloadButton.toolTip = "Reload this module."
    self.reloadButton.name = "MarginCalculator Reload"
    reloadFormLayout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.onReload)

    # reload and test button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadAndTestButton = qt.QPushButton("Reload and Test")
    self.reloadAndTestButton.toolTip = "Reload this module and then run the self tests."
    reloadFormLayout.addWidget(self.reloadAndTestButton)
    self.reloadAndTestButton.connect('clicked()', self.onReloadAndTest)

    # Collapsible button
    self.inputCollapsibleButton = ctk.ctkCollapsibleButton()
    self.inputCollapsibleButton.text = "Inputs"
    self.layout.addWidget(self.inputCollapsibleButton)

    # Layout within the collapsible button
    self.inputFormLayout = qt.QFormLayout(self.inputCollapsibleButton)

    # Input Dose Volume selector
    self.inputDoseVolumeSelectorLabel = qt.QLabel('Input Dose Volume:')
    self.inputDoseVolumeSelector = slicer.qMRMLNodeComboBox()
    self.inputDoseVolumeSelector.nodeTypes = ['vtkMRMLScalarVolumeNode']
    self.inputDoseVolumeSelector.noneEnabled = False
    self.inputDoseVolumeSelector.addEnabled = False
    self.inputDoseVolumeSelector.removeEnabled = False
    self.inputDoseVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.inputDoseVolumeSelector.setToolTip( "Set the input Dose volume" )
    self.inputFormLayout.addRow(self.inputDoseVolumeSelectorLabel, self.inputDoseVolumeSelector)

    # Reference Dose Volume selector
    self.referenceDoseVolumeSelectorLabel = qt.QLabel()
    self.referenceDoseVolumeSelectorLabel.setText( "Reference Dose Volume: " )
    self.referenceDoseVolumeSelector = slicer.qMRMLNodeComboBox()
    self.referenceDoseVolumeSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.referenceDoseVolumeSelector.noneEnabled = False
    self.referenceDoseVolumeSelector.addEnabled = False
    self.referenceDoseVolumeSelector.removeEnabled = False
    self.referenceDoseVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.referenceDoseVolumeSelector.setToolTip( "Set the Reference dose volume" )
    self.inputFormLayout.addRow(self.referenceDoseVolumeSelectorLabel, self.referenceDoseVolumeSelector)

    # Input Contour selector
    self.inputContourSelectorLabel = qt.QLabel('Input Contour:')
    self.inputContourSelector = slicer.qMRMLNodeComboBox()
    self.inputContourSelector.nodeTypes = ['vtkMRMLContourNode']
    self.inputContourSelector.noneEnabled = False
    self.inputContourSelector.addEnabled = False
    self.inputContourSelector.removeEnabled = False
    self.inputContourSelector.setMRMLScene( slicer.mrmlScene )
    self.inputContourSelector.setToolTip( "Set the input Contour" )
    self.inputFormLayout.addRow(self.inputContourSelectorLabel, self.inputContourSelector)

    # Apply button
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Perform margin calculation"
    self.layout.addWidget(self.applyButton)
    self.UpdateApplyButtonState()

    # Add vertical spacer
    self.layout.addStretch(1)

    # connections
    self.applyButton.connect('clicked()', self.onApply)
    self.inputDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onInputDoseVolumeSelect)
    self.referenceDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onReferenceDoseVolumeSelect)
    self.inputContourSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onInputContourSelect)

  def UpdateApplyButtonState(self):
    if not self.inputDoseVolumeSelector.currentNode() or not self.referenceDoseVolumeSelector.currentNode() or not self.inputContourSelector.currentNode():
      self.applyButton.enabled = False
    else:
      self.applyButton.enabled = True

  def onInputDoseVolumeSelect(self, node):
    self.UpdateApplyButtonState()

  def onReferenceDoseVolumeSelect(self, node):
    self.UpdateApplyButtonState()

  def onInputContourSelect(self, node):
    self.UpdateApplyButtonState()

  def onApply(self):
    #self.applyButton.text = "Working..."
    self.applyButton.repaint()
    slicer.app.processEvents()
    self.logic = MarginCalculatorLogic()
    self.logic.run(self.inputDoseVolumeSelector.currentNode(), self.referenceDoseVolumeSelector.currentNode(), self.inputContourSelector.currentNode())
    #self.applyButton.text = "Apply"

  def onReload(self,moduleName="MarginCalculator"):
    """Generic reload method for any scripted module.
    ModuleWizard will subsitute correct default moduleName.
    """
    import imp, sys, os, slicer

    widgetName = moduleName + "Widget"

    # reload the source code
    # - set source file path
    # - load the module to the global space
    filePath = eval('slicer.modules.%s.path' % moduleName.lower())
    p = os.path.dirname(filePath)
    if not sys.path.__contains__(p):
      sys.path.insert(0,p)
    fp = open(filePath, "r")
    globals()[moduleName] = imp.load_module(
        moduleName, fp, filePath, ('.py', 'r', imp.PY_SOURCE))
    fp.close()

    # rebuild the widget
    # - find and hide the existing widget
    # - create a new widget in the existing parent
    parent = slicer.util.findChildren(name='%s Reload' % moduleName)[0].parent().parent()
    for child in parent.children():
      try:
        child.hide()
      except AttributeError:
        pass
    # Remove spacer items
    item = parent.layout().itemAt(0)
    while item:
      parent.layout().removeItem(item)
      item = parent.layout().itemAt(0)
    # create new widget inside existing parent
    globals()[widgetName.lower()] = eval(
        'globals()["%s"].%s(parent)' % (moduleName, widgetName))
    globals()[widgetName.lower()].setup()

  def onReloadAndTest(self,moduleName="MarginCalculator"):
    try:
      self.onReload()
      evalString = 'globals()["%s"].%sTest()' % (moduleName, moduleName)
      tester = eval(evalString)
      tester.runTest()
    except Exception, e:
      import traceback
      traceback.print_exc()
      qt.QMessageBox.warning(slicer.util.mainWindow(), 
          "Reload and Test", 'Exception!\n\n' + str(e) + "\n\nSee Python Console for Stack Trace")


#
# Logic
#

class MarginCalculatorLogic:
  """Implement the logic to fill the ROI.
  Nodes are passed in as arguments.
  Results are stored as 'statistics' instance variable.
  """
  def __init__(self):
    pass
  
  def run(self, inputDoseVolumeNode, referenceDoseVolumeNode, inputContourNode):
    radius = 5.0
    for i in range(1,10): # systemaic error
      systemError = i/10.0
      for j in range(1,2): # random error
        randomError = j/10.0
        D95old = 0.0
        D95 = 0.0
        P90 = 0.0
        P95 = 0.0
        P99 = 0.0
        for k in range(1,20,2): # dose growing
          doseGrowSize = (k/10.0 + radius)/radius
          # print systemError, randomError, doseGrowSize
          D95 = self.computeDPH(inputDoseVolumeNode, referenceDoseVolumeNode, inputContourNode, systemError, randomError, doseGrowSize)
          #print "D95", D95
          if D95old < 0.90 and D95 >= 0.90 :
            #print "inside p90", k
            P90 = k/10.0
          if D95old < 0.95 and D95 >= 0.95 :
            #print "inside p95", k
            P95 = k/10.0
          if D95old < 0.99 and D95 >= 0.99 :
            #print "inside p99", k
            P99 = k/10.0
          D95old = D95
        print "sys, rad, P90, P95, P99:", systemError, randomError, P90, P95, P99
        
  def computeDPH(self, inputDoseVolumeNode, referenceDoseVolumeNode, inputContourNode, systemError, randomError, doseGrowSize):
    # Step 1: morph dose
    outputDoseVolumeNode = slicer.vtkMRMLScalarVolumeNode()
    slicer.mrmlScene.AddNode(outputDoseVolumeNode)
    
    from vtkSlicerDoseMorphologyModuleLogic import vtkMRMLDoseMorphologyNode
    doseMorphologyNode = vtkMRMLDoseMorphologyNode()
    slicer.mrmlScene.AddNode(doseMorphologyNode)
    
    doseMorphologyNode.SetAndObserveInputDoseVolumeNodeID(inputDoseVolumeNode.GetID())
    doseMorphologyNode.SetAndObserveReferenceDoseVolumeNodeID(referenceDoseVolumeNode.GetID())
    doseMorphologyNode.SetAndObserveOutputDoseVolumeNodeID(outputDoseVolumeNode.GetID())
    doseMorphologyNode.SetOperationToExpandByScaling()
    doseMorphologyNode.SetXSize(doseGrowSize)
    doseMorphologyNode.SetYSize(doseGrowSize)
    doseMorphologyNode.SetZSize(doseGrowSize)
    
    doseMorphologyLogic = slicer.modules.dosemorphology.logic()
    doseMorphologyLogic.SetAndObserveDoseMorphologyNode(doseMorphologyNode)
    doseMorphologyLogic.MorphDose()
    
    # print "finished step1"
    
    # Step 2: simulate patient motion
    # first convert ribbon model to labelmap
    #print inputContourNode.__class__
    inputContourNode.SetAndObserveRasterizationReferenceVolumeNodeId(referenceDoseVolumeNode.GetID())
    inputContourNode.SetRasterizationOversamplingFactor(1.0)
    tempLabelmapNode = inputContourNode.GetIndexedLabelmapVolumeNode()
    #print tempLabelmapNode.__class__
    
    from vtkSlicerMotionSimulatorDoubleArrayModuleMRML import vtkMRMLMotionSimulatorDoubleArrayNode
    motionSimulatorDoubleArrayNode = vtkMRMLMotionSimulatorDoubleArrayNode()
    slicer.mrmlScene.AddNode(motionSimulatorDoubleArrayNode)
    
    from vtkSlicerMotionSimulatorModuleLogic import vtkMRMLMotionSimulatorNode
    motionSimulatorNode = vtkMRMLMotionSimulatorNode()
    slicer.mrmlScene.AddNode(motionSimulatorNode)
    
    motionSimulatorNode.SetAndObserveInputDoseVolumeNodeID(outputDoseVolumeNode.GetID())
    motionSimulatorNode.SetAndObserveInputContourNodeID(inputContourNode.GetID())
    motionSimulatorNode.SetAndObserveOutputDoubleArrayNodeID(motionSimulatorDoubleArrayNode.GetID())
    motionSimulatorNode.SetNumberOfSimulation(1000)
    motionSimulatorNode.SetNumberOfFraction(1)
    motionSimulatorNode.SetXSysSD(systemError)
    motionSimulatorNode.SetYSysSD(systemError)
    motionSimulatorNode.SetZSysSD(systemError)
    motionSimulatorNode.SetXRdmSD(randomError)
    motionSimulatorNode.SetYRdmSD(randomError)
    motionSimulatorNode.SetZRdmSD(randomError)
    
    motionSimualtorLogic = slicer.modules.motionsimulator.logic()
    motionSimualtorLogic.SetAndObserveMotionSimulatorNode(motionSimulatorNode)
    motionSimualtorLogic.RunSimulation()
    
    # print "finished step2"
    
    # Step 3: dose population histogram

    from vtkSlicerDosePopulationHistogramModuleLogic import vtkMRMLDosePopulationHistogramNode
    dosePopulationHistogramNode = vtkMRMLDosePopulationHistogramNode()
    slicer.mrmlScene.AddNode(dosePopulationHistogramNode)
    
    outputDoubleArrayNode = slicer.vtkMRMLDoubleArrayNode()
    slicer.mrmlScene.AddNode(outputDoubleArrayNode)
    
    dosePopulationHistogramNode.SetAndObserveDoubleArrayNodeID(motionSimulatorDoubleArrayNode.GetID())
    dosePopulationHistogramNode.SetAndObserveDoseVolumeNodeID(outputDoseVolumeNode.GetID())
    dosePopulationHistogramNode.SetAndObserveContourNodeID(inputContourNode.GetID())
    dosePopulationHistogramNode.SetAndObserveOutputDoubleArrayNodeID(outputDoubleArrayNode.GetID())
    dosePopulationHistogramNode.SetUseDoseOptionToD98()
    
    dosePopulationHistogramLogic = slicer.modules.dosepopulationhistogram.logic()
    dosePopulationHistogramLogic.SetAndObserveDosePopulationHistogramNode(dosePopulationHistogramNode)
    dosePopulationHistogramLogic.ComputeDPH()
 
    # print "finished step3"
    
    arrayNode = outputDoubleArrayNode
    doubleArray = arrayNode.GetArray()
    numberOfTuples = doubleArray.GetNumberOfTuples()
    value = doubleArray.GetComponent(97, 1)
    
    # remove all the nodes
    slicer.mrmlScene.RemoveNode(outputDoseVolumeNode)
    del outputDoseVolumeNode
    slicer.mrmlScene.RemoveNode(doseMorphologyNode)
    del doseMorphologyNode
    slicer.mrmlScene.RemoveNode(motionSimulatorDoubleArrayNode)
    del motionSimulatorDoubleArrayNode
    slicer.mrmlScene.RemoveNode(motionSimulatorNode)
    del motionSimulatorNode
    slicer.mrmlScene.RemoveNode(dosePopulationHistogramNode)
    del dosePopulationHistogramNode
    slicer.mrmlScene.RemoveNode(outputDoubleArrayNode)
    del outputDoubleArrayNode
    
    return value/100.0
    """
    for i in range(0, numberOfTuples):
      x = doubleArray.GetComponent(i, 0)
      y = doubleArray.GetComponent(i, 1)
      value = doubleArray.GetComponent(i, 2)
      print "x,y,v", x, y, value
    """