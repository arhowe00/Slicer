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

// .NAME vtkSlicerCornerTextLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerCornerTextLogic_h
#define __vtkSlicerCornerTextLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
class vtkMRMLAnnotationNode;
class vtkMRMLSliceNode;
class vtkMRMLTextNode;
class vtkXMLDataElement;

// STD includes
#include <cstdlib>

#include "vtkSlicerCornerTextModuleLogicExport.h"


class VTK_SLICER_CORNERTEXT_MODULE_LOGIC_EXPORT vtkSlicerCornerTextLogic :
  public vtkSlicerModuleLogic
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

  static vtkMRMLNode* GetTextNodeFromSliceView(vtkMRMLSliceNode*, TextLocation);

  /// The Usual vtk class functions
  static vtkSlicerCornerTextLogic *New();
  vtkTypeMacro(vtkSlicerCornerTextLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkSlicerCornerTextLogic();
  ~vtkSlicerCornerTextLogic() override;

  void SetMRMLSceneInternal(vtkMRMLScene* newScene) override;

  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  void RegisterNodes() override;
  void UpdateFromMRMLScene() override;
  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;
private:

  vtkSlicerCornerTextLogic(const vtkSlicerCornerTextLogic&); // Not implemented
  void operator=(const vtkSlicerCornerTextLogic&); // Not implemented

  vtkXMLDataElement* ParseTextNode(vtkMRMLTextNode*);
  std::string GenerateCornerAnnotation(vtkMRMLSliceNode*, vtkMRMLTextNode*); 

};

#endif
