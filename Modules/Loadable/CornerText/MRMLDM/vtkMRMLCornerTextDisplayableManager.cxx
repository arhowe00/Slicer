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

  This file was originally developed by Andras Lasso and Franklin King at
  PerkLab, Queen's University and was supported through the Applied Cancer
  Research Unit program of Cancer Care Ontario with funds provided by the
  Ontario Ministry of Health and Long-Term Care.

==============================================================================*/


// MRMLDisplayableManager includes
#include "vtkMRMLCornerTextDisplayableManager.h"

#include "vtkSlicerCornerTextLogic.h"

// MRML includes
#include <vtkMRMLProceduralColorNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceCompositeNode.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLDisplayNode.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLTextNode.h>

// VTK includes
#include <vtkActor2D.h>
#include <vtkCallbackCommand.h>
#include <vtkColorTransferFunction.h>
#include <vtkEventBroker.h>
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkPointLocator.h>
#include <vtkCornerAnnotation.h>

// STD includes
#include <algorithm>
#include <cassert>
#include <set>
#include <map>

// Slicer includes
#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <qMRMLSliceWidget.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLCornerTextDisplayableManager );

//---------------------------------------------------------------------------
class vtkMRMLCornerTextDisplayableManager::vtkInternal
{
public:

  enum TextLocation
  {
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

  vtkInternal( vtkMRMLCornerTextDisplayableManager* external );
  ~vtkInternal();

  // CornerText
  void AddNode(vtkMRMLTransformNode* displayableNode);
  void RemoveNode(vtkMRMLTransformNode* displayableNode);
  void UpdateDisplayableCornerText(vtkMRMLNode *node);

  // Slice Node
  void SetSliceNode(vtkMRMLSliceNode* sliceNode);
  void UpdateSliceNode();
  vtkMRMLNode* GetTextNodeFromSliceNode(TextLocation);

  // Display Nodes
  void UpdateDisplayNode(vtkMRMLDisplayNode* displayNode);
  void RemoveDisplayNode(vtkMRMLDisplayNode* displayNode);

  // Observations
  void AddObservations(vtkMRMLNode* node);
  void RemoveObservations(vtkMRMLNode* node);
  bool IsNodeObserved(vtkMRMLNode* node);

  // Helper functions
  bool IsVisible(vtkMRMLDisplayNode* displayNode);
  bool UseDisplayNode(vtkMRMLDisplayNode* displayNode);
  bool UseDisplayableNode(vtkMRMLNode* node);
  void ClearDisplayableNodes();

private:
  vtkMRMLCornerTextDisplayableManager* External;
  bool AddingNode;
  vtkSmartPointer<vtkMRMLSliceNode> SliceNode;
};

//---------------------------------------------------------------------------
// vtkInternal methods

//---------------------------------------------------------------------------
vtkMRMLCornerTextDisplayableManager::vtkInternal::vtkInternal(vtkMRMLCornerTextDisplayableManager* external)
: External(external)
, AddingNode(false)
{
}

//---------------------------------------------------------------------------
vtkMRMLCornerTextDisplayableManager::vtkInternal::~vtkInternal()
{
  this->ClearDisplayableNodes();
  this->SliceNode = nullptr;
}

//---------------------------------------------------------------------------
bool vtkMRMLCornerTextDisplayableManager::vtkInternal::UseDisplayNode(vtkMRMLDisplayNode* displayNode)
{
   // allow nodes to appear only in designated viewers
  if (displayNode && !displayNode->IsDisplayableInView(this->SliceNode->GetID()))
  {
    return false;
  }

  // Check whether DisplayNode should be shown in this view
  bool use = displayNode && displayNode->IsA("vtkMRMLDisplayNode");

  return use;
}

//---------------------------------------------------------------------------
bool vtkMRMLCornerTextDisplayableManager::vtkInternal::IsVisible(vtkMRMLDisplayNode* displayNode)
{
  return displayNode && (displayNode->GetVisibility() != 0) && (displayNode->GetVisibility2D() != 0);
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLCornerTextDisplayableManager::vtkInternal::
GetTextNodeFromSliceNode(TextLocation location)
{
  switch (location) {
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
void vtkMRMLCornerTextDisplayableManager::vtkInternal::SetSliceNode(vtkMRMLSliceNode* sliceNode)
{
  if (!sliceNode || this->SliceNode == sliceNode)
  {
    return;
  }
  this->SliceNode=sliceNode;
  this->UpdateSliceNode();
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::UpdateSliceNode()
{
  QString sliceViewName = this->SliceNode->GetLayoutName();
  vtkCornerAnnotation* cA = qSlicerApplication::application()->layoutManager()->sliceWidget(sliceViewName)->overlayCornerAnnotation();

  for (TextLocation loc : locations)
  {
    const std::string& generatedText =
      vtkSlicerCornerTextLogic::GenerateCornerAnnotation(this->SliceNode,
          vtkMRMLTextNode::SafeDownCast(this->GetTextNodeFromSliceNode(loc)));
    cA->SetText(loc, generatedText.c_str());
  }
  return;
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::AddNode(vtkMRMLTransformNode* node)
{
  if (this->AddingNode)
  {
    return;
  }
  // Check if node should be used
  if (!this->UseDisplayableNode(node))
  {
    return;
  }

  this->AddingNode = true;
  // Add Display Nodes
  int nnodes = node->GetNumberOfDisplayNodes();

  this->AddObservations(node);

  for (int i=0; i<nnodes; i++)
  {
    vtkMRMLDisplayNode *dnode = vtkMRMLTransformDisplayNode::SafeDownCast(node->GetNthDisplayNode(i));
    if ( this->UseDisplayNode(dnode) )
    {
      this->ToDisplayNodes[node].insert(dnode);
      this->AddDisplayNode( node, dnode );
    }
  }
  this->AddingNode = false;
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::RemoveNode(vtkMRMLTransformNode* node)
{
  if (!node)
  {
    return;
  }
  vtkInternal::ToDisplayCacheType::iterator displayableIt =
    this->ToDisplayNodes.find(node);
  if(displayableIt == this->ToDisplayNodes.end())
  {
    return;
  }

  std::set< vtkMRMLDisplayNode* > dnodes = displayableIt->second;
  std::set< vtkMRMLDisplayNode* >::iterator diter;
  for ( diter = dnodes.begin(); diter != dnodes.end(); ++diter)
  {
    this->RemoveDisplayNode(*diter);
  }
  this->RemoveObservations(node);
  this->ToDisplayNodes.erase(displayableIt);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::UpdateDisplayableCornerText(vtkMRMLNode* mNode)
{
  // Update the pipeline for all tracked DisplayableNode

  PipelinesCacheType::iterator pipelinesIter;
  std::set< vtkMRMLDisplayNode* > displayNodes = this->TransformToDisplayNodes[mNode];
  std::set< vtkMRMLDisplayNode* >::iterator dnodesIter;
  for ( dnodesIter = displayNodes.begin(); dnodesIter != displayNodes.end(); dnodesIter++ )
  {
    if ( ((pipelinesIter = this->DisplayPipelines.find(*dnodesIter)) != this->DisplayPipelines.end()) )
    {
      this->UpdateDisplayNodePipeline(pipelinesIter->first, pipelinesIter->second);
    }
  }
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::RemoveDisplayNode(vtkMRMLDisplayNode* displayNode)
{
  PipelinesCacheType::iterator actorsIt = this->DisplayPipelines.find(displayNode);
  if(actorsIt == this->DisplayPipelines.end())
  {
    return;
  }
  const Pipeline* pipeline = actorsIt->second;
  this->External->GetRenderer()->RemoveActor(pipeline->Actor);
  delete pipeline;
  this->DisplayPipelines.erase(actorsIt);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::AddDisplayNode(vtkMRMLNode* mNode, vtkMRMLTransformDisplayNode* displayNode)
{
  if (!mNode || !displayNode)
  {
    return;
  }

  // Do not add the display node if it is already associated with a pipeline object.
  // This happens when a transform node already associated with a display node
  // is copied into an other (using vtkMRMLNode::Copy()) and is added to the scene afterward.
  // Related issue are #3428 and #2608
  PipelinesCacheType::iterator it;
  it = this->DisplayPipelines.find(displayNode);
  if (it != this->DisplayPipelines.end())
  {
    return;
  }

  vtkNew<vtkActor2D> actor;
  if (displayNode->IsA("vtkMRMLDisplayNode"))
  {
    actor->SetMapper( vtkNew<vtkPolyDataMapper2D>().GetPointer() );
  }

  // Create pipeline
  Pipeline* pipeline = new Pipeline();
  pipeline->Actor = actor.GetPointer();
  pipeline->ToSlice = vtkSmartPointer<vtkTransform>::New();
  pipeline->er = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

  // Set up pipeline
  pipeline->er->SetTransform(pipeline->TransformToSlice);
  pipeline->Actor->SetVisibility(0);

  // Add actor to Renderer and local cache
  this->External->GetRenderer()->AddActor( pipeline->Actor );
  this->DisplayPipelines.insert( std::make_pair(displayNode, pipeline) );

  // Update cached matrices. Calls UpdateDisplayNodePipeline
  this->UpdateDisplayableCornerText(mNode);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::AddObservations(vtkMRMLNode* node)
{
  vtkEventBroker* broker = vtkEventBroker::GetInstance();
  if (!broker->GetObservationExist(node, vtkMRMLDisplayableNode::DisplayModifiedEvent, this->External, this->External->GetMRMLNodesCallbackCommand() ))
  {
    broker->AddObservation(node, vtkMRMLDisplayableNode::DisplayModifiedEvent, this->External, this->External->GetMRMLNodesCallbackCommand() );
  }
  if (!broker->GetObservationExist(node, vtkMRMLableNode::TransformModifiedEvent, this->External, this->External->GetMRMLNodesCallbackCommand() ))
  {
    broker->AddObservation(node, vtkMRMLableNode::TransformModifiedEvent, this->External, this->External->GetMRMLNodesCallbackCommand() );
  }
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::RemoveObservations(vtkMRMLNode* node)
{
  vtkEventBroker* broker = vtkEventBroker::GetInstance();
  vtkEventBroker::ObservationVector observations;
  observations = broker->GetObservations(node, vtkMRMLableNode::TransformModifiedEvent, this->External, this->External->GetMRMLNodesCallbackCommand() );
  broker->RemoveObservations(observations);
  observations = broker->GetObservations(node, vtkMRMLDisplayableNode::DisplayModifiedEvent, this->External, this->External->GetMRMLNodesCallbackCommand() );
  broker->RemoveObservations(observations);
}

//---------------------------------------------------------------------------
bool vtkMRMLCornerTextDisplayableManager::vtkInternal::IsNodeObserved(vtkMRMLNode* node)
{
  vtkEventBroker* broker = vtkEventBroker::GetInstance();
  vtkCollection* observations = broker->GetObservationsForSubject(node);
  if (observations->GetNumberOfItems() > 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::vtkInternal::ClearDisplayableNodes()
{
  while(this->ToDisplayNodes.size() > 0)
  {
    this->RemoveNode(this->TransformToDisplayNodes.begin()->first);
  }
}

//---------------------------------------------------------------------------
bool vtkMRMLCornerTextDisplayableManager::vtkInternal::UseDisplayableNode(vtkMRMLNode* node)
{
  bool use = node && node->IsA("vtkMRMLNode");
  return use;
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
  this->Internal=nullptr;
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "vtkMRMLCornerTextDisplayableManager: " << this->GetClassName() << "\n";
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData)
{
  vtkMRMLScene* scene = this->GetMRMLScene();

  if (scene == nullptr || scene->IsBatchProcessing())
  {
    return;
  }

  vtkMRMLNode* displayableNode = vtkMRMLTransformNode::SafeDownCast(caller);

  if ( displayableNode )
  {
    vtkMRMLNode* callDataNode = reinterpret_cast<vtkMRMLDisplayNode *> (callData);
    vtkMRMLDisplayNode* displayNode = vtkMRMLTransformDisplayNode::SafeDownCast(callDataNode);

    if ( displayNode && (event == vtkMRMLDisplayableNode::DisplayModifiedEvent) )
    {
      this->Internal->UpdateDisplayNode(displayNode);
      this->RequestRender();
    }
    else if (event == vtkMRMLableNode::TransformModifiedEvent)
    {
      this->Internal->UpdateDisplayableCornerText(displayableNode);
      this->RequestRender();
    }
  }
  else if ( vtkMRMLSliceNode::SafeDownCast(caller) )
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

  vtkMRMLScene* scene = this->GetMRMLScene();
  if (!scene)
  {
    vtkDebugMacro( "vtkMRMLCornerTextDisplayableManager->UpdateFromMRML: Scene is not set.");
    return;
  }
  this->Internal->ClearDisplayableNodes();

  vtkMRMLNode* mNode = nullptr;
  std::vector<vtkMRMLNode *> mNodes;
  int nnodes = scene ? scene->GetNodesByClass("vtkMRMLNode", mNodes) : 0;
  for (int i=0; i<nnodes; i++)
  {
    mNode  = vtkMRMLNode::SafeDownCast(mNodes[i]);
    if (mNode && this->Internal->UseDisplayableNode(mNode))
    {
      this->Internal->AddNode(mNode);
    }
  }
  this->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::UnobserveMRMLScene()
{
  this->Internal->ClearDisplayableNodes();
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextDisplayableManager::OnMRMLSceneStartClose()
{
  this->Internal->ClearDisplayableNodes();
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
