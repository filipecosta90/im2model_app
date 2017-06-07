
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
    case _delegate_DROP:
      {
        if( item->get_dropdown_column() == index.column() ){

          const QVector<QVariant> drop_data = item->get_dropdown_data();
          const QVector<QVariant> drop_enum = item->get_dropdown_enum();
          QVariant value = index.model()->data(index, Qt::EditRole);

          QString _option_text;
          for( int enum_pos = 0; enum_pos < drop_enum.size(); enum_pos++ ){
            if( drop_enum[enum_pos] == value ){
              _option_text = drop_data[enum_pos].toString();
            }
          }
          QStyleOptionViewItem itemOption(option);
          initStyleOption(&itemOption, index);
          // override text
          itemOption.text = _option_text;
          QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter, nullptr);
        }
        else{
          QStyledItemDelegate::paint( painter, option,  index);
        }
        break;
      }
    case _delegate_TEXT_ACTION:
    case _delegate_FILE:
    case _delegate_DIR:
    case _delegate_TEXT:
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
        std::cout << " delegate FILE DIR" << std::endl;
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
        FilePushButton *button = new FilePushButton(  _button_text, parent );
        button->setFixedWidth( button->fontMetrics().width( " ... " ) );

        layout->addWidget(line);
        layout->addWidget(button);

        editor->setLayout(layout);
        editor->setSizePolicy(sizePol);

        editor->setFocusProxy( line );

        if( item->get_item_delegate_type() ==  _delegate_FILE ){
          connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_filename_slot(QWidget*)));
        }
        else{
          connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_dirname_slot(QWidget*)));
        }
        break;
      }
    case _delegate_DROP:
      {
        if( item->get_dropdown_column() == index.column() ){
          editor = new QWidget(parent);
          int value = index.model()->data(index, Qt::EditRole).toInt();
          QHBoxLayout *layout = new QHBoxLayout( editor );
          layout->setMargin(0);
          layout->setContentsMargins(0, 0, 0, 0);
          QSizePolicy sizePol(QSizePolicy::Expanding,QSizePolicy::Expanding);

          QComboBox* combo = new QComboBox(editor);
          combo->setContentsMargins(0,0,0,0);

          // combo commands
          QStringList commands;
          const QVector<QVariant> drop_data = item->get_dropdown_data();
          const QVector<QVariant> drop_enum = item->get_dropdown_enum();

          foreach (const QVariant var_command, drop_data) {
            std::cout << "command " <<  var_command.toString().toStdString() << std::endl;
            commands << var_command.toString();
          }

          foreach (const QVariant var_enum, drop_enum ) {
            std::cout << "enum " <<  var_enum.toInt() << std::endl;
          }

          for( int enum_pos = 0; enum_pos < drop_enum.size(); enum_pos++ ){
            if( drop_enum[enum_pos] == value ){
              std::cout << " initial index  " << enum_pos << std::endl;
              combo->setCurrentIndex(enum_pos);
            }
          }
          // fill combo
          combo->addItems(commands);
          layout->addWidget(combo);
          editor->setLayout(layout);
          editor->setSizePolicy(sizePol);
          editor->setFocusProxy( combo );
        }
        break;
      }
    case _delegate_TEXT_ACTION:
      {

        editor = new QWidget(parent);

        QVariant::Type t = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());
        QWidget* text_editor = QItemEditorFactory().createEditor(t,parent);

        QHBoxLayout* editor_layout = new QHBoxLayout( editor );

        editor_layout->setMargin(0);
        editor_layout->setContentsMargins(QMargins(0,0,0,0));
        editor_layout->setSpacing(0);
        editor_layout->setAlignment(Qt::AlignRight);

        QAction* auto_range_lower_upper = new QAction("Auto range");
        QAction* auto_lower_upper = new QAction("Auto lower/upper");
        QMenu* alignMenu = new QMenu();
        alignMenu->addAction(auto_range_lower_upper);
        alignMenu->addAction(auto_lower_upper);
        CustomToolButton* alignToolButton = new CustomToolButton();
        alignToolButton->setMenu(alignMenu);
        alignToolButton->setDefaultAction(auto_range_lower_upper);

        QSizePolicy spLeft(QSizePolicy::Expanding, QSizePolicy::Expanding);
        spLeft.setHorizontalStretch(3);
        text_editor->setSizePolicy(spLeft);
        editor_layout->addWidget(text_editor);

        QSizePolicy spRight(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        spRight.setHorizontalStretch(1);
        alignToolButton->setSizePolicy(spRight);
        editor_layout->addWidget(alignToolButton);

        break;
      }
    case _delegate_TEXT :
      {
        std::cout << " delegate text" << std::endl;
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
    case _delegate_DROP:
      {
        if( item->get_dropdown_column() == index.column() ){
          QComboBox* combo = editor->findChild<QComboBox*>();
          QVariant value = index.model()->data(index, Qt::EditRole);
          const QVector<QVariant> drop_data = item->get_dropdown_data();
          QString drop_string;
          const QVector<QVariant> drop_enum = item->get_dropdown_enum();
          for( int enum_pos = 0; enum_pos < drop_enum.size(); enum_pos++ ){
            if( drop_enum[enum_pos] == value ){
              combo->setCurrentIndex(enum_pos);
            }
          }
        }
        break;
      }
    case _delegate_FILE  :
    case _delegate_DIR  :
      {

        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value = index.model()->data(index, Qt::EditRole).toString();
        line->setText(value);
        QStyledItemDelegate::setEditorData(editor,index);

        break;
      }
    case _delegate_TEXT_ACTION:
      {
        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value =  index.model()->data(index, Qt::EditRole).toString();
        line->setText(  value );
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
    case _delegate_DROP:
      {
        if( item->get_dropdown_column() == index.column() ){
          QComboBox* combo = editor->findChild<QComboBox*>();
          int pos = combo->currentIndex();
          const QVector<QVariant> drop_enum = item->get_dropdown_enum();
          std::cout << "setModelData dropdown to index " <<  pos << " enum " <<  drop_enum[pos].toInt() << std::endl;
          const QVariant value = drop_enum[pos];
          model->setData(index, value , Qt::EditRole);
        }
        else{
          std::cout << " wrong column on delegate DROP" << std::endl;
        }
        break;
      }
    case _delegate_FILE:
    case _delegate_DIR:
      {
        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value = line->text();
        model->setData(index,value, Qt::EditRole);
        break;
      }
    case _delegate_TEXT_ACTION:
      {
        QLineEdit* line = editor->findChild<QLineEdit*>();
        QVariant value = QVariant::fromValue( line->text() );
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
    case _delegate_DROP:
    case _delegate_FILE  :
    case _delegate_DIR  :
      {
        editor->setGeometry(option.rect);
        break;
      }
    case _delegate_TEXT_ACTION:
    case _delegate_TEXT  :
      {
        QStyledItemDelegate::updateEditorGeometry(editor,option,index);
        break;
      }
  }
}

void TreeItemFileDelegate::get_filename_slot( QWidget *editor ) const {
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

void TreeItemFileDelegate::get_dirname_slot( QWidget *editor ) {
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


