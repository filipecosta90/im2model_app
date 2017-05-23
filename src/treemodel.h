#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <vector>

class TreeItem;

class TreeModel : public QAbstractItemModel {
  Q_OBJECT
  public:
    TreeModel(const QStringList &headers, const QStringList &data, QObject *parent = 0);
    TreeModel(TreeItem *root,  QObject *parent = 0);

    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
        const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /*
     * Read-only tree models only need to provide the above functions.
     * The following public functions provide support for editing and resizing:
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value,
        int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
        const QVariant &value, int role = Qt::EditRole) override;

    //bool setItemWidget(int row, int col, QWidget* widget);


    bool insertColumns(int position, int columns,
        const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns,
        const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows,
        const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
        const QModelIndex &parent = QModelIndex()) override;

    //bool operator==(const TreeModel &tree_item_comp );
    void set_data_unique_id( int row, int column, int unique_id );

    QStringList extractStringsFromModel( );
  private:
    TreeItem *getItem(const QModelIndex &index) const;
    TreeItem *rootItem;
};


#endif // TREEMODEL_H
