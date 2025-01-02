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
#include "vtkSlicerCornerTextLogic.h"

// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLTextNode.h>

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCornerAnnotation.h>
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
  enum TextLocation {
    CORNER_BL,
    CORNER_BR,
    CORNER_TL,
    CORNER_TR,
    EDGE_B,
    EDGE_R,
    EDGE_L,
    EDGE_T
  };

  static constexpr TextLocation locations[] = {CORNER_BL, CORNER_BR, CORNER_TL,
                                               CORNER_TR, EDGE_B,    EDGE_R,
                                               EDGE_L,    EDGE_T};

  vtkInternal(vtkMRMLCornerTextDisplayableManager *external);
  ~vtkInternal();

  // CornerText
  void AddNode(vtkMRMLTransformNode *displayableNode);
  void RemoveNode(vtkMRMLTransformNode *displayableNode);
  void UpdateDisplayableCornerText(vtkMRMLNode *node);

  // Slice Node
  void SetSliceNode(vtkMRMLSliceNode *sliceNode);
  void UpdateSliceNode();
  vtkMRMLNode *GetTextNodeFromSliceNode(TextLocation);

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
vtkMRMLNode *
vtkMRMLCornerTextDisplayableManager::vtkInternal::GetTextNodeFromSliceNode(
    TextLocation location)
{
  switch (location) 
  {
  case CORNER_BL:
    return this->SliceNode->GetNodeReference("bottomLeftText");
  case CORNER_BR:
    return this->SliceNode->GetNodeReference("bottomRightText");
  case CORNER_TL:
    return this->SliceNode->GetNodeReference("topLeftText");
  case CORNER_TR:
    return this->SliceNode->GetNodeReference("topRightText");
  case EDGE_B:
    return this->SliceNode->GetNodeReference("bottomEdgeText");
  case EDGE_R:
    return this->SliceNode->GetNodeReference("rightEdgeText");
  case EDGE_L:
    return this->SliceNode->GetNodeReference("leftEdgeText");
  case EDGE_T:
    return this->SliceNode->GetNodeReference("topEdgeText");
  default:
    return nullptr;
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
  QString sliceViewName = this->SliceNode->GetLayoutName();
  vtkCornerAnnotation *cA = qSlicerApplication::application()
                                ->layoutManager()
                                ->sliceWidget(sliceViewName)
                                ->overlayCornerAnnotation();

  for (TextLocation loc : locations) 
  {
    const std::string &generatedText =
        vtkSlicerCornerTextLogic::GenerateCornerAnnotation(
            this->SliceNode,
            vtkMRMLTextNode::SafeDownCast(this->GetTextNodeFromSliceNode(loc)));
    cA->SetText(loc, generatedText.c_str());
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
