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


#include "vtkMRMLDefaultAnnotationPropertyValueProvider.h"

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
vtkStandardNewMacro(vtkMRMLDefaultAnnotationPropertyValueProvider);

//---------------------------------------------------------------------------
// vtkMRMLDefaultAnnotationPropertyValueProvider methods

//---------------------------------------------------------------------------
void vtkMRMLDefaultAnnotationPropertyValueProvider::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "vtkMRMLDefaultAnnotationPropertyValueProvider: " << this->GetClassName() << "\n";
}

//---------------------------------------------------------------------------
bool vtkMRMLDefaultAnnotationPropertyValueProvider::CanProvideValueForProperty(
    const std::string &property)
{
  return registeredProperties.count(property);
}

//---------------------------------------------------------------------------
std::string vtkMRMLDefaultAnnotationPropertyValueProvider::GetValueForProperty(
    const std::string &property, vtkMRMLSliceNode *sliceNode)
{
  vtkMRMLSliceLogic *sliceLogic = this->GetAppLogic()->GetSliceLogic(sliceNode);

  if (!sliceLogic) return "";

  vtkMRMLSliceCompositeNode  *sliceCNode = sliceLogic->GetSliceCompositeNode();

  if (!sliceCNode) return "";

  vtkMRMLVolumeNode* vn;
  if (property == "Background")
    return (vn = sliceLogic->GetBackgroundLayer()->GetVolumeNode())
               ? vn->GetName()
               : "";
  else if (property == "Foreground")
    return (vn = sliceLogic->GetForegroundLayer()->GetVolumeNode())
               ? vn->GetName()
               : "";
  else if (property == "Label")
    return (vn = sliceLogic->GetLabelLayer()->GetVolumeNode())
               ? std::string(vn->GetName()) + " (" +
                     std::to_string(sliceCNode->GetLabelOpacity() * 100) + "%)"
               : "";
  else if (property == "SlabReconstructionThickness" && sliceNode->GetSlabReconstructionEnabled())
    return "Thickness: " +
           std::to_string(sliceNode->GetSlabReconstructionThickness());
  else if (property == "SlabReconstructionType" &&
           sliceNode->GetSlabReconstructionEnabled())
    return "Type: " +
           std::to_string(sliceNode->GetSlabReconstructionType());
  else
    return "";
}

//---------------------------------------------------------------------------
std::unordered_set<std::string>
vtkMRMLDefaultAnnotationPropertyValueProvider::GetSupportedProperties()
{
  return this->registeredProperties;
}
