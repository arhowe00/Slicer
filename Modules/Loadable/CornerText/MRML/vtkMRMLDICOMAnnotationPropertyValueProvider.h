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

#ifndef __vtkMRMLDICOMAnnotationPropertyValueProvider_h
#define __vtkMRMLDICOMAnnotationPropertyValueProvider_h

// MRMLCore includes
#include "vtkMRMLAbstractAnnotationPropertyValueProvider.h"

// CornerText includes
#include "vtkMRMLApplicationLogic.h"
#include "vtkSlicerCornerTextModuleMRMLExport.h"

/// \brief The DICOM property value provider implemented for the CornerText
/// loadable module.
///
/// Implements heuristics for providing property values based on the names of volume
/// nodes. This is intended to replicate the behavior of 'DataProbe', a Scripted
/// Loadable module
///
class VTK_SLICER_CORNERTEXT_MODULE_MRML_EXPORT vtkMRMLDICOMAnnotationPropertyValueProvider
  : public vtkMRMLAbstractAnnotationPropertyValueProvider
{

public:

  static vtkMRMLDICOMAnnotationPropertyValueProvider* New();
  vtkTypeMacro(vtkMRMLDICOMAnnotationPropertyValueProvider, vtkMRMLAbstractAnnotationPropertyValueProvider);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  bool CanProvideValueForPropertyName(const std::string &propertyName) override;
  std::string GetValueForPropertyName(const std::string &propertyName,
                                      const XMLTagAttributes &attributes,
                                      vtkMRMLSliceNode *) override;
  std::unordered_set<std::string> GetSupportedProperties() override;

  vtkGetObjectMacro(AppLogic, vtkMRMLApplicationLogic);
  vtkSetObjectMacro(AppLogic, vtkMRMLApplicationLogic);

protected:

  vtkMRMLDICOMAnnotationPropertyValueProvider() = default;
  ~vtkMRMLDICOMAnnotationPropertyValueProvider() = default;

private:

  vtkMRMLDICOMAnnotationPropertyValueProvider(const vtkMRMLDICOMAnnotationPropertyValueProvider&) = delete;
  void operator=(const vtkMRMLDICOMAnnotationPropertyValueProvider&) = delete;

  const std::unordered_set<std::string> registeredProperties = {
      "PatientName",
      "PatientID",
      "PatientInfo",
      "SeriesDate",
      "SeriesDescription",
      "InstitutionName",
      "ReferringPhysician",
      "Manufacturer",
      "Model",
      "Patient-Position",
      "TR",
      "TE"};

  vtkMRMLApplicationLogic* AppLogic;
};

#endif
