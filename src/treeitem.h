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

/** START OPENCV **/
#include <opencv2/core/types.hpp>    // for Point3d, Point, Rect, Point2d
#include <opencv2/opencv.hpp>
/** END OPENCV **/

#include "custom_tool_button.h"
#include "treemodel.h"

Q_DECLARE_METATYPE(cv::Mat)
Q_DECLARE_METATYPE(cv::Rect)

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
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(bool)> setter, boost::function<bool(void)> getter, QVector<QVariant> &legend, QVector<bool> editable, TreeItem *parent = 0);

    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(int)> setter, QVector<bool> editable, TreeItem *parent = 0);

    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<bool(std::string)> setter, QVector<bool> editable, TreeItem *parent = 0);

    explicit TreeItem( QVector<QVariant> &data, boost::function<void(QRect)> setter, TreeItem *parent = 0);
    explicit TreeItem( QVector<QVariant> &data, boost::function<void(QRect)> setter, QVector<bool> editable, TreeItem *parent = 0);

    ~TreeItem();
    TreeModel *model();
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
    bool set_model( TreeModel* parentModel );

    bool removeChildren(int position, int count);
    bool removeAllChildren();
    bool removeColumns(int position, int columns);
    int childNumber() const;
    bool isItemEditable( int column ) const;
    bool isItemCheckable( int column ) const;
    bool setData(int column, const QVariant &value , int role);
    bool appendData( int column, const QVariant &value);
    bool clearData( int column );
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
    bool  set_fp_check_getter( int col,  boost::function<bool(void)> fp_check_getter );
    bool load_check_status_from_getter( int col );

    void set_variable_name( std::string varname );
    std::string get_variable_name();

    void set_variable_description( std::string vardescription );
    std::string  get_variable_description();
    void setStatusOption( int col , ActionStatusType action );

    bool get_flag_fp_data_getter_bool( int col );
    bool call_fp_data_getter_bool();
    bool get_flag_fp_data_setter_bool();
    bool call_fp_data_setter_bool( bool value );
    bool has_hightlight_error( int column );
    bool disable_highlight_error( int column );
    bool enable_highlight_error( std::string varname , int column );

    /* field validation int */
    bool get_flag_validatable_int(int col_pos );
    bool set_flag_validatable_int(int col_pos, bool value );
    bool get_flag_validatable_int_top(int col_pos );
    bool get_flag_validatable_int_bottom(int col_pos );
    bool set_validator_int_top(int col_pos ,  boost::function<int(void)> fp_validator_max );
    bool set_validator_int_bottom(int col_pos ,  boost::function<int(void)> fp_validator_min );
    int get_validator_value_int_bottom( int col_pos );
    int get_validator_value_int_top( int col_pos );
    /* field validation double */
    bool get_flag_validatable_double(int col_pos );
    bool set_flag_validatable_double(int col_pos, bool value );
    bool get_flag_validatable_double_top(int col_pos );
    bool get_flag_validatable_double_bottom(int col_pos );
    bool set_validator_double_top(int col_pos ,  boost::function<double(void)> fp_validator_max );
    bool set_validator_double_bottom(int col_pos ,  boost::function<double(void)> fp_validator_min );
    double get_validator_value_double_bottom( int col_pos );
    double get_validator_value_double_top( int col_pos );

    bool set_fp_data_getter_bool_vec( int col_pos ,  boost::function<bool(void)> fp );
    bool set_fp_data_getter_double_vec( int col_pos ,  boost::function<double(void)> fp );
    bool set_fp_data_getter_int_vec( int col_pos ,  boost::function<int(void)> fp );
    bool set_fp_data_getter_string_vec( int col_pos ,  boost::function<std::string(void)> fp );

    public slots:
    void setToolTipText(const QString&);
    void load_data_from_getter( int column );
    void load_data_from_getter_int();
    void load_data_from_getter_double();
    void load_data_from_getter_string();
    void load_data_from_rect( QRect );

    signals:

    void dataChanged( int column );
    void dataChanged( std::string varname );
    void highlight_error( int column );

private:
    TreeModel *parentModel = nullptr;
    TreeItem *parentItem;
    QList<TreeItem*> childItems;

    /* vector of items that should update themselfs from getters when this item updates */
    std::vector<TreeItem*> pipeline_update_dependent_items;

    /* item data */
    QVector<QVariant> itemData;
    QVector<QVariant> itemLegend;

    /* editable */
    QVector<bool> itemIsEditableVec;

    /* checkable */
    QVector<bool> itemIsCheckableVec;
    QVector<bool> itemState;
    std::vector< boost::function<bool(bool)> > fp_checkable_setters;
    std::vector< boost::function<bool(void)> > fp_checkable_getters;

    /* field validation int */
    QVector<bool> itemIsValidatableIntVec;
    QVector<bool> itemIsValidatableIntTopVec;
    QVector<bool> itemIsValidatableIntBottomVec;
    std::vector< boost::function<int(void)> > fp_validator_int_range_min;
    std::vector< boost::function<int(void)> > fp_validator_int_range_max;

    /* field validation double */
    QVector<bool> itemIsValidatableDoubleVec;
    QVector<bool> itemIsValidatableDoubleTopVec;
    QVector<bool> itemIsValidatableDoubleBottomVec;
    QVector<bool> _flag_fp_data_getter_bool_vec;
    QVector<bool> _flag_fp_data_getter_double_vec;
    QVector<bool> _flag_fp_data_getter_int_vec;
    QVector<bool> _flag_fp_data_getter_string_vec;
    QVector<bool> _flag_fp_data_getter_rect_vec;

    std::vector< boost::function<double(void)> > fp_validator_double_range_min;
    std::vector< boost::function<double(void)> > fp_validator_double_range_max;
    std::vector< boost::function<bool(void)> > fp_data_getter_bool_vec;
    std::vector< boost::function<double(void)> > fp_data_getter_double_vec;
    std::vector< boost::function<int(void)> > fp_data_getter_int_vec;
    std::vector< boost::function<std::string(void)> > fp_data_getter_string_vec;
    std::vector< boost::function<QRect(void)> > fp_data_getter_rect_vec;

    /* item tooltip */
    QVector<QVariant> itemToolTip;
    QVector<bool> _flag_itemToolTip;

    QVector<QVariant> dropdown_data;
    QVector<QVariant> dropdown_enum;
    int dropdown_column;
    int slider_column = 1;
    bool _flag_dropdown = false;

    boost::function<bool(std::string)> fp_data_appender_string;
    boost::function<bool(std::string)> fp_data_setter_string;
    boost::function<bool(double)> fp_data_setter_double;
    boost::function<bool(bool)> fp_data_setter_bool;
    boost::function<bool(int)> fp_data_setter_int;
    boost::function<void(QRect)> fp_data_setter_rect;

    bool _flag_fp_data_setter_string = false;
    bool _flag_fp_data_appender_string = false;

    bool _flag_fp_data_setter_bool = false;

    bool _flag_fp_data_setter_double = false;

    bool _flag_fp_data_setter_int = false;
    bool _flag_fp_data_setter_rect = false;

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
    bool _flag_variable_name = false;
    std::string _variable_description;
    bool _flag_is_action = false;
    int _status_column = 0;
    ActionStatusType _action_status = ActionStatusType::_status_NOT_READY;

    std::vector<bool> _flag_highlight_error;
    private slots:
    void clean_highlight_status( int column );
    void force_layout_change( int column );
};

Q_DECLARE_METATYPE(std::string)


#endif // TREEITEM_H
