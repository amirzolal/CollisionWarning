
// CollisionWarning MRML includes
#include "vtkMRMLCollisionWarningNode.h"

// Other MRML includes
#include "vtkMRMLDisplayNode.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLTransformNode.h"

// VTK includes
#include <vtkNew.h>
#include <vtkIntArray.h>
#include <vtkCommand.h>

// Other includes
#include <sstream>

// Constants
static const char* MODEL_ROLE = "watchedModelNode";
static const char* TOOL_ROLE = "toolTransformNode";
static const char* SECOND_MODEL_ROLE = "secondModelNode";

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLCollisionWarningNode);

vtkMRMLCollisionWarningNode
::vtkMRMLCollisionWarningNode()
{
  this->HideFromEditorsOff();
  this->SetSaveWithScene( true );
  
  vtkNew<vtkIntArray> events;
  events->InsertNextValue( vtkCommand::ModifiedEvent );
  events->InsertNextValue( vtkMRMLTransformableNode::TransformModifiedEvent );

  this->AddNodeReferenceRole( MODEL_ROLE, NULL, events.GetPointer() );
  this->AddNodeReferenceRole( TOOL_ROLE, NULL, events.GetPointer() );
  this->AddNodeReferenceRole( SECOND_MODEL_ROLE, NULL, events.GetPointer() );

  this->OriginalColor[0] = 0.5;
  this->OriginalColor[1] = 0.5;
  this->OriginalColor[2] = 0.5;
  this->WarningColor[0] = 1;
  this->WarningColor[1] = 0;
  this->WarningColor[2] = 0;

  this->DisplayWarningColor=true;
  this->PlayWarningSound = false;

  this->ClosestDistanceToModelFromToolTip = 0.0;
  this->Collision = false;
}

vtkMRMLCollisionWarningNode
::~vtkMRMLCollisionWarningNode()
{
}

void
vtkMRMLCollisionWarningNode
::WriteXML( ostream& of, int nIndent )
{
  Superclass::WriteXML(of, nIndent); // This will take care of referenced nodes
  vtkIndent indent(nIndent);

  of << indent << " warningColor=\"" << this->WarningColor[0] << " " << this->WarningColor[1] << " " << this->WarningColor[2] << "\"";
  of << indent << " originalColor=\"" << this->OriginalColor[0] << " " << this->OriginalColor[1] << " " << this->OriginalColor[2] << "\"";
  of << indent << " displayWarningColor=\"" << ( this->DisplayWarningColor ? "true" : "false" ) << "\"";
  of << indent << " playWarningSound=\"" << ( this->PlayWarningSound ? "true" : "false" ) << "\"";
  of << indent << " collision=\"" << ( this->Collision ? "true" : "false" ) << "\"";
  of << indent << " closestDistanceToModelFromToolTip=\"" << ClosestDistanceToModelFromToolTip << "\"";
}

void
vtkMRMLCollisionWarningNode
::ReadXMLAttributes( const char** atts )
{
  Superclass::ReadXMLAttributes(atts); // This will take care of referenced nodes

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
  {
    attName  = *(atts++);
    attValue = *(atts++);
    
    if (!strcmp(attName, "warningColor"))
    {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->WarningColor[0] = val;
      ss >> val;
      this->WarningColor[1] = val;
      ss >> val;
      this->WarningColor[2] = val;
    }
    else if (!strcmp(attName, "originalColor"))
    {
      std::stringstream ss;
      ss << attValue;
      double val;
      ss >> val;
      this->OriginalColor[0] = val;
      ss >> val;
      this->OriginalColor[1] = val;
      ss >> val;
      this->OriginalColor[2] = val;
    }
    else if ( ! strcmp( attName, "displayWarningColor" ) )
    {
      if (!strcmp(attValue,"true"))
      {
        this->DisplayWarningColor = true;
      }
      else
      {
        this->DisplayWarningColor = false;
      }
    }
    else if ( ! strcmp( attName, "playWarningSound" ))
    {
      if (!strcmp(attValue,"true"))
      {
        this->PlayWarningSound = true;
      }
      else
      {
        this->PlayWarningSound = false;
      }
    }
    else if ( ! strcmp( attName, "collision" ))
    {
      if (!strcmp(attValue,"true"))
        {
          this->Collision = true;
        }
        else
        {
          this->Collision = false;
        }
    }
    else if (!strcmp(attName, "closestDistanceToModelFromToolTip"))
    {
      std::stringstream ss;
      ss << attValue;
      double val=0.0;
      ss >> val;
      this->ClosestDistanceToModelFromToolTip = val;
    }

  }
}

void
vtkMRMLCollisionWarningNode
::Copy( vtkMRMLNode *anode )
{  
  Superclass::Copy( anode ); // This will take care of referenced nodes
  
  vtkMRMLCollisionWarningNode *node = ( vtkMRMLCollisionWarningNode* ) anode;
  
  for ( int i = 0; i < 3; ++ i )
  {
    this->WarningColor[ i ] = node->WarningColor[ i ];
    this->OriginalColor[ i ] = node->OriginalColor[ i ];
  }

  this->PlayWarningSound = node->PlayWarningSound;  
  this->Collision = node->Collision;
  this->DisplayWarningColor = node->DisplayWarningColor;
  
  this->Modified();
}

void
vtkMRMLCollisionWarningNode
::PrintSelf( ostream& os, vtkIndent indent )
{
  vtkMRMLNode::PrintSelf(os,indent); // This will take care of referenced nodes

  os << indent << "WatchedModelID: " << this->GetWatchedModelNode()->GetID() << std::endl;
  os << indent << "SecondModelID: " << this->GetSecondModelNode()->GetID() << std::endl;
  os << indent << "ToolTipTransformID: " << this->GetToolTransformNode()->GetID() << std::endl;
  os << indent << "DisplayWarningColor: " << this->DisplayWarningColor << std::endl;
  os << indent << "PlayWarningSound: " << this->PlayWarningSound << std::endl;
  os << indent << "Collision: " << this->Collision << std::endl;
  os << indent << "WarningColor: " << this->WarningColor[0] << ", " << this->WarningColor[1] << ", " << this->WarningColor[2] << std::endl;
  os << indent << "OriginalColor: " << this->OriginalColor[0] << ", " << this->OriginalColor[1] << ", " << this->OriginalColor[2] << std::endl;
}

vtkMRMLModelNode*
vtkMRMLCollisionWarningNode
::GetWatchedModelNode()
{
  vtkMRMLModelNode* modelNode = vtkMRMLModelNode::SafeDownCast( this->GetNodeReference( MODEL_ROLE ) );
  return modelNode;
}

vtkMRMLModelNode*
vtkMRMLCollisionWarningNode
::GetSecondModelNode()
{
  vtkMRMLModelNode* secondModelNode = vtkMRMLModelNode::SafeDownCast( this->GetNodeReference( SECOND_MODEL_ROLE ) );
  return secondModelNode;
}

void
vtkMRMLCollisionWarningNode
::SetAndObserveWatchedModelNodeID( const char* modelId )
{
  // SetAndObserveNodeReferenceID does not handle nicely setting of the same
  // node (it should simply ignore the reques, but it adds another observer instead)
  // so check for node equality here.
  const char* currentNodeId=this->GetNodeReferenceID(MODEL_ROLE);
  if (modelId!=NULL && currentNodeId!=NULL)
  {
    if (strcmp(modelId,currentNodeId)==0)
    {
      // not changed
      return;
    }
  }
  vtkNew<vtkIntArray> events;
  events->InsertNextValue( vtkCommand::ModifiedEvent );
  events->InsertNextValue( vtkMRMLTransformNode::TransformModifiedEvent );
  this->SetAndObserveNodeReferenceID( MODEL_ROLE, modelId, events.GetPointer() );
  this->InvokeEvent(InputDataModifiedEvent);
}

void
vtkMRMLCollisionWarningNode
::SetAndObserveSecondModelNodeID( const char* modelId )
{
  // SetAndObserveNodeReferenceID does not handle nicely setting of the same
  // node (it should simply ignore the reques, but it adds another observer instead)
  // so check for node equality here.
  const char* currentNodeId=this->GetNodeReferenceID(SECOND_MODEL_ROLE);
  if (modelId!=NULL && currentNodeId!=NULL)
  {
    if (strcmp(modelId,currentNodeId)==0)
    {
      // not changed
      return;
    }
  }
  vtkNew<vtkIntArray> events;
  events->InsertNextValue( vtkCommand::ModifiedEvent );
  events->InsertNextValue( vtkMRMLTransformNode::TransformModifiedEvent );
  this->SetAndObserveNodeReferenceID( SECOND_MODEL_ROLE, modelId, events.GetPointer() );
  this->InvokeEvent(InputDataModifiedEvent);
}


vtkMRMLTransformNode*
vtkMRMLCollisionWarningNode
::GetToolTransformNode()
{
  vtkMRMLTransformNode* ltNode = vtkMRMLTransformNode::SafeDownCast( this->GetNodeReference( TOOL_ROLE ) );
  return ltNode;
}

void
vtkMRMLCollisionWarningNode
::SetAndObserveToolTransformNodeId( const char* nodeId )
{
  // SetAndObserveNodeReferenceID does not handle nicely setting of the same
  // node (it should simply ignore the request, but it adds another observer instead)
  // so check for node equality here.
  const char* currentNodeId=this->GetNodeReferenceID(TOOL_ROLE);
  if (nodeId!=NULL && currentNodeId!=NULL)
  {
    if (strcmp(nodeId,currentNodeId)==0)
    {
      // not changed
      return;
    }
  }
  vtkNew<vtkIntArray> events;
  events->InsertNextValue( vtkCommand::ModifiedEvent );
  events->InsertNextValue( vtkMRMLTransformNode::TransformModifiedEvent );
  this->SetAndObserveNodeReferenceID( TOOL_ROLE, nodeId, events.GetPointer() );
  this->InvokeEvent(InputDataModifiedEvent);

}

void
vtkMRMLCollisionWarningNode
::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  vtkMRMLNode* callerNode = vtkMRMLNode::SafeDownCast( caller );
  if ( callerNode == NULL ) return;

  if (this->GetToolTransformNode() && this->GetToolTransformNode()==caller)
  {
    this->InvokeEvent(InputDataModifiedEvent);
  }
  else if (this->GetWatchedModelNode() && this->GetWatchedModelNode()==caller)
  {
    this->InvokeEvent(InputDataModifiedEvent);
  }
}

bool vtkMRMLCollisionWarningNode::IsToolTipInsideModel()
{
//return (this->ClosestDistanceToModelFromToolTip<0); changed to
  return (this->GetCollision());
}

void vtkMRMLCollisionWarningNode::SetDisplayWarningColor(bool _arg)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting DisplayWarningColor to " << _arg);
  if (this->DisplayWarningColor != _arg)
  {
    this->DisplayWarningColor = _arg;
    this->Modified();
    this->InvokeEvent(InputDataModifiedEvent);
  }
}

void vtkMRMLCollisionWarningNode::SetPlayWarningSound(bool _arg)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting PlayWarningSound to " << _arg);
  if (this->PlayWarningSound != _arg)
  {
    this->PlayWarningSound = _arg;
    this->Modified();
    this->InvokeEvent(InputDataModifiedEvent);
  }
}

void vtkMRMLCollisionWarningNode::SetWarningColor(double _arg1, double _arg2, double _arg3)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting WarningColor to (" << _arg1 << "," << _arg2 << "," << _arg3 << ")");
  if ((this->WarningColor[0] != _arg1)||(this->WarningColor[1] != _arg2)||(this->WarningColor[2] != _arg3))
  {
    this->WarningColor[0] = _arg1;
    this->WarningColor[1] = _arg2;
    this->WarningColor[2] = _arg3;
    this->Modified();
    this->InvokeEvent(InputDataModifiedEvent);
  }
}

void vtkMRMLCollisionWarningNode::SetWarningColor(double _arg[3])
{
  this->SetWarningColor(_arg[0], _arg[1], _arg[2]);
}

void vtkMRMLCollisionWarningNode::SetOriginalColor(double _arg1, double _arg2, double _arg3)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting OriginalColor to (" << _arg1 << "," << _arg2 << "," << _arg3 << ")");
  if ((this->OriginalColor[0] != _arg1)||(this->OriginalColor[1] != _arg2)||(this->OriginalColor[2] != _arg3))
  {
    this->OriginalColor[0] = _arg1;
    this->OriginalColor[1] = _arg2;
    this->OriginalColor[2] = _arg3;
    this->Modified();
    this->InvokeEvent(InputDataModifiedEvent);
  }
}

void vtkMRMLCollisionWarningNode::SetOriginalColor(double _arg[3])
{
  this->SetOriginalColor(_arg[0], _arg[1], _arg[2]);
}
