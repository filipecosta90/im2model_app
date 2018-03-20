#ifndef SRC_FILEDELEGATE_H__
#define SRC_FILEDELEGATE_H__

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
#include <QLocale>
#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>                // for directory_iterator
#include <boost/filesystem/path.hpp>                      // for path, operator==, oper...

#include "qt_file_push_button.h"
#include "treeitem.h"
#include "treemodel.h"
#include "custom_tool_button.h"
#include "QtAwesome.h"
#include "qint_validator.h"
#include "qdouble_validator.h"
#include "qline_edit_tooltip.h"

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

private slots:
    void emitCommitData();

    public slots:
      void get_filename_slot( QWidget *editor ) const;
    void get_dirname_slot( QWidget *editor );
    void commit_and_call(  QWidget * editor, boost::function<bool()> _action  ) ;
    void showToolTipText( QWidget *editor ) const;
    void set_base_dir_path( std::string , bool use_relative );

  private:
    QToolBar* _editor_toolbar;
    bool* _flag_temp_data;
    boost::filesystem::path base_dir_path;
    bool use_base_dir_for_relative = false;
};

#endif // end FILEDELEGATE_H
