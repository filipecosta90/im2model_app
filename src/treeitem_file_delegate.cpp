
#include <QtWidgets>

#include "treeitem_file_delegate.hpp"
#include <iostream>

TreeItemFileDelegate::TreeItemFileDelegate( QObject *parent ) : QStyledItemDelegate(parent){

}

void TreeItemFileDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  switch( item->get_item_delegate_type() )
  {
    case _delegate_FILE :
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


        connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_filename_slot(QWidget*)));
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
  //QString filetype = index.model()->data(index, Qt::EditRole).toString();
  QString fileName = QFileDialog::getOpenFileName( editor,
      tr("Open"),
      tr(".")
      //, tr("Text Files")
      );
  if( ! (fileName.isNull()) ){
    line->setText(fileName);
  }
}
