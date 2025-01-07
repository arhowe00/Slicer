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

// MRMLLogic includes
#include "vtkMRMLCornerTextLogic.h"

// MRML includes
#include "vtkMRMLScene.h"
#include "vtkMRMLSliceNode.h"
#include "vtkMRMLTextNode.h"
#include "vtkXMLDataParser.h"
#include "vtkMRMLAbstractCornerTextTagValueProvider.h"

// VTK includes
#include <string>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLCornerTextLogic);

//----------------------------------------------------------------------------
vtkMRMLCornerTextLogic::vtkMRMLCornerTextLogic()
{
}

//----------------------------------------------------------------------------
vtkMRMLCornerTextLogic::~vtkMRMLCornerTextLogic()
{
}

//----------------------------------------------------------------------------
void vtkMRMLCornerTextLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkMRMLCornerTextLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkMRMLCornerTextLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}

//----------------------------------------------------------------------------
vtkXMLDataElement *
vtkMRMLCornerTextLogic::ParseTextNode(vtkMRMLTextNode *textNode)
{
  if (!textNode)
  {
    return nullptr;
  }

  std::istringstream iss(textNode->GetText(), std::istringstream::in);
  vtkNew<vtkXMLDataParser> parser;
  parser->SetStream(&iss);
  parser->Parse();

  vtkXMLDataElement *root = parser->GetRootElement();
  if (root == nullptr) 
  {
    // this should not occur because we wrapped with a root tag. However we
    // should consider a check if the node had a root tag already.
    vtkErrorWithObjectMacro(parser, "vtkMRMLCornerTextLogic::ParseTextNode: failed to parse layout description");
    return nullptr;
  }

  // if we don't register, then the root element will be destroyed when the
  // parser gets out of scope
  root->Register(nullptr);
  return root;
}

//---------------------------------------------------------------------------
bool vtkMRMLCornerTextLogic::RegisterTagValueProvider(const std::string &pluginName,
                               vtkMRMLAbstractCornerTextTagValueProvider *pluginProvider)
{
  if (registeredProviders.find(pluginName) == registeredProviders.end())
  {
    registeredProviders[pluginName] = pluginProvider;
    return true;
  }
  else
  {
    vtkWarningWithObjectMacro(
        pluginProvider, "Provider for " << pluginName << " already provided.");
  }
  return false;
}

//---------------------------------------------------------------------------
vtkMRMLTextNode *vtkMRMLCornerTextLogic::GetCornerAnnotations(vtkMRMLScene *mrmlScene,
                                               const int viewArrangement,
                                               const std::string& viewName)
{
  vtkMRMLTextNode *textNode;
  const std::string baseName = "CornerAnnotationsSingleton",
                    viewArrStr = "Layout" + std::to_string(viewArrangement);

  // check if a layout specific and view specific text node exists
  if ((textNode = vtkMRMLTextNode::SafeDownCast(mrmlScene->GetSingletonNode(
           (viewArrStr + viewName + baseName).c_str(), "vtkMRMLTextNode"))))
    return textNode;

  // check if a layout specific text node exists
  if ((textNode = vtkMRMLTextNode::SafeDownCast(mrmlScene->GetSingletonNode(
           (viewArrStr + baseName).c_str(), "vtkMRMLTextNode"))))
    return textNode;

  // if not, then CornerAnnotationsSingleton should always exist as it is
  // created with the scene.
  if (!(textNode = vtkMRMLTextNode::SafeDownCast(
            mrmlScene->GetSingletonNode(baseName.c_str(), "vtkMRMLTextNode"))))
    vtkErrorWithObjectMacro(mrmlScene,
                            "vtkMRMLCornerTextLogic::GetCornerAnnotations: "
                            "failed to get text node from scene");

  return textNode;
}

//---------------------------------------------------------------------------
std::array<vtkMRMLCornerTextLogic::Annotation, 8>
vtkMRMLCornerTextLogic::GenerateAnnotations(vtkMRMLSliceNode *sliceNode,
                                              vtkMRMLTextNode *textNode)
{
  std::array<vtkMRMLCornerTextLogic::Annotation, 8> cornerAnnotations{};
  std::string fontFamily = "Times";
  std::string fontSize = "14";

  if (!sliceNode || !textNode)
  {
    vtkErrorWithObjectMacro(sliceNode, "Invalid input nodes.");
    return cornerAnnotations;
  }

  if (textNode->GetText().size() <= 0) return cornerAnnotations;
  
  // Parse <annotations> element

  vtkXMLDataElement* annotations = this->ParseTextNode(textNode);
  if (!annotations)
  {
    vtkErrorWithObjectMacro(textNode, "Could not find <annotations> tag.");
    return cornerAnnotations;
  }
  if (annotations->GetAttribute("fontFamily"))
    fontFamily = annotations->GetAttribute("fontFamily");
  if (annotations->GetAttribute("fontSize"))
    fontSize = annotations->GetAttribute("fontSize");

  // Parse <corner>/<edge> elements within <annotations>

  const std::unordered_map<std::string, TextLocation> positionMap =
  {
      {"bottom-left", CORNER_BL},
      {"bottom-right", CORNER_BR},
      {"top-left", CORNER_TL},
      {"top-right", CORNER_TR},
      {"bottom", EDGE_B},
      {"right", EDGE_R},
      {"left", EDGE_L},
      {"top", EDGE_T}
  };

  if (annotations->GetNumberOfNestedElements() == 0)
  {
      vtkErrorWithObjectMacro(
          textNode,
          "<annotations> tag had no nested elements.");
      return cornerAnnotations;
  }

  for (int idx = 0; idx < annotations->GetNumberOfNestedElements(); ++idx)
  {
    vtkXMLDataElement* cornerOrEdge = annotations->GetNestedElement(idx);
    if (cornerOrEdge->GetAttribute("fontFamily"))
      std::string fontFamily = cornerOrEdge->GetAttribute("fontFamily");
    if (cornerOrEdge->GetAttribute("fontSize"))
      std::string fontSize = cornerOrEdge->GetAttribute("fontSize");

    const std::string tagName = std::string(cornerOrEdge->GetName());
    if (std::string(cornerOrEdge->GetName()) != "corner" ||
        std::string(cornerOrEdge->GetName()) != "edge")
    {
      vtkErrorWithObjectMacro(
          textNode,
          "<annotations> tag must be nested with <corner> or <edge> tag.");
      return cornerAnnotations;
    }
    if (!cornerOrEdge->GetAttribute("position"))
    {
      vtkErrorWithObjectMacro(
          textNode,
          "<corner> and <edge> tags must specify a position attribute (e.g. "
          "<corner position=\"bottom-left\"> or <edge position=\"bottom\">).");
      return cornerAnnotations;
    }

    std::string position = cornerOrEdge->GetAttribute("position");

    if (!positionMap.count(position))
    {
      vtkErrorWithObjectMacro(
          textNode,
          "position attribute of <corner>/<edge> tag invalid (e.g. "
          "<corner position=\"bottom-left\"> or <edge position=\"bottom\">).");
      return cornerAnnotations;
    }

    TextLocation loc = positionMap.at(position);
    int numProperties = cornerOrEdge->GetNumberOfNestedElements();

    if (numProperties == 0)
    {
        vtkErrorWithObjectMacro(
            textNode,
            "<" + std::string(cornerOrEdge->GetName()) + " position=" + position + "> had no nested elements.");
        return cornerAnnotations;
    }

    // parse each line within <corner> or <edge>

    std::string text = "";

    for (int p_idx = 0; p_idx < numProperties; ++p_idx)
    {
      vtkXMLDataElement *property = cornerOrEdge->GetNestedElement(p_idx);
      if (std::string(property->GetName()) != "property")
      {
        vtkErrorWithObjectMacro(textNode,
                                "<corner>/<edge> tags must only be nested with "
                                "self-closing <property /> tags.");
        break;
      }

      // TODO: Exclude certain categories by passing a variable to
      // GenerateAnnotations
      std::string propertyName = "", propertyValue = "", prefix = "",
                  category = "";

      if (property->GetAttribute("name"))
        propertyName = property->GetAttribute("name");
      else
        vtkWarningWithObjectMacro(textNode,
                                "<" + std::string(cornerOrEdge->GetName()) +
                                    " position=" + position +
                                    "> has a property with a missing name.");

      // we have to check if the name is registered by a plugin
      for (const auto [plugin, provider] : registeredProviders)
        if (provider->CanProvideValueForTag(propertyName))
          propertyValue = provider->GetValueForTag(propertyName, sliceNode);

      if (propertyName != "" && propertyValue == "")
        vtkWarningWithObjectMacro(
            textNode, "<" + std::string(cornerOrEdge->GetName()) +
                          " position=" + position +
                          "> had no property value for " + propertyName + ".");

      if (property->GetAttribute("prefix"))
        prefix = property->GetAttribute("prefix");
      if (property->GetAttribute("category"))
        category = property->GetAttribute("category");
      
      text += prefix + propertyValue + '\n';
    }

    // once each property has been parsed, the annotation for that position
    // is fully specified
    //
    cornerAnnotations[loc] = { text, fontFamily, std::stoi(fontSize) }; 
  }

  return cornerAnnotations;
}
