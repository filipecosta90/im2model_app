#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QAbstractItemModel>
#include <QModelIndex>

#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

Q_DECLARE_METATYPE(std::string)

  class TreeItem : public QObject
{
  Q_OBJECT
  public:
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent = 0 );
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setters, TreeItem *parent, bool checkable, boost::function<bool(bool)> check_setter);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setters, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();
    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertChildren(TreeItem *item);

    bool insertColumns(int position, int columns);
    TreeItem *parent();
    bool set_parent( TreeItem* parent );
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool isChecked() const;
    void setChecked( bool set );
    bool isCheckable() const;
    bool isItemEditable( int column ) const;
    void setCheckable( bool set );
    bool setData(int column, const QVariant &value);
    boost::property_tree::ptree*  save_data_into_property_tree( );
    QStringList extractStringsFromItem();

signals:
    void dataChanged( int column );

  private:
    TreeItem *parentItem;
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    QVector<bool> itemIsEditableVec;
    bool checked;
    bool is_checkable;
    boost::function<bool(std::string)> fp_data_setter;
    boost::function<bool(bool)> fp_check_setter;        
};

#endif // TREEITEM_H

