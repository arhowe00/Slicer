/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerCornerTextFooBarWidget_h
#define __qSlicerCornerTextFooBarWidget_h

// Qt includes
#include <QWidget>

// FooBar Widgets includes
#include "ctkPimpl.h"
#include "ctkVTKObject.h"
#include "qMRMLWidget.h"
#include "qSlicerCornerTextModuleWidgetsExport.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLLayoutLogic.h"
#include "vtkMRMLCornerTextLogic.h"

class qSlicerCornerTextFooBarWidgetPrivate;

class Q_SLICER_MODULE_CORNERTEXT_WIDGETS_EXPORT qSlicerCornerTextFooBarWidget
  : public qMRMLWidget // just so we can implement setMRMLScene
{
  Q_OBJECT
  QVTK_OBJECT

public:
  typedef qMRMLWidget Superclass;
  qSlicerCornerTextFooBarWidget(QWidget *parent=0);
  ~qSlicerCornerTextFooBarWidget() override;

  vtkMRMLLayoutLogic* layoutLogic() const;
  void setLayoutLogic(vtkMRMLLayoutLogic*);

  vtkMRMLCornerTextLogic* cornerTextLogic() const;
  void setCornerTextLogic(vtkMRMLCornerTextLogic*);

protected slots:

  void enableSliceViewAnnotations(bool enable);
  void setTopLeftCornerActive(bool enable);
  void setTopRightCornerActive(bool enable);
  void setBottomLeftCornerActive(bool enable);
  void setAnnotationDisplayLevel(int level); // 1, 2, or 3
  void setFontFamily(const QString& fontFamily);
  void setFontSize(int fontSize);
  void setDICOMAnnotationsPersistence(bool enable);

  void onLayoutLogicModifiedEvent();

protected:
  QScopedPointer<qSlicerCornerTextFooBarWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerCornerTextFooBarWidget);
  Q_DISABLE_COPY(qSlicerCornerTextFooBarWidget);
};

#endif
