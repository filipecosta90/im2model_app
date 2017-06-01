
#include <QtWidgets>

#include "treeitem_file_delegate.hpp"
#include <iostream>

TreeItemFileDelegate::TreeItemFileDelegate( QObject *parent ) : QStyledItemDelegate(parent){

}

// more work here!!!!
void TreeItemFileDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  switch( item->get_item_delegate_type() )
  {
    case _delegate_FILE :
    case _delegate_DIR :
      {
        QStyledItemDelegate::paint( painter, option,  index);
        break;
      }
    case _delegate_TEXT :
      {
        QStyledItemDelegate::paint( painter, option,  index);
        break;
      }
  }
}

QWidget *TreeItemFileDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const{

  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  QWidget *editor = 0;

  switch( item->get_item_delegate_type() )
  {
    case _delegate_FILE :
    case _delegate_DIR :
      {
        editor = new QWidget(parent);
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QHBoxLayout *layout = new QHBoxLayout( editor );
        layout->setMargin(0);
        layout->setAlignment(Qt::AlignRight);
        // layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        QSizePolicy sizePol(QSizePolicy::Expanding,QSizePolicy::Expanding);


        QLineEdit *line = new QLineEdit( editor );
        line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        line->setContentsMargins(0,0,0,0);
        line->setText(value);
        line->setVisible(true);

        QString _button_text = "...";
        FilePushButton *button = new FilePushButton(  _button_text,parent);
        button->setFixedWidth( button->fontMetrics().width( " ... " ) );

        layout->addWidget(line);
        layout->addWidget(button);

        editor->setLayout(layout);
        editor->setSizePolicy(sizePol);
        // line->setMinimumHeight(editor->height());

        editor->setFocusProxy( line );

        if( item->get_item_delegate_type() ==  _delegate_FILE ){
          connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_filename_slot(QWidget*)));
        }
        else{
          connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_dirname_slot(QWidget*)));
        }
        break;
      }
    case _delegate_TEXT :
      {
        editor = QStyledItemDelegate::createEditor(parent,option,index);
        break;
      }
  }
  return editor;
}

void TreeItemFileDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  switch(item->get_item_delegate_type())
  {
    case _delegate_FILE  :
    case _delegate_DIR  :
      {

        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value = index.model()->data(index, Qt::EditRole).toString();
        line->setText(value);
        QStyledItemDelegate::setEditorData(editor,index);

        break;
      }
    case _delegate_TEXT  :
      {
        QStyledItemDelegate::setEditorData(editor,index);
        break;
      }
  }
}

void TreeItemFileDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  switch(item->get_item_delegate_type())
  {
    case _delegate_FILE  :
    case _delegate_DIR  :
      {
        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value = line->text();
        model->setData(index,value, Qt::EditRole);
        break;
      }
    case _delegate_TEXT  :
      {
        QStyledItemDelegate::setModelData(editor,model,index);
        break;
      }
  }
}

void TreeItemFileDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  switch(item->get_item_delegate_type())
  {
    case _delegate_FILE  :
    case _delegate_DIR  :
      {
        editor->setGeometry(option.rect);
        break;
      }
    case _delegate_TEXT  :
      {
        QStyledItemDelegate::updateEditorGeometry(editor,option,index);
        break;
      }
  }
}

void TreeItemFileDelegate::get_filename_slot( QWidget *editor ){
  std::cout << " get_filename_slot called" << std::endl;
  QLineEdit* line = editor->findChild<QLineEdit*>();
  QString fileName = QFileDialog::getOpenFileName( editor,
      tr("Open File"),
      tr(".")
      //, tr("Text Files")
      );
  if( ! (fileName.isNull()) ){
    line->setText(fileName);
  }
}

void TreeItemFileDelegate::get_dirname_slot( QWidget *editor ){
  std::cout << " get_dirname_slot called" << std::endl;
  QLineEdit* line = editor->findChild<QLineEdit*>();
  QString dirName = QFileDialog::getExistingDirectory(editor,
      tr("Open Directory"),
      tr("."),
      QFileDialog::ShowDirsOnly
      | QFileDialog::DontResolveSymlinks);
  if( ! (dirName.isNull()) ){
    line->setText(dirName);
  }
}

