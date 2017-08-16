/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtrackballcameracontroller.h"
#include "qtrackballcameracontroller_p.h"

#include <Qt3DRender/QCamera>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QButtonAxisInput>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QMouseHandler>
#include <Qt3DInput/QKeyboardHandler>
#include <Qt3DLogic/QFrameAction>
#include <Qt3DInput/QLogicalDevice>
#include <QtCore/QtGlobal>
#include <Qt3DCore/QTransform>
#include <QVector3D>

#include <math.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

QTrackballCameraController::QTrackballCameraController(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(*new QTrackballCameraControllerPrivate, parent)
{
    Q_D(QTrackballCameraController);
    d->init();
}

QTrackballCameraController::~QTrackballCameraController()
{

}

Qt3DRender::QCamera *QTrackballCameraController::camera() const
{
    Q_D(const QTrackballCameraController);
    return d->m_camera;
}

void QTrackballCameraController::setCamera(Qt3DRender::QCamera *camera)
{
    Q_D(QTrackballCameraController);
    if (d->m_camera != camera) {

        if (d->m_camera)
            d->unregisterDestructionHelper(d->m_camera);

        if (camera && !camera->parent())
            camera->setParent(this);

        d->m_camera = camera;

        // Ensures proper bookkeeping
        if (d->m_camera)
            d->registerDestructionHelper(d->m_camera, &QTrackballCameraController::setCamera, d->m_camera);

        emit cameraChanged();
    }
}

float QTrackballCameraController::panSpeed() const
{
    Q_D(const QTrackballCameraController);
    return d->m_panSpeed;
}

float QTrackballCameraController::zoomSpeed() const
{
    Q_D(const QTrackballCameraController);
    return d->m_zoomSpeed;
}

float QTrackballCameraController::rotationSpeed() const
{
    Q_D(const QTrackballCameraController);
    return d->m_rotationSpeed;
}

float QTrackballCameraController::zoomCameraLimit() const
{
    Q_D(const QTrackballCameraController);
    return d->m_zoomCameraLimit;
}

float QTrackballCameraController::trackballRadius() const
{
    Q_D(const QTrackballCameraController);
    return d->m_trackballRadius;
}

QPoint QTrackballCameraController::trackballCenter() const
{
    Q_D(const QTrackballCameraController);
    return d->m_trackballCenter;
}

QSize QTrackballCameraController::windowSize() const
{
    Q_D(const QTrackballCameraController);
    return d->m_windowSize;
}

void QTrackballCameraController::setPanSpeed(float v)
{
    Q_D(QTrackballCameraController);
    if (d->m_panSpeed != v) {
        d->m_panSpeed = v;
        emit panSpeedChanged();
    }
}

void QTrackballCameraController::setZoomSpeed(float v)
{
    Q_D(QTrackballCameraController);
    if (d->m_zoomSpeed != v) {
        d->m_zoomSpeed = v;
        emit zoomSpeedChanged();
    }
}

void QTrackballCameraController::setRotationSpeed(float v)
{
    Q_D(QTrackballCameraController);
    if (d->m_rotationSpeed != v) {
        d->m_rotationSpeed = v;
        emit rotationSpeedChanged();
    }
}

void QTrackballCameraController::setZoomCameraLimit(float v)
{
    Q_D(QTrackballCameraController);
    if (d->m_zoomCameraLimit != v) {
        d->m_zoomCameraLimit = v;
        emit zoomCameraLimitChanged();
    }
}

void QTrackballCameraController::setTrackballRadius(float v)
{
    Q_D(QTrackballCameraController);
    if (d->m_trackballRadius != v) {
        d->m_trackballRadius = v;
        emit trackballRadiusChanged();
    }
}

void QTrackballCameraController::setTrackballCenter(const QPoint &v)
{
    Q_D(QTrackballCameraController);
    if (d->m_trackballCenter != v) {
        d->m_trackballCenter = v;
        emit trackballCenterChanged();
    }
}

void QTrackballCameraController::setWindowSize(const QSize &v)
{
    Q_D(QTrackballCameraController);
    if (d->m_windowSize != v) {
        d->m_windowSize = v;
        emit windowSizeChanged();
    }
}

} // Qt3DExtras

QT_END_NAMESPACE
