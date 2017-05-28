#pragma once
#include <QWidget>
#include <QtWidgets>
#include <QAbstractItemView>
#include <QImage>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <QFrame>
#include <QBoxLayout>

#include <QStyledItemDelegate>
#include <QtWidgets>

#include "cv_tdmap_cell_image_frame.h"
#include "cv_image_frame.h"

class CvTDMapImageFrameDelegate : public QStyledItemDelegate
{
  Q_OBJECT
  public:
    CvTDMapImageFrameDelegate(QWidget *parent = 0) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const override;

    private slots:

};
