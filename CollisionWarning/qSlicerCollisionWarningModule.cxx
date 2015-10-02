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
#include <QDir>
#include <QPointer>
#include <QSound>
#include <QTime>
#include <QTimer>
#include <QtPlugin>

#include "qSlicerApplication.h"

// CollisionWarning Logic includes
#include <vtkSlicerCollisionWarningLogic.h>

// CollisionWarning includes
#include "qSlicerCollisionWarningModule.h"
#include "qSlicerCollisionWarningModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerCollisionWarningModule, qSlicerCollisionWarningModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_CollisionWarning
class qSlicerCollisionWarningModulePrivate
{
public:
  qSlicerCollisionWarningModulePrivate();

  vtkSlicerCollisionWarningLogic* ObservedLogic; // should be the same as logic(), it is used for adding/removing observer safely
  QTimer UpdateWarningSoundTimer;
  QPointer<QSound> WarningSound;
  double WarningSoundPeriodSec;
};

//-----------------------------------------------------------------------------
// qSlicerCollisionWarningModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCollisionWarningModulePrivate::qSlicerCollisionWarningModulePrivate()
: ObservedLogic(NULL)
{
}

//-----------------------------------------------------------------------------
// qSlicerCollisionWarningModule methods

//-----------------------------------------------------------------------------
qSlicerCollisionWarningModule::qSlicerCollisionWarningModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerCollisionWarningModulePrivate)
{
  Q_D(qSlicerCollisionWarningModule);
  d->WarningSoundPeriodSec = 0.5;
}

//-----------------------------------------------------------------------------
QStringList qSlicerCollisionWarningModule::categories()const
{
  return QStringList() << "IGT";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCollisionWarningModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
qSlicerCollisionWarningModule::~qSlicerCollisionWarningModule()
{
  Q_D(qSlicerCollisionWarningModule);
  if (!d->WarningSound.isNull())
  {
    d->WarningSound->stop();
  }
  disconnect(&d->UpdateWarningSoundTimer, SIGNAL(timeout()), this, SLOT(updateWarningSound()));
  this->qvtkReconnect(d->ObservedLogic, NULL, vtkCommand::ModifiedEvent, this, SLOT(updateWarningSound()));
  d->ObservedLogic = NULL;
}

//-----------------------------------------------------------------------------
QString qSlicerCollisionWarningModule::helpText()const
{
  return "This module can alert the user by color change and sound signal if a tool enters a restricted area. The restricted area is defined by a surface model, the tool position is defined by a linear transform. For help on how to use this module visit: <a href='http://www.slicerigt.org/'>SlicerIGT</a>";
}

//-----------------------------------------------------------------------------
QString qSlicerCollisionWarningModule::acknowledgementText()const
{
  return "This work was was funded by Cancer Care Ontario and the Ontario Consortium for Adaptive Interventions in Radiation Oncology (OCAIRO)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCollisionWarningModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Matthew Holden (Queen's University)");
  moduleContributors << QString("Jaime Garcia Guevara (Queen's University)");
  moduleContributors << QString("Andras Lasso (Queen's University)");
  moduleContributors << QString("Tamas Ungi (Queen's University)");
  // ...
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerCollisionWarningModule::icon()const
{
  return QIcon(":/Icons/CollisionWarning.png");
}

//-----------------------------------------------------------------------------
void qSlicerCollisionWarningModule::setup()
{
  Q_D(qSlicerCollisionWarningModule);

  this->Superclass::setup();

  connect(qSlicerApplication::application(), SIGNAL(lastWindowClosed()), this, SLOT(stopSound()));  

  vtkSlicerCollisionWarningLogic* moduleLogic = vtkSlicerCollisionWarningLogic::SafeDownCast(logic());

  if (d->WarningSound == NULL)
  {
    d->WarningSound = new QSound( QDir::toNativeSeparators( QString::fromStdString( moduleLogic->GetModuleShareDirectory()+"/alarm.wav" ) ) );
  }

  this->qvtkReconnect(d->ObservedLogic, moduleLogic, vtkCommand::ModifiedEvent, this, SLOT(updateWarningSound()));
  d->ObservedLogic = moduleLogic;

  d->UpdateWarningSoundTimer.setSingleShot(true);
  connect(&d->UpdateWarningSoundTimer, SIGNAL(timeout()), this, SLOT(updateWarningSound()));
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerCollisionWarningModule::createWidgetRepresentation()
{
  return new qSlicerCollisionWarningModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerCollisionWarningModule::createLogic()
{
  return vtkSlicerCollisionWarningLogic::New();
}

//------------------------------------------------------------------------------
void qSlicerCollisionWarningModule::updateWarningSound()
{
  Q_D(qSlicerCollisionWarningModule);
  if (d->WarningSound.isNull())
  {
    qWarning("Warning sound object is invalid");
    return;
  }
  if (d->ObservedLogic==NULL)
  {
    qWarning("ObservedLogic is invalid");
    return;
  }
  bool warningSoundShouldPlay = d->ObservedLogic->GetWarningSoundPlaying();
  if (warningSoundShouldPlay)
  {
    d->WarningSound->setLoops(1);
    d->WarningSound->play();
  }
  else
  {
    d->WarningSound->stop();
  }
  d->UpdateWarningSoundTimer.start(warningSoundPeriodSec()*1000);
}


//------------------------------------------------------------------------------
void qSlicerCollisionWarningModule::stopSound()
{
  Q_D(qSlicerCollisionWarningModule);
  if (!d->WarningSound.isNull())
  {
    d->WarningSound->stop();
    d->WarningSound=NULL;
  }
}

//------------------------------------------------------------------------------
void qSlicerCollisionWarningModule::setWarningSoundPeriodSec(double periodTimeSec)
{
  Q_D(qSlicerCollisionWarningModule);
  d->WarningSoundPeriodSec = periodTimeSec;
}

//------------------------------------------------------------------------------
double qSlicerCollisionWarningModule::warningSoundPeriodSec()
{
  Q_D(qSlicerCollisionWarningModule);
  return d->WarningSoundPeriodSec;
}
