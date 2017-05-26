/****************************************************************************
 * **
 * ** Copyright (C) 2016 The Qt Company Ltd.
 * ** Contact: https://www.qt.io/licensing/
 * **
 * ** This file is part of the examples of the Qt Toolkit.
 * **
 * ** $QT_BEGIN_LICENSE:BSD$
 * ** Commercial License Usage
 * ** Licensees holding valid commercial Qt licenses may use this file in
 * ** accordance with the commercial license agreement provided with the
 * ** Software or, alternatively, in accordance with the terms contained in
 * ** a written agreement between you and The Qt Company. For licensing terms
 * ** and conditions see https://www.qt.io/terms-conditions. For further
 * ** information use the contact form at https://www.qt.io/contact-us.
 * **
 * ** BSD License Usage
 * ** Alternatively, you may use this file under the terms of the BSD license
 * ** as follows:
 * **
 * ** "Redistribution and use in source and binary forms, with or without
 * ** modification, are permitted provided that the following conditions are
 * ** met:
 * **   * Redistributions of source code must retain the above copyright
 * **     notice, this list of conditions and the following disclaimer.
 * **   * Redistributions in binary form must reproduce the above copyright
 * **     notice, this list of conditions and the following disclaimer in
 * **     the documentation and/or other materials provided with the
 * **     distribution.
 * **   * Neither the name of The Qt Company Ltd nor the names of its
 * **     contributors may be used to endorse or promote products derived
 * **     from this software without specific prior written permission.
 * **
 * **
 * ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 * **
 * ** $QT_END_LICENSE$
 * **
 * ****************************************************************************/

#include <QtWidgets>

#include "treeitem_file_delegate.hpp"
#include <iostream>

TreeItemFileDelegate::TreeItemFileDelegate( QObject *parent ) : QItemDelegate(parent){

}

QWidget *TreeItemFileDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const{

  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
  QWidget *editor = 0;

  switch(item->get_item_delegate_type())
  {
    case _delegate_FILE  :
      {
        editor = new QWidget(parent);
        QString value = index.model()->data(index, Qt::EditRole).toString();
        QHBoxLayout *layout = new QHBoxLayout( editor );
        QLineEdit *line = new QLineEdit( editor );
        FilePushButton *button = new FilePushButton(parent);
        editor->setLayout(layout);
        layout->addWidget(line);
        layout->addWidget(button);
        layout->setContentsMargins(0, 0, 0, 0);
        std::cout << " creating editor" << std::endl;
        connect(button, SIGNAL(onClick(QWidget*)), this, SLOT(get_filename_slot(QWidget*)));
        break;
      }
    case _delegate_TEXT  :
      {
        editor = QItemDelegate::createEditor(parent,option,index);
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
        break;
      }
    case _delegate_TEXT  :
      {
        QItemDelegate::setEditorData(editor,index);
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
        QItemDelegate::setModelData(editor,model,index);
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
        QItemDelegate::updateEditorGeometry(editor,option,index);
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



