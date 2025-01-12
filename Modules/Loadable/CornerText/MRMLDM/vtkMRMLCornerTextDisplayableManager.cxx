/*==============================================================================

  Program: 3D Slicer

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// MRMLDisplayableManager includes
#include "vtkMRMLCornerTextDisplayableManager.h"
#include "vtkMRMLSliceLogic.h"

// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLTextNode.h>
#include <vtkMRMLCornerTextLogic.h>
#include <vtkMRMLApplicationLogic.h>

// VTK includes
#include <vtkSetGet.h>
#include <vtkCallbackCommand.h>
#include <vtkCornerAnnotation.h>
#include <vtkTextProperty.h>
#include <vtkEventBroker.h>
#include <vtkRenderer.h>

// STD includes
#include <algorithm>
#include <cassert>
#include <map>
#include <set>

// Slicer includes
#include <qMRMLSliceWidget.h>
#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLCornerTextDisplayableManager);

//---------------------------------------------------------------------------
class vtkMRMLCornerTextDisplayableManager::vtkInternal {
public:
  vtkInternal(vtkMRMLCornerTextDisplayableManager *external);
  ~vtkInternal();

  // CornerText
  void AddNode(vtkMRMLTransformNode *displayableNode);
  void RemoveNode(vtkMRMLTransformNode *displayableNode);
  void UpdateDisplayableCornerText(vtkMRMLNode *node);

  // Slice Node
  void UpdateCornerAnnotationsFromSliceNode();
  bool GetLocationEnabled(int);
  vtkMRMLTextNode *GetTextNode();

private:
  vtkMRMLCornerTextDisplayableManager *External;
  bool AddingNode;
};

//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLCornerTextDisplayableManager::vtkInternal::vtkInternal(
    vtkMRMLCornerTextDisplayableManager *external)
    : External(external), AddingNode(false) {}

//---------------------------------------------------------------------------
vtkMRMLCornerTextDisplayableManager::vtkInternal::~vtkInternal()
{
}

//---------------------------------------------------------------------------
vtkMRMLTextNode *
vtkMRMLCornerTextDisplayableManager::vtkInternal::GetTextNode()
{
  return this->External->GetMRMLSliceNode()->GetCornerAnnotationsTextNode();
}

//---------------------------------------------------------------------------
bool vtkMRMLCornerTextDisplayableManager::vtkInternal::GetLocationEnabled(
    int location)
{
  switch (location) 
  {
  case vtkMRMLCornerTextLogic::CORNER_BL:
    return this->External->GetMRMLSliceNode()->GetBottomLeftTextEnabled();
  case vtkMRMLCornerTextLogic::CORNER_BR:
    return this->External->GetMRMLSliceNode()->GetBottomRightTextEnabled();
  case vtkMRMLCornerTextLogic::CORNER_TL:
    return this->External->GetMRMLSliceNode()->GetTopLeftTextEnabled();
  case vtkMRMLCornerTextLogic::CORNER_TR:
    return this->External->GetMRMLSliceNode()->GetTopRightTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_B:
    return this->External->GetMRMLSliceNode()->GetBottomEdgeTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_R:
    return this->External->GetMRMLSliceNode()->GetRightEdgeTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_L:
    return this->External->GetMRMLSliceNode()->GetLeftEdgeTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_T:
    return this->External->GetMRMLSliceNode()->GetTopEdgeTextEnabled();
  default:
    return false;
  }
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::UpdateCornerAnnotationsFromSliceNode() 
{
  // Get vtkMRMLCornerTextLogic
  vtkMRMLCornerTextLogic *cornerTextLogic =
      this->External->GetMRMLApplicationLogic()->GetCornerTextLogic();

  if (!cornerTextLogic)
  {
    vtkErrorWithObjectMacro(
        this->External, "vtkMRMLCornerTextDisplayableManager::vtkInternal::"
                        "UpdateCornerAnnotationsFromSliceNode() failed: invalid CornerText logic.");
    return;
  }

  // Get vtkCornerAnnotation from slice widget
  std::string sliceViewName = this->External->GetMRMLSliceNode()->GetLayoutName();
  // This should be specific to each viewer and not shared between viewers.
  // DM insantiates the corner annotation and associate with the renderer.
  // TODO: Remove dependency on qSlicerApplication
  vtkCornerAnnotation* cornerAnnotation =
      qSlicerApplication::application()
          ->layoutManager()
          ->sliceWidget(QString::fromStdString(sliceViewName))
          ->overlayCornerAnnotation();

  const std::array<std::string, 8> generatedText =
      cornerTextLogic->GenerateAnnotations(
          this->External->GetMRMLSliceNode(),
          this->GetTextNode());
  for (int idx = 0; idx < vtkMRMLCornerTextLogic::TextLocation_Last; ++idx)
  {
    if (this->GetLocationEnabled(idx))
    {
      cornerAnnotation->SetText(idx, generatedText[idx].c_str());
      cornerAnnotation->GetTextProperty()->SetFontSize(cornerTextLogic->GetFontSize());
      if (cornerTextLogic->GetFontFamily() == "Arial")
      {
        cornerAnnotation->GetTextProperty()->SetFontFamilyToArial();
      }
      else // times is the default, but the font family should be == "Times"
      {
        cornerAnnotation->GetTextProperty()->SetFontFamilyToTimes();
      }
    }
  }
  return;
}

//---------------------------------------------------------------------------
// vtkMRMLCornerTextDisplayableManager methods

//---------------------------------------------------------------------------
vtkMRMLCornerTextDisplayableManager::vtkMRMLCornerTextDisplayableManager() 
{
  this->Internal = new vtkInternal(this);
}

//---------------------------------------------------------------------------
vtkMRMLCornerTextDisplayableManager::~vtkMRMLCornerTextDisplayableManager() 
{
  delete this->Internal;
  this->Internal = nullptr;
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::PrintSelf(ostream &os,
                                                    vtkIndent indent) 
{
  this->Superclass::PrintSelf(os, indent);
  os << indent
     << "vtkMRMLCornerTextDisplayableManager: " << this->GetClassName() << "\n";
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::ProcessMRMLNodesEvents(
    vtkObject *caller, unsigned long event, void *callData) 
{
  this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::ProcessMRMLLogicsEvents(
    vtkObject* caller, unsigned long event, void *callData) 
{
  vtkMRMLScene* scene = this->GetMRMLScene();

  if (scene == nullptr || scene->IsBatchProcessing()) 
  {
    return;
  }

  if (vtkMRMLSliceLogic::SafeDownCast(caller) != nullptr)
  {
    this->Internal->UpdateCornerAnnotationsFromSliceNode();
  }

  this->Superclass::ProcessMRMLLogicsEvents(caller, event, callData);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::UpdateFromMRML() 
{
  this->SetUpdateFromMRMLRequested(false);

  vtkMRMLScene *scene = this->GetMRMLScene();
  if (!scene) 
  {
    vtkDebugMacro("vtkMRMLCornerTextDisplayableManager->UpdateFromMRML: Scene "
                  "is not set.");
    return;
  }

  this->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::UnobserveMRMLScene() 
{ 
  return; 
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::OnMRMLSceneStartClose() 
{ 
  return; 
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::OnMRMLSceneEndClose() 
{
  this->SetUpdateFromMRMLRequested(true);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::OnMRMLSceneEndBatchProcess() 
{
  this->SetUpdateFromMRMLRequested(true);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::Create() 
{
  vtkMRMLSliceLogic *sliceLogic =
      this->GetMRMLApplicationLogic()->GetSliceLogic(this->GetMRMLSliceNode());
  vtkEventBroker::GetInstance()->AddObservation(
      sliceLogic, vtkCommand::ModifiedEvent, this, this->GetMRMLLogicsCallbackCommand());

  this->SetUpdateFromMRMLRequested(true);
}
