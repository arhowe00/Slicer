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


#include "vtkMRMLDICOMAnnotationPropertyValueProvider.h"

// MRML includes
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceCompositeNode.h>
#include <vtkMRMLSliceLayerLogic.h>
#include <vtkMRMLSliceLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLVolumeNode.h>

// VTK includes
#include <vtkNew.h>

// STD includes
#include <cassert>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLDICOMAnnotationPropertyValueProvider);

//---------------------------------------------------------------------------
// vtkMRMLDICOMAnnotationPropertyValueProvider methods

//---------------------------------------------------------------------------
void vtkMRMLDICOMAnnotationPropertyValueProvider::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "vtkMRMLDICOMAnnotationPropertyValueProvider: " << this->GetClassName() << "\n";
}

//---------------------------------------------------------------------------
bool vtkMRMLDICOMAnnotationPropertyValueProvider::CanProvideValueForPropertyName(
    const std::string &propertyName)
{
  return registeredProperties.count(propertyName);
}

//---------------------------------------------------------------------------
std::string
vtkMRMLDICOMAnnotationPropertyValueProvider::GetValueForPropertyName(
    const std::string &propertyName, const XMLTagAttributes &attributes,
    vtkMRMLSliceNode *sliceNode)
{
  vtkMRMLSliceLogic *sliceLogic = this->GetAppLogic()->GetSliceLogic(sliceNode);

  if (!sliceLogic) { return ""; }

  std::string output = "";

  if (propertyName == "VolumeName")
  {
    vtkMRMLVolumeNode* volumeNode;
    const int layer = this->GetLayerValueAsInteger(attributes);
    switch (layer)
    {
      case LAYER_FOREGROUND:
        {
          output = (volumeNode = sliceLogic->GetForegroundLayer()->GetVolumeNode()) != nullptr
                     ? volumeNode->GetName()
                     : "";
          break;
        }
      case LAYER_BACKGROUND:
        {
          output = (volumeNode = sliceLogic->GetBackgroundLayer()->GetVolumeNode()) != nullptr
                     ? volumeNode->GetName()
                     : "";
          break;
        }
      case LAYER_LABEL:
        {
          output = (volumeNode = sliceLogic->GetLabelLayer()->GetVolumeNode()) != nullptr
                     ? volumeNode->GetName()
                     : "";
          break;
        }
      default:
        {
          break;
        }
    }
  }
  else if (propertyName == "SlabReconstructionThickness" &&
             sliceNode->GetSlabReconstructionEnabled())
  {
    output = std::to_string(sliceNode->GetSlabReconstructionThickness());
  }
  else if (propertyName == "SlabReconstructionType" &&
             sliceNode->GetSlabReconstructionEnabled())
  {
    output = std::to_string(sliceNode->GetSlabReconstructionType());
  }
  
  if (output != "" && attributes.count("prefix"))
  {
    output = attributes.at("prefix") + output;
  }

  return output;
}

//---------------------------------------------------------------------------
std::unordered_set<std::string>
vtkMRMLDICOMAnnotationPropertyValueProvider::GetSupportedProperties()
{
  return this->registeredProperties;
}
