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

// FooBar Widgets includes
#include "qSlicerCornerTextFooBarWidget.h"
#include "ctkPimpl.h"
#include "ui_qSlicerCornerTextFooBarWidget.h"
#include "vtkMRMLCornerTextLogic.h"

// MRML includes
#include <vtkMRMLLayoutLogic.h>
#include <vtkMRMLSliceNode.h>

//-----------------------------------------------------------------------------
class qSlicerCornerTextFooBarWidgetPrivate
  : public Ui_qSlicerCornerTextFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerCornerTextFooBarWidget);
protected:
  qSlicerCornerTextFooBarWidget* const q_ptr;

public:
  qSlicerCornerTextFooBarWidgetPrivate(
    qSlicerCornerTextFooBarWidget& object);
  virtual void setupUi(qSlicerCornerTextFooBarWidget*);
  void init();
  bool ToggleLocation(vtkMRMLCornerTextLogic::TextLocation, bool enabled);
  
  vtkMRMLLayoutLogic* LayoutLogic;
  vtkMRMLCornerTextLogic* CornerTextLogic;
};

// --------------------------------------------------------------------------
qSlicerCornerTextFooBarWidgetPrivate
::qSlicerCornerTextFooBarWidgetPrivate(
  qSlicerCornerTextFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidgetPrivate
::setupUi(qSlicerCornerTextFooBarWidget* widget)
{
  this->Ui_qSlicerCornerTextFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidgetPrivate
::init()
{
  Q_Q(qSlicerCornerTextFooBarWidget);

  QObject::connect(this->sliceViewAnnotationsCheckBox, SIGNAL(toggled(bool)), q, SLOT(enableSliceViewAnnotations(bool)));

  // Corner Text Annotation ctk collapsible button pane.
  this->sliceViewAnnotationsCheckBox->setChecked(true);
  this->cornerTextParametersCollapsibleButton->setEnabled(true);

  QObject::connect(this->topLeftCheckBox, SIGNAL(toggled(bool)), q, SLOT(setTopLeftCornerActive(bool)));
  QObject::connect(this->topRightCheckBox, SIGNAL(toggled(bool)), q, SLOT(setTopRightCornerActive(bool)));
  QObject::connect(this->bottomLeftCheckBox, SIGNAL(toggled(bool)), q, SLOT(setBottomLeftCornerActive(bool)));

  // Amount subpanel
  this->annotationsAmountGroupBox->setEnabled(true);
  QObject::connect(this->level1RadioButton, &QRadioButton::toggled, [=](bool checked) {
      if (checked) q->setAnnotationDisplayLevel(1);
  });

  QObject::connect(this->level2RadioButton, &QRadioButton::toggled, [=](bool checked) {
      if (checked) q->setAnnotationDisplayLevel(2);
  });

  QObject::connect(this->level3RadioButton, &QRadioButton::toggled, [=](bool checked) {
      if (checked) q->setAnnotationDisplayLevel(3);
  });

  // Font Properties subpanel
  this->fontPropertiesGroupBox->setEnabled(true);
  QObject::connect(this->timesFontRadioButton, &QRadioButton::toggled, [=](bool checked) {
      if (checked) q->setFontFamily("Times");
  });

  QObject::connect(this->arialFontRadioButton, &QRadioButton::toggled, [=](bool checked) {
      if (checked) q->setFontFamily("Arial");
  });
  QObject::connect(this->fontSizeSpinBox, SIGNAL(valueChanged(int)), q, SLOT(setFontSize(int)));

  // DICOM Annotations subpanel
  this->dicomAnnotationsCollapsibleGroupBox->setEnabled(false);
  QObject::connect(this->backgroundPersistenceCheckBox, SIGNAL(toggled(bool)), q, SLOT(setDICOMAnnotationsPersistence(bool)));

  q->updateWidgetFromCornerTextLogic();
}

bool qSlicerCornerTextFooBarWidgetPrivate::ToggleLocation(vtkMRMLCornerTextLogic::TextLocation location, bool enabled)
{  
  Q_Q(qSlicerCornerTextFooBarWidget);
  vtkMRMLScene* mrmlScene = q->mrmlScene();
  if (mrmlScene == nullptr)
  {
    return false;
  }

  for (int i = 0; i < mrmlScene->GetNumberOfNodesByClass("vtkMRMLSliceNode"); ++i)
  {
      vtkMRMLNode* node = mrmlScene->GetNthNodeByClass(i, "vtkMRMLSliceNode");
      if (node == nullptr)
      {
        return false;
      }

      vtkMRMLSliceNode* sliceNode = vtkMRMLSliceNode::SafeDownCast(node);
      if (sliceNode == nullptr)
      {
        return false;
      }

      this->CornerTextLogic->ToggleLocation(sliceNode, location, enabled);
  }
  return true;
 }

//-----------------------------------------------------------------------------
// qSlicerCornerTextFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerCornerTextFooBarWidget
::qSlicerCornerTextFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerCornerTextFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerCornerTextFooBarWidget);
  d->setupUi(this);
  d->init();
}

//-----------------------------------------------------------------------------
qSlicerCornerTextFooBarWidget
::~qSlicerCornerTextFooBarWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::enableSliceViewAnnotations(bool enable)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  // For graying out the section in the GUI
  d->cornerTextParametersCollapsibleButton->setEnabled(enable);
  // For informing the logic that it should not generate annotations
  d->CornerTextLogic->SetSliceViewAnnotationsEnabled(enable);
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setTopLeftCornerActive(bool enable)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  d->ToggleLocation(vtkMRMLCornerTextLogic::CORNER_TL, enable);
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setTopRightCornerActive(bool enable)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  d->ToggleLocation(vtkMRMLCornerTextLogic::CORNER_TR, enable);
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setBottomLeftCornerActive(bool enable)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  d->ToggleLocation(vtkMRMLCornerTextLogic::CORNER_BL, enable);
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setAnnotationDisplayLevel(int level)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  d->CornerTextLogic->SetDisplayStrictness(level);
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setFontFamily(const QString& fontFamily)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  d->CornerTextLogic->SetFontFamily(fontFamily.toStdString());
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setFontSize(int fontSize)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  d->CornerTextLogic->SetFontSize(fontSize);
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setDICOMAnnotationsPersistence(bool enable)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->CornerTextLogic)
  {
    return;
  }
  // TODO: Not supported yet.
  // d->CornerTextLogic->SetDICOMAnnotationsPersistence(enable);
}

// ----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::onLayoutLogicModifiedEvent()
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (!d->LayoutLogic)
  {
    return;
  }

  setTopLeftCornerActive(d->topLeftCheckBox->isChecked());
  setTopRightCornerActive(d->topRightCheckBox->isChecked());
  setBottomLeftCornerActive(d->bottomLeftCheckBox->isChecked());
}

CTK_GET_CPP(qSlicerCornerTextFooBarWidget, vtkMRMLLayoutLogic*, layoutLogic, LayoutLogic)

void qSlicerCornerTextFooBarWidget::setLayoutLogic(vtkMRMLLayoutLogic* newLayoutLogic)
{
  Q_D(qSlicerCornerTextFooBarWidget);
  if (d->LayoutLogic == newLayoutLogic)
  {
    return;
  }

  qvtkReconnect(d->LayoutLogic, newLayoutLogic, vtkCommand::ModifiedEvent,
                   this, SLOT(onLayoutLogicModifiedEvent()));

  d->LayoutLogic = newLayoutLogic;

  if (d->LayoutLogic && d->LayoutLogic->GetMRMLScene())
  {
    this->setMRMLScene(d->LayoutLogic->GetMRMLScene());
  }

  this->onLayoutLogicModifiedEvent();
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::setAndObserveCornerTextLogic(vtkMRMLCornerTextLogic* cornerTextLogic)
{
  Q_D(qSlicerCornerTextFooBarWidget);

  qvtkReconnect(d->LayoutLogic, cornerTextLogic, vtkCommand::ModifiedEvent,
                   this, SLOT(onLayoutLogicModifiedEvent()));

  d->CornerTextLogic = cornerTextLogic;
  this->updateWidgetFromCornerTextLogic();
}

CTK_GET_CPP(qSlicerCornerTextFooBarWidget, vtkMRMLCornerTextLogic*, cornerTextLogic, CornerTextLogic)


//-----------------------------------------------------------------------------
void qSlicerCornerTextFooBarWidget::updateWidgetFromCornerTextLogic()
{
  Q_D(qSlicerCornerTextFooBarWidget);

  if (!d->CornerTextLogic)
  {
    return;
  }

  // Presumably from DataProbe settings

  d->sliceViewAnnotationsCheckBox->setChecked(d->CornerTextLogic->GetSliceViewAnnotationsEnabled());
  (d->CornerTextLogic->GetFontFamily() == "Arial") ? d->arialFontRadioButton->toggle() : d->timesFontRadioButton->toggle();
  d->fontSizeSpinBox->setValue(d->CornerTextLogic->GetFontSize());

  switch (d->CornerTextLogic->GetDisplayStrictness())
  {
    case 1:
    {
      d->level1RadioButton->toggle();
      break;
    }
    case 2:
    {
      d->level2RadioButton->toggle();
      break;
    }
    case 3:
    {
      d->level3RadioButton->toggle();
      break;
    }
    default:
    {
      break;
    }
  }
}
