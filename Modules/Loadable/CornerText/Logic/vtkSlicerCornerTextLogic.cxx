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

//----------------------------------------------------------------------------
vtkXMLDataElement *
vtkSlicerCornerTextLogic::ParseTextNode(vtkMRMLTextNode *textNode)
{
  if (!textNode)
  {
    return nullptr;
  }

  std::string wrappedText = "<root>" + textNode->GetText() + "</root>";

  std::istringstream iss(wrappedText, std::istringstream::in);
  vtkNew<vtkXMLDataParser> parser;
  parser->SetStream(&iss);
  parser->Parse();

  vtkXMLDataElement *root = parser->GetRootElement();
  if (root == nullptr) 
  {
    // this should not occur because we wrapped with a root tag. However we
    // should consider a check if the node had a root tag already.
    vtkErrorWithObjectMacro(parser, "vtkSlicerCornerTextLogic::ParseTextNode: failed to parse layout description");
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
