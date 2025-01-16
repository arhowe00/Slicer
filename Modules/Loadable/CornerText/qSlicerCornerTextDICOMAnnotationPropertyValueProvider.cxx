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
#include <qSlicerApplication.h>
#include <qSlicerPythonManager.h>

// CornerText includes
#include "qSlicerCornerTextDICOMAnnotationPropertyValueProvider.h"

// DisplayableManager initialization
#include <vtkAutoInit.h>

// Qt includes
#include <QSettings>
#include <QObject>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

// MRML includes
#include <vtkMRMLCornerTextLogic.h>
#include <qMRMLSliceWidget.h>
#include <qMRMLSliceView.h>

// VTK includes
#include <vtkCornerAnnotation.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro(qSlicerCornerTextDICOMAnnotationPropertyValueProvider);

//-----------------------------------------------------------------------------
class qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate
{
public:
  qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate();
  QJsonObject convertToJson(const std::unordered_map<std::string, std::string>& attributes);
};

//-----------------------------------------------------------------------------
// qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate methods

//-----------------------------------------------------------------------------
qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate::qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate()
{
}

//-----------------------------------------------------------------------------
QJsonObject
qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate::convertToJson(
    const std::unordered_map<std::string, std::string> &attributes)
{
  QJsonObject jsonObject;
  for (const auto& pair : attributes) {
    jsonObject[QString::fromStdString(pair.first)] =
        QString::fromStdString(pair.second);
  }
  return jsonObject;
}

//-----------------------------------------------------------------------------
// qSlicerCornerTextDICOMAnnotationPropertyValueProvider methods

//-----------------------------------------------------------------------------
qSlicerCornerTextDICOMAnnotationPropertyValueProvider::
    qSlicerCornerTextDICOMAnnotationPropertyValueProvider()
    : d_ptr(new qSlicerCornerTextDICOMAnnotationPropertyValueProviderPrivate)
{
  qSlicerApplication::application()->pythonManager()->executeString(
      QString("from DataProbeLib import DICOMAnnotationPropertyValueProvider"));
}

//-----------------------------------------------------------------------------
qSlicerCornerTextDICOMAnnotationPropertyValueProvider::~qSlicerCornerTextDICOMAnnotationPropertyValueProvider()
{
}

//-----------------------------------------------------------------------------
// void qSlicerCornerTextDICOMAnnotationPropertyValueProvider::setup()
// {
//   this->Superclass::setup();
// }

//-----------------------------------------------------------------------------
bool qSlicerCornerTextDICOMAnnotationPropertyValueProvider::
    CanProvideValueForPropertyName(const std::string &propertyName)
{
  Q_D(qSlicerCornerTextDICOMAnnotationPropertyValueProvider);

  QString command = QString("DICOMAnnotationPropertyValueProvider."
                            "DICOMAnnotationPropertyValueProvider."
                            "CanProvideValueForPropertyName('%1')")
                        .arg(QString::fromStdString(propertyName));
  QVariant result = qSlicerApplication::application()->pythonManager()->executeString(command, ctkAbstractPythonManager::EvalInput);
  return result.toBool();
}

//-----------------------------------------------------------------------------
std::string
qSlicerCornerTextDICOMAnnotationPropertyValueProvider::GetValueForPropertyName(
    const std::string &propertyName, const XMLTagAttributes& attributes,
    vtkMRMLSliceNode *sliceNode)
{
  Q_D(qSlicerCornerTextDICOMAnnotationPropertyValueProvider);

  QJsonObject jsonAttributes = d->convertToJson(attributes);
  QJsonDocument jsonDoc(jsonAttributes);
  QString jsonString = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));

  // prepend a command that will get the slice node
  QString command = QString("DICOMAnnotationPropertyValueProvider."
                           "DICOMAnnotationPropertyValueProvider."
                           "GetValueForPropertyName('%1', %2, '%3')")
                         .arg(QString::fromStdString(propertyName))
                         .arg(jsonString)
                         .arg(sliceNode->GetID());
  QVariant result = qSlicerApplication::application()->pythonManager()->executeString(command, ctkAbstractPythonManager::EvalInput);
  return result.toString().toStdString();
}

//-----------------------------------------------------------------------------
std::unordered_set<std::string>
qSlicerCornerTextDICOMAnnotationPropertyValueProvider::
    GetSupportedProperties()
{
  Q_D(qSlicerCornerTextDICOMAnnotationPropertyValueProvider);

  QString command = QString("DICOMAnnotationPropertyValueProvider."
                           "DICOMAnnotationPropertyValueProvider."
                           "GetSupportedProperties()");
  QVariant result = qSlicerApplication::application()->pythonManager()->executeString(command, ctkAbstractPythonManager::EvalInput);

  QStringList stringList = result.toStringList();
  std::unordered_set<std::string> supportedProperties;

  for (const QString& str : stringList)
  {
    supportedProperties.insert(str.toStdString());
  }

  return supportedProperties;
}
