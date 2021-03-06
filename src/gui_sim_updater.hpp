/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef SRC_GUI_SIM_UPDATER_H__
#define SRC_GUI_SIM_UPDATER_H__

#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QFile>
#include <QtWidgets>
#include <QThread>
#include <QDebug>
#include <iostream>

#include "td_map.hpp"
#include "super_cell.hpp"
#include "gui_stream.hpp"

class GuiSimOutUpdater : public QObject {
  Q_OBJECT

  private:
    /**
     * @brief Process is aborted when @em true
     */
    bool _abort;
    /**
     * @brief @em true when Worker is doing work
     */
    bool _working;
    /**
     * @brief Protects access to #_abort and #_working
     */
    QMutex mutex;

    /**
     * @brief TDMap var
     */
    TDMap* tdmap;

    /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

  public:
    GuiSimOutUpdater(TDMap* core_tdmap ,  QObject *parent = 0 );
    bool set_application_logger( ApplicationLog::ApplicationLog* logger );

    public slots:
    void newSuperCellFull();
    void newSuperCellEdge();
    void newSuperCellFull_intensity_cols();
    void newTDMapSim();
    void requestTDMap();
    void requestSuperCellEdge();
    void requestFullSuperCell();
    void requestFullSuperCellComputeIntensityCols();

    void abort();

signals:
    void TDMap_request();
    void TDMap_started();
    void TDMap_sucess();
    void TDMap_sucess_no_correlation();
    void TDMap_failure();
    void TDMap_finished();

    void SuperCell_edge_request();
    void SuperCell_edge_sucess();
    void SuperCell_edge_failure();
    void SuperCell_edge_finished();

    void SuperCell_full_request();
    void SuperCell_full_sucess();
    void SuperCell_full_failure();
    void SuperCell_full_finished();

    void SuperCell_full_intensity_cols_request();
    void SuperCell_full_intensity_cols_sucess();
    void SuperCell_full_intensity_cols_failure();
    void SuperCell_full_intensity_cols_finished();

};

#endif
