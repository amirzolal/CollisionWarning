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

// CollisionWarning includes
#include "vtkSlicerCollisionWarningLogic.h"

// vtkbioeng includes
#include "vtkCollisionDetectionFilter.h"

// MRML includes
#include "vtkMRMLCollisionWarningNode.h"
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLTransformNode.h"

// VTK includes
#include <vtkGeneralTransform.h>
#include <vtkImplicitPolyDataDistance.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>

// STD includes

// Slicer methods 

vtkStandardNewMacro(vtkSlicerCollisionWarningLogic);

//------------------------------------------------------------------------------
vtkSlicerCollisionWarningLogic::vtkSlicerCollisionWarningLogic()
: WarningSoundPlaying(false)
{
}


//------------------------------------------------------------------------------
vtkSlicerCollisionWarningLogic::~vtkSlicerCollisionWarningLogic()
{
}

//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::RegisterNodes()
{
  if( ! this->GetMRMLScene() )
  {
    vtkWarningMacro( "MRML scene not yet created" );
    return;
  }

  this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLCollisionWarningNode >::New() );
}
/* original method
//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::UpdateToolState( vtkMRMLCollisionWarningNode* bwNode )
{
  if ( bwNode == NULL )
  {
    return;
  }

  vtkMRMLModelNode* modelNode = bwNode->GetWatchedModelNode();
  vtkMRMLTransformNode* toolToRasNode = bwNode->GetToolTransformNode();

  if ( modelNode == NULL || toolToRasNode == NULL )
  {
    bwNode->SetClosestDistanceToModelFromToolTip(0);
    return;
  }

  vtkPolyData* body = modelNode->GetPolyData();
  if ( body == NULL )
  {
    vtkWarningMacro( "No surface model in node" );
    return;
  }
  
  vtkSmartPointer< vtkImplicitPolyDataDistance > implicitDistanceFilter = vtkSmartPointer< vtkImplicitPolyDataDistance >::New();

  // Transform the body poly data if there is a parent transform.
  vtkMRMLTransformNode* bodyParentTransform = modelNode->GetParentTransformNode();
  if ( bodyParentTransform != NULL )
  {
    vtkSmartPointer< vtkGeneralTransform > bodyToRasTransform = vtkSmartPointer< vtkGeneralTransform >::New();
    bodyParentTransform->GetTransformToWorld( bodyToRasTransform );

    vtkSmartPointer< vtkTransformPolyDataFilter > bodyToRasFilter = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
#if (VTK_MAJOR_VERSION <= 5)
    bodyToRasFilter->SetInput( body );
#else
    bodyToRasFilter->SetInputData( body );
#endif
    bodyToRasFilter->SetTransform( bodyToRasTransform );
    bodyToRasFilter->Update();

    implicitDistanceFilter->SetInput( bodyToRasFilter->GetOutput() ); // expensive: builds a locator
  }
  else
  {
    implicitDistanceFilter->SetInput( body ); // expensive: builds a locator
  }
  
  // Note: Performance could be improved by
  // - keeping the filter in memory for all the observed nodes.
  // - in case of linear transform of model and tooltip: transform only the tooltip (with the tooltip to model transform),
  //   and not transform the model at all

  vtkSmartPointer<vtkGeneralTransform> toolToRasTransform = vtkSmartPointer<vtkGeneralTransform>::New();
  toolToRasNode->GetTransformToWorld( toolToRasTransform ); //toolToRasNode = transform that was specified in the widget
  //toolToRasTransform now has information about the position of the Tool in the world
  double toolTipPosition_Tool[4] = { 0.0, 0.0, 0.0, 1.0 };
  //this will be the tooltip of the tool, but I need the model, not just the tooltip
  double* toolTipPosition_Ras = toolToRasTransform->TransformDoublePoint( toolTipPosition_Tool);
  //position of the Tooltip by using the Transform

  bwNode->SetClosestDistanceToModelFromToolTip(implicitDistanceFilter->EvaluateFunction( toolTipPosition_Ras ));
}

original method */

void vtkSlicerCollisionWarningLogic::UpdateToolState( vtkMRMLCollisionWarningNode* bwNode )
{
  if ( bwNode == NULL )
  {
    return;
  }

  vtkMRMLModelNode* modelNode = bwNode->GetWatchedModelNode();
  vtkMRMLTransformNode* toolToRasNode = bwNode->GetToolTransformNode();
  vtkMRMLModelNode* secondModelNode = bwNode->GetSecondModelNode();

  if ( modelNode == NULL || secondModelNode == NULL )
  {
    bwNode->SetClosestDistanceToModelFromToolTip(0);
    return;
  }

  //vtkCollisionDetectionFilter only accepts triangles, hope it works this way
  vtkSmartPointer<vtkTriangleFilter> triangleFilter0 = vtkSmartPointer<vtkTriangleFilter>::New();
  vtkSmartPointer<vtkTriangleFilter> triangleFilter1 = vtkSmartPointer<vtkTriangleFilter>::New();
  triangleFilter0->SetInputConnection(modelNode->GetPolyDataConnection());
  triangleFilter1->SetInputConnection(secondModelNode->GetPolyDataConnection());
  //triangleFilters are set

  vtkPolyData* body = triangleFilter0->GetOutput();
  if ( body == NULL )
  {
    vtkWarningMacro( "No surface model in first node" );
    return;
  }

  vtkPolyData* secondBody = triangleFilter1->GetOutput();
  if ( secondBody == NULL )
  {
    vtkWarningMacro( "No surface model in second node" );
    return;
  }

  vtkSmartPointer< vtkCollisionDetectionFilter> collisionDetectionFilter = vtkSmartPointer< vtkCollisionDetectionFilter>::New();
  collisionDetectionFilter->SetCollisionModeToFirstContact(); // should be faster

// start transform first model
  // Transform the first body poly data if there is a parent transform.
  vtkMRMLTransformNode* bodyParentTransform = modelNode->GetParentTransformNode();
  if ( bodyParentTransform != NULL )
  {
    vtkSmartPointer< vtkGeneralTransform > bodyToRasTransform = vtkSmartPointer< vtkGeneralTransform >::New();
    bodyParentTransform->GetTransformToWorld( bodyToRasTransform );

    vtkSmartPointer< vtkTransformPolyDataFilter > bodyToRasFilter = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
#if (VTK_MAJOR_VERSION <= 5)
    bodyToRasFilter->SetInput( body );
#else
    bodyToRasFilter->SetInputData( body );
#endif
    bodyToRasFilter->SetTransform( bodyToRasTransform );
    bodyToRasFilter->Update();

    collisionDetectionFilter->SetInput(0, bodyToRasFilter->GetOutput() );
  }
  else
  {
    collisionDetectionFilter->SetInput(0, body ); // no idea how expensive
  } // don't care about expensive now, jut want this to work


// second model ----------------------------------
  vtkMRMLTransformNode* secondBodyParentTransform = secondModelNode->GetParentTransformNode();
  if ( secondBodyParentTransform != NULL )
  {
    vtkSmartPointer< vtkGeneralTransform > secondBodyToRasTransform = vtkSmartPointer< vtkGeneralTransform >::New();
    secondBodyParentTransform->GetTransformToWorld( secondBodyToRasTransform );

    vtkSmartPointer< vtkTransformPolyDataFilter > secondBodyToRasFilter = vtkSmartPointer< vtkTransformPolyDataFilter >::New();
#if (VTK_MAJOR_VERSION <= 5)
    secondBodyToRasFilter->SetInput( secondBody );
#else
    secondBodyToRasFilter->SetInputData( secondBody );
#endif
    secondBodyToRasFilter->SetTransform( secondBodyToRasTransform );
    secondBodyToRasFilter->Update();

    collisionDetectionFilter->SetInput(0, secondBodyToRasFilter->GetOutput() );
  }
  else
  {
    collisionDetectionFilter->SetInput(0, secondBody ); // no idea how expensive
  }
// end transform second model
  vtkSmartPointer<vtkMatrix4x4> matrix0 = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> matrix1 = vtkSmartPointer<vtkMatrix4x4>::New();
  collisionDetectionFilter->SetMatrix(0, matrix0);
  collisionDetectionFilter->SetMatrix(1, matrix1);
  collisionDetectionFilter->GenerateScalarsOff();

  if (collisionDetectionFilter->GetNumberOfContacts() > 0)
  {
     bwNode->SetCollision(true);
  }
     else
  {
     bwNode->SetCollision(false);
  }



  // Note: Performance could be improved by
  // - keeping the filter in memory for all the observed nodes.
  // - in case of linear transform of model and tooltip: transform only the tooltip (with the tooltip to model transform),
  //   and not transform the model at all
  //   --> not valid for collision detection



}


//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::UpdateModelColor( vtkMRMLCollisionWarningNode* bwNode )
{
  if ( bwNode == NULL )
  {
    return;
  }
  vtkMRMLModelNode* modelNode = bwNode->GetWatchedModelNode();
  if ( modelNode == NULL )
  {
    return;
  }
  if ( modelNode->GetDisplayNode() == NULL )
  {
    return;
  }

  if ( bwNode->IsToolTipInsideModel())
  {
    double* color = bwNode->GetWarningColor();
    modelNode->GetDisplayNode()->SetColor(color);
  }
  else
  {
    double* color = bwNode->GetOriginalColor();
    modelNode->GetDisplayNode()->SetColor(color);
  }
}


//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::OnMRMLSceneNodeAdded( vtkMRMLNode* node )
{
  if ( node == NULL || this->GetMRMLScene() == NULL )
  {
    vtkWarningMacro( "OnMRMLSceneNodeAdded: Invalid MRML scene or node" );
    return;
  }

  vtkMRMLCollisionWarningNode* bwNode = vtkMRMLCollisionWarningNode::SafeDownCast(node);
  if ( bwNode )
  {
    vtkDebugMacro( "OnMRMLSceneNodeAdded: Module node added." );
    vtkUnObserveMRMLNodeMacro( bwNode ); // Remove previous observers.
    vtkNew<vtkIntArray> events;
    events->InsertNextValue( vtkCommand::ModifiedEvent );
    events->InsertNextValue( vtkMRMLCollisionWarningNode::InputDataModifiedEvent );
    vtkObserveMRMLNodeEventsMacro( bwNode, events.GetPointer() );
    if(bwNode->GetPlayWarningSound() && bwNode->IsToolTipInsideModel())
    {
      // Add to list of playing nodes (if not there already)
      std::deque< vtkWeakPointer< vtkMRMLCollisionWarningNode > >::iterator foundPlayingNodeIt = this->WarningSoundPlayingNodes.begin();    
      for (; foundPlayingNodeIt!=this->WarningSoundPlayingNodes.end(); ++foundPlayingNodeIt)
      {
        if (foundPlayingNodeIt->GetPointer()==bwNode)
        {
          // found, current bw node is already in the playing list
          break;
        }
      }
      if (foundPlayingNodeIt==this->WarningSoundPlayingNodes.end())
      {
        this->WarningSoundPlayingNodes.push_back(bwNode);
      }
      this->SetWarningSoundPlaying(true);
    }
  }
}





//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::OnMRMLSceneNodeRemoved( vtkMRMLNode* node )
{
  if ( node == NULL || this->GetMRMLScene() == NULL )
  {
    vtkWarningMacro( "OnMRMLSceneNodeRemoved: Invalid MRML scene or node" );
    return;
  }

  if ( node->IsA( "vtkMRMLCollisionWarningNode" ) )
  {
    vtkDebugMacro( "OnMRMLSceneNodeRemoved" );
    vtkUnObserveMRMLNodeMacro( node );
    for (std::deque< vtkWeakPointer< vtkMRMLCollisionWarningNode > >::iterator it=this->WarningSoundPlayingNodes.begin(); it!=this->WarningSoundPlayingNodes.end(); ++it)
    {
      if (it->GetPointer()==node)
      {
        this->WarningSoundPlayingNodes.erase(it);
        break;
      }
    }
    this->SetWarningSoundPlaying(!this->WarningSoundPlayingNodes.empty());
  }
}

//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::SetWatchedModelNode( vtkMRMLModelNode* newModel, vtkMRMLCollisionWarningNode* moduleNode )
{
  if ( moduleNode == NULL )
  {
    vtkWarningMacro( "SetWatchedModelNode: Module node is invalid" );
    return;
  }

  // Get the original color of the old model node
  vtkMRMLModelNode* previousModel=moduleNode->GetWatchedModelNode();

  if (previousModel==newModel)
  {
    // no change
    return;
  }

  double previousOriginalColor[3]={0.5,0.5,0.5};
  if(previousModel)
  {
    moduleNode->GetOriginalColor(previousOriginalColor);
  }

  // Save the original color of the new model node
  if(newModel!=NULL)
  {
    double originalColor[3]={0.5,0.5,0.5};
    if ( newModel->GetDisplayNode() != NULL )
    {
      newModel->GetDisplayNode()->GetColor(originalColor);
    }
    moduleNode->SetOriginalColor(originalColor);
  }

  // Switch to the new model node
  moduleNode->SetAndObserveWatchedModelNodeID( (newModel!=NULL) ? newModel->GetID() : NULL );

  // Restore the color of the old model node
  if(previousModel!=NULL && previousModel->GetDisplayNode()!=NULL)
  {
    previousModel->GetDisplayNode()->SetColor(previousOriginalColor[0],previousOriginalColor[1],previousOriginalColor[2]);
  }
}

void vtkSlicerCollisionWarningLogic::SetSecondModelNode( vtkMRMLModelNode* newModel, vtkMRMLCollisionWarningNode* moduleNode )
{
  if ( moduleNode == NULL )
  {
    vtkWarningMacro( "SetSecondModelNode: Module node is invalid" );
    return;
  }

  // Get the original color of the old model node
  vtkMRMLModelNode* previousModel=moduleNode->GetSecondModelNode();

  if (previousModel==newModel)
  {
    // no change
    return;
  }

  double previousOriginalColor[3]={0.5,0.5,0.5};
  if(previousModel)
  {
    moduleNode->GetOriginalColor(previousOriginalColor);
  }

  // Save the original color of the new model node
  if(newModel!=NULL)
  {
    double originalColor[3]={0.5,0.5,0.5};
    if ( newModel->GetDisplayNode() != NULL )
    {
      newModel->GetDisplayNode()->GetColor(originalColor);
    }
    moduleNode->SetOriginalColor(originalColor);
  }

  // Switch to the new model node
  moduleNode->SetAndObserveSecondModelNodeID( (newModel!=NULL) ? newModel->GetID() : NULL );

  // Restore the color of the old model node
  if(previousModel!=NULL && previousModel->GetDisplayNode()!=NULL)
  {
    previousModel->GetDisplayNode()->SetColor(previousOriginalColor[0],previousOriginalColor[1],previousOriginalColor[2]);
  }
}


/*
//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData )
{
  vtkMRMLNode* callerNode = vtkMRMLNode::SafeDownCast( caller );
  if (callerNode == NULL)
  {
    return;
  }
  
  vtkMRMLCollisionWarningNode* bwNode = vtkMRMLCollisionWarningNode::SafeDownCast( callerNode );
  if (bwNode == NULL)
  {
    return;
  }
  
  if (event==vtkMRMLCollisionWarningNode::InputDataModifiedEvent)
  {
    // only recompute output if the input is changed
    // (for example we do not recompute the distance if the computed distance is changed)
    this->UpdateToolState(bwNode);
    if(bwNode->GetDisplayWarningColor())
    {
      this->UpdateModelColor(bwNode);
    }
    if(bwNode->GetPlayWarningSound() && bwNode->IsToolTipInsideModel())
    {
      this->SetWarningSoundPlaying(true);
    }
    else
    {
      this->SetWarningSoundPlaying(false);
    }
  }
}
*/

//------------------------------------------------------------------------------
void vtkSlicerCollisionWarningLogic::ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData )
{
  vtkMRMLNode* callerNode = vtkMRMLNode::SafeDownCast( caller );
  if (callerNode == NULL)
  {
    return;
  }
  
  vtkMRMLCollisionWarningNode* bwNode = vtkMRMLCollisionWarningNode::SafeDownCast( callerNode );
  if (bwNode == NULL)
  {
    return;
  }
  
  if (event==vtkMRMLCollisionWarningNode::InputDataModifiedEvent)
  {
    // only recompute output if the input is changed
    // (for example we do not recompute the distance if the computed distance is changed)
    this->UpdateToolState(bwNode);
    if(bwNode->GetDisplayWarningColor())
    {
      this->UpdateModelColor(bwNode);
    }
    std::deque< vtkWeakPointer< vtkMRMLCollisionWarningNode > >::iterator foundPlayingNodeIt = this->WarningSoundPlayingNodes.begin();    
    for (; foundPlayingNodeIt!=this->WarningSoundPlayingNodes.end(); ++foundPlayingNodeIt)
    {
      if (foundPlayingNodeIt->GetPointer()==bwNode)
      {
        // found current bw node is already in the playing list
        break;
      }
    }
    if(bwNode->GetPlayWarningSound() && bwNode->IsToolTipInsideModel())
    {
      // Add to list of playing nodes (if not there already)
      if (foundPlayingNodeIt==this->WarningSoundPlayingNodes.end())
      {
        this->WarningSoundPlayingNodes.push_back(bwNode);
      }
    }
    else
    {
      // Remove from list of playing nodes (if still there)
      if (foundPlayingNodeIt!=this->WarningSoundPlayingNodes.end())
      {
        this->WarningSoundPlayingNodes.erase(foundPlayingNodeIt);
      }
    }
    this->SetWarningSoundPlaying(!this->WarningSoundPlayingNodes.empty());
  }
}
