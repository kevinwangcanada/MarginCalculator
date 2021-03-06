import os
import unittest
import string
from __main__ import vtk, qt, ctk, slicer

#
# Margin calculator module
#

class MarginCalculator:
  def __init__(self, parent):
    parent.title = "Margin Calculator"
    parent.categories = ["Margin Calculator"]
    parent.dependencies = []
    parent.contributors = ["Kevin Wang (Princess Margaret Cancer Centre)"]
    parent.helpText = string.Template("""
    Use this module to calculate the margin required for coverage in population. 
""").substitute({ 'a':parent.slicerWikiUrl, 'b':slicer.app.majorVersion, 'c':slicer.app.minorVersion })
    parent.acknowledgementText = """
    Supported by OCAIRO and Cancer Care Ontario.  Module implemented by Kevin Wang.
    """
    self.parent = parent

#
# Widget
#

class MarginCalculatorWidget:

  def __init__(self, parent=None):
    self.chartOptions = ("System", "Random", "P90", "P95", "P99")
    self.tableWidget = qt.QTableWidget()
    self.items = []
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
    self.layout = self.parent.layout()
    self.logic = None
    self.fileDialog = None
    self.numberOfSimulations = 100
    self.numberOfFractions = 1
    self.systematicErrorRange = 0.5
    self.randomErrorRange = 0.5
    self.doseGrowRange = 5
    self.ROIRadiusX = 10
    self.ROIRadiusY = 10
    self.ROIRadiusZ = 10
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
    self.marginCalculationCollapsibleButton = ctk.ctkCollapsibleButton()
    self.marginCalculationCollapsibleButton.text = "Margin Calculation"
    self.layout.addWidget(self.marginCalculationCollapsibleButton)

    # Layout within the collapsible button
    self.marginCalculationFormLayout = qt.QFormLayout(self.marginCalculationCollapsibleButton)

    # Input Dose Volume selector
    self.inputDoseVolumeSelectorLabel = qt.QLabel('Input Dose Volume:')
    self.inputDoseVolumeSelector = slicer.qMRMLNodeComboBox()
    self.inputDoseVolumeSelector.nodeTypes = ['vtkMRMLScalarVolumeNode']
    self.inputDoseVolumeSelector.noneEnabled = False
    self.inputDoseVolumeSelector.addEnabled = False
    self.inputDoseVolumeSelector.removeEnabled = False
    self.inputDoseVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.inputDoseVolumeSelector.setToolTip( "Set the input Dose volume" )
    self.marginCalculationFormLayout.addRow(self.inputDoseVolumeSelectorLabel, self.inputDoseVolumeSelector)

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
    self.marginCalculationFormLayout.addRow(self.referenceDoseVolumeSelectorLabel, self.referenceDoseVolumeSelector)

    # Input Contour selector
    self.inputContourSelectorLabel = qt.QLabel('Input Contour Labelmap:')
    self.inputContourSelector = slicer.qMRMLNodeComboBox()
    self.inputContourSelector.nodeTypes = ['vtkMRMLScalarVolumeNode']
    self.inputContourSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", 1 )
    self.inputContourSelector.noneEnabled = False
    self.inputContourSelector.addEnabled = False
    self.inputContourSelector.removeEnabled = False
    self.inputContourSelector.setMRMLScene( slicer.mrmlScene )
    self.inputContourSelector.setToolTip( "Set the input Contour" )
    self.marginCalculationFormLayout.addRow(self.inputContourSelectorLabel, self.inputContourSelector)

    # Number of simulations
    self.numberOfSimulationsSlider = ctk.ctkSliderWidget()
    self.numberOfSimulationsSlider.decimals = 0
    self.numberOfSimulationsSlider.minimum = 0
    self.numberOfSimulationsSlider.maximum = 2000
    self.numberOfSimulationsSlider.singleStep = 1
    self.numberOfSimulationsSlider.pageStep = 100
    self.numberOfSimulationsSlider.value = 100
    self.marginCalculationFormLayout.addRow("Number of Simulations: ", self.numberOfSimulationsSlider)

    # Number of fractions
    self.numberOfFractionsSlider = ctk.ctkSliderWidget()
    self.numberOfFractionsSlider.decimals = 0
    self.numberOfFractionsSlider.minimum = 0
    self.numberOfFractionsSlider.maximum = 10
    self.numberOfFractionsSlider.singleStep = 1
    self.numberOfFractionsSlider.pageStep = 5
    self.numberOfFractionsSlider.value = 1
    self.marginCalculationFormLayout.addRow("Number of Fractions: ", self.numberOfFractionsSlider)

    # Systematic error slider
    self.systematicErrorRangeSlider = ctk.ctkSliderWidget()
    self.systematicErrorRangeSlider.decimals = 1
    self.systematicErrorRangeSlider.minimum = 0
    self.systematicErrorRangeSlider.maximum = 5
    self.systematicErrorRangeSlider.singleStep = 0.1
    self.systematicErrorRangeSlider.pageStep = 1.0
    self.systematicErrorRangeSlider.value = 0.5
    self.marginCalculationFormLayout.addRow("Systematic Error Range (mm): ", self.systematicErrorRangeSlider)

    # Random error slider
    self.randomErrorRangeSlider = ctk.ctkSliderWidget()
    self.randomErrorRangeSlider.decimals = 1
    self.randomErrorRangeSlider.minimum = 0
    self.randomErrorRangeSlider.maximum = 5
    self.randomErrorRangeSlider.singleStep = 0.1
    self.randomErrorRangeSlider.pageStep = 1.0
    self.randomErrorRangeSlider.value = 0.5
    self.marginCalculationFormLayout.addRow("Random Error Range (mm): ", self.randomErrorRangeSlider)

    self.doseGrowingOptions = ("Dilation","Scaling")
    #
    #Initialization
    #
    self.groupBoxDoseGrow = qt.QGroupBox("Dose Growing Technique")
    self.groupBoxLayout = qt.QHBoxLayout(self.groupBoxDoseGrow)
    self.marginCalculationFormLayout.addRow(self.groupBoxDoseGrow)

    #
    # layout selection
    #
    for doseGrowingOption in self.doseGrowingOptions:
      doseGrowingOptionButton = qt.QRadioButton(doseGrowingOption)
      doseGrowingOptionButton.connect('clicked()', lambda dgo=doseGrowingOption: self.selectDoseGrowingOption(dgo))
      self.groupBoxLayout.addWidget(doseGrowingOptionButton)
      if doseGrowingOption == "Dilation":
        doseGrowingOptionButton.checked = True
      self.doseGrowingOption = "Dilation"
    #self.groupBoxLayout.addRow("Layout", layoutHolder)

    # Dose growing range slider
    self.doseGrowRangeSlider = ctk.ctkSliderWidget()
    self.doseGrowRangeSlider.decimals = 1
    self.doseGrowRangeSlider.minimum = 0
    self.doseGrowRangeSlider.maximum = 10
    self.doseGrowRangeSlider.singleStep = 0.2
    self.doseGrowRangeSlider.pageStep = 1.0
    self.doseGrowRangeSlider.value = 5
    self.marginCalculationFormLayout.addRow("Dose grow Range (mm): ", self.doseGrowRangeSlider)

    # ROI radius x slider
    self.ROIRadiusXSlider = ctk.ctkSliderWidget()
    self.ROIRadiusXSlider.decimals = 1
    self.ROIRadiusXSlider.minimum = 2
    self.ROIRadiusXSlider.maximum = 30
    self.ROIRadiusXSlider.singleStep = 0.1
    self.ROIRadiusXSlider.pageStep = 2.0
    self.ROIRadiusXSlider.value = 10
    self.marginCalculationFormLayout.addRow("ROI Radius X(mm): ", self.ROIRadiusXSlider)

    # ROI radius y slider
    self.ROIRadiusYSlider = ctk.ctkSliderWidget()
    self.ROIRadiusYSlider.decimals = 1
    self.ROIRadiusYSlider.minimum = 2
    self.ROIRadiusYSlider.maximum = 30
    self.ROIRadiusYSlider.singleStep = 0.1
    self.ROIRadiusYSlider.pageStep = 2.0
    self.ROIRadiusYSlider.value = 10
    self.marginCalculationFormLayout.addRow("ROI Radius Y(mm): ", self.ROIRadiusYSlider)

    # ROI radius z slider
    self.ROIRadiusZSlider = ctk.ctkSliderWidget()
    self.ROIRadiusZSlider.decimals = 1
    self.ROIRadiusZSlider.minimum = 2
    self.ROIRadiusZSlider.maximum = 30
    self.ROIRadiusZSlider.singleStep = 0.1
    self.ROIRadiusZSlider.pageStep = 2.0
    self.ROIRadiusZSlider.value = 10
    self.marginCalculationFormLayout.addRow("ROI Radius Z(mm): ", self.ROIRadiusZSlider)

    # Apply button
    self.applyButton = qt.QPushButton("Calculate")
    self.applyButton.toolTip = "Perform margin calculation"
    self.marginCalculationFormLayout.addRow(self.applyButton)
    self.UpdateApplyButtonState()

    # model and view for stats table
    self.tableWidget.sortingEnabled = False
    self.tableWidget.setRowCount(0)
    self.tableWidget.setColumnCount(5)
    self.tableWidget.setHorizontalHeaderLabels(['Systematic','Random','P90','P95','P99'])
    self.marginCalculationFormLayout.addRow(self.tableWidget)

    # Save button
    self.saveButton = qt.QPushButton("Save Result")
    self.saveButton.toolTip = "Save result as csv."
    self.saveButton.enabled = False
    self.marginCalculationFormLayout.addRow(self.saveButton)
    
    # Add vertical spacer
    self.layout.addStretch(1)

    # connections
    self.applyButton.connect('clicked()', self.onApply)
    self.saveButton.connect('clicked()', self.onSave)
    self.inputDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onInputDoseVolumeSelect)
    self.referenceDoseVolumeSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onReferenceDoseVolumeSelect)
    self.inputContourSelector.connect('currentNodeChanged(vtkMRMLNode*)', self.onInputContourSelect)
    self.numberOfSimulationsSlider.connect('valueChanged(double)', self.onNumberOfSimulationsChanged)
    self.numberOfFractionsSlider.connect('valueChanged(double)', self.onNumberOfFractionsChanged)
    self.systematicErrorRangeSlider.connect('valueChanged(double)', self.onSystematicErrorRangeChanged)
    self.randomErrorRangeSlider.connect('valueChanged(double)', self.onRandomErrorRangeChanged)
    self.doseGrowRangeSlider.connect('valueChanged(double)', self.onDoseGrowRangeChanged)
    self.ROIRadiusXSlider.connect('valueChanged(double)', self.onROIRadiusXChanged)
    self.ROIRadiusYSlider.connect('valueChanged(double)', self.onROIRadiusYChanged)
    self.ROIRadiusZSlider.connect('valueChanged(double)', self.onROIRadiusZChanged)

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

  def onNumberOfSimulationsChanged(self, value):
    self.numberOfSimulations = value

  def onNumberOfFractionsChanged(self, value):
    self.numberOfFractions = value

  def onSystematicErrorRangeChanged(self, value):
    self.systematicErrorRange = value

  def onRandomErrorRangeChanged(self, value):
    self.randomErrorRange = value

  def selectDoseGrowingOption(self,doseGrowingOption):
    """Keep track of the currently selected layout and trigger an update"""
    # print doseGrowingOption
    self.doseGrowingOption = doseGrowingOption

  def onDoseGrowRangeChanged(self, value):
    self.doseGrowRange = value

  def onROIRadiusXChanged(self, value):
    self.ROIRadiusX = value

  def onROIRadiusYChanged(self, value):
    self.ROIRadiusY = value

  def onROIRadiusZChanged(self, value):
    self.ROIRadiusZ = value

  def onApply(self):
    self.applyButton.text = "Working..."
    self.applyButton.repaint()
    slicer.app.processEvents()
    self.logic = MarginCalculatorLogic()
    self.logic.run(self.inputDoseVolumeSelector.currentNode(), self.referenceDoseVolumeSelector.currentNode(), self.inputContourSelector.currentNode(), self.numberOfSimulations, self.numberOfFractions, self.systematicErrorRange, self.randomErrorRange, self.doseGrowRange, self.ROIRadiusX, self.ROIRadiusY, self.ROIRadiusZ, self.doseGrowingOption)
    self.applyButton.text = "Calculate"

    marginResult = self.logic.getMarginResult()
    print marginResult
    if marginResult:
      self.saveButton.enabled = True
    numberOfRows = len(marginResult)
    self.tableWidget.clearContents()
    self.tableWidget.setRowCount(numberOfRows)
    for i in range(numberOfRows):
      item = qt.QTableWidgetItem()
      item.setText(str(marginResult[i][0]))
      self.items.append(item)
      self.tableWidget.setItem(i, 0, item )
      item2 = qt.QTableWidgetItem()
      item2.setText(str(marginResult[i][1]))
      self.items.append(item2)
      self.tableWidget.setItem(i, 1, item2 )
      item3 = qt.QTableWidgetItem()
      item3.setText(str(marginResult[i][2]))
      self.items.append(item3)
      self.tableWidget.setItem(i, 2, item3 )
      item4 = qt.QTableWidgetItem()
      item4.setText(str(marginResult[i][3]))
      self.items.append(item4)
      self.tableWidget.setItem(i, 3, item4 )
      item5 = qt.QTableWidgetItem()
      item5.setText(str(marginResult[i][4]))
      self.items.append(item5)
      self.tableWidget.setItem(i, 4, item5 )
    

  def onSave(self):
    """save the margin calculation 
    """
    if not self.fileDialog:
      self.fileDialog = qt.QFileDialog(self.parent)
      self.fileDialog.options = self.fileDialog.DontUseNativeDialog
      self.fileDialog.acceptMode = self.fileDialog.AcceptSave
      self.fileDialog.defaultSuffix = "csv"
      self.fileDialog.setNameFilter("Comma Separated Values (*.csv)")
      self.fileDialog.connect("fileSelected(QString)", self.onFileSelected)
    self.fileDialog.show()

  def onFileSelected(self,fileName):
    self.logic.saveMarginCalculation(fileName)

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
    self.__marginResult = []

  def getMarginResult(self):
    return self.__marginResult

  def run(self, inputDoseVolumeNode, referenceDoseVolumeNode, inputContourNode, numberOfSimulations, numberOfFractions, systematicErrorRange, randomErrorRange, doseGrowRange, ROIRadiusX, ROIRadiusY, ROIRadiusZ, doseGrowOption):
    radiusX = ROIRadiusX
    radiusY = ROIRadiusY
    radiusZ = ROIRadiusZ
    systematicErrorRange = int(systematicErrorRange*10)
    randomErrorRange = int(randomErrorRange*10)
    doseGrowRange = int(doseGrowRange*10)
    self.__marginResult = []
    for i in range(0,systematicErrorRange,5): # systemaic error
      systemError = i/10.0
      for j in range(0,randomErrorRange,5): # random error
        randomError = j/10.0
        D95old = 0.0
        D95 = 0.0
        P90 = 0.0
        P95 = 0.0
        P99 = 0.0
        P90found = 0
        P95found = 0
        P99found = 0
        for k in range(0,doseGrowRange,2): # dose growing
          if doseGrowOption == "Dilation":
            doseGrowSizeX = k/10.0
            doseGrowSizeY = k/10.0
            doseGrowSizeZ = k/10.0
          elif doseGrowOption == "Scaling":
            doseGrowSizeX = (k/10.0 + radiusX)/radiusX
            doseGrowSizeY = (k/10.0 + radiusY)/radiusY
            doseGrowSizeZ = (k/10.0 + radiusZ)/radiusZ
          # print systemError, randomError, doseGrowSizeX, doseGrowSizeY, doseGrowSizeZ
          # the following code is to fix the random number generator crush using std::tr1 lib for not allowing 0 sigma.
          if abs(systemError) < 0.0001:
            systemError = 0.0001
          if abs(randomError) < 0.0001:
            randomError = 0.0001
          D95 = self.computeDPH(inputDoseVolumeNode, referenceDoseVolumeNode, inputContourNode, numberOfSimulations, numberOfFractions, systemError, randomError, doseGrowSizeX, doseGrowSizeY, doseGrowSizeZ, doseGrowOption)
          #print "D95", D95
          if D95old < 0.90 and D95 >= 0.90 :
            #print "inside p90", k
            P90 = k/10.0
            P90found = 1
          if D95old < 0.95 and D95 >= 0.95 :
            #print "inside p95", k
            P95 = k/10.0
            P95found = 1
          if D95old < 0.99 and D95 >= 0.99 :
            #print "inside p99", k
            P99 = k/10.0
            P99found = 1
          D95old = D95
        if P90found == 0:
          P90 = 'N/A'
        if P95found == 0:
          P95 = 'N/A'
        if P99found == 0:
          P99 = 'N/A'
        #print "sys, rad, P90, P95, P99:", systemError, randomError, P90, P95, P99
        self.__marginResult.append([systemError, randomError, P90, P95, P99])
        
  
  def marginAsCSV(self):
    """
    print comma separated value file with header keys in quotes
    """
    csv = ""
    header = "Systematic error, Random Error, P90, P95, P99\n"
    csv = header
    for i in range(len(self.__marginResult)):
      line = ""
      for k in range(len(self.__marginResult[i])):
        line += str(self.__marginResult[i][k]) + ","
      line += "\n"
      csv += line
    return csv

  def saveMarginCalculation(self, filename):
    fp = open(filename, "w")
    fp.write(self.marginAsCSV())
    fp.close()
  
  def computeDPH(self, inputDoseVolumeNode, referenceDoseVolumeNode, inputContourNode, numberOfSimulations, numberOfFractions, systemError, randomError, doseGrowSizeX, doseGrowSizeY, doseGrowSizeZ, doseGrowOption):
    # Step 1: morph dose
    outputDoseVolumeNode = slicer.vtkMRMLScalarVolumeNode()
    slicer.mrmlScene.AddNode(outputDoseVolumeNode)
    
    from vtkSlicerDoseMorphologyModuleLogic import vtkMRMLDoseMorphologyNode
    doseMorphologyNode = vtkMRMLDoseMorphologyNode()
    slicer.mrmlScene.AddNode(doseMorphologyNode)
    
    doseMorphologyNode.SetAndObserveInputDoseVolumeNode(inputDoseVolumeNode)
    doseMorphologyNode.SetAndObserveReferenceDoseVolumeNode(referenceDoseVolumeNode)
    doseMorphologyNode.SetAndObserveOutputDoseVolumeNode(outputDoseVolumeNode)
    if doseGrowOption == "Dilation":
      doseMorphologyNode.SetOperationToExpandByDilation()
    elif doseGrowOption == "Scaling":
      doseMorphologyNode.SetOperationToExpandByScaling()
    doseMorphologyNode.SetXSize(doseGrowSizeX)
    doseMorphologyNode.SetYSize(doseGrowSizeY)
    doseMorphologyNode.SetZSize(doseGrowSizeZ)
    
    doseMorphologyLogic = slicer.modules.dosemorphology.logic()
    doseMorphologyLogic.SetAndObserveDoseMorphologyNode(doseMorphologyNode)
    doseMorphologyLogic.MorphDose()
    
    # print "finished step1"
    
    # Step 2: simulate patient motion
    # #first convert ribbon model to labelmap
    # inputContourNode.SetAndObserveRasterizationReferenceVolumeNodeId(referenceDoseVolumeNode.GetID())
    # inputContourNode.SetRasterizationOversamplingFactor(1.0)
    # tempLabelmapNode = inputContourNode.GetIndexedLabelmapVolumeNode()
    
    from vtkSlicerMotionSimulatorDoubleArrayModuleMRML import vtkMRMLMotionSimulatorDoubleArrayNode
    motionSimulatorDoubleArrayNode = vtkMRMLMotionSimulatorDoubleArrayNode()
    slicer.mrmlScene.AddNode(motionSimulatorDoubleArrayNode)
    
    from vtkSlicerMotionSimulatorModuleLogic import vtkMRMLMotionSimulatorNode
    motionSimulatorNode = vtkMRMLMotionSimulatorNode()
    slicer.mrmlScene.AddNode(motionSimulatorNode)
    
    motionSimulatorNode.SetAndObserveInputDoseVolumeNode(outputDoseVolumeNode)
    motionSimulatorNode.SetAndObserveInputContourNode(inputContourNode)
    motionSimulatorNode.SetAndObserveOutputDoubleArrayNode(motionSimulatorDoubleArrayNode)
    motionSimulatorNode.SetNumberOfSimulation(numberOfSimulations)
    motionSimulatorNode.SetNumberOfFraction(numberOfFractions)
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
    
    dosePopulationHistogramNode.SetAndObserveDoubleArrayNode(motionSimulatorDoubleArrayNode)
    dosePopulationHistogramNode.SetAndObserveDoseVolumeNode(inputDoseVolumeNode)
    dosePopulationHistogramNode.SetAndObserveContourNode(inputContourNode)
    dosePopulationHistogramNode.SetAndObserveOutputDoubleArrayNode(outputDoubleArrayNode)
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
    
#
#
#
  
class Slicelet(object):
  """A slicer slicelet is a module widget that comes up in stand alone mode
  implemented as a python class.
  This class provides common wrapper functionality used by all slicer modlets.
  """
  # TODO: put this in a SliceletLib
  # TODO: parse command line arge


  def __init__(self, widgetClass=None):
    self.parent = qt.QFrame()
    self.parent.setLayout( qt.QHBoxLayout() )

    self.sliceletPanel = qt.QFrame(self.parent)
    self.sliceletPanelLayout = qt.QVBoxLayout(self.sliceletPanel)
    self.sliceletPanelLayout.setMargin(4)
    self.sliceletPanelLayout.setSpacing(0)
    self.parent.layout().addWidget(self.sliceletPanel,1)

    # TODO: should have way to pop up python interactor
    # self.buttons = qt.QFrame()
    # self.buttons.setLayout( qt.QHBoxLayout() )
    # self.parent.layout().addWidget(self.buttons)
    # self.addDataButton = qt.QPushButton("Add Data")
    # self.buttons.layout().addWidget(self.addDataButton)
    # self.addDataButton.connect("clicked()",slicer.app.ioManager().openAddDataDialog)
    # self.loadSceneButton = qt.QPushButton("Load Scene")
    # self.buttons.layout().addWidget(self.loadSceneButton)
    # self.loadSceneButton.connect("clicked()",slicer.app.ioManager().openLoadSceneDialog)

    self.layoutManager = slicer.qMRMLLayoutWidget()
    self.layoutManager.setMRMLScene(slicer.mrmlScene)
    self.layoutManager.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutFourUpView)
    # self.layoutManager.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutConventionalView)
    # self.layoutManager.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutOneUp3DView)
    # self.layoutManager.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutTabbedSliceView)
    # self.layoutManager.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutDual3DView)
    # self.layoutManager.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpRedSliceView)
    self.parent.layout().addWidget(self.layoutManager,2)

    if widgetClass:
      self.widget = widgetClass(self.sliceletPanel)
      self.widget.setup()
    self.parent.show()



class MarginCalculatorSlicelet(Slicelet):
  """ Creates the interface when module is run as a stand alone gui app.
  """

  def __init__(self):
    super(MarginCalculatorSlicelet,self).__init__(MarginCalculatorWidget)


if __name__ == "__main__":
  # TODO: need a way to access and parse command line arguments
  # TODO: ideally command line args should handle --xml

  import sys
  print( sys.argv )

  slicelet = MarginCalculatorSlicelet()