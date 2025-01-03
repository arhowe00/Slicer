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

// Qt includes
#include <QDebug>

// Slicer includes
#include "qSlicerCornerTextModuleWidget.h"
#include "ui_qSlicerCornerTextModuleWidget.h"

//-----------------------------------------------------------------------------
class qSlicerCornerTextModuleWidgetPrivate: public Ui_qSlicerCornerTextModuleWidget
{
public:
  qSlicerCornerTextModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCornerTextModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCornerTextModuleWidgetPrivate::qSlicerCornerTextModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCornerTextModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerCornerTextModuleWidget::qSlicerCornerTextModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerCornerTextModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerCornerTextModuleWidget::~qSlicerCornerTextModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerCornerTextModuleWidget::setup()
{
  Q_D(qSlicerCornerTextModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
}
