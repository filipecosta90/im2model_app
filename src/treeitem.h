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
#include <boost/program_options.hpp>
/** END BOOST **/

#include "custom_tool_button.h"


class TreeItem : public QObject {
  Q_OBJECT
  public:
    enum DelegateType { _delegate_ACTION_CHECK, _delegate_FILE, _delegate_DIR, _delegate_TEXT, _delegate_TEXT_ACTION, _delegate_SLIDER_INT, _delegate_TEXT_DOCUMENT, _delegate_TEXT_BROWSER, _delegate_DROP };
    enum ActionStatusType { _status_NOT_READY, _status_READY_TO_RUN, _status_RUNNING, _status_RUNNED_OK, _status_RUNNED_ERROR };

    explicit TreeItem( QVector<QVariant> &data, TreeItem *parent = 0 );
    explicit TreeItem( QVector<QVariant> &data, QVector<bool> editable, TreeItem *parent = 0 );

    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(double)> setter, QVector<bool> editable, TreeItem *parent = 0);

    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, QVector<QVariant> &legend, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, boost::function<bool(void)> getter, QVector<QVariant> &legend, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, boost::function<bool(void)> getter, QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent = 0);

    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, boost::function<int(void)> getter, TreeItem *parent = 0 );
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, QVector<bool> editable, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, boost::function<int(void)> getter, QVector<bool> editable, TreeItem *parent = 0);

    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent = 0);

    ~TreeItem();
    TreeItem *parent();
    TreeItem *child(int number);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column , int role) const;
    QVariant get_legend(int column) const;
    bool insertChildren(int position, int count, int columns);
    bool insertChildren(TreeItem *item);
    bool insertColumns(int position, int columns);
    bool set_parent( TreeItem* parent );
    bool removeChildren(int position, int count);
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool isItemEditable( int column ) const;
    bool isItemCheckable( int column ) const;
    bool setData(int column, const QVariant &value , int role);
    bool appendData( int column, const QVariant &value);
    bool set_fp_data_data_appender_col_pos( int col );
    bool set_flag_fp_data_appender_string( bool set );

    boost::property_tree::ptree*  save_data_into_property_tree( );
    bool load_data_from_property_tree( boost::property_tree::ptree pt_root );
    bool load_data_from_getter();
    DelegateType get_item_delegate_type();
    QVector<QVariant> get_dropdown_data();
    QVector<QVariant> get_dropdown_enum();
    int get_dropdown_column();
    /* slider */
    int get_slider_column();
    int get_slider_int_range_min();
    int get_slider_int_range_max();

    void set_slider_int_range_min( int min );
    void set_slider_int_range_max( int max );

    void set_item_delegate_type( DelegateType _delegate_type );
    void set_dropdown_options( int column , QVector<QVariant>& drop, QVector<QVariant>& drop_enum );
    void set_action_toolBar( CustomToolButton* editToolBar );
    CustomToolButton* get_action_toolBar();
    void add_toolbar ( QVector<QVariant> actions_description , std::vector<boost::function<bool()>> actions );
    QVector<QVariant> get_toolbar_actions_description();
    std::vector<boost::function<bool()>> get_toolbar_actions();
    bool _is_toolbar_defined();
    bool set_fp_check_setter( int col,  boost::function<bool(bool)> fp_check_setter );
    bool  set_fp_check_getter( int col,  boost::function<bool()> fp_check_getter );
    bool load_check_status_from_getter( int col );

    void set_variable_name( std::string varname );
    std::string get_variable_name();

    void set_variable_description( std::string vardescription );
    std::string  get_variable_description();
    void setStatusOption( int col , ActionStatusType action );

    bool get_flag_fp_data_getter_bool();
    bool call_fp_data_getter_bool();
    bool get_flag_fp_data_setter_bool();
    bool call_fp_data_setter_bool( bool value );
    bool has_hightlight_error( int column );
    bool disable_highlight_error( int column );
    bool enable_highlight_error( int column );


signals:

    void dataChanged( int column );
    void dataChanged( std::string varname );
    void highlight_error( int column );

  private:
    TreeItem *parentItem;
    QList<TreeItem*> childItems;
    QVector<QVariant> itemData;
    QVector<QVariant> itemLegend;
    QVector<bool> itemIsEditableVec;

    QVector<bool> itemIsCheckableVec;
    QVector<bool> itemState;
    std::vector< boost::function<bool(bool)> > fp_checkable_setters;
    std::vector< boost::function<bool(void)> > fp_checkable_getters;

    QVector<QVariant> dropdown_data;
    QVector<QVariant> dropdown_enum;
    int dropdown_column;
    int slider_column = 1;
    bool _flag_dropdown = false;

    boost::function<bool(std::string)> fp_data_setter_string;
    boost::function<bool(std::string)> fp_data_appender_string;
    boost::function<std::string(void)> fp_data_getter_string;

    boost::function<bool(double)> fp_data_setter_double;
    boost::function<double(void)> fp_data_getter_double;

    boost::function<bool(bool)> fp_data_setter_bool;
    boost::function<bool(void)> fp_data_getter_bool;

    boost::function<bool(int)> fp_data_setter_int;
    boost::function<int(void)> fp_data_getter_int;

    bool _flag_fp_data_setter_string = false;
    bool _flag_fp_data_appender_string = false;
    bool _flag_fp_data_getter_string = false;

    bool _flag_fp_data_setter_bool = false;
    bool _flag_fp_data_getter_bool = false;

    bool _flag_fp_data_setter_double = false;
    bool _flag_fp_data_getter_double = false;

    bool _flag_fp_data_setter_int = false;
    bool _flag_fp_data_getter_int = false;

    int _fp_data_setter_col_pos = 1;
    int _fp_data_getter_col_pos = 1;
    int _fp_data_data_appender_col_pos = 1;

    /* slider */
    int _slider_int_range_min;
    int _slider_int_range_max;

    DelegateType _item_delegate_type = _delegate_TEXT;
    QToolBar* _action_toolBar;
    CustomToolButton* alignToolButton;
    QVector<QVariant> _toolbar_actions_description;
    std::vector<boost::function<bool()>> _toolbar_actions;
    bool _flag_toolbar = false;

    std::string _variable_name;
    std::string _variable_description;
    bool _flag_is_action = false;
    int _status_column = 0;
    ActionStatusType _action_status = ActionStatusType::_status_NOT_READY;

    std::vector<bool> _flag_highlight_error;
private slots:
    void clean_highlight_status( int column );
};

Q_DECLARE_METATYPE(std::string)


#endif // TREEITEM_H

