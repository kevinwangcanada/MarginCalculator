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
  and Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO).

==============================================================================*/

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerMotionSimulatorModuleWidget.h"
#include "ui_qSlicerMotionSimulatorModule.h"

// Motion simulation includes
#include "vtkSlicerMotionSimulatorModuleLogic.h"
#include "vtkMRMLMotionSimulatorNode.h"

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
//#include <vtkMRMLContourNode.h>
#include <vtkMRMLMotionSimulatorDoubleArrayNode.h>

// VTK includes
#include <vtkLookupTable.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

// STD includes
#include <sstream>
#include <string>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerMotionSimulatorModuleWidgetPrivate: public Ui_qSlicerMotionSimulatorModule
{
  Q_DECLARE_PUBLIC(qSlicerMotionSimulatorModuleWidget);
protected:
  qSlicerMotionSimulatorModuleWidget* const q_ptr;
public:
  qSlicerMotionSimulatorModuleWidgetPrivate(qSlicerMotionSimulatorModuleWidget &object);
  ~qSlicerMotionSimulatorModuleWidgetPrivate();
  vtkSlicerMotionSimulatorModuleLogic* logic() const;

};

//-----------------------------------------------------------------------------
// qSlicerMotionSimulatorModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorModuleWidgetPrivate::qSlicerMotionSimulatorModuleWidgetPrivate(qSlicerMotionSimulatorModuleWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorModuleWidgetPrivate::~qSlicerMotionSimulatorModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerMotionSimulatorModuleLogic*
qSlicerMotionSimulatorModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerMotionSimulatorModuleWidget);
  return vtkSlicerMotionSimulatorModuleLogic::SafeDownCast(q->logic());
} 

//-----------------------------------------------------------------------------
// qSlicerMotionSimulatorModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorModuleWidget::qSlicerMotionSimulatorModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerMotionSimulatorModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerMotionSimulatorModuleWidget::~qSlicerMotionSimulatorModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);

  this->Superclass::setMRMLScene(scene);

  qvtkReconnect( d->logic(), scene, vtkMRMLScene::EndImportEvent, this, SLOT(onSceneImportedEvent()) );

  // Find parameters node or create it if there is no one in the scene
  if (scene &&  d->logic()->GetMotionSimulatorNode() == 0)
    {
    vtkMRMLNode* node = scene->GetNthNodeByClass(0, "vtkMRMLMotionSimulatorNode");
    if (node)
      {
      this->setMotionSimulatorNode( vtkMRMLMotionSimulatorNode::SafeDownCast(node) );
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::onSceneImportedEvent()
{
  this->onEnter();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::onEnter()
{
  if (!this->mrmlScene())
    {
    return;
    }

  Q_D(qSlicerMotionSimulatorModuleWidget);

  // First check the logic if it has a parameter node
  if (d->logic() == NULL)
    {
    return;
    }
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();

  // If we have a parameter node select it
  if (paramNode == NULL)
    {
    vtkMRMLNode* node = this->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLMotionSimulatorNode");
    if (node)
      {
      paramNode = vtkMRMLMotionSimulatorNode::SafeDownCast(node);
      d->logic()->SetAndObserveMotionSimulatorNode(paramNode);
      return;
      }
    else 
      {
      vtkSmartPointer<vtkMRMLMotionSimulatorNode> newNode = vtkSmartPointer<vtkMRMLMotionSimulatorNode>::New();
      this->mrmlScene()->AddNode(newNode);
      d->logic()->SetAndObserveMotionSimulatorNode(newNode);
      }
    }

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerMotionSimulatorModuleWidget);

  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (paramNode && this->mrmlScene())
  {
    d->MRMLNodeComboBox_ParameterSet->setCurrentNode(paramNode);
    if (paramNode->GetInputDoseVolumeNodeID() && strcmp(paramNode->GetInputDoseVolumeNodeID(),""))
    {
      d->MRMLNodeComboBox_DoseVolume->setCurrentNode(paramNode->GetInputDoseVolumeNodeID());
    }
    else
    {
      this->doseVolumeNodeChanged(d->MRMLNodeComboBox_DoseVolume->currentNode());
    }
    if (paramNode->GetInputContourNodeID() && strcmp(paramNode->GetInputContourNodeID(),""))
    {
      d->MRMLNodeComboBox_StructureSet->setCurrentNode(paramNode->GetInputContourNodeID());
    }
    else
    {
      this->structureSetNodeChanged(d->MRMLNodeComboBox_StructureSet->currentNode());
    }
    if (paramNode->GetOutputDoubleArrayNodeID() && strcmp(paramNode->GetOutputDoubleArrayNodeID(),""))
    {
      d->MRMLNodeComboBox_OutputArray->setCurrentNode(paramNode->GetOutputDoubleArrayNodeID());
    }
    else
    {
      this->outputArrayNodeChanged(d->MRMLNodeComboBox_OutputArray->currentNode());
    }
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
    //d->lineEdit_NumberOfSimulation->setText(QString(paramNode->GetNumberOfSimulation()));
  }
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::onLogicModified()
{
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::setup()
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Make connections
  this->connect( d->MRMLNodeComboBox_ParameterSet, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( setMotionSimulatorNode(vtkMRMLNode*) ) );
  this->connect( d->MRMLNodeComboBox_DoseVolume, SIGNAL( currentNodeChanged(vtkMRMLNode*) ), this, SLOT( doseVolumeNodeChanged(vtkMRMLNode*) ) );
  this->connect( d->MRMLNodeComboBox_StructureSet, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( structureSetNodeChanged(vtkMRMLNode*) ) );
  this->connect( d->MRMLNodeComboBox_OutputArray, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( outputArrayNodeChanged(vtkMRMLNode*) ) );

  this->connect( d->lineEdit_XSysSD, SIGNAL(textChanged(QString)), this, SLOT(lineEditXSysSDChanged(QString)));
  this->connect( d->lineEdit_YSysSD, SIGNAL(textChanged(QString)), this, SLOT(lineEditYSysSDChanged(QString)));
  this->connect( d->lineEdit_ZSysSD, SIGNAL(textChanged(QString)), this, SLOT(lineEditZSysSDChanged(QString)));
  this->connect( d->lineEdit_XRdmSD, SIGNAL(textChanged(QString)), this, SLOT(lineEditXRdmSDChanged(QString)));
  this->connect( d->lineEdit_YRdmSD, SIGNAL(textChanged(QString)), this, SLOT(lineEditYRdmSDChanged(QString)));
  this->connect( d->lineEdit_ZRdmSD, SIGNAL(textChanged(QString)), this, SLOT(lineEditZRdmSDChanged(QString)));

  this->connect( d->lineEdit_NumberOfSimulation, SIGNAL(textChanged(QString)), this, SLOT(lineEditNumberSimulationChanged(QString)));

  this->connect( d->lineEdit_NumberOfFractions, SIGNAL(textChanged(QString)), this, SLOT(lineEditNumberOfFractionsChanged(QString)));

  this->connect( d->pushButton_RunSimulation, SIGNAL(clicked()), this, SLOT(runSimulationClicked()) );

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::setMotionSimulatorNode(vtkMRMLNode *node)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);

  vtkMRMLMotionSimulatorNode* paramNode = vtkMRMLMotionSimulatorNode::SafeDownCast(node);

  // Each time the node is modified, the qt widgets are updated
  qvtkReconnect( d->logic()->GetMotionSimulatorNode(), paramNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()) );

  d->logic()->SetAndObserveMotionSimulatorNode(paramNode);
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::doseVolumeNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);

  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveInputDoseVolumeNodeID(node->GetID());
  paramNode->DisableModifiedEventOff();

  if (d->logic()->DoseVolumeContainsDose())
  {
    //d->label_NotDoseVolumeWarning->setText("");
  }
  else
  {
    //d->label_NotDoseVolumeWarning->setText(tr(" Selected volume is not a dose"));
  }

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::structureSetNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);

  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveInputContourNodeID(node->GetID());
  paramNode->DisableModifiedEventOff();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::outputArrayNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);

  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveOutputDoubleArrayNodeID(node->GetID());
  paramNode->DisableModifiedEventOff();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditXSysSDChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetXSysSD(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditYSysSDChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetYSysSD(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditZSysSDChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetZSysSD(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditXRdmSDChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetXRdmSD(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditYRdmSDChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetYRdmSD(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditZRdmSDChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetZRdmSD(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditNumberSimulationChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetNumberOfSimulation(text.toInt());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::lineEditNumberOfFractionsChanged(const QString & text)
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  
  vtkMRMLMotionSimulatorNode* paramNode = d->logic()->GetMotionSimulatorNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetNumberOfFraction(text.toInt());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::updateButtonsState()
{
  Q_D(qSlicerMotionSimulatorModuleWidget);
  if (!this->mrmlScene())
  {
    return;
  }

  bool applyEnabled = d->logic()->GetMotionSimulatorNode()
                   && d->logic()->GetMotionSimulatorNode()->GetInputDoseVolumeNodeID()
                   && strcmp(d->logic()->GetMotionSimulatorNode()->GetInputDoseVolumeNodeID(), "")
                   && d->logic()->GetMotionSimulatorNode()->GetInputContourNodeID()
                   && strcmp(d->logic()->GetMotionSimulatorNode()->GetInputContourNodeID(), "")
                   && d->logic()->GetMotionSimulatorNode()->GetOutputDoubleArrayNodeID()
                   && strcmp(d->logic()->GetMotionSimulatorNode()->GetOutputDoubleArrayNodeID(), "")
                   ;
  d->pushButton_RunSimulation->setEnabled(true);
}

//-----------------------------------------------------------------------------
void qSlicerMotionSimulatorModuleWidget::runSimulationClicked()
{
  Q_D(qSlicerMotionSimulatorModuleWidget);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // 
  d->logic()->RunSimulation();

  QApplication::restoreOverrideCursor();
}

