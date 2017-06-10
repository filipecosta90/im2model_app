
#include <QtWidgets>
#include <QItemDelegate>

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
    case TreeItem::_delegate_DROP:
      {
    /*  if( item->get_dropdown_column() == index.column() ){
        int value = index.model()->data(index, Qt::EditRole).toInt();
        QHBoxLayout *layout = new QHBoxLayout( );
        layout->setMargin(0);
        layout->setContentsMargins(0, 0, 0, 0);
        QSizePolicy sizePol(QSizePolicy::Expanding,QSizePolicy::Expanding);

        QComboBox* combo = new QComboBox();
        combo->setContentsMargins(0,0,0,0);

        // combo commands
        QStringList commands;
        const QVector<QVariant> drop_data = item->get_dropdown_data();
        const QVector<QVariant> drop_enum = item->get_dropdown_enum();

        foreach (const QVariant var_command, drop_data) {
          commands << var_command.toString();
        }

        for( int enum_pos = 0; enum_pos < drop_enum.size(); enum_pos++ ){
          if( drop_enum[enum_pos] == value ){
            combo->setCurrentIndex(enum_pos);
          }
        }
        // fill combo
        combo->addItems(commands);
        layout->addWidget(combo);
     //   editor->setLayout(layout);
        //editor->setSizePolicy(sizePol);
       // editor->setFocusProxy( combo );

        painter->save();
        painter->translate(option.rect.topLeft());
        combo->render(painter);
        painter->restore();
      }
      */
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
          QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &itemOption, painter);
        }
        else{
          QStyledItemDelegate::paint( painter, option,  index);
        }
        break;
      }
    case TreeItem::_delegate_CHECK:
      {
        if( item->get_checkbox_column() == index.column() ){

            QString legend = item->get_legend( index.column() ).toString();
            QCheckBox *checkbox = new QCheckBox( legend  );
            bool _is_checked = index.model()->data(index, Qt::EditRole).toBool();
            checkbox->setChecked(_is_checked);
            painter->save();
            painter->translate(option.rect.topLeft());
            checkbox->render(painter);
            painter->restore();
            break;

        }
        else{
          QStyledItemDelegate::paint( painter, option,  index);
        }
        break;
      }
    case TreeItem::_delegate_TEXT_DOCUMENT:
      {
        QStyleOptionViewItemV4 options = option;
        initStyleOption(&options, index);
        painter->save();
        QTextDocument doc;
        doc.setHtml(options.text);
        QSizeF size(200, 200);
        doc.setPageSize(size);
        // options.text = "";
        options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

        painter->translate(options.rect.left(), options.rect.top());
        QRect clip(0, 0, options.rect.width(), options.rect.height());
        doc.drawContents(painter, clip);
        painter->restore();
        break;
      }
    case TreeItem::_delegate_TEXT_BROWSER:
      {
      QStyledItemDelegate::paint( painter, option,  index);
      break;
/*
        if( 1 == index.column() ){
         QTextBrowser browser;
          QString value  = index.model()->data(index, Qt::EditRole).toString();
          browser.setText( value );
          QSize browser_size = option.rect.size();
          browser_size.setWidth( browser_size.width() - 20 );
          browser.resize( browser_size );
          std::cout << "option size w: " << option.rect.width() << " h " << option.rect.height() << std::endl;
          std::cout << "text browser size w: " << browser.width() << " h " << browser.height() << std::endl;
          browser.moveCursor (QTextCursor::End);
          painter->save();
          painter->translate(option.rect.topLeft());
          browser.render(painter);
          painter->restore();
          break;
        }
        else{
          QStyledItemDelegate::paint( painter, option,  index);
          break;
        }*/
      }
    case TreeItem::_delegate_TEXT_ACTION:
    case TreeItem::_delegate_FILE:
    case TreeItem::_delegate_DIR:
    case TreeItem::_delegate_TEXT:
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
    case TreeItem::_delegate_FILE :
    case TreeItem::_delegate_DIR :
      {
        editor = new QWidget(parent);
        QVariant::Type t = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());
        QWidget* text_editor = QItemEditorFactory().createEditor(t,parent);
        //text_editor->setMinimumHeight(box_size.height());

        QHBoxLayout* editor_layout = new QHBoxLayout( editor );
        editor_layout->setMargin(0);
        editor_layout->setContentsMargins(QMargins(0,0,0,0));
        editor_layout->setSpacing(0);
        editor_layout->setAlignment(Qt::AlignRight);

        QString _button_text = "...";
        FilePushButton *button = new FilePushButton( _button_text, editor );

        if( item->get_item_delegate_type() ==  TreeItem::_delegate_FILE ){
          connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_filename_slot(QWidget*)));
        }
        else{
          connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_dirname_slot(QWidget*)));
        }

        QSizePolicy spLeft(QSizePolicy::Expanding, QSizePolicy::Expanding);
        spLeft.setHorizontalStretch(3);
        text_editor->setSizePolicy(spLeft);
        editor_layout->addWidget(text_editor);

        QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
        spRight.setHorizontalStretch(1);
        button->setFixedWidth( button->fontMetrics().width( " ... " ) );
        editor_layout->addWidget(button);
        break;
      }
    case TreeItem::_delegate_DROP:
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
        else{
            editor = QStyledItemDelegate::createEditor(parent,option,index);
        }
        break;
      }
    case TreeItem::_delegate_CHECK:
      {
        editor = new QWidget(parent);

        QHBoxLayout *editor_layout = new QHBoxLayout(editor);
        QString legend = item->get_legend( index.column() ).toString();
        QCheckBox *checkbox = new QCheckBox( legend , editor );
        editor_layout->setAlignment(Qt::AlignLeft);

        bool _is_checked = index.model()->data(index, Qt::EditRole).toBool();
        checkbox->setChecked(_is_checked);

        editor_layout->setContentsMargins( QMargins(0,0,0,0) );
        editor_layout->addWidget( checkbox );
        editor->setLayout( editor_layout );
        break;
      }
    case TreeItem::_delegate_TEXT_ACTION:
      {
        editor = new QWidget(parent);
        QVariant::Type t = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());
        QWidget* text_editor = QItemEditorFactory().createEditor(t,parent);
        QHBoxLayout* editor_layout = new QHBoxLayout( editor );

        editor_layout->setMargin(0);
        editor_layout->setContentsMargins(QMargins(0,0,0,0));
        editor_layout->setSpacing(0);
        editor_layout->setAlignment(Qt::AlignRight);

        QMenu* alignMenu = new QMenu();
        CustomToolButton* alignToolButton = new CustomToolButton();
        alignToolButton->setMenu(alignMenu);

        /* get actions */
        if( item->_is_toolbar_defined() ){
          QVector<QVariant> _actions_description = item->get_toolbar_actions_description();
          std::vector<boost::function<bool()>> _actions = item->get_toolbar_actions();
          std::vector<QAction*> _actions_vec;
          assert( _actions_description.size() == _actions.size() );
          for( int _n_action = 0; _n_action < _actions.size(); _n_action++ ){
            QString _act_description = _actions_description.at(_n_action).toString();
            QAction* action = new QAction( _act_description);
            //connect(action, &QAction::triggered, this, commit_and_call( editor, _actions.at(_n_action)) );
            connect(action, &QAction::triggered, item,  _actions.at(_n_action) );
            // more work here
            //   connect(action, &QAction::triggered, this, SLOT(commit_and_call(  editor, _actions.at(_n_action))  ) );
            _actions_vec.push_back(action);
            alignMenu->addAction(action);
          }
          if( _actions_vec.size() > 0 ){
            alignToolButton->setDefaultAction(_actions_vec.at(0));
          }
        }


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
       case TreeItem::_delegate_TEXT_BROWSER:
  {
      editor = new QWidget(parent);
      QHBoxLayout* editor_layout = new QHBoxLayout( parent );
      editor_layout->setMargin(0);
      editor_layout->setContentsMargins(QMargins(0,0,0,0));
      editor_layout->setSpacing(0);
      editor_layout->setAlignment(Qt::AlignRight);

      QTextBrowser* browser = new QTextBrowser( editor );
      QString value  = index.model()->data(index, Qt::EditRole).toString();
      browser->setText( value );
      browser->moveCursor (QTextCursor::End);
      editor_layout->addWidget(browser);
      editor->setLayout( editor_layout );
      break;
  }

    default:
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
    case TreeItem::_delegate_DROP:
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
    case TreeItem::_delegate_CHECK:
      {
        QCheckBox* checkbox = editor->findChild<QCheckBox*>();
        bool _is_checked = index.model()->data(index, Qt::EditRole).toBool();
        checkbox->setChecked(_is_checked);
        break;
      }
    case TreeItem::_delegate_FILE  :
    case TreeItem::_delegate_DIR  :
      {

        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value = index.model()->data(index, Qt::EditRole).toString();
        line->setText(value);
        QStyledItemDelegate::setEditorData(editor,index);

        break;
      }
  case TreeItem::_delegate_TEXT_BROWSER:
    {
      QTextBrowser* browser = editor->findChild<QTextBrowser*>();
      QString value  = index.model()->data(index, Qt::EditRole).toString();
      browser->setText( value );
      browser->moveCursor (QTextCursor::End);
      std::cout << " setEditorData text browser" << std::endl;
      break;
    }
    case TreeItem::_delegate_TEXT_ACTION:
      {
        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value =  index.model()->data(index, Qt::EditRole).toString();
        line->setText(  value );
        break;
      }
    default  :
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
    case TreeItem::_delegate_DROP:
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
    case TreeItem::_delegate_CHECK:
      {
        QCheckBox* checkbox = editor->findChild<QCheckBox*>();
        bool _is_checked = checkbox->isChecked();
        const QVariant value = QVariant::fromValue( _is_checked );
        model->setData(index, value , Qt::EditRole);
        break;
      }
    case TreeItem::_delegate_FILE:
    case TreeItem::_delegate_DIR:
      {
        QLineEdit* line = editor->findChild<QLineEdit*>();
        QString value = line->text();
        model->setData(index,value, Qt::EditRole);
        break;
      }
    case TreeItem::_delegate_TEXT_ACTION:
      {
        QLineEdit* line = editor->findChild<QLineEdit*>();
        QVariant value = QVariant::fromValue( line->text() );
        model->setData(index,value, Qt::EditRole);
        break;
      }
  case TreeItem::_delegate_TEXT_BROWSER:
    {
      std::cout << " setModelData text browser" << std::endl;
      break;
    }

    default:
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
   /* case TreeItem::_delegate_TEXT_BROWSER:
      {
        QTextBrowser* browser  = editor->findChild<QTextBrowser*>();
        QRect browser_rect = browser->rect();
        editor->setGeometry( browser_rect );
        std::cout << " adjusting  cell size to browser size" <<  browser_rect.width() << " heigth " << browser_rect.height() << std::endl;
        break;
      }*/
    default:
      {
        QStyledItemDelegate::updateEditorGeometry(editor,option,index);
        break;
      }

  }
}

QSize TreeItemFileDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  switch(item->get_item_delegate_type())
  {
    case TreeItem::_delegate_TEXT_DOCUMENT:
      {
        QStyleOptionViewItemV4 options = option;
        initStyleOption(&options, index);
        QString value = index.model()->data(index, Qt::EditRole).toString();

        int lines_count = value.count("\n") +1 ;

        QTextDocument doc;
        doc.setHtml(options.text);
        QFontMetricsF fm(doc.defaultFont());
        auto const _line_height = fm.height() + fm.leading();
        int _ideal_doc_heigth = lines_count * _line_height;
        std::cout << "doc lines" << lines_count << std::endl;
        std::cout << "doc line heigth" << _line_height << std::endl;
        std::cout << "doc heigth" << doc.size().height() << std::endl;
        std::cout << "doc ideal heigth" << _ideal_doc_heigth << std::endl;

        return QSize(doc.idealWidth(), _ideal_doc_heigth );
        break;
      }
    case TreeItem::_delegate_TEXT_BROWSER:
      {

        //QTextBrowser* browser  = editor->findChild<QTextBrowser*>();
       // QRect browser_rect = browser->rect();
        //std::cout << " sizeHint for _delegate_TEXT_EDITOR" << std::endl; //<<  browser_rect.width() << " heigth " << browser_rect.height() << std::endl;

        return QSize( option.rect.width(), 100 );
        break;
      }
    default:
      {
        return QStyledItemDelegate::sizeHint(option,index);
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

void TreeItemFileDelegate::commit_and_call( QWidget * editor, boost::function<bool()> _action  ) {
  std::cout << " commit and call" << std::endl;
  emit commitData(editor);

}


QRect TreeItemFileDelegate::alignRect( QRect object,  QRect frame, int position,   Qt::AlignmentFlag alignment ) const {
  QRect rect = object;

  rect.setTopLeft( QPoint( 0, 0 ) );
  // Moves the object to the middle of the item.
  if ( rect.height() < frame.height() ) {
    rect.moveTop( ( frame.height() - rect.height() ) / 2 );
  }

  if ( alignment & Qt::AlignLeft ) {
    rect.moveLeft( position );
  }
  else if ( alignment & Qt::AlignRight ) {
    rect.moveRight( position );
  }

  return rect;
}

QStyleOptionButton TreeItemFileDelegate::checkboxOption( const QStyleOptionViewItem& option,  const QModelIndex& index, int position, Qt::AlignmentFlag alignment ) const {
  QStyleOptionButton checkboxOption;
  if ( index.data( Qt::CheckStateRole ).toBool() )
    checkboxOption.state = option.state | QStyle::State_On;
  else
    checkboxOption.state = option.state | QStyle::State_Off;
  QSize size = QApplication::style()->sizeFromContents( QStyle::CT_CheckBox, &option, QSize() );
  if ( size.isEmpty() ) {
    // A checkbox has definately a size != 0
    checkboxOption.rect.setSize( QSize( 22, 22 ) );
  }
  else {
    checkboxOption.rect.setSize( QSize( size.width(), size.height() ) );
  }
  checkboxOption.rect = alignRect( checkboxOption.rect, option.rect, position, alignment );
  return checkboxOption;
}
