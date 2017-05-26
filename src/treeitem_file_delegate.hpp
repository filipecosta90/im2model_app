#ifndef FILEDELEGATE_H
#define FILEDELEGATE_H

#include <QItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <Qsize>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFile>
#include <QPushButton>
#include <QTextLine>

#include "qt_file_push_button.h"
#include "treeitem.h"
#include "treemodel.h"

class TreeItemFileDelegate : public QItemDelegate
{
  Q_OBJECT
  public:
    explicit TreeItemFileDelegate( QObject *parent = 0 );

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

    public slots:
      void get_filename_slot( QWidget *editor );

  private:

};

#endif // end FILEDELEGATE_H
