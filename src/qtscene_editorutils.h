/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D Editor of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef EDITORUTILS_H
#define EDITORUTILS_H

#include <QtCore/QObject>
#include <QtGui/QMatrix4x4>

namespace Qt3DCore {
class QEntity;
class QComponent;
class QTransform;
}
namespace Qt3DRender {
class QGeometryRenderer;
class QGeometry;
class QBuffer;
class QAbstractLight;
class QObjectPicker;
class QCamera;
class QCameraLens;
class QAttribute;
class QRenderPass;
class QSceneLoader;
}

class EditorUtils : public QObject
{
    Q_OBJECT
public:

    static Qt3DRender::QGeometryRenderer *createArrowMesh();
    static Qt3DCore::QEntity *createArrowEntity(const QColor &color, Qt3DCore::QEntity *parent, const QMatrix4x4 &matrix, const QString &name);
  static Qt3DRender::QObjectPicker *entityPicker(Qt3DCore::QEntity *entity);
    static void setEnabledToSubtree(Qt3DCore::QEntity *entity, bool enable);
    static Qt3DCore::QEntity *findEntityByName(Qt3DCore::QEntity *entity, const QString &name);
private:
    // Private constructor to ensure no actual instance is created
    explicit EditorUtils() {}
};

#endif // EDITORUTILS_H
