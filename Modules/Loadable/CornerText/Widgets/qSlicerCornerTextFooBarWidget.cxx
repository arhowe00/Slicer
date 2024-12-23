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

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerCornerTextFooBarWidget.h"
#include "ui_qSlicerCornerTextFooBarWidget.h"

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
// qSlicerCornerTextFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerCornerTextFooBarWidget
::qSlicerCornerTextFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerCornerTextFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerCornerTextFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerCornerTextFooBarWidget
::~qSlicerCornerTextFooBarWidget()
{
}
