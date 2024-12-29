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

// CornerText Logic includes
#include "vtkSlicerCornerTextLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLTextNode.h>
#include <vtkXMLDataParser.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerCornerTextLogic);

//----------------------------------------------------------------------------
vtkSlicerCornerTextLogic::vtkSlicerCornerTextLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerCornerTextLogic::~vtkSlicerCornerTextLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerCornerTextLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerCornerTextLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerCornerTextLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCornerTextLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerCornerTextLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerCornerTextLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
vtkMRMLNode* vtkSlicerCornerTextLogic
::GetTextNodeFromSliceView(vtkMRMLSliceNode* sliceNode, TextLocation location)
{
  switch (location) {
      case CORNER_BL:
          return sliceNode->GetNodeReference("bottomLeftText");
      case CORNER_BR:
          return sliceNode->GetNodeReference("bottomRightText");
      case CORNER_TL:
          return sliceNode->GetNodeReference("topLeftText");
      case CORNER_TR:
          return sliceNode->GetNodeReference("topRightText");
      case EDGE_B:
          return sliceNode->GetNodeReference("bottomEdgeText");
      case EDGE_R:
          return sliceNode->GetNodeReference("rightEdgeText");
      case EDGE_L:
          return sliceNode->GetNodeReference("leftEdgeText");
      case EDGE_T:
          return sliceNode->GetNodeReference("topEdgeText");
      default:
          return nullptr;
  }
}

//----------------------------------------------------------------------------
vtkXMLDataElement* vtkSlicerCornerTextLogic::ParseTextNode(vtkMRMLTextNode* textNode)
{
  if (!textNode)
  {
    return nullptr;
  }

  std::istringstream iss(textNode->GetText(), std::istringstream::in);
  vtkNew<vtkXMLDataParser> parser;
  parser->SetStream(&iss);
  parser->Parse();

  vtkXMLDataElement* root = parser->GetRootElement();
  if (root==nullptr)
  {
    vtkErrorWithObjectMacro(parser, "vtkMRMLLayoutNode::ParseLayout: failed to parse layout description");
    return nullptr;
  }

  // if we don't register, then the root element will be destroyed when the
  // parser gets out of scope
  root->Register(nullptr);
  return root;
}


// //---------------------------------------------------------------------------
// std::string vtkSlicerCornerTextLogic
// ::GenerateCornerAnnotation(vtkMRMLSliceNode* sliceNode, vtkMRMLTextNode* textNode)
// {
//  vtkXMLDataElement* ele = ParseTextNode(textNode);
// 
// 
// }
