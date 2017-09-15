#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options.hpp>

class TreeItem;

class TreeModel : public QAbstractItemModel {
  Q_OBJECT
  public:
    TreeModel(TreeItem *root,  QObject *parent = 0);

    ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /*
     * Read-only tree models only need to provide the above functions.
     * The following public functions provide support for editing and resizing:
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool appendData(const QModelIndex &index, const QVariant &value);
    bool clearData(const QModelIndex &index);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
    bool insertColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex &parent = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
    bool insertChildren(TreeItem *item, TreeItem* parent );

    void set_data_unique_id( int row, int column, int unique_id );
    bool _was_model_modified();
    bool _reset_model_modified();
    bool enable_highlight_error( std::string varname , int column );

    boost::property_tree::ptree* save_data_into_property_tree( );
    bool load_data_from_property_tree( boost::property_tree::ptree pt_root );
    TreeItem *getItem(const QModelIndex &index) const;

    public slots:
      void set_model_modified();
  private:

    TreeItem *rootItem;
    bool _flag_model_modified = false;
};


#endif // TREEMODEL_H
