#ifndef TREEITEM_H
#define TREEITEM_H

/** START QT **/
#include <QList>
#include <QVariant>
#include <QVector>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QToolBar>
#include <QMenu>
#include <QAction>
/** END QT **/

/** START BOOST **/
#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
/** END BOOST **/

#include "custom_tool_button.h"

Q_DECLARE_METATYPE(std::string)


class TreeItem : public QObject {
  Q_OBJECT
  public:
    enum DelegateType { _delegate_FILE, _delegate_DIR, _delegate_TEXT, _delegate_CHECK, _delegate_TEXT_ACTION, _delegate_DROP };

    explicit TreeItem( QVector<QVariant> &data, TreeItem *parent = 0);
    explicit  TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, QVector<QVariant> &legend, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, bool checkable, boost::function<bool(bool)> check_setter, TreeItem *parent = 0);

    ~TreeItem();
    TreeItem *parent();
    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    QVariant get_legend(int column) const;
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
    int get_checkbox_column();

    void set_item_delegate_type( DelegateType _delegate_type );
    void set_dropdown_options( int column , QVector<QVariant>& drop, QVector<QVariant>& drop_enum );
    void set_action_toolBar( CustomToolButton* editToolBar );
    CustomToolButton* get_action_toolBar();
    void add_toolbar ( QVector<QVariant> actions_description , std::vector<boost::function<bool()>> actions );
    QVector<QVariant> get_toolbar_actions_description();
    std::vector<boost::function<bool()>> get_toolbar_actions();
    bool _is_toolbar_defined();
    bool set_fp_checker( int col,  boost::function<bool(bool)> fp_check_setter );

signals:

    void dataChanged( int column );

  private:
    TreeItem *parentItem;
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    QVector<QVariant> itemLegend;
    QVector<bool> itemIsEditableVec;
    QVector<QVariant> dropdown_data;
    QVector<QVariant> dropdown_enum;
    int dropdown_column;
    bool _flag_dropdown = false;

    bool checked = false;
    bool is_checkable = false;
    boost::function<bool(std::string)> fp_data_setter_string;
    boost::function<bool(double)> fp_data_setter_double;
    boost::function<bool(bool)> fp_data_setter_bool;
    boost::function<bool(int)> fp_data_setter_int;

    bool _flag_fp_data_setter_string = false;
    bool _flag_fp_data_setter_bool = false;
    bool _flag_fp_data_setter_double = false;
    bool _flag_fp_data_setter_int = false;
    int _fp_data_setter_col_pos = 1;

    boost::function<bool(bool)> fp_check_setter;
    bool _flag_fp_check_setter = false;
    int _fp_check_setter_col_pos = 1;


    DelegateType _item_delegate_type = _delegate_TEXT;
    QToolBar* _action_toolBar;
    CustomToolButton* alignToolButton;
    QVector<QVariant> _toolbar_actions_description;
    std::vector<boost::function<bool()>> _toolbar_actions;
    bool _flag_toolbar = false;
};

#endif // TREEITEM_H

