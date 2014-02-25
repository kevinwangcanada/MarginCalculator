import os
import unittest
from __main__ import vtk, qt, ctk, slicer

#
# SyntheticRTDose
#

class SyntheticRTDose:
  def __init__(self, parent):
    parent.title = "Synthetic RT Dose" # TODO make this more human readable by adding spaces
    parent.categories = ["Margin Calculator"]
    parent.dependencies = []
    parent.contributors = ["Kevin Wang (Princess Margaret Cancer Centre)"] # replace with "Firstname Lastname (Org)"
    parent.helpText = """
    This is an example of scripted loadable module bundled in an extension.
    """
    parent.acknowledgementText = """
    This file was originally developed by Kevin Wang, PMH. and was partially funded by OCAIRO.
""" # replace with organization, grant and thanks.
    self.parent = parent

    # Add this test to the SelfTest module's list for discovery when the module
    # is created.  Since this module may be discovered before SelfTests itself,
    # create the list if it doesn't already exist.
    try:
      slicer.selfTests
    except AttributeError:
      slicer.selfTests = {}
    slicer.selfTests['SyntheticRTDose'] = self.runTest

  def runTest(self):
    tester = SyntheticRTDoseTest()
    tester.runTest()

#
# qSyntheticRTDoseWidget
#

class SyntheticRTDoseWidget:
  def __init__(self, parent = None):
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
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
    self.reloadButton.name = "SyntheticRTDose Reload"
    reloadFormLayout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.onReload)

    # reload and test button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadAndTestButton = qt.QPushButton("Reload and Test")
    self.reloadAndTestButton.toolTip = "Reload this module and then run the self tests."
    reloadFormLayout.addWidget(self.reloadAndTestButton)
    self.reloadAndTestButton.connect('clicked()', self.onReloadAndTest)

    #
    # Parameters Area
    #
    parametersCollapsibleButton = ctk.ctkCollapsibleButton()
    parametersCollapsibleButton.text = "Parameters"
    self.layout.addWidget(parametersCollapsibleButton)

    # Layout within the dummy collapsible button
    parametersFormLayout = qt.QFormLayout(parametersCollapsibleButton)

    #
    # reference volume selector
    #
    self.inputSelector = slicer.qMRMLNodeComboBox()
    self.inputSelector.nodeTypes = ( ("vtkMRMLScalarVolumeNode"), "" )
    self.inputSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", 0 )
    self.inputSelector.selectNodeUponCreation = True
    self.inputSelector.addEnabled = False
    self.inputSelector.removeEnabled = False
    self.inputSelector.noneEnabled = False
    self.inputSelector.showHidden = False
    self.inputSelector.showChildNodeTypes = False
    self.inputSelector.setMRMLScene( slicer.mrmlScene )
    self.inputSelector.setToolTip( "Pick the reference dose volume." )
    parametersFormLayout.addRow("Reference Dose Volume: ", self.inputSelector)

    #
    # output dose volume selector
    #
    self.outputSelector = slicer.qMRMLNodeComboBox()
    self.outputSelector.nodeTypes = ( ("vtkMRMLScalarVolumeNode"), "" )
    self.outputSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", 0 )
    self.outputSelector.selectNodeUponCreation = False
    self.outputSelector.addEnabled = True
    self.outputSelector.removeEnabled = True
    self.outputSelector.noneEnabled = False
    self.outputSelector.showHidden = False
    self.outputSelector.showChildNodeTypes = False
    self.outputSelector.setMRMLScene( slicer.mrmlScene )
    self.outputSelector.setToolTip( "Pick the output dose volume." )
    parametersFormLayout.addRow("Output Dose Volume: ", self.outputSelector)

    #
    # output maskvolume selector
    #
    self.outputMaskSelector = slicer.qMRMLNodeComboBox()
    self.outputMaskSelector.nodeTypes = ( ("vtkMRMLScalarVolumeNode"), "" )
    self.outputMaskSelector.addAttribute( "vtkMRMLScalarVolumeNode", "LabelMap", 1 )
    self.outputMaskSelector.selectNodeUponCreation = False
    self.outputMaskSelector.addEnabled = True
    self.outputMaskSelector.removeEnabled = True
    self.outputMaskSelector.noneEnabled = False
    self.outputMaskSelector.showHidden = False
    self.outputMaskSelector.showChildNodeTypes = False
    self.outputMaskSelector.setMRMLScene( slicer.mrmlScene )
    self.outputMaskSelector.setToolTip( "Pick the output mask volume." )
    parametersFormLayout.addRow("Output Mask Volume: ", self.outputMaskSelector)

    #
    # effective dose radius
    #
    self.doseRadiusSliderWidget = ctk.ctkSliderWidget()
    self.doseRadiusSliderWidget.singleStep = 1.0
    self.doseRadiusSliderWidget.minimum = 5.0
    self.doseRadiusSliderWidget.maximum = 50.0
    self.doseRadiusSliderWidget.value = 1.0
    self.doseRadiusSliderWidget.setToolTip("Set effective radius for min dose.")
    parametersFormLayout.addRow("Radius for Min Dose:", self.doseRadiusSliderWidget)

    #
    # effective dose radius
    #
    self.doseValueSliderWidget = ctk.ctkSliderWidget()
    self.doseValueSliderWidget.singleStep = 1.0
    self.doseValueSliderWidget.minimum = 10.0
    self.doseValueSliderWidget.maximum = 70.0
    self.doseValueSliderWidget.value = 50.0
    self.doseValueSliderWidget.setToolTip("Set dose.")
    parametersFormLayout.addRow("Dose Value:", self.doseValueSliderWidget)

    #
    # Apply Button
    #
    self.applyButton = qt.QPushButton("Apply")
    self.applyButton.toolTip = "Run the algorithm."
    self.applyButton.enabled = False
    parametersFormLayout.addRow(self.applyButton)

    # connections
    self.applyButton.connect('clicked(bool)', self.onApplyButton)
    self.inputSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.outputSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.outputMaskSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)

    # Add vertical spacer
    self.layout.addStretch(1)

  def cleanup(self):
    pass

  def onSelect(self):
    self.applyButton.enabled = self.inputSelector.currentNode() and self.outputSelector.currentNode() and self.outputMaskSelector.currentNode() 

  def onApplyButton(self):
    logic = SyntheticRTDoseLogic()
    doseRadius = int(self.doseRadiusSliderWidget.value)
    doseValue = int(self.doseValueSliderWidget.value)
    print("Run the algorithm")
    logic.run(self.inputSelector.currentNode(), self.outputSelector.currentNode(), self.outputMaskSelector.currentNode() , doseRadius, doseValue)

  def onReload(self,moduleName="SyntheticRTDose"):
    """Generic reload method for any scripted module.
    ModuleWizard will subsitute correct default moduleName.
    """
    globals()[moduleName] = slicer.util.reloadScriptedModule(moduleName)

  def onReloadAndTest(self,moduleName="SyntheticRTDose"):
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
# SyntheticRTDoseLogic
#

class SyntheticRTDoseLogic:
  """This class should implement all the actual 
  computation done by your module.  The interface 
  should be such that other python code can import
  this class and make use of the functionality without
  requiring an instance of the Widget
  """
  def __init__(self):
    pass

  def hasImageData(self,volumeNode):
    """This is a dummy logic method that 
    returns true if the passed in volume
    node has valid image data
    """
    if not volumeNode:
      print('no volume node')
      return False
    if volumeNode.GetImageData() == None:
      print('no image data')
      return False
    return True

  def delayDisplay(self,message,msec=1000):
    #
    # logic version of delay display
    #
    print(message)
    self.info = qt.QDialog()
    self.infoLayout = qt.QVBoxLayout()
    self.info.setLayout(self.infoLayout)
    self.label = qt.QLabel(message,self.info)
    self.infoLayout.addWidget(self.label)
    qt.QTimer.singleShot(msec, self.info.close)
    self.info.exec_()

  def run(self,inputVolume,outputVolume,maskVolume,doseRadius=10, doseValue=50):
    """
    Run the actual algorithm
    """

    self.delayDisplay('Running the aglorithm')

    self.createSyntheticDose(inputVolume, outputVolume, doseRadius, doseValue)
    self.createSyntheticMask(inputVolume, maskVolume, doseRadius)

    return True

  def createSyntheticDose(self, refVolumeNode, doseVolumeNode, radius, value):
    """
    Run the actual algorithm
    """
    
    roiSphere = vtk.vtkSphere()
    roiCenter = [0, 0, 0]
    roiOrigin = [0, 0, 0]
    roiSpacing = [1, 1, 1]
    roiDimension = [1, 1, 1]
    roiDimension = refVolumeNode.GetImageData().GetDimensions()
    roiCenter[0] = roiOrigin[0] + roiDimension[0]*roiSpacing[0]/2
    roiCenter[1] = roiOrigin[1] + roiDimension[1]*roiSpacing[1]/2
    roiCenter[2] = roiOrigin[2] + roiDimension[2]*roiSpacing[2]/2
    print "roiCenter", roiCenter

    ijkToRas = vtk.vtkMatrix4x4()
    refVolumeNode.GetIJKToRASMatrix( ijkToRas )
    roiPoint = [0,0,0,1];
    roiPoint[0] = roiCenter[0]
    roiPoint[1] = roiCenter[1]
    roiPoint[2] = roiCenter[2]
    roiPoint = ijkToRas.MultiplyPoint(roiPoint)
    roiCenter[0] = roiPoint[0]
    roiCenter[1] = roiPoint[1]
    roiCenter[2] = roiPoint[2]

    print "roiCenter", roiCenter
    
    roiSphere.SetCenter( roiCenter )
    roiSphere.SetRadius( radius+1 )

    # Determine the transform between the box and the image IJK coordinate systems

    rasToSphere = vtk.vtkMatrix4x4()
    # if roiNode.GetTransformNodeID() != None:
      # roiSphereTransformNode = slicer.mrmlScene.GetNodeByID(roiNode.GetTransformNodeID())
      # sphereToRas = vtk.vtkMatrix4x4()
      # roiSphereTransformNode.GetMatrixTransformToWorld(sphereToRas)
      # rasToSphere.DeepCopy(sphereToRas)
      # rasToSphere.Invert()

    ijkToSphere = vtk.vtkMatrix4x4()
    vtk.vtkMatrix4x4.Multiply4x4(rasToSphere,ijkToRas,ijkToSphere)
    ijkToSphereTransform = vtk.vtkTransform()
    ijkToSphereTransform.SetMatrix(ijkToSphere)
    roiSphere.SetTransform(ijkToSphereTransform)

    # Use the stencil to fill the volume
    refImageData = refVolumeNode.GetImageData()

    imageSource = vtk.vtkImageNoiseSource()
    imageSource.SetMinimum(0)
    imageSource.SetMaximum(0)
    imageSource.SetWholeExtent(refImageData.GetWholeExtent())
    imageSource.Update()
    
    changeInfo = vtk.vtkImageChangeInformation()
    changeInfo.SetInput(imageSource.GetOutput())
    changeInfo.SetOutputOrigin(refImageData.GetOrigin())
    changeInfo.SetOutputSpacing(refImageData.GetSpacing())
    changeInfo.Update()

    # Convert the implicit function to a stencil
    functionToStencil = vtk.vtkImplicitFunctionToImageStencil()
    functionToStencil.SetInput(roiSphere)
    functionToStencil.SetOutputOrigin(refImageData.GetOrigin())
    functionToStencil.SetOutputSpacing(refImageData.GetSpacing())
    functionToStencil.SetOutputWholeExtent(refImageData.GetWholeExtent())
    functionToStencil.Update()

    # Apply the stencil to the volume
    stencilToImage=vtk.vtkImageStencil()
    stencilToImage.SetInput(changeInfo.GetOutput())
    stencilToImage.SetStencil(functionToStencil.GetOutput())
    stencilToImage.ReverseStencilOn()
    stencilToImage.SetBackgroundValue(value)
    stencilToImage.Update()
    
    smooth = vtk.vtkImageGaussianSmooth()
    smooth.SetInput(stencilToImage.GetOutput())
    smooth.SetStandardDeviations(3,3,3) # place to change gradient size.
    smooth.SetRadiusFactors(3,3,3)
    smooth.SetDimensionality(3)
    smooth.Update()
    
    # Update the volume with the stencil operation result
    doseVolumeNode.SetAndObserveImageData(smooth.GetOutput())
    doseVolumeNode.CopyOrientation(refVolumeNode)
    doseVolumeNode.SetAttribute("DicomRtImport.DoseVolume", "1")

  def createSyntheticMask(self, refVolumeNode, maskVolumeNode, radius):
    """
    Run the actual algorithm
    """

    roiSphere = vtk.vtkSphere()
    roiCenter = [0, 0, 0]
    roiOrigin = [0, 0, 0]
    roiSpacing = [1, 1, 1]
    roiDimension = [1, 1, 1]
    roiDimension = refVolumeNode.GetImageData().GetDimensions()
    roiCenter[0] = roiOrigin[0] + roiDimension[0]*roiSpacing[0]/2
    roiCenter[1] = roiOrigin[1] + roiDimension[1]*roiSpacing[1]/2
    roiCenter[2] = roiOrigin[2] + roiDimension[2]*roiSpacing[2]/2
    print "roiCenter", roiCenter

    ijkToRas = vtk.vtkMatrix4x4()
    refVolumeNode.GetIJKToRASMatrix( ijkToRas )
    roiPoint = [0,0,0,1];
    roiPoint[0] = roiCenter[0]
    roiPoint[1] = roiCenter[1]
    roiPoint[2] = roiCenter[2]
    roiPoint = ijkToRas.MultiplyPoint(roiPoint)
    roiCenter[0] = roiPoint[0]
    roiCenter[1] = roiPoint[1]
    roiCenter[2] = roiPoint[2]

    print "roiCenter", roiCenter
    
    roiSphere.SetCenter( roiCenter )
    roiSphere.SetRadius( radius )

    # Determine the transform between the box and the image IJK coordinate systems

    rasToSphere = vtk.vtkMatrix4x4()
    # if roiNode.GetTransformNodeID() != None:
      # roiSphereTransformNode = slicer.mrmlScene.GetNodeByID(roiNode.GetTransformNodeID())
      # sphereToRas = vtk.vtkMatrix4x4()
      # roiSphereTransformNode.GetMatrixTransformToWorld(sphereToRas)
      # rasToSphere.DeepCopy(sphereToRas)
      # rasToSphere.Invert()

    ijkToSphere = vtk.vtkMatrix4x4()
    vtk.vtkMatrix4x4.Multiply4x4(rasToSphere,ijkToRas,ijkToSphere)
    ijkToSphereTransform = vtk.vtkTransform()
    ijkToSphereTransform.SetMatrix(ijkToSphere)
    roiSphere.SetTransform(ijkToSphereTransform)

    # Use the stencil to fill the volume
    refImageData = refVolumeNode.GetImageData()

    imageSource = vtk.vtkImageNoiseSource()
    imageSource.SetMinimum(0)
    imageSource.SetMaximum(0)
    imageSource.SetWholeExtent(refImageData.GetWholeExtent())
    imageSource.Update()
    
    imageCast = vtk.vtkImageCast()
    imageCast.SetInput(imageSource.GetOutput())
    imageCast.SetOutputScalarTypeToUnsignedChar()
    imageCast.Update()
   
    changeInfo = vtk.vtkImageChangeInformation()
    changeInfo.SetInput(imageCast.GetOutput())
    changeInfo.SetOutputOrigin(refImageData.GetOrigin())
    changeInfo.SetOutputSpacing(refImageData.GetSpacing())
    changeInfo.Update()

    # Convert the implicit function to a stencil
    functionToStencil = vtk.vtkImplicitFunctionToImageStencil()
    functionToStencil.SetInput(roiSphere)
    functionToStencil.SetOutputOrigin(refImageData.GetOrigin())
    functionToStencil.SetOutputSpacing(refImageData.GetSpacing())
    functionToStencil.SetOutputWholeExtent(refImageData.GetWholeExtent())
    functionToStencil.Update()

    # Apply the stencil to the volume
    stencilToImage=vtk.vtkImageStencil()
    stencilToImage.SetInput(changeInfo.GetOutput())
    stencilToImage.SetStencil(functionToStencil.GetOutput())
    stencilToImage.ReverseStencilOn()
    stencilToImage.SetBackgroundValue(1)
    stencilToImage.Update()

    # Update the volume with the stencil operation result
    maskVolumeNode.SetAndObserveImageData(stencilToImage.GetOutput())
    maskVolumeNode.CopyOrientation(refVolumeNode)

class SyntheticRTDoseTest(unittest.TestCase):
  """
  This is the test case for your scripted module.
  """

  def delayDisplay(self,message,msec=1000):
    """This utility method displays a small dialog and waits.
    This does two things: 1) it lets the event loop catch up
    to the state of the test so that rendering and widget updates
    have all taken place before the test continues and 2) it
    shows the user/developer/tester the state of the test
    so that we'll know when it breaks.
    """
    print(message)
    self.info = qt.QDialog()
    self.infoLayout = qt.QVBoxLayout()
    self.info.setLayout(self.infoLayout)
    self.label = qt.QLabel(message,self.info)
    self.infoLayout.addWidget(self.label)
    qt.QTimer.singleShot(msec, self.info.close)
    self.info.exec_()

  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    self.setUp()
    self.test_SyntheticRTDose1()

  def test_SyntheticRTDose1(self):
    """ Ideally you should have several levels of tests.  At the lowest level
    tests sould exercise the functionality of the logic with different inputs
    (both valid and invalid).  At higher levels your tests should emulate the
    way the user would interact with your code and confirm that it still works
    the way you intended.
    One of the most important features of the tests is that it should alert other
    developers when their changes will have an impact on the behavior of your
    module.  For example, if a developer removes a feature that you depend on,
    your test should break so they know that the feature is needed.
    """

    self.delayDisplay("Starting the test")
    #
    # first, get some data
    #
    import urllib
    downloads = (
        ('http://slicer.kitware.com/midas3/download?items=5767', 'FA.nrrd', slicer.util.loadVolume),
        )

    for url,name,loader in downloads:
      filePath = slicer.app.temporaryPath + '/' + name
      if not os.path.exists(filePath) or os.stat(filePath).st_size == 0:
        print('Requesting download %s from %s...\n' % (name, url))
        urllib.urlretrieve(url, filePath)
      if loader:
        print('Loading %s...\n' % (name,))
        loader(filePath)
    self.delayDisplay('Finished with download and loading\n')

    volumeNode = slicer.util.getNode(pattern="FA")
    logic = SyntheticRTDoseLogic()
    self.assertTrue( logic.hasImageData(volumeNode) )
    self.delayDisplay('Test passed!')
