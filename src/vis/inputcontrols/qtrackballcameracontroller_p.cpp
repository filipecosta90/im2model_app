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

  QTrackballCameraControllerPrivate::QTrackballCameraControllerPrivate()
    : Qt3DCore::QEntityPrivate()
      , m_camera(nullptr)
      , m_midMouseButtonAction(new Qt3DInput::QAction())
      , m_leftMouseButtonAction(new Qt3DInput::QAction())
      , m_rightMouseButtonAction(new Qt3DInput::QAction())
      , m_altKeyAction(new Qt3DInput::QAction())
      , m_leftKeyAction(new Qt3DInput::QAction())
      , m_rightKeyAction(new Qt3DInput::QAction())
      , m_downKeyAction(new Qt3DInput::QAction())
      , m_upKeyAction(new Qt3DInput::QAction())
      , m_changeProjectionAction(new Qt3DInput::QAction())
      , m_panXAxis(new Qt3DInput::QAxis())
      , m_panYAxis(new Qt3DInput::QAxis())
      , m_wheelAxis(new Qt3DInput::QAxis())
      , m_leftMouseButtonInput(new Qt3DInput::QActionInput())
      , m_midMouseButtonInput(new Qt3DInput::QActionInput())
      , m_rightMouseButtonInput(new Qt3DInput::QActionInput())
      , m_altKeyInput(new Qt3DInput::QActionInput())
      , m_changeProjectionTypeInput(new Qt3DInput::QActionInput())
      , m_leftKeyInput(new Qt3DInput::QActionInput())
      , m_rightKeyInput(new Qt3DInput::QActionInput())
      , m_downKeyInput(new Qt3DInput::QActionInput())
      , m_upKeyInput(new Qt3DInput::QActionInput())
      , m_panXInput(new Qt3DInput::QAnalogAxisInput())
      , m_panYInput(new Qt3DInput::QAnalogAxisInput())
      , m_wheelXInput(new Qt3DInput::QAnalogAxisInput())
      , m_wheelYInput(new Qt3DInput::QAnalogAxisInput())
      , m_mouseDevice(new Qt3DInput::QMouseDevice())
      , m_keyboardDevice(new Qt3DInput::QKeyboardDevice())
      , m_mouseHandler(new Qt3DInput::QMouseHandler())
      , m_logicalDevice(new Qt3DInput::QLogicalDevice())
      , m_frameAction(new Qt3DLogic::QFrameAction())
      , m_windowSize(QSize(1920,1080))
      , m_trackballCenter(QPoint(0,0))
      , m_trackballRadius(20.0f)
      , m_panSpeed(50.0)
      , m_zoomSpeed(5.0f)
      , m_rotationSpeed(5.0)
      , m_rotationAngle(5.0)
      , m_rotationAngle_fine(1.0)
      , m_zoomCameraLimit(1.0)
  {
  }

  void QTrackballCameraControllerPrivate::init()
  {
    //// Actions

    // Left button action (triggers rotation and panning)
    m_leftMouseButtonInput->setButtons(QVector<int>() << Qt::LeftButton);
    m_leftMouseButtonInput->setSourceDevice( m_mouseDevice );
    m_leftMouseButtonAction->addInput( m_leftMouseButtonInput );

    //Alt key (triggers panning)
    m_altKeyInput->setButtons(QVector<int>() << Qt::Key_Shift);
    m_altKeyInput->setSourceDevice(m_keyboardDevice);
    m_altKeyAction->addInput(m_altKeyInput);

    //Projection control
    m_changeProjectionTypeInput->setButtons(QVector<int>() << Qt::Key_5);
    m_changeProjectionTypeInput->setSourceDevice(m_keyboardDevice);
    m_changeProjectionAction->addInput(m_changeProjectionTypeInput);

    //// Axes

    // Mouse X
    m_panXInput->setAxis(Qt3DInput::QMouseDevice::X);
    m_panXInput->setSourceDevice(m_mouseDevice);
    m_panXAxis->addInput(m_panXInput);

    // Mouse Y
    m_panYInput->setAxis(Qt3DInput::QMouseDevice::Y);
    m_panYInput->setSourceDevice(m_mouseDevice);
    m_panYAxis->addInput(m_panYInput);

    // Mouse wheel X
    m_wheelXInput->setAxis(Qt3DInput::QMouseDevice::WheelY);
    m_wheelXInput->setSourceDevice(m_mouseDevice);
    m_wheelAxis->addInput(m_wheelXInput);

    m_wheelYInput->setAxis(Qt3DInput::QMouseDevice::WheelX);
    m_wheelYInput->setSourceDevice(m_mouseDevice);
    m_wheelAxis->addInput(m_wheelYInput);

    // Keyboard Pos Txy
    m_rightKeyInput->setButtons(QVector<int>() << Qt::Key_Right);
    m_rightKeyInput->setSourceDevice(m_keyboardDevice);
    m_rightKeyAction->addInput(m_rightKeyInput);

    // Keyboard Neg Txy
    m_leftKeyInput->setButtons(QVector<int>() << Qt::Key_Left);
    m_leftKeyInput->setSourceDevice(m_keyboardDevice);
    m_leftKeyAction->addInput(m_leftKeyInput);

    // Keyboard Pos Tz
    m_upKeyInput->setButtons(QVector<int>() << Qt::Key_Up);
    m_upKeyInput->setSourceDevice(m_keyboardDevice);
    m_upKeyAction->addInput(m_upKeyInput);

    // Keyboard Neg Tz
    m_downKeyInput->setButtons(QVector<int>() << Qt::Key_Down);
    m_downKeyInput->setSourceDevice(m_keyboardDevice);
    m_downKeyAction->addInput(m_downKeyInput);

    //// Logical device
    m_logicalDevice->addAction(m_midMouseButtonAction);
    m_logicalDevice->addAction(m_leftMouseButtonAction);
    m_logicalDevice->addAction(m_altKeyAction);
    m_logicalDevice->addAction(m_leftKeyAction);
    m_logicalDevice->addAction(m_rightKeyAction);
    m_logicalDevice->addAction(m_downKeyAction);
    m_logicalDevice->addAction(m_upKeyAction);
    m_logicalDevice->addAction(m_changeProjectionAction);
    m_logicalDevice->addAxis(m_panXAxis);
    m_logicalDevice->addAxis(m_panYAxis);
    m_logicalDevice->addAxis(m_wheelAxis);

    //// Mouse handler
    Qt3DInput::QMouseHandler *mouseHandler = new Qt3DInput::QMouseHandler();
    mouseHandler->setSourceDevice(m_mouseDevice);

    QObject::connect(mouseHandler, &Qt3DInput::QMouseHandler::pressed,
        [this](Qt3DInput::QMouseEvent *pressedEvent) {
        pressedEvent->setAccepted(true);
        this->m_mouseLastPosition = QPoint(pressedEvent->x(),
            pressedEvent->y());
        });

    QObject::connect(mouseHandler, &Qt3DInput::QMouseHandler::positionChanged,
        [this](Qt3DInput::QMouseEvent *positionChangedEvent) {
        positionChangedEvent->setAccepted(true);
        this->m_mouseCurrentPosition = QPoint(positionChangedEvent->x(),
            positionChangedEvent->y());
        });

    QObject::connect(m_changeProjectionAction, &Qt3DInput::QAction::activeChanged,
        [this](bool isActive) {
        if (isActive) {
        Qt3DRender::QCamera *camera = this->m_camera;
        Qt3DRender::QCameraLens::ProjectionType projectionType = camera->projectionType();
        switch (projectionType) {
        case Qt3DRender::QCameraLens::PerspectiveProjection:
        {
        camera->setProjectionType(Qt3DRender::QCameraLens::OrthographicProjection);
        break;
        }
        case Qt3DRender::QCameraLens::OrthographicProjection:
        {
        camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
        break;
        }

        default:
        break;
        }
        }
        });

    Q_Q(QTrackballCameraController);
    //// FrameAction

    QObject::connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered,
        [this](float dt) {
        this->onTriggered(dt);
        });

    // Disable the logical device when the entity is disabled
    QObject::connect(q, &Qt3DCore::QEntity::enabledChanged, m_logicalDevice, &Qt3DInput::QLogicalDevice::setEnabled);

    q->addComponent(m_frameAction);
    q->addComponent(m_logicalDevice);
    q->addComponent(mouseHandler);
  }

  QVector3D QTrackballCameraControllerPrivate::projectScreenToTrackball(const QPoint &screenCoords,
      const QSize &windowSize,
      const QPoint &trackballCenter,
      float trackballRadius) const
  {
    float normalizeValue = static_cast<float>(qMin(windowSize.width(), windowSize.height()));
    QVector3D p3D(2*( screenCoords.x()                        - trackballCenter.x())/normalizeValue,
        2*((windowSize.height() - screenCoords.y()) - trackballCenter.y())/normalizeValue,
        0.0f);

    float r = trackballRadius;
    float z0 = r * 0.5f;

    float z = 0;
    if (r*r - p3D.lengthSquared() >= z0*z0) {
      z = sqrt(r*r - p3D.lengthSquared());
    }
    else {
      // Original (hyperbolic):
      // z = r*r / (2 * v.length());

      // Consistent (hyperbolic):
      z = z0*std::sqrt(r*r - z0*z0) / p3D.length();
    }

    return QVector3D(p3D[0], p3D[1], z);
  }

  QQuaternion QTrackballCameraControllerPrivate::createRotation(const QPoint &firstPoint,
      const QPoint &nextPoint,
      const QSize &windowSize,
      const QPoint &trackballCenter,
      float trackBallRadius) const
  {
    QVector3D lastPos3D = projectScreenToTrackball(firstPoint, windowSize, trackballCenter, trackBallRadius);
    QVector3D currentPos3D = projectScreenToTrackball(nextPoint, windowSize, trackballCenter, trackBallRadius);

    QVector3D posAvg = (lastPos3D + currentPos3D) / 2;

    // Compute axis of rotation:
    QVector3D dir = QVector3D::crossProduct(currentPos3D,lastPos3D);

    // Approximate rotation angle:
    float t = dir.length()/posAvg.length();

    QQuaternion q = QQuaternion::fromAxisAndAngle(dir, t);
    return q;
  }

  void QTrackballCameraControllerPrivate::onTriggered(float)
  {
    // Perform the control based on the input received during the frame
    if (m_camera != nullptr) {
/*
      std::cout << " camera position: "<< "x" << m_camera->position().x() << "y" << m_camera->position().y() << "z" << m_camera->position().z() << std::endl;
      std::cout << " \tcamera view vector: "<< "x" << m_camera->viewVector().x() << "y" << m_camera->viewVector().y() << "z" << m_camera->viewVector().z() << std::endl;
      std::cout << " \tcamera up vector: "<< "x" << m_camera->upVector().x() << "y" << m_camera->upVector().y() << "z" << m_camera->upVector().z() << std::endl;
*/
      if (m_leftMouseButtonAction->isActive()) {
      //  std::cout << " 3d window size bottom: " << m_camera->lens()->bottom() <<  " top: " << m_camera->lens()->top() << " left " << m_camera->lens()->left() << "right " << m_camera->lens()->right() << std::endl;
        if (m_altKeyAction->isActive()) {
          //Panning mode
          m_camera->translate(m_panSpeed * QVector3D(-m_panXAxis->value(), -m_panYAxis->value(), 0), Qt3DRender::QCamera::TranslateViewCenter);
        } else {
          QQuaternion rotation = createRotation( m_mouseLastPosition,
              m_mouseCurrentPosition,
              m_windowSize,
              m_trackballCenter,
              m_trackballRadius );

          QQuaternion currentRotation = m_camera->transform()->rotation();
          QQuaternion currentRotationInversed = currentRotation.conjugated();

          QVector3D rotatedAxis = currentRotationInversed.rotatedVector(rotation.vector());
          //std::cout << "rotatedAxis" << QString("(%1, %2, %3)").arg(rotatedAxis.x()).arg(rotatedAxis.y()).arg(rotatedAxis.z()).toStdString() <<  std::endl;
          float angle = m_rotationSpeed*rotation.scalar();
          m_camera->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle(rotatedAxis, angle));
          m_mouseLastPosition = m_mouseCurrentPosition;
        }
      } else {
        float temp_m_rotationAngle = m_rotationAngle;
        if (m_altKeyAction->isActive()) {
          temp_m_rotationAngle = m_rotationAngle_fine;
        }
        if( m_downKeyAction->isActive() ){
          m_camera->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle( QVector3D(1, 0, 0), temp_m_rotationAngle ));
        }
        if( m_upKeyAction->isActive() ){
          m_camera->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle( QVector3D(1, 0, 0), -temp_m_rotationAngle ));
        }
        if( m_leftKeyAction->isActive() ){
          m_camera->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle( QVector3D(0, 1, 0), temp_m_rotationAngle ));
        }
        if( m_rightKeyAction->isActive() ){
          m_camera->rotateAboutViewCenter(QQuaternion::fromAxisAndAngle( QVector3D(0, 1, 0), -temp_m_rotationAngle ));
        }
        if( m_wheelAxis->value() != 0 ){
            zoomOnCenter(m_camera->position(), m_zoomSpeed * m_wheelAxis->value(), m_camera->viewCenter() );
        }
      }
    }
  }

  void QTrackballCameraControllerPrivate::zoomOnCenter( QVector3D cam_pos, double value, QVector3D center ){
    const float distance = (cam_pos - center).length();
    if ( (distance -  value ) > m_zoomCameraLimit){
      m_camera->translate( QVector3D(0, 0, value ), Qt3DRender::QCamera::DontTranslateViewCenter );
      float size = std::abs( std::tan( m_camera->fieldOfView() )* distance );
      m_camera->lens()->setOrthographicProjection( -size*m_camera->aspectRatio(), size*m_camera->aspectRatio(), -size, size, 0.1f, 1000.0f );
      m_camera->setTop(size);
      m_camera->setBottom(-size);
      m_camera->setLeft(-size*m_camera->aspectRatio());
      m_camera->setRight(size*m_camera->aspectRatio());
    }
  }

} // Qt3DExtras

QT_END_NAMESPACE
