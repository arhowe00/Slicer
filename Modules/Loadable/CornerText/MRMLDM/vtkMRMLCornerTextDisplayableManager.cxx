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
#include "vtkMRMLAbstractDisplayableManager.h"
#include "vtkMRMLSliceLogic.h"

// MRML includes
#include <valarray>
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
#include <vtkSmartPointer.h>
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
  void UpdateCornerAnnotationsFromSliceNode(bool printWarnings = true);
  bool GetLocationEnabled(int);
  vtkMRMLTextNode *GetTextNode();

  vtkCornerAnnotation* CornerAnnotation;

private:
  vtkMRMLCornerTextDisplayableManager *External;
};

//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLCornerTextDisplayableManager::vtkInternal::vtkInternal(
    vtkMRMLCornerTextDisplayableManager *external)
    : External(external) {}

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
void vtkMRMLCornerTextDisplayableManager::vtkInternal::
    UpdateCornerAnnotationsFromSliceNode(bool printWarnings)
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

  // Make sure we have set CornerAnnotation
  if (this->CornerAnnotation == nullptr)
  {
    vtkWarningWithObjectMacro(
        this->External, "vtkMRMLCornerTextDisplayableManager::vtkInternal::"
                        "UpdateCornerAnnotationsFromSliceNode() failed: invalid CornerAnnotation.");
    return;
  }

  const std::array<std::string, 8> generatedText =
      cornerTextLogic->GenerateAnnotations(
          this->External->GetMRMLSliceNode(),
          this->GetTextNode(),
          printWarnings);
  for (int idx = 0; idx < vtkMRMLCornerTextLogic::TextLocation_Last; ++idx)
  {
    this->CornerAnnotation->SetText(idx, this->GetLocationEnabled(idx) ? generatedText[idx].c_str() : "");
  }

  if (cornerTextLogic->GetFontFamily() == "Arial")
  {
    this->CornerAnnotation->GetTextProperty()->SetFontFamilyToArial();
  }
  else
  {
    this->CornerAnnotation->GetTextProperty()->SetFontFamilyToTimes();
  }
  this->CornerAnnotation->SetMinimumFontSize(cornerTextLogic->GetFontSize());
  this->CornerAnnotation->SetMaximumFontSize(cornerTextLogic->GetFontSize());
  this->CornerAnnotation->SetNonlinearFontScaleFactor(1);

  this->External->RequestRender();
}

//---------------------------------------------------------------------------
// vtkMRMLCornerTextDisplayableManager methods

//---------------------------------------------------------------------------

vtkCornerAnnotation *
vtkMRMLCornerTextDisplayableManager::GetCornerAnnotation() const
{
  return this->Internal->CornerAnnotation;
}

void vtkMRMLCornerTextDisplayableManager::SetCornerAnnotation(
    vtkCornerAnnotation *cornerAnnotation) const
{
  this->Internal->CornerAnnotation = cornerAnnotation;
}

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
  vtkMRMLScene* scene = this->GetMRMLScene();

  if (scene == nullptr || scene->IsBatchProcessing()) 
  {
    return;
  }

  if (vtkMRMLTextNode::SafeDownCast(caller) != nullptr)
  {
    this->Internal->UpdateCornerAnnotationsFromSliceNode(/* printWarnings= */ true);
  }

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

  if (vtkMRMLCornerTextLogic::SafeDownCast(caller) != nullptr)
  {
    this->Internal->UpdateCornerAnnotationsFromSliceNode(/* printWarnings= */ false);
  }
  if (vtkMRMLSliceLogic::SafeDownCast(caller) != nullptr)
  {
    this->Internal->UpdateCornerAnnotationsFromSliceNode(/* printWarnings= */ false);
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
void vtkMRMLCornerTextDisplayableManager::OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller)
{
    this->Internal->UpdateCornerAnnotationsFromSliceNode(/* printWarnings= */ false);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::Create() 
{
  // Observe the slice logic
  vtkMRMLSliceLogic *sliceLogic =
      this->GetMRMLApplicationLogic()->GetSliceLogic(this->GetMRMLSliceNode());
  if (sliceLogic == nullptr)
  {
    vtkErrorWithObjectMacro(
        sliceLogic, "vtkMRMLCornerTextDisplayableManager::Create() failed: invalid sliceLogic.");
    return;
  }
  vtkEventBroker::GetInstance()->AddObservation(
      sliceLogic, vtkCommand::ModifiedEvent, this, this->GetMRMLLogicsCallbackCommand());

  // Observe the corner text logic
  vtkMRMLCornerTextLogic *cornerTextLogic = this->GetMRMLApplicationLogic()->GetCornerTextLogic();
  if (cornerTextLogic == nullptr)
  {
    vtkErrorWithObjectMacro(
        cornerTextLogic, "vtkMRMLCornerTextDisplayableManager::Create() failed: invalid cornerTextLogic.");
    return;
  }
  vtkEventBroker::GetInstance()->AddObservation(
      cornerTextLogic, vtkCommand::ModifiedEvent, this, this->GetMRMLLogicsCallbackCommand());

  // Observe the text node
  vtkMRMLTextNode *textNode = this->GetMRMLSliceNode()->GetCornerAnnotationsTextNode();
  if (textNode == nullptr)
  {
    vtkErrorWithObjectMacro(
        cornerTextLogic, "vtkMRMLCornerTextDisplayableManager::Create() failed: invalid textNode.");
    return;
  }
  vtkEventBroker::GetInstance()->AddObservation(
      textNode, vtkCommand::ModifiedEvent, this, this->GetMRMLNodesCallbackCommand());

  // As our slice logic callback does not generate warnings, we want to do an
  // initial render with warnings printed.
  this->Internal->UpdateCornerAnnotationsFromSliceNode(true);

  this->SetUpdateFromMRMLRequested(true);
}
