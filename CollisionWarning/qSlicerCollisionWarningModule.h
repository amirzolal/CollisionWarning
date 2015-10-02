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

#ifndef __qSlicerCollisionWarningModule_h
#define __qSlicerCollisionWarningModule_h

// SlicerQt includes
#include "qSlicerLoadableModule.h"
#include "qSlicerCoreApplication.h"
#include "qSlicerModuleManager.h"

#include <ctkVTKObject.h>

#include "qSlicerCollisionWarningModuleExport.h"

class qSlicerCollisionWarningModulePrivate;

/// \ingroup Slicer_QtModules_CollisionWarning
class Q_SLICER_QTMODULES_COLLISIONWARNING_EXPORT qSlicerCollisionWarningModule :
  public qSlicerLoadableModule
{
  Q_OBJECT
  QVTK_OBJECT
  Q_INTERFACES(qSlicerLoadableModule);
  Q_PROPERTY(double warningSoundPeriodSec READ warningSoundPeriodSec WRITE setWarningSoundPeriodSec)

public:

  typedef qSlicerLoadableModule Superclass;
  explicit qSlicerCollisionWarningModule(QObject *parent=0);
  virtual ~qSlicerCollisionWarningModule();

  qSlicerGetTitleMacro(QTMODULE_TITLE);
  
  /// Help to use the module
  virtual QString helpText()const;

  /// Return acknowledgements
  virtual QString acknowledgementText()const;

  /// Return the authors of the module
  virtual QStringList  contributors()const;

  /// Return a custom icon for the module
  virtual QIcon icon()const;

  /// Return the categories for the module
  virtual QStringList categories()const;
  
  /// Return the dependencies for the module  
  virtual QStringList dependencies() const;

  /// Set period time of the warning sound. If period is 0.5 sec then the sound will be played 2x while inside
  /// the breach region.
  void setWarningSoundPeriodSec(double periodTimeSec);
  double warningSoundPeriodSec();

public slots:
/*
  virtual void setMRMLScene(vtkMRMLScene*);
  void onNodeAddedEvent(vtkObject*, vtkObject*);
  void onNodeRemovedEvent(vtkObject*, vtkObject*);
*/
  void updateWarningSound();
  void stopSound();

protected:

  /// Initialize the module. Register the volumes reader/writer
  virtual void setup();

  /// Create and return the widget representation associated to this module
  virtual qSlicerAbstractModuleRepresentation * createWidgetRepresentation();

  /// Create and return the logic associated to this module
  virtual vtkMRMLAbstractLogic* createLogic();

protected:
  QScopedPointer<qSlicerCollisionWarningModulePrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerCollisionWarningModule);
  Q_DISABLE_COPY(qSlicerCollisionWarningModule);

};

#endif
