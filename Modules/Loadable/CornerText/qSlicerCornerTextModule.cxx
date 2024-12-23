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

// CornerText Logic includes
#include <vtkSlicerCornerTextLogic.h>

// CornerText includes
#include "qSlicerCornerTextModule.h"
#include "qSlicerCornerTextModuleWidget.h"

//// DisplayableManager initialization
//#include <vtkAutoInit.h>
//VTK_MODULE_INIT(vtkSlicerCornerTextModuleMRMLDisplayableManager)

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

// see here for displayable manager stuff which should be done here
// https://www.slicer.org/wiki/Documentation/4.5/Developers/DisplayableManagers
/*
 * jcfr wants you to add a text node and associated displayable manager to
 * create the vtk actors when a display node is added onto the scene. It needs
 * to be registered into the view factory which will instantiate the displayable
 * manager for the text node. (maybe one displayable manager for each text node
 * in the slice view).
 *
 * Useful methods of displayable manager to reimplement:
 OnMRMLSceneNodeAdded(vtkMRMLNode*): to observe the scene and setup observations
 of nodes of interest as soon as they are added into the scene
 OnMRMLNodeModified(vtkMRMLNode*): called each time a node is modified, it is
 here that you can update the display (typically update the VTK widgets).
 *
 * Displayable managers automatically observe the scene and when a matching
 * display node is added into the scene, they create corresponding VTK actors to
 * add them in their VTK renderer. For example, the model displayable manager
 * creates a vtkPolyDataActor and vtkPolyDataMapper when a
 * vtkMRMLModelDisplayNode is added into the scene.
 Displayable managers are registered into a view factory (e.g. 3D view
 displayable manager factory) and when a view is created, the factory
 instantiate each displayable manager and associate them to the view. There is 1
 instance of each displayable manager per view/renderer. For example, in the
 conventional layout, the 2D model displayable manager
 (vtkMRMLModelSliceDisplayableManager) is instantiated 3 times: once for the red
 slice view, once for the yellow slice view and once for the green slice view.
 Theoretically the slice logics should be displayable managers ( however, it
 might be a huge effort for just the sake of being consistent ).
 */
//-----------------------------------------------------------------------------
void qSlicerCornerTextModule::setup()
{
  this->Superclass::setup();

  // Register displayable managers
  //vtkMRMLSliceViewDisplayableManagerFactory::GetInstance()->RegisterDisplayableManager("vtkMRMLCornerTextDisplayableManager2D");

}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerCornerTextModule
::createWidgetRepresentation()
{
  return new qSlicerCornerTextModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerCornerTextModule::createLogic()
{
  return vtkSlicerCornerTextLogic::New();
}
