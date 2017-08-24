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


#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QAbstractTexture>
#include <Qt3DRender/QTextureImage>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderState>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QAbstractTextureImage>
#include <Qt3DRender/QSceneLoader>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <Qt3DExtras/QDiffuseMapMaterial>
#include <Qt3DExtras/QDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QGoochMaterial>
#include <Qt3DExtras/QNormalDiffuseMapMaterial>
#include <Qt3DExtras/QNormalDiffuseMapAlphaMaterial>
#include <Qt3DExtras/QNormalDiffuseSpecularMapMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QPhongMaterial>

#include <Qt3DRender/QAbstractLight>
#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QSpotLight>

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QBufferDataGenerator>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QGeometryFactory>

#include <Qt3DRender/QAlphaCoverage>
#include <Qt3DRender/QAlphaTest>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QColorMask>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QMultiSampleAntiAliasing>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QDithering>
#include <Qt3DRender/QFrontFace>
#include <Qt3DRender/QPointSize>
#include <Qt3DRender/QPolygonOffset>
#include <Qt3DRender/QScissorTest>
#include <Qt3DRender/QStencilTest>
#include <Qt3DRender/QStencilTestArguments>
#include <Qt3DRender/QStencilMask>
#include <Qt3DRender/QStencilOperation>
#include <Qt3DRender/QStencilOperationArguments>
#include <Qt3DRender/QClipPlane>
#include <Qt3DRender/QSeamlessCubemap>

#include <iostream>
#include <QtCore/QtMath>
#include <QtCore/QObject>
#include <QtGui/QMatrix4x4>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DRender/QMesh>

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

    static void removeExpandedChildEntities(Qt3DCore::QEntity *entity, const QString &childName );
    static Qt3DRender::QObjectPicker *entityPicker(Qt3DCore::QEntity *entity);
    static void setEnabledToSubtree(Qt3DCore::QEntity *entity, bool enable);
    static Qt3DCore::QEntity *findEntityByName(Qt3DCore::QEntity *entity, const QString &name);
  private:
    // Private constructor to ensure no actual instance is created
    explicit EditorUtils() {}
};

#endif // EDITORUTILS_H
