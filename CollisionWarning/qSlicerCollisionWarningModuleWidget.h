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

#ifndef __qSlicerCollisionWarningModuleWidget_h
#define __qSlicerCollisionWarningModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"


#include "qSlicerCollisionWarningModuleExport.h"

class qSlicerCollisionWarningModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_CollisionWarning
class Q_SLICER_QTMODULES_COLLISIONWARNING_EXPORT qSlicerCollisionWarningModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerCollisionWarningModuleWidget(QWidget *parent=0);
  virtual ~qSlicerCollisionWarningModuleWidget();

public slots:

  virtual void setMRMLScene( vtkMRMLScene* scene );
  void onSceneImportedEvent();

protected slots:

  void onParameterNodeChanged();

  void onModelNodeChanged();
  void onToolTransformChanged();
  void PlayWarningSound(bool warningSound);
  void DisplayWarningColor(bool displayWarningColor);
  void UpdateWarningColor( QColor newColor );
  void UpdateFromMRMLNode();

protected:
  QScopedPointer<qSlicerCollisionWarningModuleWidgetPrivate> d_ptr;
  
  virtual void setup();
  virtual void enter();

private:
  Q_DECLARE_PRIVATE(qSlicerCollisionWarningModuleWidget);
  Q_DISABLE_COPY(qSlicerCollisionWarningModuleWidget);

};

#endif
