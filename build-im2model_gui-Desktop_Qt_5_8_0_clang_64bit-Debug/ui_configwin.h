/********************************************************************************
** Form generated from reading UI file 'configwin.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGWIN_H
#define UI_CONFIGWIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_Structure;
    QWidget *centralWidget;
    QTabWidget *tabWidget;
    QWidget *structure_tab;
    QWidget *experimental_data_tab;
    QGraphicsView *qgraphics_experimental_data;
    QLineEdit *qline_image_path;
    QLabel *qlabel_image_path;
    QPushButton *qpush_load_image;
    QLineEdit *qline_roi_center_x;
    QLineEdit *qline_roi_center_y;
    QLabel *qlabel_roi_center;
    QLabel *qlabel_roi_center_x;
    QLabel *qlabel_roi_center_y;
    QLabel *qlabel_experimental_sampling;
    QLabel *qlabel_exp_nx;
    QLineEdit *qline_exp_nx;
    QLabel *qlabel_exp_ny;
    QLineEdit *qline_exp_ny;
    QLabel *qlabel_roi_size;
    QLabel *qlabel_roi_size_x;
    QLabel *qlabel_roi_size_y;
    QLineEdit *qline_roi_size_x;
    QLineEdit *qline_roi_size_y;
    QLabel *qlabel_edge_detection;
    QSlider *qslider_experimental_edge_detection;
    QLabel *qlabel_max_contour_distance_px;
    QSlider *qslider_max_contour_distance_px;
    QWidget *unit_cell_tab;
    QWidget *simulation_data_tab;
    QWidget *super_cell_tab;
    QWidget *crystal_shape_tab;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1280, 960);
        actionNew_Structure = new QAction(MainWindow);
        actionNew_Structure->setObjectName(QStringLiteral("actionNew_Structure"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(50, 20, 1024, 720));
        structure_tab = new QWidget();
        structure_tab->setObjectName(QStringLiteral("structure_tab"));
        structure_tab->setEnabled(true);
        tabWidget->addTab(structure_tab, QString());
        experimental_data_tab = new QWidget();
        experimental_data_tab->setObjectName(QStringLiteral("experimental_data_tab"));
        qgraphics_experimental_data = new QGraphicsView(experimental_data_tab);
        qgraphics_experimental_data->setObjectName(QStringLiteral("qgraphics_experimental_data"));
        qgraphics_experimental_data->setGeometry(QRect(350, 10, 640, 640));
        qline_image_path = new QLineEdit(experimental_data_tab);
        qline_image_path->setObjectName(QStringLiteral("qline_image_path"));
        qline_image_path->setGeometry(QRect(20, 50, 191, 21));
        qlabel_image_path = new QLabel(experimental_data_tab);
        qlabel_image_path->setObjectName(QStringLiteral("qlabel_image_path"));
        qlabel_image_path->setGeometry(QRect(20, 30, 81, 16));
        qpush_load_image = new QPushButton(experimental_data_tab);
        qpush_load_image->setObjectName(QStringLiteral("qpush_load_image"));
        qpush_load_image->setGeometry(QRect(220, 50, 113, 32));
        qline_roi_center_x = new QLineEdit(experimental_data_tab);
        qline_roi_center_x->setObjectName(QStringLiteral("qline_roi_center_x"));
        qline_roi_center_x->setGeometry(QRect(30, 170, 113, 21));
        qline_roi_center_y = new QLineEdit(experimental_data_tab);
        qline_roi_center_y->setObjectName(QStringLiteral("qline_roi_center_y"));
        qline_roi_center_y->setGeometry(QRect(190, 170, 113, 21));
        qlabel_roi_center = new QLabel(experimental_data_tab);
        qlabel_roi_center->setObjectName(QStringLiteral("qlabel_roi_center"));
        qlabel_roi_center->setGeometry(QRect(20, 150, 91, 16));
        qlabel_roi_center_x = new QLabel(experimental_data_tab);
        qlabel_roi_center_x->setObjectName(QStringLiteral("qlabel_roi_center_x"));
        qlabel_roi_center_x->setGeometry(QRect(20, 170, 16, 16));
        qlabel_roi_center_y = new QLabel(experimental_data_tab);
        qlabel_roi_center_y->setObjectName(QStringLiteral("qlabel_roi_center_y"));
        qlabel_roi_center_y->setGeometry(QRect(170, 170, 16, 16));
        qlabel_experimental_sampling = new QLabel(experimental_data_tab);
        qlabel_experimental_sampling->setObjectName(QStringLiteral("qlabel_experimental_sampling"));
        qlabel_experimental_sampling->setGeometry(QRect(20, 90, 201, 16));
        qlabel_exp_nx = new QLabel(experimental_data_tab);
        qlabel_exp_nx->setObjectName(QStringLiteral("qlabel_exp_nx"));
        qlabel_exp_nx->setGeometry(QRect(20, 110, 16, 16));
        qline_exp_nx = new QLineEdit(experimental_data_tab);
        qline_exp_nx->setObjectName(QStringLiteral("qline_exp_nx"));
        qline_exp_nx->setGeometry(QRect(30, 110, 113, 21));
        qlabel_exp_ny = new QLabel(experimental_data_tab);
        qlabel_exp_ny->setObjectName(QStringLiteral("qlabel_exp_ny"));
        qlabel_exp_ny->setGeometry(QRect(170, 110, 16, 16));
        qline_exp_ny = new QLineEdit(experimental_data_tab);
        qline_exp_ny->setObjectName(QStringLiteral("qline_exp_ny"));
        qline_exp_ny->setGeometry(QRect(190, 110, 113, 21));
        qlabel_roi_size = new QLabel(experimental_data_tab);
        qlabel_roi_size->setObjectName(QStringLiteral("qlabel_roi_size"));
        qlabel_roi_size->setGeometry(QRect(20, 220, 60, 16));
        qlabel_roi_size_x = new QLabel(experimental_data_tab);
        qlabel_roi_size_x->setObjectName(QStringLiteral("qlabel_roi_size_x"));
        qlabel_roi_size_x->setGeometry(QRect(20, 250, 16, 16));
        qlabel_roi_size_y = new QLabel(experimental_data_tab);
        qlabel_roi_size_y->setObjectName(QStringLiteral("qlabel_roi_size_y"));
        qlabel_roi_size_y->setGeometry(QRect(170, 250, 21, 16));
        qline_roi_size_x = new QLineEdit(experimental_data_tab);
        qline_roi_size_x->setObjectName(QStringLiteral("qline_roi_size_x"));
        qline_roi_size_x->setGeometry(QRect(30, 250, 113, 21));
        qline_roi_size_y = new QLineEdit(experimental_data_tab);
        qline_roi_size_y->setObjectName(QStringLiteral("qline_roi_size_y"));
        qline_roi_size_y->setGeometry(QRect(190, 250, 113, 21));
        qlabel_edge_detection = new QLabel(experimental_data_tab);
        qlabel_edge_detection->setObjectName(QStringLiteral("qlabel_edge_detection"));
        qlabel_edge_detection->setGeometry(QRect(20, 300, 111, 16));
        qslider_experimental_edge_detection = new QSlider(experimental_data_tab);
        qslider_experimental_edge_detection->setObjectName(QStringLiteral("qslider_experimental_edge_detection"));
        qslider_experimental_edge_detection->setGeometry(QRect(20, 330, 281, 22));
        qslider_experimental_edge_detection->setInputMethodHints(Qt::ImhNone);
        qslider_experimental_edge_detection->setMaximum(255);
        qslider_experimental_edge_detection->setOrientation(Qt::Horizontal);
        qslider_experimental_edge_detection->setInvertedAppearance(false);
        qslider_experimental_edge_detection->setInvertedControls(false);
        qlabel_max_contour_distance_px = new QLabel(experimental_data_tab);
        qlabel_max_contour_distance_px->setObjectName(QStringLiteral("qlabel_max_contour_distance_px"));
        qlabel_max_contour_distance_px->setGeometry(QRect(20, 370, 191, 16));
        qslider_max_contour_distance_px = new QSlider(experimental_data_tab);
        qslider_max_contour_distance_px->setObjectName(QStringLiteral("qslider_max_contour_distance_px"));
        qslider_max_contour_distance_px->setGeometry(QRect(20, 400, 281, 22));
        qslider_max_contour_distance_px->setMaximum(255);
        qslider_max_contour_distance_px->setOrientation(Qt::Horizontal);
        tabWidget->addTab(experimental_data_tab, QString());
        unit_cell_tab = new QWidget();
        unit_cell_tab->setObjectName(QStringLiteral("unit_cell_tab"));
        unit_cell_tab->setAutoFillBackground(false);
        tabWidget->addTab(unit_cell_tab, QString());
        simulation_data_tab = new QWidget();
        simulation_data_tab->setObjectName(QStringLiteral("simulation_data_tab"));
        tabWidget->addTab(simulation_data_tab, QString());
        super_cell_tab = new QWidget();
        super_cell_tab->setObjectName(QStringLiteral("super_cell_tab"));
        tabWidget->addTab(super_cell_tab, QString());
        crystal_shape_tab = new QWidget();
        crystal_shape_tab->setObjectName(QStringLiteral("crystal_shape_tab"));
        tabWidget->addTab(crystal_shape_tab, QString());
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1280, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionNew_Structure);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        actionNew_Structure->setText(QApplication::translate("MainWindow", "New Structure", Q_NULLPTR));
#ifndef QT_NO_ACCESSIBILITY
        structure_tab->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(structure_tab), QApplication::translate("MainWindow", "Structure Properties", Q_NULLPTR));
        qlabel_image_path->setText(QApplication::translate("MainWindow", "Image Path", Q_NULLPTR));
        qpush_load_image->setText(QApplication::translate("MainWindow", "Load image", Q_NULLPTR));
        qlabel_roi_center->setText(QApplication::translate("MainWindow", "Roi Center", Q_NULLPTR));
        qlabel_roi_center_x->setText(QApplication::translate("MainWindow", "x", Q_NULLPTR));
        qlabel_roi_center_y->setText(QApplication::translate("MainWindow", "y", Q_NULLPTR));
        qlabel_experimental_sampling->setText(QApplication::translate("MainWindow", "Experimental Sampling (nm/pixel)", Q_NULLPTR));
        qlabel_exp_nx->setText(QApplication::translate("MainWindow", "x", Q_NULLPTR));
        qlabel_exp_ny->setText(QApplication::translate("MainWindow", "y", Q_NULLPTR));
        qlabel_roi_size->setText(QApplication::translate("MainWindow", "Roi Size", Q_NULLPTR));
        qlabel_roi_size_x->setText(QApplication::translate("MainWindow", "x", Q_NULLPTR));
        qlabel_roi_size_y->setText(QApplication::translate("MainWindow", "y", Q_NULLPTR));
        qlabel_edge_detection->setText(QApplication::translate("MainWindow", "Edge Detection", Q_NULLPTR));
        qlabel_max_contour_distance_px->setText(QApplication::translate("MainWindow", "Max Contour Distance (pixels)", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(experimental_data_tab), QApplication::translate("MainWindow", "Experimental Data", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(unit_cell_tab), QApplication::translate("MainWindow", "Unit-Cell", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(simulation_data_tab), QApplication::translate("MainWindow", "Simulation Data", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(super_cell_tab), QApplication::translate("MainWindow", "Super-Cell", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(crystal_shape_tab), QApplication::translate("MainWindow", "Crystal Shape", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGWIN_H
