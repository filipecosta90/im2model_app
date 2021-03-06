/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#ifndef TDMap_CELL_H
#define TDMap_CELL_H

#include <QTableWidgetItem>
#include "base_logger.hpp"

class TDMap_Cell : public QTableWidgetItem
{
  public:
    TDMap_Cell();

    QTableWidgetItem *clone() const;
    void setData(int role, const QVariant &value);
    QVariant data(int role) const;

     /* Loggers */
  bool set_application_logger( ApplicationLog::ApplicationLog* logger );

  private:
    QVariant value() const;

        /* Loggers */
    ApplicationLog::ApplicationLog* logger = nullptr;
    bool _flag_logger = false;

};

#endif
