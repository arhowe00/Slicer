/*==============================================================================

  Program: 3D Slicer

  Copyright(c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __vtkMRMLAbstractCornerTextTagValueProvider_h
#define __vtkMRMLAbstractCornerTextTagValueProvider_h

// MRML includes
class vtkMRMLSliceNode;

// VTK includes
#include <vtkObject.h>

/// \brief Base class for CornerText tag value provider.
///
/// Sub-classes must implement this interface to provide slice view annotations.
class vtkMRMLAbstractCornerTextTagValueProvider : public vtkObject
{
public:
  vtkTypeMacro(vtkMRMLAbstractCornerTextTagValueProvider, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual bool CanProvideValueForTag(const std::string& tag) = 0;
  virtual std::string GetValueForTag(const std::string& tag,
                                     vtkMRMLSliceNode *sliceNode) = 0;
  virtual std::vector<std::string> GetSupportedTags() = 0;

protected:
  vtkMRMLAbstractCornerTextTagValueProvider() = default;
  ~vtkMRMLAbstractCornerTextTagValueProvider() override = default;
  vtkMRMLAbstractCornerTextTagValueProvider(const vtkMRMLAbstractCornerTextTagValueProvider&) = delete;
  void operator=(const vtkMRMLAbstractCornerTextTagValueProvider&) = delete;
};

#endif
