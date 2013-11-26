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

  This file was originally developed by Kevin Wang, MotionSimulatorculator Medicine Program, 
  University Health Network and was supported by Cancer Care Ontario (CCO)'s ACRU program 
  with funds provided by the Ontario Ministry of Health and Long-Term Care
  and Ontario Consortium for Adaptive Interventions in MotionSimulatorculator Oncology (OCAIRO).

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QFileDialog>
#include <QCheckBox>

// SlicerRt includes
#include "SlicerRtCommon.h"
#include "vtkMRMLMotionSimulatorDoubleArrayNode.h"

// SlicerQt includes
#include "qSlicerDosePopulationHistogramModuleWidget.h"
#include "ui_qSlicerDosePopulationHistogramModule.h"

// DosePopulationHistogram includes
#include "vtkSlicerDosePopulationHistogramModuleLogic.h"
#include "vtkMRMLDosePopulationHistogramNode.h"

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLChartNode.h>
#include <vtkMRMLDoubleArrayNode.h>


// VTK includes
#include <vtkStringArray.h>

// VTKSYS includes
#include <vtksys/SystemTools.hxx>

// STD includes
#include <set>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_DosePopulationHistogram
class qSlicerDosePopulationHistogramModuleWidgetPrivate: public Ui_qSlicerDosePopulationHistogramModule
{
  Q_DECLARE_PUBLIC(qSlicerDosePopulationHistogramModuleWidget);
protected:
  qSlicerDosePopulationHistogramModuleWidget* const q_ptr;
public:
  qSlicerDosePopulationHistogramModuleWidgetPrivate(qSlicerDosePopulationHistogramModuleWidget &object);
  ~qSlicerDosePopulationHistogramModuleWidgetPrivate();
  vtkSlicerDosePopulationHistogramModuleLogic* logic() const;
public:
  /// Map that associates a string pair containing the structure set plot name (including table row number) and the vtkMRMLDoubleArrayNode id (respectively) to the show/hide in chart checkboxes
  QMap<QCheckBox*, QPair<QString, QString> > ChartCheckboxToStructureSetNameMap;

  /// Flag whether show/hide all checkbox has been clicked - some operations are not necessary when it was clicked
  bool ShowHideAllClicked;
};

//-----------------------------------------------------------------------------
// qSlicerDosePopulationHistogramModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDosePopulationHistogramModuleWidgetPrivate::qSlicerDosePopulationHistogramModuleWidgetPrivate(qSlicerDosePopulationHistogramModuleWidget& object)
 : q_ptr(&object)
{
  this->ChartCheckboxToStructureSetNameMap.clear();
  this->ShowHideAllClicked = false;
}

//-----------------------------------------------------------------------------
qSlicerDosePopulationHistogramModuleWidgetPrivate::~qSlicerDosePopulationHistogramModuleWidgetPrivate()
{
  this->ChartCheckboxToStructureSetNameMap.clear();
}

//-----------------------------------------------------------------------------
vtkSlicerDosePopulationHistogramModuleLogic*
qSlicerDosePopulationHistogramModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerDosePopulationHistogramModuleWidget);
  return vtkSlicerDosePopulationHistogramModuleLogic::SafeDownCast(q->logic());
} 


//-----------------------------------------------------------------------------
// qSlicerDosePopulationHistogramModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerDosePopulationHistogramModuleWidget::qSlicerDosePopulationHistogramModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerDosePopulationHistogramModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerDosePopulationHistogramModuleWidget::~qSlicerDosePopulationHistogramModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  this->Superclass::setMRMLScene(scene);

  qvtkReconnect( d->logic(), scene, vtkMRMLScene::EndImportEvent, this, SLOT(onSceneImportedEvent()) );

  // Find parameters node or create it if there is no one in the scene
  if (scene &&  d->logic()->GetDosePopulationHistogramNode() == 0)
  {
    vtkMRMLNode* node = scene->GetNthNodeByClass(0, "vtkMRMLDosePopulationHistogramNode");
    if (node)
    {
      this->setDosePopulationHistogramNode( vtkMRMLDosePopulationHistogramNode::SafeDownCast(node) );
    }
  }
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::onSceneImportedEvent()
{
  this->onEnter();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::onEnter()
{
  if (!this->mrmlScene())
  {
    return;
  }

  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  // First check the logic if it has a parameter node
  if (d->logic() == NULL)
  {
    return;
  }
  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();

  // If we have a parameter node select it
  if (paramNode == NULL)
  {
    vtkMRMLNode* node = this->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLDosePopulationHistogramNode");
    if (node)
    {
      paramNode = vtkMRMLDosePopulationHistogramNode::SafeDownCast(node);
      d->logic()->SetAndObserveDosePopulationHistogramNode(paramNode);
      return;
    }
    else 
    {
      vtkSmartPointer<vtkMRMLDosePopulationHistogramNode> newNode = vtkSmartPointer<vtkMRMLDosePopulationHistogramNode>::New();
      this->mrmlScene()->AddNode(newNode);
      d->logic()->SetAndObserveDosePopulationHistogramNode(newNode);
    }
  }

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::setDosePopulationHistogramNode(vtkMRMLNode *node)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = vtkMRMLDosePopulationHistogramNode::SafeDownCast(node);

  // Each time the node is modified, the qt widgets are updated
  qvtkReconnect( d->logic()->GetDosePopulationHistogramNode(), paramNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()) );

  d->logic()->SetAndObserveDosePopulationHistogramNode(paramNode);
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (paramNode && this->mrmlScene())
  {
    d->MRMLNodeComboBox_ParameterSet->setCurrentNode(paramNode);
    if (paramNode->GetDoubleArrayNode())
    {
      d->MRMLNodeComboBox_DoubleArray->setCurrentNode(paramNode->GetDoubleArrayNode());
    }
    else
    {
      this->doubleArrayNodeChanged(d->MRMLNodeComboBox_DoubleArray->currentNode());
    }

    if (paramNode->GetDoseVolumeNode())
    {
      d->MRMLNodeComboBox_DoseVolume->setCurrentNode(paramNode->GetDoseVolumeNode());
    }
    else
    {
      this->doseVolumeNodeChanged(d->MRMLNodeComboBox_DoseVolume->currentNode());
    }

    if (paramNode->GetContourNode())
    {
      d->MRMLNodeComboBox_Contour->setCurrentNode(paramNode->GetContourNode());
    }
    else
    {
      this->contourNodeChanged(d->MRMLNodeComboBox_Contour->currentNode());
    }

    if (paramNode->GetChartNode())
    {
      d->MRMLNodeComboBox_Chart->setCurrentNode(paramNode->GetChartNode());
    }
    else
    {
      this->chartNodeChanged(d->MRMLNodeComboBox_Chart->currentNode());
    }
  }

  this->refreshDPHTable();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::setup()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Filter out StructureSet Nodes that are not labelmaps
  d->MRMLNodeComboBox_Contour->addAttribute( QString("Labelmap"), QString("1") );

  // Make connections
  this->connect( d->MRMLNodeComboBox_ParameterSet, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( setDosePopulationHistogramNode(vtkMRMLNode*) ) );

  this->connect( d->MRMLNodeComboBox_DoubleArray, SIGNAL( currentNodeChanged(vtkMRMLNode*) ), this, SLOT( doubleArrayNodeChanged(vtkMRMLNode*) ) );
  this->connect( d->MRMLNodeComboBox_DoseVolume, SIGNAL( currentNodeChanged(vtkMRMLNode*) ), this, SLOT( doseVolumeNodeChanged(vtkMRMLNode*) ) );
  this->connect( d->MRMLNodeComboBox_Contour, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( contourNodeChanged(vtkMRMLNode*) ) );

  this->connect( d->radioButton_UseDmin, SIGNAL(clicked()), this, SLOT(radioButtonUseDminClicked()));
  this->connect( d->radioButton_UseD98, SIGNAL(clicked()), this, SLOT(radioButtonUseD98Clicked()));

  this->connect( d->pushButton_ComputeDPH, SIGNAL( clicked() ), this, SLOT( computeDPH() ) );

  this->connect( d->MRMLNodeComboBox_Chart, SIGNAL( currentNodeChanged(vtkMRMLNode*) ), this, SLOT( chartNodeChanged(vtkMRMLNode*) ) );
  this->connect( d->checkBox_ShowHideAll, SIGNAL( stateChanged(int) ), this, SLOT( showHideAllCheckedStateChanged(int) ) );

  // Handle scene change event if occurs
  qvtkConnect( d->logic(), vtkCommand::ModifiedEvent, this, SLOT( onLogicModified() ) );

  this->updateChartCheckboxesState();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::updateButtonsState()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (paramNode)
  {
    // Enable/disable ComputeDVH button
    bool DPHCanBeComputed = paramNode->GetDoubleArrayNode();
    d->pushButton_ComputeDPH->setEnabled(DPHCanBeComputed);
  }
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::updateChartCheckboxesState()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  vtkMRMLChartNode* chartNode = paramNode->GetChartNode();

  // If there is no chart node selected, disable all checkboxes
  if (chartNode == NULL)
  {
    QMapIterator<QCheckBox*, QPair<QString, QString> > it(d->ChartCheckboxToStructureSetNameMap);
    while (it.hasNext())
    {
      it.next();
      it.key()->setEnabled(false);
    }
    d->checkBox_ShowHideAll->setEnabled(false);

    return;
  }

  vtkStringArray* arraysInSelectedChart = chartNode->GetArrays();
  paramNode->GetShowInChartCheckStates()->clear();

  QMapIterator<QCheckBox*, QPair<QString, QString> > it(d->ChartCheckboxToStructureSetNameMap);
  while (it.hasNext())
  {
    it.next();

    it.key()->setEnabled(true);
    it.key()->blockSignals(true); // block signals for the checkboxes so that changing it do not toggle the visibility of the plot
    it.key()->setChecked(false);

    for (int i=0; i<arraysInSelectedChart->GetNumberOfValues(); ++i)
    {
      if (arraysInSelectedChart->GetValue(i).compare(it.value().second.toLatin1()) == 0)
      {
        it.key()->setChecked(true);
        break;
      }
    }

    paramNode->GetShowInChartCheckStates()->push_back(it.key()->isChecked());

    it.key()->blockSignals(false); // unblock signal for the checkbox in question
  }

  // Change show/hide all checkbox state
  d->checkBox_ShowHideAll->setEnabled(true);
  d->checkBox_ShowHideAll->blockSignals(true);
  if (arraysInSelectedChart->GetNumberOfValues() == 0)
  {
    d->checkBox_ShowHideAll->setCheckState(Qt::Unchecked);
    paramNode->DisableModifiedEventOn();
    paramNode->SetShowHideAll(Qt::Unchecked);
    paramNode->DisableModifiedEventOff();
  }
  else if (arraysInSelectedChart->GetNumberOfValues() == d->ChartCheckboxToStructureSetNameMap.size())
  {
    d->checkBox_ShowHideAll->setCheckState(Qt::Checked);
    paramNode->DisableModifiedEventOn();
    paramNode->SetShowHideAll(Qt::Checked);
    paramNode->DisableModifiedEventOff();
  }
  else
  {
    d->checkBox_ShowHideAll->setCheckState(Qt::PartiallyChecked);
    paramNode->DisableModifiedEventOn();
    paramNode->SetShowHideAll(Qt::PartiallyChecked);
    paramNode->DisableModifiedEventOff();
  }
  d->checkBox_ShowHideAll->blockSignals(false);
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::doubleArrayNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveDoubleArrayNode(vtkMRMLMotionSimulatorDoubleArrayNode::SafeDownCast(node));
  paramNode->DisableModifiedEventOff();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::doseVolumeNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveDoseVolumeNode(vtkMRMLScalarVolumeNode::SafeDownCast(node));
  paramNode->DisableModifiedEventOff();

  //if (d->logic()->DoseVolumeContainsDose())
  //{
  //  //d->label_NotDoseVolumeWarning->setText("");
  //}
  //else
  //{
  //  //d->label_NotDoseVolumeWarning->setText(tr(" Selected volume is not a dose"));
  //}

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::contourNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveContourNode(vtkMRMLScalarVolumeNode::SafeDownCast(node));
  paramNode->DisableModifiedEventOff();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::radioButtonUseDminClicked()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }
  paramNode->DisableModifiedEventOn();
  paramNode->SetUseDoseOptionToDmin();
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::radioButtonUseD98Clicked()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }
  paramNode->DisableModifiedEventOn();
  paramNode->SetUseDoseOptionToD98();
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::chartNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveChartNode(vtkMRMLChartNode::SafeDownCast(node));
  paramNode->DisableModifiedEventOff();

  this->updateButtonsState();
  this->updateChartCheckboxesState();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::onLogicModified()
{
  this->refreshDPHTable();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::refreshDPHTable()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  // Clear the table
  d->tableWidget_ChartStatistics->setRowCount(0);
  d->tableWidget_ChartStatistics->setColumnCount(0);
  d->tableWidget_ChartStatistics->clearContents();

  QMapIterator<QCheckBox*, QPair<QString, QString> > it(d->ChartCheckboxToStructureSetNameMap);
  while (it.hasNext())
  {
    it.next();

    QCheckBox* checkbox = it.key();
    disconnect( checkbox, SIGNAL( stateChanged(int) ), this, SLOT( showInChartCheckStateChanged(int) ) );
    delete checkbox;
  }

  d->ChartCheckboxToStructureSetNameMap.clear();

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  std::vector<std::string>* DPHNodes = paramNode->GetDPHDoubleArrayNodeIDs();

  if (DPHNodes->size() < 1)
  {
    return;
  }

  // Set up the table
  d->tableWidget_ChartStatistics->setColumnCount(3);
  QStringList headerLabels;
  headerLabels << "" << "Plan" << "";

  d->tableWidget_ChartStatistics->setColumnWidth(0, 24);
  d->tableWidget_ChartStatistics->setHorizontalHeaderLabels(headerLabels);
  d->tableWidget_ChartStatistics->setRowCount(DPHNodes->size());

  // Fill the table
  std::vector<std::string>::iterator DPHIt;
  int i;
  QList<QString> structureNames;
  for (i=0, DPHIt = DPHNodes->begin(); DPHIt != DPHNodes->end(); ++DPHIt, ++i)
  {
    vtkMRMLDoubleArrayNode* DPHNode = vtkMRMLDoubleArrayNode::SafeDownCast(
      this->mrmlScene()->GetNodeByID(DPHIt->c_str()));
    if (!DPHNode)
    {
      continue;
    }

    // Create checkbox
    QCheckBox* checkbox = new QCheckBox(d->tableWidget_ChartStatistics);
    checkbox->setToolTip(tr("Show/hide DVH plot of structure '%1' in selected chart").arg(
      QString(DPHNode->GetAttribute(SlicerRtCommon::DVH_STRUCTURE_NAME_ATTRIBUTE_NAME.c_str())) ));
    connect( checkbox, SIGNAL( stateChanged(int) ), this, SLOT( showInChartCheckStateChanged(int) ) );

    // Assign line style and plot name
    QString plotName( DPHNode->GetName() );
    int numberOfStructuresWithSameName = structureNames.count(plotName);
    structureNames << plotName;

    plotName.append( QString(" (%1)").arg(i+1) );

    if (numberOfStructuresWithSameName % 4 == 1)
    {
      DPHNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_PLOT_LINE_STYLE_ATTRIBUTE_NAME.c_str(), "dashed");
      plotName.append( " [- -]" );
    }
    else if (numberOfStructuresWithSameName % 4 == 2)
    {
      DPHNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_PLOT_LINE_STYLE_ATTRIBUTE_NAME.c_str(), "dotted");
      plotName.append( " [...]" );
    }
    else if (numberOfStructuresWithSameName % 4 == 3)
    {
      DPHNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_PLOT_LINE_STYLE_ATTRIBUTE_NAME.c_str(), "dashed-dotted");
      plotName.append( " [-.-]" );
    }
    else
    {
      DPHNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_PLOT_LINE_STYLE_ATTRIBUTE_NAME.c_str(), "solid");
    }
    DPHNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_COLOR_ATTRIBUTE_NAME.c_str(), "blue");

    // Store checkbox with the augmented structure set name and the double array ID
    DPHNode->SetAttribute(SlicerRtCommon::DVH_STRUCTURE_PLOT_NAME_ATTRIBUTE_NAME.c_str(), plotName.toLatin1());
    d->ChartCheckboxToStructureSetNameMap[checkbox] = QPair<QString, QString>(plotName, DPHNode->GetID());

    d->tableWidget_ChartStatistics->setCellWidget(i, 0, checkbox);

    d->tableWidget_ChartStatistics->setItem(i, 1, new QTableWidgetItem(
      QString(DPHNode->GetAttribute(SlicerRtCommon::DVH_STRUCTURE_PLOT_NAME_ATTRIBUTE_NAME.c_str())) ));    

    //vtkMRMLVolumeNode* volumeNode = vtkMRMLVolumeNode::SafeDownCast( this->mrmlScene()->GetNodeByID(
    //  DPHNode->GetAttribute(SlicerRtCommon::DVH_DOSE_VOLUME_NODE_ID_ATTRIBUTE_NAME.c_str()) ) );
    //if (volumeNode)
    //{
    //  d->tableWidget_ChartStatistics->setItem(i, 2, new QTableWidgetItem( QString(volumeNode->GetName()) ));    
    //}
  }

  this->updateButtonsState();
  this->updateChartCheckboxesState();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::computeDPH()
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // Compute the DPH for the selected margin cal double array
  std::string errorMessage;
  d->logic()->ComputeDPH();

  QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::showInChartCheckStateChanged(int aState)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  QCheckBox* senderCheckbox = dynamic_cast<QCheckBox*>(sender());
  if (!senderCheckbox)
  {
    std::cerr << "Error: Invalid sender checkbox for show/hide in chart checkbox state change" << std::endl;
    return;
  }

  if (aState)
  {
    d->logic()->AddDPHToSelectedChart(d->ChartCheckboxToStructureSetNameMap[senderCheckbox].first.toLatin1(), d->ChartCheckboxToStructureSetNameMap[senderCheckbox].second.toLatin1());
  }
  else
  {
    d->logic()->RemoveDPHFromSelectedChart(d->ChartCheckboxToStructureSetNameMap[senderCheckbox].first.toLatin1());
  }

  if (!d->ShowHideAllClicked)
  {
    // Update states vector
    QMap<QCheckBox*, QPair<QString, QString> >::const_iterator checkboxIt;
    std::vector<bool>::iterator stateIt;
    for (checkboxIt=d->ChartCheckboxToStructureSetNameMap.begin(),
      stateIt=paramNode->GetShowInChartCheckStates()->begin();
      checkboxIt!=d->ChartCheckboxToStructureSetNameMap.end(); ++checkboxIt, ++stateIt)
    {
      if (checkboxIt.key() == senderCheckbox)
      {
        (*stateIt) = (bool)aState;
      }
    }

    // Change show/hide all checkbox state
    d->checkBox_ShowHideAll->blockSignals(true);
    bool isThereChecked = false;
    bool isThereUnchecked = false;
    QMapIterator<QCheckBox*, QPair<QString, QString> > it(d->ChartCheckboxToStructureSetNameMap);
    while (it.hasNext())
    {
      it.next();

      if (it.key()->isChecked())
      {
        isThereChecked = true;
      }
      else
      {
        isThereUnchecked = true;
      }
    }
    if (isThereChecked && isThereUnchecked)
    {
      d->checkBox_ShowHideAll->setCheckState(Qt::PartiallyChecked);
    }
    else if (isThereChecked && !isThereUnchecked)
    {
      d->checkBox_ShowHideAll->setCheckState(Qt::Checked);
    }
    else
    {
      d->checkBox_ShowHideAll->setCheckState(Qt::Unchecked);
    }
    d->checkBox_ShowHideAll->blockSignals(false);
  }

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDosePopulationHistogramModuleWidget::showHideAllCheckedStateChanged(int aState)
{
  Q_D(qSlicerDosePopulationHistogramModuleWidget);

  vtkMRMLDosePopulationHistogramNode* paramNode = d->logic()->GetDosePopulationHistogramNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetShowHideAll(aState);
  paramNode->DisableModifiedEventOff();

  QMap<QCheckBox*, QPair<QString, QString> >::const_iterator checkboxIt;

  d->ShowHideAllClicked = true;

  if (aState == Qt::PartiallyChecked)
  {
    std::vector<bool>::iterator stateIt;

    bool noneIsOn = true;
    bool noneIsOff = true;
    for (stateIt=paramNode->GetShowInChartCheckStates()->begin(); stateIt!=paramNode->GetShowInChartCheckStates()->end(); ++stateIt)
    {
      if (*stateIt)
      {
        noneIsOn = false;
      }
      else
      {
        noneIsOff = false;
      }
    }
    // If all the states are the same then advance to Checked state
    if (noneIsOn || noneIsOff)
    {
      d->checkBox_ShowHideAll->setCheckState(Qt::Checked);
      return;
    }
    else // Else set the states one by one and leave it PartiallyChecked
    {
      for (checkboxIt=d->ChartCheckboxToStructureSetNameMap.begin(),
        stateIt=paramNode->GetShowInChartCheckStates()->begin();
        checkboxIt!=d->ChartCheckboxToStructureSetNameMap.end(); ++checkboxIt, ++stateIt)
      {
        checkboxIt.key()->setChecked(*stateIt);
      }
    }
  }
  else
  {
    bool state = (aState==Qt::Checked ? true : false);
    for (checkboxIt=d->ChartCheckboxToStructureSetNameMap.begin(); checkboxIt!=d->ChartCheckboxToStructureSetNameMap.end(); ++checkboxIt)
    {
      checkboxIt.key()->setChecked(state);
    }
  }

  // Workaround for a bug
  d->checkBox_ShowHideAll->blockSignals(true);
  d->checkBox_ShowHideAll->setCheckState((Qt::CheckState)aState);
  d->checkBox_ShowHideAll->blockSignals(false);

  d->ShowHideAllClicked = false;
}
