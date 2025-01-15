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

#ifndef __vtkMRMLCornerTextDisplayableManager_h
#define __vtkMRMLCornerTextDisplayableManager_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractSliceViewDisplayableManager.h"
#include "vtkMRMLCornerTextLogic.h"
#include "vtkSlicerCornerTextModuleMRMLDisplayableManagerExport.h"

// VTK includes
#include <vtkCornerAnnotation.h>

/// \brief Displayable manager for showing annotations in slice (2D) views.
///
/// Displays annotations in corners & edges of slice views
///
class VTK_SLICER_CORNERTEXT_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkMRMLCornerTextDisplayableManager
  : public vtkMRMLAbstractSliceViewDisplayableManager
{

public:

  static vtkMRMLCornerTextDisplayableManager* New();
  vtkTypeMacro(vtkMRMLCornerTextDisplayableManager, vtkMRMLAbstractSliceViewDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

//  vtkMRMLCornerTextLogic* GetCornerTextLogic() const;
//  void SetCornerTextLogic(vtkMRMLCornerTextLogic*);

  vtkCornerAnnotation* GetCornerAnnotation() const;
  void SetCornerAnnotation(vtkCornerAnnotation* cornerAnnotation) const;

protected:

  vtkMRMLCornerTextDisplayableManager();
  ~vtkMRMLCornerTextDisplayableManager() override;

  void UnobserveMRMLScene() override;
  void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) override;
  void ProcessMRMLLogicsEvents(vtkObject* caller, unsigned long event, void* callData) override;

  /// Update Actors based on changes in the scene (TODO: Clarify this
  /// behavior)
  void UpdateFromMRML() override;

  void OnMRMLSceneStartClose() override;
  void OnMRMLSceneEndClose() override;
  void OnMRMLDisplayableNodeModifiedEvent(vtkObject* caller) override;

  void OnMRMLSceneEndBatchProcess() override;

  /// Initialize the displayable manager based on its associated
  /// vtkMRMLSliceNode
  void Create() override;

private:

  vtkMRMLCornerTextDisplayableManager(const vtkMRMLCornerTextDisplayableManager&) = delete;
  void operator=(const vtkMRMLCornerTextDisplayableManager&) = delete;

  class vtkInternal;
  vtkInternal * Internal;
  friend class vtkInternal;
};

#endif
