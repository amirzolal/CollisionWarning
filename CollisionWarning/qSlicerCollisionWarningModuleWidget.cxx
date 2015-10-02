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

==============================================================================*/


// Qt includes
#include <QtGui>

// SlicerQt includes
#include "qSlicerCollisionWarningModuleWidget.h"
#include "ui_qSlicerCollisionWarningModule.h"

#include "vtkSlicerCollisionWarningLogic.h"

#include "vtkMRMLCollisionWarningNode.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CollisionWarning
class qSlicerCollisionWarningModuleWidgetPrivate: public Ui_qSlicerCollisionWarningModule
{
  Q_DECLARE_PUBLIC( qSlicerCollisionWarningModuleWidget ); 
  
protected:
  qSlicerCollisionWarningModuleWidget* const q_ptr;
public:
  qSlicerCollisionWarningModuleWidgetPrivate( qSlicerCollisionWarningModuleWidget& object );
  vtkSlicerCollisionWarningLogic* logic() const;

  bool ModuleWindowInitialized;
};

//-----------------------------------------------------------------------------
// qSlicerCollisionWarningModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCollisionWarningModuleWidgetPrivate::qSlicerCollisionWarningModuleWidgetPrivate( qSlicerCollisionWarningModuleWidget& object ) 
  : q_ptr( &object )
  , ModuleWindowInitialized( false )
{
}

//-----------------------------------------------------------------------------
vtkSlicerCollisionWarningLogic* qSlicerCollisionWarningModuleWidgetPrivate::logic() const
{
  Q_Q( const qSlicerCollisionWarningModuleWidget );
  return vtkSlicerCollisionWarningLogic::SafeDownCast( q->logic() );
}

//-----------------------------------------------------------------------------
// qSlicerCollisionWarningModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCollisionWarningModuleWidget::qSlicerCollisionWarningModuleWidget( QWidget* _parent )
  : Superclass( _parent )
  , d_ptr( new qSlicerCollisionWarningModuleWidgetPrivate( *this ) )
{

}

//-----------------------------------------------------------------------------
qSlicerCollisionWarningModuleWidget::~qSlicerCollisionWarningModuleWidget()
{
 Q_D(qSlicerCollisionWarningModuleWidget);
  disconnect( d->ParameterNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onParameterNodeChanged() ) );

  // Make connections to update the mrml from the widget
  disconnect( d->ModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  disconnect( d->SecondModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  disconnect( d->ToolComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onToolTransformChanged() ) );
  disconnect( d->ColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT( UpdateWarningColor( QColor ) ) );
  disconnect(d->SoundCheckBox, SIGNAL(toggled(bool)), this, SLOT(PlayWarningSound(bool)));
  disconnect(d->colorCheckBox, SIGNAL(toggled(bool)), this, SLOT(DisplayWarningColor(bool)));


}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::setup()
{
  Q_D(qSlicerCollisionWarningModuleWidget);

  d->setupUi(this);
  this->Superclass::setup();

  this->setMRMLScene( d->logic()->GetMRMLScene() );

  connect( d->ParameterNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onParameterNodeChanged() ) );

  // Make connections to update the mrml from the widget
  connect( d->ModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  connect( d->SecondModelNodeComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onModelNodeChanged() ) );
  connect( d->ToolComboBox, SIGNAL( currentNodeChanged( vtkMRMLNode* ) ), this, SLOT( onToolTransformChanged() ) );
  connect( d->ColorPickerButton, SIGNAL( colorChanged( QColor ) ), this, SLOT( UpdateWarningColor( QColor ) ) );
  connect(d->SoundCheckBox, SIGNAL(toggled(bool)), this, SLOT(PlayWarningSound(bool)));
  connect(d->colorCheckBox, SIGNAL(toggled(bool)), this, SLOT(DisplayWarningColor(bool)));
  
  this->UpdateFromMRMLNode();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::enter()
{
  Q_D(qSlicerCollisionWarningModuleWidget);

  if ( this->mrmlScene() == NULL )
  {
    qCritical() << "Invalid scene!";
    return;
  }
  
  // Create a module MRML node if there is none in the scene.

  vtkMRMLNode* node = this->mrmlScene()->GetNthNodeByClass(0, "vtkMRMLCollisionWarningNode");
  if ( node == NULL )
  {
    vtkSmartPointer< vtkMRMLCollisionWarningNode > newNode = vtkSmartPointer< vtkMRMLCollisionWarningNode >::New();
    this->mrmlScene()->AddNode( newNode );
  }

  node = this->mrmlScene()->GetNthNodeByClass( 0, "vtkMRMLCollisionWarningNode" );
  if ( node == NULL )
  {
    qCritical( "Failed to create module node" );
    return;
  }

  // For convenience, select a default module.

  if ( d->ParameterNodeComboBox->currentNode() == NULL )
  {
    d->ParameterNodeComboBox->setCurrentNodeID( node->GetID() );
  }

  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::setMRMLScene( vtkMRMLScene* scene )
{
  Q_D( qSlicerCollisionWarningModuleWidget );
  this->Superclass::setMRMLScene( scene );
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::onSceneImportedEvent()
{
  this->enter();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::onParameterNodeChanged()
{
  Q_D( qSlicerCollisionWarningModuleWidget );
  this->UpdateFromMRMLNode();
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::onModelNodeChanged()
{
  Q_D( qSlicerCollisionWarningModuleWidget );
  
  vtkMRMLCollisionWarningNode* bwNode = vtkMRMLCollisionWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( bwNode == NULL )
  {
    qCritical( "Model node changed with no module node selection" );
    return;
  }
  
//first model
  vtkMRMLModelNode* currentNode = vtkMRMLModelNode::SafeDownCast(d->ModelNodeComboBox->currentNode());
  d->logic()->SetWatchedModelNode( currentNode, bwNode );

//second model
  vtkMRMLModelNode* secondCurrentNode = vtkMRMLModelNode::SafeDownCast(d->SecondModelNodeComboBox->currentNode());
  d->logic()->SetSecondModelNode( secondCurrentNode, bwNode );
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::onToolTransformChanged()
{
  Q_D( qSlicerCollisionWarningModuleWidget );
  
  vtkMRMLCollisionWarningNode* parameterNode = vtkMRMLCollisionWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "Transform node should not be changed when no parameter node selected" );
    return;
  }
  
  vtkMRMLTransformNode* currentNode = vtkMRMLTransformNode::SafeDownCast(d->ToolComboBox->currentNode());
  parameterNode->SetAndObserveToolTransformNodeId( (currentNode!=NULL) ? currentNode->GetID() : NULL);
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::PlayWarningSound(bool playWarningSound)
{
  Q_D(qSlicerCollisionWarningModuleWidget);
  vtkMRMLCollisionWarningNode* parameterNode = vtkMRMLCollisionWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "Transform node should not be changed when no module node selected" );
    return;
  }
  parameterNode->SetPlayWarningSound(playWarningSound);
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::DisplayWarningColor(bool displayWarningColor)
{
  Q_D(qSlicerCollisionWarningModuleWidget);
  vtkMRMLCollisionWarningNode* parameterNode = vtkMRMLCollisionWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "Transform node should not be changed when no module node selected" );
    return;
  }
  parameterNode->SetDisplayWarningColor(displayWarningColor);
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::UpdateWarningColor( QColor newColor )
{
  Q_D(qSlicerCollisionWarningModuleWidget);

  vtkMRMLCollisionWarningNode* parameterNode = vtkMRMLCollisionWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( parameterNode == NULL )
  {
    qCritical( "Color selected without module node" );
    return;
  }

  parameterNode->SetWarningColor( newColor.redF(), newColor.greenF(), newColor.blueF() );
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModuleWidget::UpdateFromMRMLNode()
{
  Q_D( qSlicerCollisionWarningModuleWidget );
  
  vtkMRMLCollisionWarningNode* bwNode = vtkMRMLCollisionWarningNode::SafeDownCast( d->ParameterNodeComboBox->currentNode() );
  if ( bwNode == NULL )
  {
    d->ToolComboBox->setCurrentNodeID( "" );
    d->ModelNodeComboBox->setCurrentNodeID( "" );
    d->ModelNodeComboBox->setEnabled( false );
    d->SecondModelNodeComboBox->setCurrentNodeID( "" );
    d->SecondModelNodeComboBox->setEnabled( false );
    d->ToolComboBox->setEnabled( false );
    d->colorCheckBox->setEnabled( false );
    d->ColorPickerButton->setEnabled( false );
    d->SoundCheckBox->setEnabled( false );
    return;
  }
    
  d->ModelNodeComboBox->setEnabled( true );
  d->SecondModelNodeComboBox->setEnabled( true );
  d->ToolComboBox->setEnabled( true );
  d->colorCheckBox->setEnabled( true );
  d->ColorPickerButton->setEnabled( true );
  d->SoundCheckBox->setEnabled( true );
  
  d->ToolComboBox->setCurrentNode( bwNode->GetToolTransformNode() );
  d->ModelNodeComboBox->setCurrentNode( bwNode->GetWatchedModelNode() );
  
  double* warningColor = bwNode->GetWarningColor();
  QColor nodeWarningColor;
  nodeWarningColor.setRgbF(warningColor[0],warningColor[1],warningColor[2]);
  d->ColorPickerButton->setColor(nodeWarningColor);

  d->colorCheckBox->setChecked( bwNode->GetDisplayWarningColor() );
  d->SoundCheckBox->setChecked( bwNode->GetPlayWarningSound() );
}
