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

// .NAME vtkSlicerCollisionWarningLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic of the Collision Warning module.


#ifndef __vtkSlicerCollisionWarningLogic_h
#define __vtkSlicerCollisionWarningLogic_h


#include <string>
#include <deque>

// VTK includes
#include "vtkWeakPointer.h"

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
#include "vtkMRML.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLScene.h"
#include <vtkLandmarkTransform.h>
#include <vtkPoints.h>
#include "vtkSmartPointer.h"

// For referencing own MRML node
class vtkMRMLCollisionWarningNode;

class vtkMRMLModelNode;
class vtkMRMLTransformNode;

// STD includes
#include <cstdlib>

#include "vtkSlicerCollisionWarningModuleLogicExport.h"


/// \ingroup Slicer_QtModules_CollisionWarning
class VTK_SLICER_COLLISIONWARNING_MODULE_LOGIC_EXPORT vtkSlicerCollisionWarningLogic :
  public vtkSlicerModuleLogic
{
public:
  static vtkSlicerCollisionWarningLogic *New();
  vtkTypeMacro(vtkSlicerCollisionWarningLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// Changes the watched model node, making sure the original color of the previously selected model node is restored
  void SetWatchedModelNode( vtkMRMLModelNode* newModel, vtkMRMLCollisionWarningNode* moduleNode );
  void SetSecondModelNode( vtkMRMLModelNode* newModel, vtkMRMLCollisionWarningNode* moduleNode );

  void ProcessMRMLNodesEvents( vtkObject* caller, unsigned long event, void* callData );

  /// Returns true if a warning sound has to be played
  vtkGetMacro(WarningSoundPlaying, bool);
  vtkSetMacro(WarningSoundPlaying, bool);

protected:
  vtkSlicerCollisionWarningLogic();
  virtual ~vtkSlicerCollisionWarningLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene * newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

  void UpdateToolState( vtkMRMLCollisionWarningNode* bwNode );
  void UpdateModelColor( vtkMRMLCollisionWarningNode* bwNode );

private:
  vtkSlicerCollisionWarningLogic(const vtkSlicerCollisionWarningLogic&); // Not implemented
  void operator=(const vtkSlicerCollisionWarningLogic&);               // Not implemented

  std::deque< vtkWeakPointer< vtkMRMLCollisionWarningNode > > WarningSoundPlayingNodes;
  bool WarningSoundPlaying;
};

#endif
