/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qt_scene_supercell.h"

#include <QtCore/QDebug>

QtSceneSuperCell::QtSceneSuperCell(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity)
{

    // Plane shape data
    Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh();
    planeMesh->setWidth(2);
    planeMesh->setHeight(2);

    // Plane mesh transform
    Qt3DCore::QTransform *planeTransform = new Qt3DCore::QTransform();
    planeTransform->setScale(1.3f);
    planeTransform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 45.0f));
    planeTransform->setTranslation(QVector3D(0.0f, -4.0f, 0.0f));

    Qt3DExtras::QPhongMaterial *planeMaterial = new Qt3DExtras::QPhongMaterial();
    planeMaterial->setDiffuse(QColor(QRgb(0xa69929)));

    // Plane
    m_planeEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_planeEntity->addComponent(planeMesh);
    m_planeEntity->addComponent(planeMaterial);
    m_planeEntity->addComponent(planeTransform);
    m_planeEntity->setEnabled(true);

    // Sphere shape data
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(20);
    sphereMesh->setSlices(20);
    sphereMesh->setRadius(2);

    // Sphere mesh transform
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();

    sphereTransform->setScale(1.3f);
    sphereTransform->setTranslation(QVector3D(-5.0f, -4.0f, 0.0f));

    Qt3DExtras::QPhongMaterial *sphereMaterial = new Qt3DExtras::QPhongMaterial();
    sphereMaterial->setDiffuse(QColor(QRgb(0xa69929)));

    // Sphere
    m_sphereEntity = new Qt3DCore::QEntity(m_rootEntity);
    m_sphereEntity->addComponent(sphereMesh);
    m_sphereEntity->addComponent(sphereMaterial);
    m_sphereEntity->addComponent(sphereTransform);
        m_sphereEntity->setEnabled(true);


}

QtSceneSuperCell::~QtSceneSuperCell()
{
}

void QtSceneSuperCell::enablePlane(bool enabled)
{
    m_planeEntity->setEnabled(enabled);
}

void QtSceneSuperCell::enableSphere(bool enabled)
{
    m_sphereEntity->setEnabled(enabled);
}
