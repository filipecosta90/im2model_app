
#pragma once
#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QFile>
#include <QtWidgets>
#include <QThread>
#include <QDebug>
#include <iostream>
#include "td_map.hpp"

class GuiSimOutUpdater : public QObject {
  Q_OBJECT

  public:
    explicit GuiSimOutUpdater(TDMap* core_tdmap, QObject *parent = 0) : QObject(parent) {
        _working =false;
        _abort = false;
        tdmap = core_tdmap;
    }

public slots:
    void newTDMapSim() {
        std::cout << "Slave thread " << QThread::currentThreadId() << std::endl;
        qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

        bool result = tdmap->run_tdmap();
        if ( result ){
               emit TDMap_sucess();
        }
        else {
            emit TDMap_failure();
        }
        // Set _working to false, meaning the process can't be aborted anymore.
           mutex.lock();
           _working = false;
           mutex.unlock();
           emit finished();

    }

    void requestTDMap(){
        mutex.lock();
            _working = true;
            _abort = false;
            qDebug()<<"Request worker start in Thread "<<thread()->currentThreadId();
            mutex.unlock();
        emit TDMap_request();
    }

    void abort()
    {
        mutex.lock();
        if (_working) {
            _abort = true;
            qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
        }
        mutex.unlock();
    }


signals:
    void TDMap_request();
    void TDMap_sucess();
    void TDMap_failure();
    void finished();


private:

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
};
