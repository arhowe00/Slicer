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

#ifndef __vtkMRMLCornerTextDisplayableManager_h
#define __vtkMRMLCornerTextDisplayableManager_h

// MRMLDisplayableManager includes
#include "vtkMRMLAbstractSliceViewDisplayableManager.h"
#include "vtkSlicerCornerTextModuleMRMLDisplayableManagerExport.h"

/// \brief Displayable manager for showing transforms in slice (2D) views.
///
/// Displays transforms in slice viewers as glyphs, deformed grid, or
/// contour lines
///
class VTK_SLICER_CORNERTEXT_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT vtkMRMLCornerTextDisplayableManager
  : public vtkMRMLAbstractSliceViewDisplayableManager
{

public:

  static vtkMRMLCornerTextDisplayableManager* New();
  vtkTypeMacro(vtkMRMLCornerTextDisplayableManager, vtkMRMLAbstractSliceViewDisplayableManager);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:

  vtkMRMLCornerTextDisplayableManager();
  ~vtkMRMLCornerTextDisplayableManager() override;

  void UnobserveMRMLScene() override;
  void OnMRMLSceneNodeAdded(vtkMRMLNode* node) override;
  void OnMRMLSceneNodeRemoved(vtkMRMLNode* node) override;
  void ProcessMRMLNodesEvents(vtkObject* caller, unsigned long event, void* callData) override;

  /// Update Actors based on transforms in the scene
  void UpdateFromMRML() override;

  void OnMRMLSceneStartClose() override;
  void OnMRMLSceneEndClose() override;

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
