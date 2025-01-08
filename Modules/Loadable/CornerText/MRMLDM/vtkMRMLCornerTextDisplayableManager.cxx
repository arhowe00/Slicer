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
  void SetSliceNode(vtkMRMLSliceNode *sliceNode);
  void UpdateSliceNode();
  bool GetLocationEnabled(vtkMRMLCornerTextLogic::TextLocation);
  vtkMRMLTextNode *GetTextNode();

private:
  vtkMRMLCornerTextDisplayableManager *External;
  bool AddingNode;
  vtkSmartPointer<vtkMRMLSliceNode> SliceNode;
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
  this->SliceNode = nullptr;
}

//---------------------------------------------------------------------------
vtkMRMLTextNode *
vtkMRMLCornerTextDisplayableManager::vtkInternal::GetTextNode()
{
  return this->SliceNode->GetCornerAnnotationsTextNode();
}

//---------------------------------------------------------------------------
bool vtkMRMLCornerTextDisplayableManager::vtkInternal::GetLocationEnabled(
    vtkMRMLCornerTextLogic::TextLocation location)
{
  switch (location) 
  {
  case vtkMRMLCornerTextLogic::CORNER_BL:
    return this->SliceNode->GetBottomLeftTextEnabled();
  case vtkMRMLCornerTextLogic::CORNER_BR:
    return this->SliceNode->GetBottomRightTextEnabled();
  case vtkMRMLCornerTextLogic::CORNER_TL:
    return this->SliceNode->GetTopLeftTextEnabled();
  case vtkMRMLCornerTextLogic::CORNER_TR:
    return this->SliceNode->GetTopRightTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_B:
    return this->SliceNode->GetBottomEdgeTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_R:
    return this->SliceNode->GetRightEdgeTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_L:
    return this->SliceNode->GetLeftEdgeTextEnabled();
  case vtkMRMLCornerTextLogic::EDGE_T:
    return this->SliceNode->GetTopEdgeTextEnabled();
  default:
    return false;
  }
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::SetSliceNode(
    vtkMRMLSliceNode *sliceNode)
{
  if (!sliceNode || this->SliceNode == sliceNode) 
  {
    return;
  }
  this->SliceNode = sliceNode;
  this->UpdateSliceNode();
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::UpdateSliceNode() 
{
  // Get vtkMRMLCornerTextLogic
  vtkMRMLCornerTextLogic *cornerTextLogic =
      this->External->GetMRMLApplicationLogic()->GetCornerTextLogic();

  if (!cornerTextLogic)
  {
    vtkErrorWithObjectMacro(
        this->External, "vtkMRMLCornerTextDisplayableManager::vtkInternal::"
                        "UpdateSliceNode() failed: invalid CornerText logic.");
    return;
  }

  // Get vtkCornerAnnotation from slice widget
  QString sliceViewName = this->SliceNode->GetLayoutName();
  vtkCornerAnnotation *cA = qSlicerApplication::application()
                                ->layoutManager()
                                ->sliceWidget(sliceViewName)
                                ->overlayCornerAnnotation();

  const std::array<std::string, 8> &generatedText =
      cornerTextLogic->GenerateAnnotations(
          this->SliceNode,
          this->GetTextNode());
  for (vtkMRMLCornerTextLogic::TextLocation loc :
       vtkMRMLCornerTextLogic::locations)
  {
    if (this->GetLocationEnabled(loc))
    {
      cA->SetText(loc, generatedText[loc].c_str());
      cA->GetTextProperty()->SetFontSize(14);
      cA->GetTextProperty()->SetFontFamilyToTimes();
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
  vtkMRMLScene *scene = this->GetMRMLScene();

  if (scene == nullptr || scene->IsBatchProcessing()) 
  {
    return;
  }

  if (vtkMRMLSliceNode::SafeDownCast(caller)) 
  {
    this->Internal->UpdateSliceNode();
    this->RequestRender();
  } 
  else 
  {
    this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
  }
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
  this->Internal->SetSliceNode(this->GetMRMLSliceNode());
  this->SetUpdateFromMRMLRequested(true);
}
