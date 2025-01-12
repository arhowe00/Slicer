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

#include <vtkMRMLSliceViewDisplayableManagerFactory.h>

// Slicer includes
#include <vtkMRMLCornerTextLogic.h>
#include <vtkSlicerApplicationLogic.h>
#include "qSlicerApplication.h"

// CornerText includes
#include "qSlicerCornerTextModule.h"
#include "qSlicerCornerTextModuleWidget.h"
#include "vtkMRMLDefaultAnnotationPropertyValueProvider.h"
#include "vtkMRMLNode.h"

// DisplayableManager initialization
#include <vtkAutoInit.h>

// Qt includes
#include <QSettings>

VTK_MODULE_INIT(vtkSlicerCornerTextModuleMRMLDisplayableManager)

//-----------------------------------------------------------------------------
class qSlicerCornerTextModulePrivate
{
public:
  qSlicerCornerTextModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCornerTextModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCornerTextModulePrivate::qSlicerCornerTextModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCornerTextModule methods

//-----------------------------------------------------------------------------
qSlicerCornerTextModule::qSlicerCornerTextModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerCornerTextModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerCornerTextModule::~qSlicerCornerTextModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerCornerTextModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerCornerTextModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCornerTextModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerCornerTextModule::icon() const
{
  return QIcon(":/Icons/CornerText.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerCornerTextModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCornerTextModule::dependencies() const
{
  return QStringList();
}

void qSlicerCornerTextModule::readSettings() const
{
  QSettings* settings = qSlicerApplication::application()->settingsDialog()->settings();
  vtkMRMLCornerTextLogic *cornerTextLogic =
      this->appLogic()->GetCornerTextLogic();

  cornerTextLogic->SetDisplayStrictness(settings->value("DataProbe/sliceViewAnnotations.displayLevel", 1).toInt());
  cornerTextLogic->SetFontSize(settings->value("DataProbe/sliceViewAnnotations.fontSize", 14).toInt());
  cornerTextLogic->SetFontFamily(settings->value("DataProbe/sliceViewAnnotations.fontFamily", "Times").toString().toStdString());

  return;
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextModule::setup()
{
  this->Superclass::setup();

  // Register displayable managers
  vtkMRMLSliceViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager("vtkMRMLCornerTextDisplayableManager");

  // Register default annotation provider
  vtkNew<vtkMRMLDefaultAnnotationPropertyValueProvider> provider;
  provider->SetAppLogic(this->appLogic());
  this->appLogic()->GetCornerTextLogic()->RegisterPropertyValueProvider(
      "Default", provider);

  // Read DataProbe settings
  this->readSettings();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerCornerTextModule
::createWidgetRepresentation()
{
  return new qSlicerCornerTextModuleWidget;
}
