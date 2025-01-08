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

#ifndef __vtkMRMLAbstractAnnotationPropertyValueProvider_h
#define __vtkMRMLAbstractAnnotationPropertyValueProvider_h

// MRML includes
class vtkMRMLSliceNode;

// VTK includes
#include <vtkObject.h>

/// \brief Base class for Annotation property value provider.
///
/// Sub-classes must implement this interface to provide slice view annotations.
class vtkMRMLAbstractAnnotationPropertyValueProvider : public vtkObject
{
public:
  vtkTypeMacro(vtkMRMLAbstractAnnotationPropertyValueProvider, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual bool CanProvideValueForProperty(const std::string& property) = 0;
  virtual std::string GetValueForProperty(const std::string& property,
                                     vtkMRMLSliceNode *sliceNode) = 0;
  virtual std::vector<std::string> GetSupportedProperties() = 0;

protected:
  vtkMRMLAbstractAnnotationPropertyValueProvider() = default;
  ~vtkMRMLAbstractAnnotationPropertyValueProvider() override = default;
  vtkMRMLAbstractAnnotationPropertyValueProvider(const vtkMRMLAbstractAnnotationPropertyValueProvider&) = delete;
  void operator=(const vtkMRMLAbstractAnnotationPropertyValueProvider&) = delete;
};

#endif
