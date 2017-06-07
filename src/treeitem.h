#ifndef TREEITEM_H
#define TREEITEM_H

/** START QT **/
#include <QList>
#include <QVariant>
#include <QVector>
#include <QAbstractItemModel>
#include <QModelIndex>
/** END QT **/

/** START BOOST **/
#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
/** END BOOST **/

Q_DECLARE_METATYPE(std::string)

  enum DelegateType { _delegate_FILE, _delegate_DIR, _delegate_TEXT, _delegate_DROP };

class TreeItem : public QObject {
  Q_OBJECT
  public:
    explicit TreeItem( QVector<QVariant> &data, TreeItem *parent = 0);
    explicit  TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent = 0);


    ~TreeItem();
    TreeItem *parent();
    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertChildren(TreeItem *item);
    bool insertColumns(int position, int columns);
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
    bool load_data_from_property_tree( boost::property_tree::ptree pt_root );
    DelegateType get_item_delegate_type();
    QVector<QVariant> get_dropdown_data();
    QVector<QVariant> get_dropdown_enum();
    int get_dropdown_column();

    void set_item_delegate_type( DelegateType _delegate_type );
    void set_dropdown_options( int column , QVector<QVariant>& drop, QVector<QVariant>& drop_enum );

signals:
    void dataChanged( int column );

  private:
    TreeItem *parentItem;
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    QVector<bool> itemIsEditableVec;
    QVector<QVariant> dropdown_data;
    QVector<QVariant> dropdown_enum;
    int dropdown_column;
    bool _flag_dropdown = false;

    bool checked = false;
    bool is_checkable = false;
    boost::function<bool(std::string)> fp_data_setter_string;
    boost::function<bool(bool)> fp_check_setter;
    boost::function<bool(double)> fp_data_setter_double;
    boost::function<bool(int)> fp_data_setter_int;

    bool _flag_fp_data_setter_string = false;
    bool _flag_fp_data_setter_double = false;
    bool _flag_fp_data_setter_int = false;
    int _fp_data_setter_col_pos = 1;
    bool _flag_fp_check_setter = false;

    DelegateType _item_delegate_type = _delegate_TEXT;
};

#endif // TREEITEM_H

