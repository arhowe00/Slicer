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

#include <vtkMRMLSliceViewDisplayableManagerFactory.h>

// Slicer includes
#include <vtkSlicerApplicationLogic.h>

// CornerText includes
#include "qSlicerCornerTextDICOMAnnotationPropertyValueProvider.h"

// DisplayableManager initialization
#include <vtkAutoInit.h>

// Qt includes
#include <QSettings>
#include <QObject>
#include <QDebug>

// MRML includes
#include <vtkMRMLCornerTextLogic.h>
#include <qMRMLSliceWidget.h>
#include <qMRMLSliceView.h>

// VTK includes
#include <vtkCornerAnnotation.h>

//-----------------------------------------------------------------------------
class qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate
{
public:
  qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate methods

//-----------------------------------------------------------------------------
qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate::qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCornerTextDICOMAnnotationPropertyValueProvider methods

//-----------------------------------------------------------------------------
// void qSlicerCornerTextDICOMAnnotationPropertyValueProvider::setup()
// {
//   this->Superclass::setup();
// }
