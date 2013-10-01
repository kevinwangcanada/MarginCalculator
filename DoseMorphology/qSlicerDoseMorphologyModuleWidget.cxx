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
#include "qSlicerDoseMorphologyModuleWidget.h"
#include "ui_qSlicerDoseMorphologyModule.h"

// Isodose includes
#include "vtkSlicerDoseMorphologyModuleLogic.h"
#include "vtkMRMLDoseMorphologyNode.h"

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>

// VTK includes
#include <vtkLookupTable.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

// STD includes
#include <sstream>
#include <string>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerDoseMorphologyModuleWidgetPrivate: public Ui_qSlicerDoseMorphologyModule
{
  Q_DECLARE_PUBLIC(qSlicerDoseMorphologyModuleWidget);
protected:
  qSlicerDoseMorphologyModuleWidget* const q_ptr;
public:
  qSlicerDoseMorphologyModuleWidgetPrivate(qSlicerDoseMorphologyModuleWidget &object);
  ~qSlicerDoseMorphologyModuleWidgetPrivate();
  vtkSlicerDoseMorphologyModuleLogic* logic() const;
};

//-----------------------------------------------------------------------------
// qSlicerDoseMorphologyModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerDoseMorphologyModuleWidgetPrivate::qSlicerDoseMorphologyModuleWidgetPrivate(qSlicerDoseMorphologyModuleWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
qSlicerDoseMorphologyModuleWidgetPrivate::~qSlicerDoseMorphologyModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerDoseMorphologyModuleLogic*
qSlicerDoseMorphologyModuleWidgetPrivate::logic() const
{
  Q_Q(const qSlicerDoseMorphologyModuleWidget);
  return vtkSlicerDoseMorphologyModuleLogic::SafeDownCast(q->logic());
} 

//-----------------------------------------------------------------------------
// qSlicerDoseMorphologyModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerDoseMorphologyModuleWidget::qSlicerDoseMorphologyModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerDoseMorphologyModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
qSlicerDoseMorphologyModuleWidget::~qSlicerDoseMorphologyModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  this->Superclass::setMRMLScene(scene);

  qvtkReconnect( d->logic(), scene, vtkMRMLScene::EndImportEvent, this, SLOT(onSceneImportedEvent()) );

  // Find parameters node or create it if there is no one in the scene
  if (scene &&  d->logic()->GetDoseMorphologyNode() == 0)
    {
    vtkMRMLNode* node = scene->GetNthNodeByClass(0, "vtkMRMLDoseMorphologyNode");
    if (node)
      {
      this->setDoseMorphologyNode( vtkMRMLDoseMorphologyNode::SafeDownCast(node) );
      }
    }
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::onSceneImportedEvent()
{
  this->onEnter();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::onEnter()
{
  if (!this->mrmlScene())
    {
    return;
    }

  Q_D(qSlicerDoseMorphologyModuleWidget);

  // First check the logic if it has a parameter node
  if (d->logic() == NULL)
    {
    return;
    }
  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();

  // If we have a parameter node select it
  if (paramNode == NULL)
    {
    vtkMRMLNode* node = this->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLDoseMorphologyNode");
    if (node)
      {
      paramNode = vtkMRMLDoseMorphologyNode::SafeDownCast(node);
      d->logic()->SetAndObserveDoseMorphologyNode(paramNode);
      return;
      }
    else 
      {
      vtkSmartPointer<vtkMRMLDoseMorphologyNode> newNode = vtkSmartPointer<vtkMRMLDoseMorphologyNode>::New();
      this->mrmlScene()->AddNode(newNode);
      d->logic()->SetAndObserveDoseMorphologyNode(newNode);
      }
    }

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::updateWidgetFromMRML()
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (paramNode && this->mrmlScene())
  {
    d->MRMLNodeComboBox_ParameterSet->setCurrentNode(paramNode);
    if (paramNode->GetReferenceDoseVolumeNodeID() && strcmp(paramNode->GetReferenceDoseVolumeNodeID(),""))
    {
      d->MRMLNodeComboBox_CurrentDoseVolume->setCurrentNode(paramNode->GetReferenceDoseVolumeNodeID());
    }
    else
    {
      this->currentDoseVolumeNodeChanged(d->MRMLNodeComboBox_CurrentDoseVolume->currentNode());
    }

    if (paramNode->GetInputDoseVolumeNodeID() && strcmp(paramNode->GetInputDoseVolumeNodeID(),""))
    {
      d->MRMLNodeComboBox_SecondaryDoseVolume->setCurrentNode(paramNode->GetInputDoseVolumeNodeID());
    }
    else
    {
      this->secondaryDoseVolumeNodeChanged(d->MRMLNodeComboBox_SecondaryDoseVolume->currentNode());
    }

    int op = paramNode->GetOperation();
    switch (op)
    {
      case SLICERRT_EXPAND_BY_SCALING:
        d->radioButton_ExpandByScaling->setChecked(true);
        d->radioButton_ExpandByDilation->setChecked(false);
        break;
      case SLICERRT_EXPAND_BY_DILATION:
        d->radioButton_ExpandByScaling->setChecked(false);
        d->radioButton_ExpandByDilation->setChecked(true);
        break;
    }

    std::ostringstream sstream_xsize;
    sstream_xsize << paramNode->GetXSize();
    d->lineEdit_XSize->setText(QString(sstream_xsize.str().c_str()));

    std::ostringstream sstream_ysize;
    sstream_ysize << paramNode->GetYSize();
    d->lineEdit_YSize->setText(QString(sstream_ysize.str().c_str()));

    std::ostringstream sstream_zsize;
    sstream_zsize << paramNode->GetZSize();
    d->lineEdit_ZSize->setText(QString(sstream_zsize.str().c_str()));
  }
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::onLogicModified()
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::setup()
{
  Q_D(qSlicerDoseMorphologyModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // Make connections
  this->connect( d->MRMLNodeComboBox_ParameterSet, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( setDoseMorphologyNode(vtkMRMLNode*) ) );

  this->connect( d->MRMLNodeComboBox_CurrentDoseVolume, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( currentDoseVolumeNodeChanged(vtkMRMLNode*) ) );
  this->connect( d->MRMLNodeComboBox_SecondaryDoseVolume, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( secondaryDoseVolumeNodeChanged(vtkMRMLNode*) ) );
  this->connect( d->MRMLNodeComboBox_OutputDoseVolume, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT( outputDoseVolumeNodeChanged(vtkMRMLNode*) ) );

  this->connect( d->radioButton_ExpandByScaling, SIGNAL(clicked()), this, SLOT(radioButtonExpandByScalingClicked()));
  this->connect( d->radioButton_ExpandByDilation, SIGNAL(clicked()), this, SLOT(radioButtonExpandByDilationClicked()));

  this->connect( d->lineEdit_XSize, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditXSizeChanged(const QString &)));
  this->connect( d->lineEdit_YSize, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditYSizeChanged(const QString &)));
  this->connect( d->lineEdit_ZSize, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditZSizeChanged(const QString &)));

  this->connect( d->pushButton_Apply, SIGNAL(clicked()), this, SLOT(applyClicked()) );

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::setDoseMorphologyNode(vtkMRMLNode *node)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  vtkMRMLDoseMorphologyNode* paramNode = vtkMRMLDoseMorphologyNode::SafeDownCast(node);

  // Each time the node is modified, the qt widgets are updated
  qvtkReconnect( d->logic()->GetDoseMorphologyNode(), paramNode, vtkCommand::ModifiedEvent, this, SLOT(updateWidgetFromMRML()) );

  d->logic()->SetAndObserveDoseMorphologyNode(paramNode);
  this->updateWidgetFromMRML();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::currentDoseVolumeNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveReferenceDoseVolumeNodeID(node->GetID());
  paramNode->DisableModifiedEventOff();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::secondaryDoseVolumeNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveInputDoseVolumeNodeID(node->GetID());
  paramNode->DisableModifiedEventOff();

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::outputDoseVolumeNodeChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene() || !node)
  {
    return;
  }

  paramNode->DisableModifiedEventOn();
  paramNode->SetAndObserveOutputDoseVolumeNodeID(node->GetID());
  paramNode->DisableModifiedEventOff();

  if (d->logic()->VolumeContainsDose())
  {
    d->label_NotDoseVolumeWarning->setText("");
  }
  else
  {
    d->label_NotDoseVolumeWarning->setText(tr("Selected node is not a dose volume!"));
  }

  this->updateButtonsState();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::radioButtonExpandByScalingClicked()
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }
  paramNode->DisableModifiedEventOn();
  paramNode->SetOperationToExpandByScaling();
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::radioButtonExpandByDilationClicked()
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }
  paramNode->DisableModifiedEventOn();
  paramNode->SetOperationToExpandByDilation();
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::lineEditXSizeChanged(const QString & text)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);
  
  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }
  paramNode->DisableModifiedEventOn();
  paramNode->SetXSize(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::lineEditYSizeChanged(const QString & text)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);
  
  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }
  paramNode->DisableModifiedEventOn();
  paramNode->SetYSize(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::lineEditZSizeChanged(const QString & text)
{
  Q_D(qSlicerDoseMorphologyModuleWidget);
  
  vtkMRMLDoseMorphologyNode* paramNode = d->logic()->GetDoseMorphologyNode();
  if (!paramNode || !this->mrmlScene())
  {
    return;
  }
  paramNode->DisableModifiedEventOn();
  paramNode->SetZSize(text.toDouble());
  paramNode->DisableModifiedEventOff();
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::updateButtonsState()
{
  Q_D(qSlicerDoseMorphologyModuleWidget);
  if (!this->mrmlScene())
  {
    return;
  }

  bool applyEnabled = d->logic()->GetDoseMorphologyNode()
                   && d->logic()->GetDoseMorphologyNode()->GetReferenceDoseVolumeNodeID()
                   && strcmp(d->logic()->GetDoseMorphologyNode()->GetReferenceDoseVolumeNodeID(), "")
                   && d->logic()->GetDoseMorphologyNode()->GetInputDoseVolumeNodeID()
                   && strcmp(d->logic()->GetDoseMorphologyNode()->GetInputDoseVolumeNodeID(), "")
                   && d->logic()->GetDoseMorphologyNode()->GetOutputDoseVolumeNodeID()
                   && strcmp(d->logic()->GetDoseMorphologyNode()->GetOutputDoseVolumeNodeID(), "");
  d->pushButton_Apply->setEnabled(applyEnabled);
}

//-----------------------------------------------------------------------------
void qSlicerDoseMorphologyModuleWidget::applyClicked()
{
  Q_D(qSlicerDoseMorphologyModuleWidget);

  QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

  // 
  d->logic()->MorphDose();

  QApplication::restoreOverrideCursor();
}

