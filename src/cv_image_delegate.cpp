#include "cv_image_delegate.h"
#include "cv_image_widget.h"

CvImageDelegate::CvImageDelegate(QObject *parent) :
  QItemDelegate(parent)
{
}

// TableView need to create an Editor
// Create Editor when we construct CvImageDelegate
// and return the Editor
QWidget* CvImageDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{

  CVImageWidget* tdmap_cell_widget = new CVImageWidget();
  QSpinBox *editor = new QSpinBox(parent);
  editor->setMinimum(0);
  editor->setMaximum(100);
  return editor;
}

// Then, we set the Editor
// Gets the data from Model and feeds the data to Editor
void CvImageDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
  // Get the value via index of the Model
  int value = index.model()->data(index, Qt::EditRole).toInt();

  // Put the value into the SpinBox
  QSpinBox *spinbox = static_cast<QSpinBox*>(editor);
  spinbox->setValue(value);
}

// When we modify data, this model reflect the change
void CvImageDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{

}

// Give the SpinBox the info on size and location
void CvImageDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
  editor->setGeometry(option.rect);
}

