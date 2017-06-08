#ifndef FILEDELEGATE_H
#define FILEDELEGATE_H

#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QObject>
#include <Qsize>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QFile>
#include <QPushButton>
#include <QTextLine>
#include <QtGui>
#include <QToolBar>
#include <QMenu>
#include <QAction>

#include "qt_file_push_button.h"
#include "treeitem.h"
#include "treemodel.h"
#include "custom_tool_button.h"

class TreeItemFileDelegate : public QStyledItemDelegate {
  Q_OBJECT
  public:
    explicit TreeItemFileDelegate( QObject *parent = 0 );

    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QStyleOptionButton checkboxOption( const QStyleOptionViewItem& option,const QModelIndex& index, int position,  Qt::AlignmentFlag alignment ) const;

    QRect alignRect( QRect object, QRect frame, int position,  Qt::AlignmentFlag alignment ) const;

    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

signals:

    public slots:
      void get_filename_slot( QWidget *editor ) const;
    void get_dirname_slot( QWidget *editor );
    void commit_and_call(  QWidget * editor, boost::function<bool()> _action  ) ;

  private:
    QToolBar* _editor_toolbar;
};

#endif // end FILEDELEGATE_H
